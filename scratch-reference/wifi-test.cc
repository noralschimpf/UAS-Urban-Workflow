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

#include <string>

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

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Example");

long recv_counter = 0;


//===================   GLOBAL OPTIONS   ===================//
// #define DRATE "1Mbps"                                    //
#define DRATE "10Mbps"                                   //
//                                                          //
#define TRAFFIC 1                                        // Constant Traffic Rate
// #define TRAFFIC 2                                        // Bursty Traffic Rate
#define BURST 20                                         //
// #define BURST 100                                        //
//                                                          //
#define PROPMOD "ns3::LogDistancePropagationLossModel"   //
// #define PROPMOD "ns3::Cost231PropagationLossModel"       //
//                                                          //
//===================   GLOBAL OPTIONS   ===================//

class MyApp : public Application
{
public:

  MyApp ();
  virtual ~MyApp();

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate, uint32_t appId, int isTraffic=0, int burstSize=0);
  void ChangeRate(DataRate newrate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void ScheduleTrafficTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_traffic;
  uint32_t        m_packetsSent;
  uint32_t        m_appId;
  uint32_t        i_burst;
  uint32_t        m_burstSize;


};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_traffic (0),
    m_packetsSent (0),
    m_appId(0)
{
}

MyApp::~MyApp()
{
  m_socket = 0;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate, uint32_t appId, int isTraffic, int burstSize)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
  m_appId = appId;
  m_traffic = isTraffic;
  m_burstSize = burstSize;
  i_burst = 0;
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
      if (m_traffic == 0)   {ScheduleTx ();}
      else                  {ScheduleTrafficTx();}      
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

void
MyApp::ScheduleTrafficTx (void)
{
  if (m_running)
    {
      // Time tNext = Seconds(0.001)
      // PERIODIC TRAFFIC
      // Time tNext  = Seconds(0.001);
      Time tNext;
      if (m_traffic == 1)   {tNext = (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));}
      
      else if (m_traffic == 2){
        if (i_burst == m_burstSize) {
          tNext = Seconds (1.000);
          i_burst = 0;
          }
        else{
          tNext = (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
          i_burst++;
        }

      }
      
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

/********* Definition of the Callback function on PacketSink **********/ 
void RcvPacket(Ptr<const Packet> p, const Address &addr){
        char application[20]="";
        int  currentSequenceNumber = 0;
        // currentSequenceNumber = recv_counter++;
        // IF source address matches sender address, increment packet count
        if(InetSocketAddress::ConvertFrom(addr).GetIpv4().Get() == Ipv4Address("10.1.3.1").Get()){
                currentSequenceNumber = recv_counter++;
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
  uint32_t nWifi = 3;
  bool tracing = false;
  std::string PREFIX = "scratchlogs/wifitest/";

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
  wifiChannel.AddPropagationLoss (PROPMOD);
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
  MobilityHelper mobilityStaSRC;
  Ptr<ListPositionAllocator> positionAllocStaSRC = CreateObject<ListPositionAllocator> ();
  positionAllocStaSRC->Add (Vector(10, 10, 10));
  mobilityStaSRC.SetPositionAllocator(positionAllocStaSRC);
  mobilityStaSRC.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityStaSRC.Install (wifiStaNodes);
  mobility.Install (wifiStaNodes);

  MobilityHelper mobilityStaSNK;
  Ptr<ListPositionAllocator> positionAllocStaSNK = CreateObject<ListPositionAllocator> ();
  positionAllocStaSNK->Add (Vector(40, 10, 10));
  mobilityStaSNK.SetPositionAllocator(positionAllocStaSNK);
  mobilityStaSNK.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityStaSNK.Install (wifiStaNodes.Get(1));
  mobility.Install (wifiStaNodes.Get(1));



  InternetStackHelper stack;
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

  Ipv4AddressHelper address;

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer staInterfaces;
  Ipv4InterfaceContainer apInterfaces;
  staInterfaces = address.Assign (staDevices);
  apInterfaces = address.Assign (apDevices);

  // UdpEchoServerHelper echoServer (9);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


   /*************** WRITE APPLICATIONS ******************/

    // UDP Video connfection from sta1 to AP
    ApplicationContainer sinkApps1;
    uint16_t sinkPort1 = 1;
    Address sinkAddress1 (InetSocketAddress (apInterfaces.GetAddress(0), sinkPort1)); // interface of AP1
    PacketSinkHelper packetSinkHelper1 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort1));
    sinkApps1 = packetSinkHelper1.Install (wifiApNode); // AP1 as sink
    // sinkApps1 = packetSinkHelper1.Install (wifiApNode); // Sta2 as sink
    sinkApps1.Start (Seconds (0.));
    sinkApps1.Stop (Seconds (100.));

    // Create TCP application at sta1
    Ptr<Socket> ns3TcpSocket1 = Socket::CreateSocket (wifiStaNodes.Get (0), TcpSocketFactory::GetTypeId ()); //source at sta1
    Ptr<MyApp> app1 = CreateObject<MyApp> ();
    app1->Setup (ns3TcpSocket1, sinkAddress1, 1400, 10000, DataRate ("1Mbps"), 1);
    wifiStaNodes.Get (0)->AddApplication (app1);
    app1->SetStartTime (Seconds (1.));
    app1->SetStopTime (Seconds (10.));

    // Create traffic-generating node at sta2
    Ptr<Socket> ns3TcpSocket2 = Socket::CreateSocket (wifiStaNodes.Get (1), TcpSocketFactory::GetTypeId ()); //source at sta1
    Ptr<MyApp> app2 = CreateObject<MyApp> ();
    app2->Setup (ns3TcpSocket2, sinkAddress1, 1400, 10000, DataRate (DRATE), 2, TRAFFIC, BURST);
    wifiStaNodes.Get (0)->AddApplication (app2);
    app2->SetStartTime (Seconds (1.));
    app2->SetStopTime (Seconds (10.));


    /**************** FLOW MONITOR can track packet statistics at Layer 3 ********************/
    /**************** FLOW MONITOR SETUP ********************/
    Ptr<FlowMonitor> flowmon;
    FlowMonitorHelper flowmonHelper;
    flowmon = flowmonHelper.InstallAll();
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

    // flowmon->SerializeToXmlFile(PREFIX + "burst_wifi_ns3.flowmon", true, true);

    Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::PacketSink/Rx", MakeCallback(&RcvPacket));


   /******** Writing into pcap files for collecting packetsniff and analyze with tools like Wireshark or tcpdump ****/ 
      phy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
      phy.EnablePcap (PREFIX + "example_test", apDevices.Get (0), true);


    /************* RUN AND SCHEDULE THE SIMULATOR ***************/
    NS_LOG_INFO ("Run Simulation");
    // Simulator::Schedule(Seconds(1.001), RcvPacket);
    Simulator::Stop (Seconds(400.0));
    Simulator::Run ();
    Simulator::Destroy ();
    flowmon->SerializeToXmlFile(PREFIX + "burst_wifi_ns3.flowmon", true, true);

  return 0;
}
