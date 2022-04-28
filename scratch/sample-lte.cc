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

// NS_LOG_COMPONENT_DEFINE ("FinalProject");

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


void
PrintGnuplottableBuildingListToFile 
(std::string dirname, ns3::Ptr<ListPositionAllocator> enbPositionAlloc, ns3::Ptr<ListPositionAllocator> uePositionAlloc)
{

  //-----Log Building Positions-----//
  std::ofstream outFile;
  std::string filename = dirname + "/buildings.txt";
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_LOG_ERROR ("Can't open file " << filename);
      return;
    }
  
  outFile << "building,rectFromX,rectFromY,rectToX,rectToY"<<std::endl;

  uint32_t index = 0;
  for (BuildingList::Iterator it = BuildingList::Begin (); it != BuildingList::End (); ++it)
    {
      ++index;
      Box box = (*it)->GetBoundaries ();
        outFile << index << "," << box.xMin  << "," << box.yMin
              << ","   << box.xMax  << "," << box.yMax
              << std::endl;
    }
  outFile.close();



  //-----Log enb positions-----//
  filename = dirname + "/enbs.txt";
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_LOG_ERROR ("Can't open file " << filename);
      return;
    }
  
  outFile << "enb,X,Y,Z"<<std::endl;
  for (uint32_t i = 0; i< enbPositionAlloc->GetSize(); i++)
  {
    {
    ns3::Vector vec = enbPositionAlloc->GetNext();
    outFile << i << ',' << vec.x << ',' << vec.y << ',' << vec.z << std::endl;
    }
  }
  outFile.close();

  //-----Log ue starting positions-----//
  filename = dirname + "/ues.txt";
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_LOG_ERROR ("Can't open file " << filename);
      return;
    }
  
  outFile << "ue,X,Y,Z"<<std::endl;
  for (uint32_t i = 0; i< uePositionAlloc->GetSize(); i++)
  {
    {
    ns3::Vector vec = enbPositionAlloc->GetNext();
    outFile << i << ',' << vec.x << ',' << vec.y << ',' << vec.z << std::endl;
    }
  }
  outFile.close();
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
  remoteHostStaticRouting->TraceConnectWithoutContext("Drop", MakeBoundCallback(&IpDropTrace, callbackFile));
  remoteHostStaticRouting->TraceConnectWithoutContext("Tx", MakeBoundCallback(&ipTxTrace, callbackFile));
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  NodeContainer ueNodes;
  NodeContainer enbNodes;
  enbNodes.Create (numNodePairs);
  ueNodes.Create (numNodePairs);

  std::cout<< "Adding Mobility" << std::endl;

  // Set mobility, channel, propagation models
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
  

  //------------------------------------------------------------//
  //-------------Install LTE-V2V Channel Modelling--------------//
  //------------------------------------------------------------//
    std::cout << "LTE Models...Set Fading...";
  // lteHelper->SetFadingModel("ns3::ThreeGPPV2vUrbanChannelModel");
  lteHelper->SetFadingModel("ns3::ThreeGppChannelModel");
  
  std::cout << ".....Set Path Loss...";
  ThreeGppV2vUrbanPropagationLossModel v2v = ThreeGppV2vUrbanPropagationLossModel();
  lteHelper->SetPathlossModelType(v2v.GetTypeId());
  
  std::cout <<".....Set Spectrum Channel...";
  lteHelper->SetSpectrumChannelType("ns3::ThreeGppChannelModel");

  std::cout << ".....Done!" << std::endl;

  

  

  // Install the IP stack on the UEs
  std::cout << "installing UE Nodes" << std::endl;
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
  std::cout<<"ATTACHING UE";
  // uint16_t minNodes = std::min(ueLteDevs.GetN(), enbLteDevs.GetN());
  // for (uint16_t i = 0; i < minNodes; i++)
  for (uint16_t i = 0; i < numNodePairs; i++)
    {
      lteHelper->Attach (ueLteDevs.Get(i), enbLteDevs.Get(i));
      // side effect: the default EPS bearer will be activated
    }


  std::cout << "Installing UDP Echo Client/Server" << std::endl;

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

          // UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
          // dlClient.SetAttribute ("Interval", TimeValue (interPacketInterval));
          // dlClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
          // clientApps.Add (dlClient.Install (remoteHost));
          UdpEchoClientHelper dlClient (ueIpIface.GetAddress(u), dlPort);
          dlClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
          dlClient.SetAttribute ("Interval", TimeValue(interPacketInterval));
          dlClient.SetAttribute ("PacketSize", UintegerValue (1024));

          ApplicationContainer clientApps = dlClient.Install(remoteHost);
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

          ApplicationContainer clientApps = ulClient.Install(ueNodes.Get(u));
        }
      // if (!disablePl && numMinNodes > 1)
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
    }

  std::cout << "running.....";
  Simulator::Stop (simTime);
  Simulator::Run ();
  std::cout << "\t Done!" << std::endl;

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
