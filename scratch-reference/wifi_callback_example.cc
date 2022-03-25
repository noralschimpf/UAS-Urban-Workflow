/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"

#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"

#include <string>
// #define PREFIX "scratchlogs/wificallback"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Example");

long recv_counter = 0;


class MyApp : public Application
{
public:

  MyApp ();
  virtual ~MyApp();

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate, uint32_t appId);
  void ChangeRate(DataRate newrate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
  uint32_t        m_appId;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0),
    m_appId(0)
{
}

MyApp::~MyApp()
{
  m_socket = 0;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate, uint32_t appId)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
  m_appId = appId;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}


void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
MyApp::SendPacket (void)
{
  SeqTsHeader seqTs;
  seqTs.SetSeq (m_packetsSent);

  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  char application[20]="";

  if(m_appId == 1){
        strcpy(application, "VIDEO: ");
  }else{
        strcpy(application, "D2D: ");
  }

  std::cout << "SendApp:" << Simulator::Now ().GetSeconds () << "\t" << application << m_packetsSent << "\t" << m_packetSize << "\t" << "to :" << InetSocketAddress::ConvertFrom(m_peer).GetIpv4 () <<"\n";

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}


void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      //Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      Time tNext (Seconds (0.0025));
      //Time tNext (Seconds (0.0001));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

/********* Definition of the Callback function on PacketSink **********/ 
void RcvPacket(Ptr<const Packet> p, const Address &addr){
        int currentSequenceNumber = recv_counter++;
        char application[20]="";

        if(InetSocketAddress::ConvertFrom(addr).GetIpv4().Get() == Ipv4Address("10.1.3.1").Get()){
                strcpy(application, "VIDEO: ");
        }else{
                strcpy(application, "D2D: ");
        }

        std::cout << "ReceiveApp:" << Simulator::Now ().GetSeconds () << "\t" << application << "\t" << currentSequenceNumber << "\t" << p->GetSize() <<"\t" << "from :" << InetSocketAddress::ConvertFrom(addr).GetIpv4 () << "  at port : " << InetSocketAddress::ConvertFrom(addr).GetPort() <<"\n" ;
}


