
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/lte-module.h"
//#include "ns3/gtk-config-store.h"

#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

/**
 * Sample simulation script for LTE+EPC. It instantiates several eNodeBs,
 * attaches one UE per eNodeB starts a flow for each UE to and from a remote host.
 * It also starts another flow between each UE pair.
 */

NS_LOG_COMPONENT_DEFINE ("Sample_LTE");

long recv_counter = 0;

class MyApp : public Application
{
public:

  MyApp ();
  virtual ~MyApp();

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);
  void ChangePacketSize(uint32_t packetSize);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void LoadCSV (void);//load file
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

};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp()
{
  m_socket = 0;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)//appId
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
  LoadCSV();//load CSV data from file
}

void
MyApp::StartApplication (void)
{
  std::cout << "Start Application: " << std::endl;
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);//initiate connection with remote peer using socket
  SendPacket ();
}


void
MyApp::StopApplication (void)
{
  //std::cout << "Stop Application: " << std::endl;
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
//Helper method to change packe size 
void 
MyApp::ChangePacketSize(uint32_t packetSize) {
    m_packetSize = packetSize;
}

std::vector<float> v1;//packet arrival time
std::vector<float> v2;//packet schedule time
std::vector<int> v3;//packet size
int csv_size;//csv entry

//Helper method to load csv packet information into vectors 
void
MyApp::LoadCSV(){//csv entry
    float x = 0.0;
    int x1 = 0;
    //adjust path string to host computer.
    CsvReader csv ("/Users/rafaapaza/ns-allinone-3.35/ns-3.35/scratch/CSVRead/CSE-UDP1-Large.csv", ',');
    while (csv.FetchNextRow()) {  
        csv.GetValue(0, x); //read time value
        v1.push_back(x);//store in vector V1
        csv.GetValue(1, x1); //read packet size value
        v3.push_back(x1);//store in vector V3
    }
    csv_size = v1.size();// number of packets in file
    v2.push_back(v1.at(0));
    for (int i =1; i < csv_size; i++){
       v2.push_back(v1.at(i) - v1.at(i-1)); //packet schedule time, store in vector v2
    }
}

void
MyApp::SendPacket (void)
{
  SeqTsHeader seqTs;
  seqTs.SetSeq (m_packetsSent);

  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

bool variableBurst;
bool toggle2 = true;
int i_var = 0;//packet counter

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
        if(i_var <= csv_size) { 
            this ->ChangePacketSize(v3.at(i_var));//packet size update
            Time tNext (Seconds (v2.at(i_var)));//schedule packet send
            //std::cout << v2.at(i_var) << " " << i_var << std::endl;//test code
            m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
            i_var++;           
      }
    }
}

void RcvPacket(Ptr<const Packet> p, const Address &addr){

    std::cout << "receiving" << std::endl;
    std::cout << "ReceiveApp:" << Simulator::Now ().GetSeconds () << std::endl; 
    //std::cout << "ReceiveApp:" << Simulator::Now ().GetSeconds () << "\t" << application << "\t" << currentSequenceNumber << "\t" << p->GetSize() <<"\t" << "from :" << InetSocketAddress::ConvertFrom(addr).GetIpv4 () << "  at port : " << InetSocketAddress::ConvertFrom(addr).GetPort() <<"\n" ;
}


