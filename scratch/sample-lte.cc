#include "ns3/buildings-module.h"
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/lte-module.h"
// #include "math.h"
//#include "ns3/gtk-config-store.h"

#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/three-gpp-spectrum-propagation-loss-model.h"
#include "ns3/three-gpp-v2v-propagation-loss-model.h"
#include "ns3/three-gpp-channel-model.h"

#include "UAS-Mobility.h"

using namespace ns3;

/**
 * Sample simulation script for LTE+EPC. It instantiates several eNodeBs,
 * attaches one UE per eNodeB starts a flow for each UE to and from a remote host.
 * It also starts another flow between each UE pair.
 */

NS_LOG_COMPONENT_DEFINE ("Sample_LTE");

ns3::Time constAccelDuration(ns3::Vector3D start, ns3::Vector3D end)
{
  //start and end in m, vScatt in m/s
  
  double dist; double dur;
  dist = sqrt(pow(end.x - start.x, 2) + pow(end.y - start.y, 2) + pow(end.z - start.z, 2));
  dur = vScatt / dist;
  
  return ns3::MilliSeconds(1000*dur);
}

ns3::Time createUASMobility(ns3::Ptr<ns3::Node> node, uint16_t scenario, ns3::Time duration)
{
  Ptr<MobilityModel> uasMob;
  // set the mobility model
//   double vTx = vScatt;
  Time t_count = MilliSeconds (0);

  if(scenario==0)
    { 
    uasMob = CreateObject<RandomWalk2dMobilityModel>();
    uasMob->SetAttribute ("Bounds", RectangleValue (Rectangle (0, maxAxisX, 0, maxAxisY)));
    uasMob->SetAttribute ("Speed", StringValue ("ns3::ConstantRandomVariable[Constant=" + std::to_string(vScatt) + "]"));
    t_count = duration;
    }
  else
    {
    uasMob = CreateObject<WaypointMobilityModel> ();  
    Vector vec1; Vector vec2;

  switch(scenario)
    {
      //TODO: Adjust time duration: t += v/dist
    case 1:
      //Scenario: Out-and-back, diagonally, full enb coverage
      vec1 = Vector(0 + 1*buildingSizeX + 0*streetWidth, 0 + 1*buildingSizeY + 0*streetWidth, buildingHeight);

      vec2 = Vector(0 + 1*buildingSizeX + 0*streetWidth, 0 + 1*buildingSizeY + 0*streetWidth, buildingHeight);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      

      vec1 = vec2;
      vec2 = Vector (maxAxisX - 1*buildingSizeX - 0*streetWidth, 0 + 3*buildingSizeY + 2*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisX - streetWidth) / 2 / vTx);    
      

      vec1 = vec2;
      vec2 = Vector ( 0 + 1*buildingSizeX + 0*streetWidth, 0 + 1*buildingSizeY + 0*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisX - streetWidth) / 2 / vTx);    
      break;

    case 2:
      //Scenario: recangular loop, along street, within enb coverage
      vec1 = Vector ( 0 + 1*buildingSizeX + 0.5*streetWidth, 0 + 1*buildingSizeY + 0.5*streetWidth, buildingHeight);

      vec2 = Vector ( 0 + 1*buildingSizeX + 0.5*streetWidth, 0 + 1*buildingSizeY + 0.5*streetWidth, buildingHeight);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      vec2 = Vector ( maxAxisX - 1*buildingSizeX - 0.5*streetWidth, 0 + 1*buildingSizeY + 0.5*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      vec2 = Vector ( maxAxisX - 1*buildingSizeX - 0.5*streetWidth, 0 + 2*buildingSizeY + 1.5*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      vec2 = Vector ( 0 + 1*buildingSizeX + 0.5*streetWidth, 0 + 2*buildingSizeY + 1.5*streetWidth, buildingHeight); 
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      vec2 = Vector ( 0 + 1*buildingSizeX + 0.5*streetWidth, 0 + 1*buildingSizeY + 0.5*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      break;

    case 3:
      //Scenario: Traingular loop, along street, within enb coverage
      vec1 = Vector ( 0 + 3*buildingSizeX + 2.5*streetWidth, 0 + 2.5*buildingSizeY + 2*streetWidth, buildingHeight);
      
      vec2 = Vector ( 0 + 3*buildingSizeX + 2.5*streetWidth, 0 + 2.5*buildingSizeY + 2*streetWidth, buildingHeight);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      vec2 = Vector ( 0 + 3*buildingSizeX + 2.5*streetWidth, 0 + 0.5*buildingSizeY + 0*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      vec2 = Vector ( 0 + 7.5*buildingSizeX + 7*streetWidth, 0 + 2.5*buildingSizeY + 2*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      vec2 = Vector ( 0 + 3*buildingSizeX + 2.5*streetWidth, 0 + 2.5*buildingSizeY + 2*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      break;

    case 4:
      //Scenario: out-and-back, diagonally, exits enb coverage
      vec1 = Vector ( maxAxisX - 1.5*buildingSizeX - 1*streetWidth, 0 + 2.5*buildingSizeY + 2*streetWidth, buildingHeight);

      vec2 = Vector ( maxAxisX - 1.5*buildingSizeX - 1*streetWidth, 0 + 2.5*buildingSizeY + 2*streetWidth, buildingHeight);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      vec2 = Vector ( 0 + 2.5*buildingSizeX + 2*streetWidth, maxAxisY - 1.5*buildingSizeY + 1*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      vec2 = Vector ( maxAxisX - 1.5*buildingSizeX - 1*streetWidth, 0 + 2.5*buildingSizeY + 2*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      break;

    case 5:
      //Scenario: Snake across complete map, in-and-out of coverage
      vec1 = Vector ( 0 + 0.5*buildingSizeX + 0*streetWidth, maxAxisY - 0.5*buildingSizeY - 0*streetWidth, buildingHeight);

      vec2 = Vector ( 0 + 0.5*buildingSizeX + 0*streetWidth, maxAxisY - 0.5*buildingSizeY - 0*streetWidth, buildingHeight);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      vec2 = Vector ( 0 + 0.5*buildingSizeX + 0*streetWidth, 0 + 0.5*buildingSizeY + 0*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;


      vec2 = Vector ( 0 + 2.5*buildingSizeX + 2*streetWidth, 0 + 0.5*buildingSizeY + 0*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      vec2 = Vector ( 0 + 2.5*buildingSizeX + 2*streetWidth, maxAxisY - 0.5*buildingSizeY - 0*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;


      vec2 = Vector ( 0 + 4.5*buildingSizeX + 4*streetWidth, maxAxisY - 0.5*buildingSizeY - 0*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      vec2 = Vector ( 0 + 4.5*buildingSizeX + 4*streetWidth, 0 + 0.5*buildingSizeY + 0*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;



      vec2 = Vector ( 0 + 6.5*buildingSizeX + 6*streetWidth, 0 + 0.5*buildingSizeY + 0*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      vec2 = Vector ( 0 + 6.5*buildingSizeX + 6*streetWidth, maxAxisY - 0.5*buildingSizeY - 0*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;


      vec2 = Vector ( 0 + 8.5*buildingSizeX + 8*streetWidth, maxAxisY - 0.5*buildingSizeY - 0*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      vec2 = Vector ( 0 + 8.5*buildingSizeX + 8*streetWidth, 0 + 0.5*buildingSizeY + 0*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;


      vec2 = Vector ( 0 + 10.5*buildingSizeX + 10*streetWidth, 0 + 0.5*buildingSizeY + 0*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      vec2 = Vector ( 0 + 10.5*buildingSizeX + 10*streetWidth, maxAxisY - 0.5*buildingSizeY - 0*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;


      vec2 = Vector ( 0 + 12.5*buildingSizeX + 12*streetWidth, maxAxisY - 0.5*buildingSizeY - 0*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      vec2 = Vector ( 0 + 12.5*buildingSizeX + 12*streetWidth, 0 + 0.5*buildingSizeY + 0*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;


      vec2 = Vector ( 0 + 14.5*buildingSizeX + 14*streetWidth, 0 + 0.5*buildingSizeY + 0*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      vec1 = vec2;

      vec2 = Vector ( 0 + 14.5*buildingSizeX + 14*streetWidth, maxAxisY - 0.5*buildingSizeY - 0*streetWidth, buildingHeight);
      t_count += constAccelDuration(vec1, vec2);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, vec2));
      // t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      break;

    default:
      break;
      }
    }


  node->AggregateObject (uasMob);
  return t_count;
//   return uasMob;
}


void createBSSMobility(ns3::Ptr<ns3::Node> node, ns3::Vector v)
{
  Ptr<MobilityModel> bssMob;  
  // set the mobility model
  bssMob = CreateObject<ConstantPositionMobilityModel>();
  bssMob->SetPosition(v);
  node->AggregateObject(bssMob);
//   return bssMob;
}





int main (int argc, char *argv[])
{
  uint16_t numNodePairs = 2;
//   uint16_t scenario = 0;
//   Time simTime = MilliSeconds (1100);

  uint16_t scenario = 1;
  Time simTime = MilliSeconds (0);

  double distance = 60.0;
  Time interPacketInterval = MilliSeconds (100);
  bool useCa = false;
  bool disableDl = false;
  bool disableUl = false;
  bool disablePl = false;
  bool tracing = false;

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
  cmd.AddValue("tracing", "Enable Packet Capture", tracing);
  cmd.AddValue("scenario", "UAS Flight Path", scenario);
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
  //TODO: MODIFY WITH V2V EX
  // Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  // for (uint16_t i = 0; i < numNodePairs; i++)
  //   {
  //     positionAlloc->Add (Vector (distance * i, 0, 0));
  //   }
  // MobilityHelper mobility;
  // mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  // mobility.SetPositionAllocator(positionAlloc);
  // mobility.Install(enbNodes);
  // mobility.Install(ueNodes);
  for(uint32_t i=0; i<ueNodes.GetN(); i++){
    ns3::Time t_uas = createUASMobility(ueNodes.Get(i), scenario, simTime) + MilliSeconds(500);
    if (t_uas > simTime) {simTime = t_uas;}
  }
  for(uint32_t i=0; i<enbNodes.GetN(); i++){
    createBSSMobility(enbNodes.Get(i), Vector(distance*i,0,0));
  }

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

          UdpClientHelper ulClient (remoteHostAddr, ulPort);
          ulClient.SetAttribute ("Interval", TimeValue (interPacketInterval));
          ulClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
          clientApps.Add (ulClient.Install (ueNodes.Get(u)));
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
  if(tracing){
  p2ph.EnablePcapAll("scratchlogs/LTE/lena-simple-epc");}

  Simulator::Stop (simTime);
  Simulator::Run ();

  /*GtkConfigStore config;
  config.ConfigureAttributes();*/

  Simulator::Destroy ();
  return 0;
}