/************************************************************/
int 
main (int argc, char *argv[])
{
  std::string phyMode ("DsssRate1Mbps");
  bool verbose = true;
  uint32_t nWifi = 2;
  bool tracing = false;
  std::string PREFIX = "scratchlogs/wificallback/";

  /********* You can add commandline arguments that you can feed at runtime *******/
  CommandLine cmd (__FILE__);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);


  // The underlying restriction of 18 is due to the grid position
  // allocator's configuration; the grid layout will exceed the
  // bounding box if more than 18 nodes are provided.
  if (nWifi > 18)
    {
      std::cout << "nWifi should be 18 or less; otherwise grid layout exceeds the bounding box" << std::endl;
      return 1;
    }


  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi-1);
  NodeContainer wifiApNode;
  wifiApNode.Create(1);

  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  YansWifiPhyHelper phy;
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::LogDistancePropagationLossModel");
  phy.SetChannel (wifiChannel.Create ());


  WifiMacHelper mac;
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue (phyMode),
                                "ControlMode",StringValue (phyMode));

  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);

  
  /********** Define Mobility Model that includes position allocator + mobility pattern *********/
  /****** Example: define list position allocator and constantposition mobility for Base station *****/
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAllocBS = CreateObject<ListPositionAllocator> ();
  positionAllocBS->Add (Vector(0, 0, 0));
  mobility.SetPositionAllocator(positionAllocBS);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);


  /****** Example: define a separate list position allocator and constantposition mobility for mobile station(s) *****/
  /****** You can use different mobility and allocator for different nodes, just need to create separate mobility object *****/
  MobilityHelper mobilitySta;
  Ptr<ListPositionAllocator> positionAllocSta = CreateObject<ListPositionAllocator> ();
  positionAllocSta->Add (Vector(10, 10, 10));
  mobilitySta.SetPositionAllocator(positionAllocSta);
  mobilitySta.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilitySta.Install (wifiStaNodes);
  mobility.Install (wifiStaNodes);


  InternetStackHelper stack;
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

  Ipv4AddressHelper address;

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer staInterfaces;
  Ipv4InterfaceContainer apInterfaces;
  staInterfaces = address.Assign (staDevices);
  apInterfaces = address.Assign (apDevices);

  UdpEchoServerHelper echoServer (9);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


   /*************** WRITE APPLICATIONS ******************/

    // UDP Video connfection from sta1 to AP1
    ApplicationContainer sinkApps1;
    uint16_t sinkPort1 = 1;
    Address sinkAddress1 (InetSocketAddress (apInterfaces.GetAddress (0), sinkPort1)); // interface of AP1
    PacketSinkHelper packetSinkHelper1 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort1));
    sinkApps1 = packetSinkHelper1.Install (wifiApNode); // AP1 as sink
    sinkApps1.Start (Seconds (0.));
    sinkApps1.Stop (Seconds (100.));

    // Create UDP application at sta1
    Ptr<Socket> ns3UdpSocket1 = Socket::CreateSocket (wifiStaNodes.Get (0), UdpSocketFactory::GetTypeId ()); //source at sta1
    Ptr<MyApp> app1 = CreateObject<MyApp> ();
    app1->Setup (ns3UdpSocket1, sinkAddress1, 1400, 1000, DataRate ("1Mbps"), 1);
    wifiStaNodes.Get (0)->AddApplication (app1);
    app1->SetStartTime (Seconds (1.));
    app1->SetStopTime (Seconds (10.));


    /**************** FLOW MONITOR can track packet statistics at Layer 3 ********************/
    /**************** FLOW MONITOR SETUP ********************/
    Ptr<FlowMonitor> flowmon;
    FlowMonitorHelper flowmonHelper;
    flowmon = flowmonHelper.InstallAll ();
    flowmon->CheckForLostPackets ();

    /**************** FLOW MONITOR ANALYZE ********************/
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonHelper.GetClassifier ());
    std::map<FlowId, FlowMonitor::FlowStats> stats = flowmon->GetFlowStats ();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
          Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
          std::cout << std::endl;
          std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
          std::cout << "------------------------------------" << std::endl;
          NS_LOG_UNCOND("Tx Packets = " << i->second.txPackets);
          NS_LOG_UNCOND("Tx Bytes = " << i->second.txBytes);
          NS_LOG_UNCOND("Sum jitter = " << i->second.jitterSum);
          NS_LOG_UNCOND("Delay Sum = " << i->second.delaySum);
          NS_LOG_UNCOND("Lost Packet = " << i->second.lostPackets);
          NS_LOG_UNCOND("Rx Bytes = " << i->second.rxBytes);
          NS_LOG_UNCOND("Rx Packets = " << i->second.rxPackets);
          std::cout << "Throughput = " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n";
          NS_LOG_UNCOND("Throughput = " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n");
          NS_LOG_UNCOND("Packet loss = " << (((i->second.txPackets-i->second.rxPackets)*1.0)/i->second.txPackets)*100 << " %");
    }

    flowmon->SerializeToXmlFile(PREFIX + "burst_wifi_ns3.flowmon", true, true);

    Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::PacketSink/Rx", MakeCallback(&RcvPacket));


   /******** Writing into pcap files for collecting packetsniff and analyze with tools like Wireshark or tcpdump ****/ 
      phy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
      phy.EnablePcap (PREFIX + "example_test", apDevices.Get (0));



    /************* RUN AND SCHEDULE THE SIMULATOR ***************/
    NS_LOG_INFO ("Run Simulation");
    //Simulator::Schedule(Seconds(1.001), RcvPktTrace);
    Simulator::Stop (Seconds(400.0));
    Simulator::Run ();
    Simulator::Destroy ();

  return 0;
}