int
main (int argc, char *argv[])
{
  uint16_t numNodePairs = 1;//adjusted from 2 for testing
  Time simTime = MilliSeconds (1100);
  Time appEndTime = MilliSeconds (1000);//stop application
  double distance = 60.0;
  Time interPacketInterval = MilliSeconds (100);
  bool useCa = false;
  bool disableDl = true;
  bool disableUl = false;
  bool disablePl = true;

  // Command line arguments
  CommandLine cmd (__FILE__);
  cmd.AddValue ("numNodePairs", "Number of eNodeBs + UE pairs", numNodePairs);
  cmd.AddValue ("simTime", "Total duration of the simulation", simTime);
  cmd.AddValue ("distance", "Distance between eNBs [m]", distance);
  cmd.AddValue ("interPacketInterval", "Inter packet interval", interPacketInterval);
  cmd.AddValue ("useCa", "Whether to use carrier aggregation.", useCa);
  cmd.AddValue ("disableDl", "Disable downlink data flows", disableDl);
  cmd.AddValue ("disableUl", "Disable uplink data flows", disableUl);
  cmd.AddValue ("disablePl", "Disable data flows between peer UEs", disablePl);
  cmd.Parse (argc, argv);

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();

  // parse again so you can override default values from the command line
  cmd.Parse(argc, argv);

  if (useCa)
   {
     Config::SetDefault ("ns3::LteHelper::UseCa", BooleanValue (useCa));
     Config::SetDefault ("ns3::LteHelper::NumberOfComponentCarriers", UintegerValue (2));
     Config::SetDefault ("ns3::LteHelper::EnbComponentCarrierManager", StringValue ("ns3::RrComponentCarrierManager"));
   }

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

  Ptr<Node> pgw = epcHelper->GetPgwNode ();

   // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (10)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  // interface 0 is localhost, 1 is the p2p device
  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  NodeContainer ueNodes;
  NodeContainer enbNodes;
  enbNodes.Create (numNodePairs);
  ueNodes.Create (numNodePairs);

  // Install Mobility Model
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  for (uint16_t i = 0; i < numNodePairs; i++)
    {
      positionAlloc->Add (Vector (distance * i, 0, 0));
    }
  MobilityHelper mobility;
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator(positionAlloc);
  mobility.Install(enbNodes);
  mobility.Install(ueNodes);

  // Install LTE Devices to the nodes
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

  // Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));
  // Assign IP address to UEs, and install applications
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  // Attach one UE per eNodeB
  for (uint16_t i = 0; i < numNodePairs; i++)
    {
      lteHelper->Attach (ueLteDevs.Get(i), enbLteDevs.Get(i));
      // side effect: the default EPS bearer will be activated
    }


  // Install and start applications on UEs and remote host
  uint16_t dlPort = 1100;
  uint16_t ulPort = 2000;
  uint16_t otherPort = 3000;
  ApplicationContainer clientApps;
  ApplicationContainer serverApps;
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      if (!disableDl)
        {
          PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
          serverApps.Add (dlPacketSinkHelper.Install (ueNodes.Get (u)));

          UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
          dlClient.SetAttribute ("Interval", TimeValue (interPacketInterval));
          dlClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
          clientApps.Add (dlClient.Install (remoteHost));
        }

      if (!disableUl)
        {
          ++ulPort;
          PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
          serverApps.Add (ulPacketSinkHelper.Install (remoteHost));

          //UdpClientHelper ulClient (remoteHostAddr, ulPort);
          //ulClient.SetAttribute ("Interval", TimeValue (interPacketInterval));
          //ulClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
          //clientApps.Add (ulClient.Install (ueNodes.Get(u)));

          /* New code with */
          Ptr<Socket> ns3UdpSocket1 = Socket::CreateSocket (ueNodes.Get (0), UdpSocketFactory::GetTypeId ()); //Server at UE create socket
          Ptr<MyApp> app2 = CreateObject<MyApp> ();
          app2->Setup (ns3UdpSocket1, remoteHostAddr, 1400, 1000, DataRate ("1Mbps"));//UE socket, peer address, pkt size 1400
          ueNodes.Get (0)->AddApplication (app2);//connect app1 to ue: internetIpIfaces.GetAddress(0)
          app2->SetStartTime (MilliSeconds (500));
          app2->SetStopTime (appEndTime);
        }

      if (!disablePl && numNodePairs > 1)
        {
          ++otherPort;
          PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), otherPort));
          serverApps.Add (packetSinkHelper.Install (ueNodes.Get (u)));

          UdpClientHelper client (ueIpIface.GetAddress (u), otherPort);
          client.SetAttribute ("Interval", TimeValue (interPacketInterval));
          client.SetAttribute ("MaxPackets", UintegerValue (1000000));
          clientApps.Add (client.Install (ueNodes.Get ((u + 1) % numNodePairs)));
        }
    }

  serverApps.Start (MilliSeconds (500));
  clientApps.Start (MilliSeconds (500));
  lteHelper->EnableTraces ();
  // Uncomment to enable PCAP tracing
  //p2ph.EnablePcapAll("lena-simple-epc");
  
  Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::PacketSink/Rx", MakeCallback(&RcvPacket));
  
  Simulator::Stop (simTime);
  Simulator::Run ();

  /*GtkConfigStore config;
  config.ConfigureAttributes();*/

  Simulator::Destroy ();
  return 0;
}
