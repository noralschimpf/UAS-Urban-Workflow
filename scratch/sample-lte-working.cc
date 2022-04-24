#include "ns3/buildings-module.h"
#include <ns3/buildings-propagation-loss-model.h>
#include <ns3/buildings-helper.h>
#include <ns3/radio-environment-map-helper.h>
#include <iomanip>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store.h"
#include "ns3/config-store-module.h"
#include "ns3/lte-module.h"
// #include "math.h"
//#include "ns3/gtk-config-store.h"

#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/three-gpp-spectrum-propagation-loss-model.h"
#include "ns3/three-gpp-v2v-propagation-loss-model.h"
#include "ns3/three-gpp-channel-model.h"
#include "ns3/spectrum-helper.h"

#include "UAS-Mobility.h"
#include "QoS-Metrics.h"

using namespace ns3;

/**
 * Sample simulation script for LTE+EPC. It instantiates several eNodeBs,
 * attaches one UE per eNodeB starts a flow for each UE to and from a remote host.
 * It also starts another flow between each UE pair.
 */

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
      NS_LOG_ERROR("ERR: INVALID SCENARIO SPECIFIED");
      exit(1);
      break;
      }
    }
  
  // mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  // mobility.SetPositionAllocator(enbPositionAlloc);
  // mobility.Install(enbNodes);
  // mobility.SetPositionAllocator(enbUePositionAlloc);
  // mobility.Install(enbUeNodes);
  // BuildingsHelper::Install (enbNodes);
  // BuildingsHelper::Install (enbUeNodes);
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

  // mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  // mobility.SetPositionAllocator(enbPositionAlloc);
  // mobility.Install(enbNodes);
  // mobility.SetPositionAllocator(enbUePositionAlloc);
  // mobility.Install(enbUeNodes);
  // BuildingsHelper::Install (enbNodes);
  // BuildingsHelper::Install (enbUeNodes);
}


uint32_t getNearestEnB(ns3::Ptr<ns3::Node> ueNode, ns3::NodeContainer enbNodes)
{
  uint32_t idxAssociate = 0;
  Ptr<MobilityModel> ueMob = ueNode->GetObject<MobilityModel>();
  Vector ueVec = ueMob->GetPosition();
  Vector curEnbVec; 
  double minDist = __DBL_MAX__; double curDist;
  for (uint32_t i = 0; i < enbNodes.GetN(); i++)
  {
    curEnbVec = enbNodes.Get(i)->GetObject<MobilityModel>()->GetPosition();
    curDist = sqrt(pow(ueVec.x - curEnbVec.x, 2) + pow(ueVec.y - curEnbVec.y, 2) + pow(ueVec.z - curEnbVec.z, 2));
    if(curDist < minDist)
    {
      idxAssociate = i;
      minDist = curDist;
    }
    
  }
  return idxAssociate;
}

std::vector<float> v1;//store packet arrival time
std::vector<float> v2;//store packet schedule time, used to schedule packet send
std::vector<int> v3;//store packet size
int csv_size;//csv entry
void packetRead()
{
//Helper method to load csv packet information into vectors
    float x = 0.0;
    int x1 = 0;
    //adjust path string to host computer.
    CsvReader csv ("path to directory where file is/CSE-UDP1-Large.csv", ',');
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


int main (int argc, char *argv[])
{
  uint16_t numNodePairs = 2;
  uint16_t scenario = 0;
  Time simTime = MilliSeconds (1100);

  double distance = 60.0;
  Time interPacketInterval = MilliSeconds (100);
  bool useCa = false;
  bool disableDl = false;
  bool disableUl = false;
  bool disablePl = false;
  bool tracing = false;
  std::string handoverMetric = "RSRQ";

  scenario = 3;
  // simTime = MilliSeconds (0);
  tracing = true;

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
  cmd.AddValue("handover", "Algorithm for Handover", handoverMetric);
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

  std::string algo;
  for(std::size_t i = 0; i < handoverMetric.length(); i++){
    handoverMetric[i] = std::tolower(handoverMetric[i]);
  }
  if (handoverMetric == "rsrp"){algo = "ns3::A3RsrpHandoverAlgorithm";}
  else if (handoverMetric == "rsrq"){algo = "ns3::A2A4RsrqHandoverAlgorithm";}
  else if (handoverMetric == "none") {algo = "ns3::NoOpHandoverAlgorithm";}
  else{NS_LOG_ERROR("INVALID HANDOVER METRIC");}
  std::cout << "handover algo: " << algo << std::endl;
  lteHelper->SetHandoverAlgorithmType(algo);
  
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
  remoteHostStaticRouting->TraceConnectWithoutContext("Drop", MakeBoundCallback(&IpDropTrace, callbackFile));
  remoteHostStaticRouting->TraceConnectWithoutContext("Tx", MakeBoundCallback(&ipTxTrace, callbackFile));
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  NodeContainer ueNodes;
  NodeContainer enbNodes;
  NodeContainer enbUeNodes;
  enbNodes.Create (18);
  enbUeNodes.Create(18);
  ueNodes.Create (1);


  // a grid of buildings
  std::vector<Ptr<Building>> buildingVector;
  for (uint32_t buildingIdX = 0; buildingIdX < numBuildingsX; ++buildingIdX)
  {
      for (uint32_t buildingIdY = 0; buildingIdY < numBuildingsY; ++buildingIdY)
      {
          Ptr < Building > building;
          building = CreateObject<Building> ();

          building->SetBoundaries (Box (buildingIdX * (buildingSizeX + streetWidth),
                                        buildingIdX * (buildingSizeX + streetWidth) + buildingSizeX,
                                        buildingIdY * (buildingSizeY + streetWidth),
                                        buildingIdY * (buildingSizeY + streetWidth) + buildingSizeY,
                                        0.0, buildingHeight));
          building->SetNRoomsX (1);
          building->SetNRoomsY (1);
          building->SetNFloors (1);
          buildingVector.push_back (building);
      }
  }

  // Add Static Positions for EnB and Traffic Loads
  std::vector<Vector> positions{Vector(0,0,30),Vector(1600,0,30),Vector(3200,0,30),Vector(800,1380,30),Vector(2400,1380,30),Vector(4000,1380,30)};

  Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator> ();
  Ptr<ListPositionAllocator> enbUePositionAlloc = CreateObject<ListPositionAllocator> ();
   for(int i = 0; i < 6; i++)
  {
      for(int j = 0; j < 3; j++)
      {
          uint32_t nodeNum = j + i*3;
          createBSSMobility(enbNodes.Get(nodeNum),positions[i]);
          createBSSMobility(enbUeNodes.Get(nodeNum), positions[i]);
      }
  }


  std::cout<< "Adding Mobility" << std::endl;

  // Set UAS mobility models
  for(uint32_t i=0; i<ueNodes.GetN(); i++){
    ns3::Time t_uas = createUASMobility(ueNodes.Get(i), scenario, simTime) + MilliSeconds(500);
    if (t_uas > simTime) {simTime = t_uas;}
  }

  // // mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  // // mobility.SetPositionAllocator(enbPositionAlloc);
  // // mobility.Install(enbNodes);
  // // mobility.SetPositionAllocator(enbUePositionAlloc);
  // // mobility.Install(enbUeNodes);
  // std::cout << std::endl << "ENB BUILDING INSTALL...";
  // BuildingsHelper::Install (enbNodes);
  // std::cout << ".....ENB UE BUILDING INSTALL...";
  // BuildingsHelper::Install (enbUeNodes);
  // std::cout << ".....DONE" << std::endl;



  // Install EnB / UE Devices, include EnB Antenna Sectorization
  // Install LTE Devices to the nodes
  std::cout << "HANDOVER ALGORITHM" <<std::endl;
  
  std::cout << "NETDEVICE INSTALLS: ENB....";
  // NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice(enbNodes);
  std::cout << "ENB UE.....";
  NetDeviceContainer enbUeDevs = lteHelper->InstallUeDevice(enbUeNodes);
  std::cout << "UAS UE.....";
  NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);
  std::cout << "DONE" << std::endl;

    //------------------------------------------------------------//
  //-------------Install LTE-V2V Channel Modelling--------------//
  //------------------------------------------------------------//
  Config::SetDefault("ns3::LteEnbPhy::TxPower", DoubleValue(43.0));

  std::cout << "LTE Models...Set Fading...";
  // lteHelper->SetFadingModel("ns3::ThreeGPPV2vUrbanChannelModel");
  lteHelper->SetFadingModel("ns3::ThreeGppChannelModel");
  std::cout <<".....Set Spectrum Channel...";
  lteHelper->SetSpectrumChannelType("ns3::ThreeGppChannelModel");
  std::cout << "Set Path Loss...";
  ThreeGppV2vUrbanPropagationLossModel v2v = ThreeGppV2vUrbanPropagationLossModel();
  lteHelper->SetPathlossModelType(v2v.GetTypeId());

  // for(int i = 0; i < 18; i++)
  // {
  //   envDevs
  // }
  NetDeviceContainer enbDevs;
  for(int i = 0; i < 18; i += 3)
  {
    std::cout << std::endl << "Sectorizing nodes " << i << " - " << i+2 << "\t";
    lteHelper->SetEnbAntennaModelType ("ns3::CosineAntennaModel");
    lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (0));
    lteHelper->SetEnbAntennaModelAttribute ("HorizontalBeamwidth", DoubleValue (120));
    lteHelper->SetEnbAntennaModelAttribute ("MaxGain", DoubleValue (0.0));
    std::cout << "Install antenna " << i << "...";
    enbDevs.Add ( lteHelper->InstallEnbDevice (enbNodes.Get (i)));


    lteHelper->SetEnbAntennaModelType ("ns3::CosineAntennaModel");
    lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (360/3));
    lteHelper->SetEnbAntennaModelAttribute ("HorizontalBeamwidth", DoubleValue (120));
    lteHelper->SetEnbAntennaModelAttribute ("MaxGain", DoubleValue (0.0));
    std::cout << "....." << i+1 << "...";
    enbDevs.Add ( lteHelper->InstallEnbDevice (enbNodes.Get (i + 1)));


    lteHelper->SetEnbAntennaModelType ("ns3::CosineAntennaModel");
    lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (2*360/3));
    lteHelper->SetEnbAntennaModelAttribute ("HorizontalBeamwidth", DoubleValue (120));
    lteHelper->SetEnbAntennaModelAttribute ("MaxGain", DoubleValue (0.0));
    std::cout << "....." << i+2 << "...";
    enbDevs.Add ( lteHelper->InstallEnbDevice (enbNodes.Get (i + 2)));  
    std::cout << std::endl;
  }
  


  

  // Install the IP stack on the UEs
  std::cout << std::endl << "installing UE Nodes....." << std::endl;
  internet.Install (ueNodes); internet.Install (enbUeNodes);
  Ipv4InterfaceContainer ueIpIface;
  Ipv4InterfaceContainer enbUeIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueDevs));
  enbUeIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (enbUeDevs));
  // Assign IP address to UEs, and install applications
  std::cout << "UE DEVICES: ";
  for (uint32_t i = 0; i < 18; i++)
  {
    Ptr<NetDevice> dev = enbUeDevs.Get(i);
    std::cout << i <<"...";
  } std::cout <<".....Done!" << std::endl;
  std::cout << "ENB DEVICES: ";
  for (uint32_t i = 0; i < 18; i++)
  {
    Ptr<NetDevice> dev = enbDevs.Get(i);
    std::cout << i <<"...";
  } std::cout <<".....Done!" << std::endl;

  std::cout << "UE IPV4 ROUTING: ";
  for (uint32_t i = 0; i < 18; i++)
    {
      //Assign IP address to UEs, and install applications
      // Set the default gateway for the UE
      std::cout << i << "...";
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (enbUeNodes.Get (i)->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    } std::cout << ".....Done!" << std::endl << "ATTACH LOADING UEs: ";
for (uint32_t i = 0; i < 18; i++){
      //Attach each EnB-Loading UE to associated EnB
      Ptr<NetDevice> dev1, dev2;
      dev1 = enbUeDevs.Get(i); dev2 = enbDevs.Get(i);
      std::cout << i << "...";
      lteHelper->Attach (enbUeDevs.Get(i), enbDevs.Get(i));
    } std::cout << ".....Done!" << std::endl;

  // Attach one UE per eNodeB
  // std::cout<<"ATTACHING UE";
  // uint16_t minNodes = std::min(ueDevs.GetN(), enbDevs.GetN());
  // for (uint16_t i = 0; i < minNodes; i++)


  //TODO: Attach UAS-UE to nearest EnB
  uint32_t idxNearestEnb = getNearestEnB(ueNodes.Get(0), enbNodes);
  // uint32_t idxNearestEnb = 0;
  lteHelper->Attach(ueDevs.Get(0), enbDevs.Get(idxNearestEnb));

  std::cout << "Installing UDP Echo Client/Server" << std::endl;

  // Install and start applications on UEs and remote host
  uint16_t dlPort = 1100;
  uint16_t ulPort = 2000;
  uint16_t otherPort = 3000;
  ApplicationContainer clientApps;
  ApplicationContainer serverApps;



  //------------------------------------------------------------//
  //--------------------Install UDP Sockets---------------------//
  //------------------------------------------------------------//

  // Install UAS UDP Sockets
  PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
  serverApps.Add (dlPacketSinkHelper.Install (ueNodes.Get (0)));
  UdpEchoClientHelper dlClient (ueIpIface.GetAddress(0), dlPort);
  dlClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
  dlClient.SetAttribute ("Interval", TimeValue(interPacketInterval));
  dlClient.SetAttribute ("PacketSize", UintegerValue (1024));

  clientApps = dlClient.Install(ueNodes.Get(0));
  // ++ulPort;
  PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
  serverApps.Add (ulPacketSinkHelper.Install (remoteHost));

  UdpEchoClientHelper ulClient (remoteHostAddr, ulPort);
  ulClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
  ulClient.SetAttribute ("Interval", TimeValue(interPacketInterval));
  ulClient.SetAttribute ("PacketSize", UintegerValue (1024));

  clientApps = ulClient.Install(enbUeNodes.Get(0));


  // Set LTE Loading UDP Sockets
  for (uint32_t u = 0; u < enbUeNodes.GetN (); ++u)
    {
      if (!disableDl)
        {
          PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
          serverApps.Add (dlPacketSinkHelper.Install (enbUeNodes.Get (u)));
          // UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
          // dlClient.SetAttribute ("Interval", TimeValue (interPacketInterval));
          // dlClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
          // clientApps.Add (dlClient.Install (remoteHost));
          UdpEchoClientHelper dlClient (enbUeIpIface.GetAddress(u), dlPort);
          dlClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
          dlClient.SetAttribute ("Interval", TimeValue(interPacketInterval));
          dlClient.SetAttribute ("PacketSize", UintegerValue (1024));

          clientApps = dlClient.Install(remoteHost);
        }

      if (!disableUl)
        {
          ++ulPort;
          PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
          serverApps.Add (ulPacketSinkHelper.Install (remoteHost));

          // UdpClientHelper ulClient (remoteHostAddr, ulPort);
          // ulClient.SetAttribute ("Interval", TimeValue (interPacketInterval));
          // ulClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
          // clientApps.Add (ulClient.Install (ueNodes.Get(u)));
          UdpEchoClientHelper ulClient (remoteHostAddr, ulPort);
          ulClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
          ulClient.SetAttribute ("Interval", TimeValue(interPacketInterval));
          ulClient.SetAttribute ("PacketSize", UintegerValue (1024));

          clientApps = ulClient.Install(enbUeNodes.Get(u));
        // }
      // if (!disablePl && numMinNodes > 1)
      // if (!disablePl)
        // {
          ++otherPort;
          PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), otherPort));
          serverApps.Add (packetSinkHelper.Install (enbUeNodes.Get (u)));

          UdpClientHelper client (enbUeIpIface.GetAddress (u), otherPort);
          client.SetAttribute ("Interval", TimeValue (interPacketInterval));
          client.SetAttribute ("MaxPackets", UintegerValue (1000000));
          
          clientApps.Add (client.Install (enbUeNodes.Get ((u + 1) % numNodePairs)));
        }
    }



  /**************** FLOW MONITOR can track packet statistics at Layer 3 ********************/
    /**************** FLOW MONITOR SETUP ********************/
  Ptr<FlowMonitor> flowmon;
  FlowMonitorHelper flowmonHelper;
  if(tracing){
    flowmon = flowmonHelper.InstallAll();
    flowmon->CheckForLostPackets ();
  }

  serverApps.Start (MilliSeconds (500)); serverApps.Stop  (simTime);
  clientApps.Start (MilliSeconds (500)); clientApps.Stop  (simTime);

  
  //--------------------------------------//
  //----------SIMULATION LOGGING----------//
  //--------------------------------------//
  AsciiTraceHelper ascii;
  if(tracing)
  { 
    //touch logfile
    std::ofstream touchfile; 
    touchfile.open(callbackFile, std::ios_base::openmode::_S_trunc);
    if(!touchfile) {NS_LOG_ERROR("Cannot open " + callbackFile); exit(1);}
    touchfile << "[" << std::endl;
    touchfile.close();

    //enable automated traces    
    lteHelper->EnableTraces ();
    p2ph.EnablePcapAll(PREFIX + "lena-simple-epc", true); 
    p2ph.EnableAsciiAll(ascii.CreateFileStream(PREFIX + "uavsim.ascii"));


    // IPV4 / UDP Traces
    std::string udpClientPrefix = "/NodeList/*/ApplicationList/*/$ns3::UdpEchoClient/";
    std::string udpServerPrefix = "/NodeList/*/ApplicationList/*/$ns3::UdpEchoServer/";
    std::string udpSocketPrefix = "/NodeList/*/$ns3::UdpL4Protocol/SocketList/*/";
    // std::string ipPrefix = "/NodeList/*/$ns3::Ipv4L3Protocol/";

    Config::ConnectWithoutContext(udpClientPrefix + "Rx", MakeBoundCallback(&UdpRxTrace, callbackFile));
    Config::ConnectWithoutContext(udpClientPrefix + "Tx", MakeBoundCallback(&UdpTxTrace, callbackFile));
    Config::ConnectWithoutContext(udpSocketPrefix + "Drop", MakeBoundCallback(&udpDropTrace, callbackFile));
    // Config::ConnectWithoutContext(udpServerPrefix + "Rx", MakeCallback(&UdpRxTrace));
    // Config::Connect(ipPrefix + "Drop", MakeCallback(&IpDropTrace));
    // Config::Connect(ipPrefix + "Tx", MakeCallback(&ipTxTrace));
    
    
    // Config::Connect("/NodeList/*/ApplicationList/*/$ns3::UdpEchoClient/RxWithAddresses", MakeCallback(&RxTrace));

    //RLC/RRC Traces
    std::string ueRrcPrefix = "/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/LteUeRrc/";
    std::string enbRrcPrefix = "/NodeList/*/DeviceList/*/$ns3::LteEnbNetDevice/LteEnbRrc/" ;
    std::string lteRlcPrefix = "/NodeList/*/TxDrop/DeviceList/$nse::LteNetDevice/LteUeRrc/DataRadioBearerMap/*/LteRlc/";
    
    // Config::ConnectWithoutContext(lteRlcPrefix + "RxDrop", MakeCallback(&TxDropTrace));
    Config::ConnectWithoutContext(ueRrcPrefix + "HandoverStart", MakeBoundCallback(&HandoverStartTrace, callbackFile));
    Config::ConnectWithoutContext(ueRrcPrefix + "HandoverEndOk", MakeBoundCallback(&HandoverEndOkTrace, callbackFile));
    Config::ConnectWithoutContext(ueRrcPrefix + "HandoverEndError", MakeBoundCallback(&HandoverEndErrorTrace, callbackFile));
    // Config::ConnectWithoutContext(enbRrcPrefix + "ConnectionTimeout", MakeCallback(&RrcTimeoutTrace));
    Config::ConnectWithoutContext(ueRrcPrefix + "StateTransition", MakeBoundCallback(&StateTransitionTrace, callbackFile));
    Config::ConnectWithoutContext(ueRrcPrefix + "InitialCellSelectionEndError", MakeBoundCallback(&InitCellSelectErrTrace, callbackFile));
    Config::ConnectWithoutContext(ueRrcPrefix + "ConnectionTimeout", MakeBoundCallback(&ConnectionTimeoutTrace, callbackFile));


    // //MAC and PHY Traces
    std::string uePhyPrefix = "/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/ComponentCarrierMapUe/*/LteUePhy/";
    std::string lteSpectrumPrefixDl =  "/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/ComponentCarrierMapUe/*/LteUePhy/DlSpectrumPhy/" ;
    std::string lteSpectrumPrefixUl =  "/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/ComponentCarrierMapUe/*/LteUePhy/UlSpectrumPhy/" ;
    std::string lteMacPrefix = "/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/ComponentCarrierMapUe/*/LteUeMac/";

    Config::ConnectWithoutContext(lteMacPrefix + "RaResponseTimeout", MakeBoundCallback(&RaResponseTimeoutTrace, callbackFile));
    Config::ConnectWithoutContext(uePhyPrefix + "ReportUeMeasurements", MakeBoundCallback(&UeMeasTrace, callbackFile));
    Config::ConnectWithoutContext(uePhyPrefix + "ReportCurrentCellRsrpSinr", MakeBoundCallback(&CellRsrpSinrTrace, callbackFile));
    Config::ConnectWithoutContext(lteSpectrumPrefixDl + "RxEndError", MakeBoundCallback(&RxEndErrorTrace, callbackFile));
    Config::ConnectWithoutContext(lteSpectrumPrefixUl + "RxEndError", MakeBoundCallback(&RxEndErrorTrace, callbackFile));
    // Config::ConnectWithoutContext("dl", MakeCallback(&HandoverEndOkTrace));

    Config::ConnectWithoutContext("/NodeList/*/$ns3::MobilityModel/CourseChange", MakeBoundCallback(&UASCourseChange, callbackFile));
    }

  std::cout << "running.....";
  Simulator::Stop (simTime);
  Simulator::Run ();
  std::cout << ".....Done!" << std::endl;

  /*GtkConfigStore config;
  config.ConfigureAttributes();*/

  Simulator::Destroy ();

  if(tracing){
    flowmon->SerializeToXmlFile(PREFIX + "uavsim.flowmon", true, true);

    std::ofstream touchfile; std::fstream readfile;
    touchfile.open(callbackFile, std::ios_base::openmode::_S_app);
    if(!touchfile) {NS_LOG_ERROR("Cannot open " + callbackFile); exit(1);}
    touchfile << std::endl << "]";
    touchfile.close();
    }
  return 0;
}
