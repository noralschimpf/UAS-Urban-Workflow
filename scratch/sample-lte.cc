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
#include "ns3/csv-reader.h"
#include <vector>
#include <iostream>

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
      uasMob = CreateObject<ConstantPositionMobilityModel>();
      uasMob->SetPosition(Vector(10,10,buildingHeight));
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

    case 6:
      uasMob = CreateObject<RandomWalk2dMobilityModel>();
      uasMob->SetAttribute ("Bounds", RectangleValue (Rectangle (0, maxAxisX, 0, maxAxisY)));
      uasMob->SetAttribute ("Speed", StringValue ("ns3::ConstantRandomVariable[Constant=" + std::to_string(vScatt) + "]"));
      t_count = duration;
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


// void packetRead()
// {
// //Helper method to load csv packet information into vectors
//     float x = 0.0;
//     int x1 = 0;
//     //adjust path string to host computer.
//     CsvReader csv ("path to directory where file is/CSE-UDP1-Large.csv", ',');
//     while (csv.FetchNextRow()) {  
//         csv.GetValue(0, x); //read time value
//         m_arrivalVector.push_back(x);//store in vector V1
//         csv.GetValue(1, x1); //read packet size value
//         sizeVector.push_back(x1);//store in vector V3
//     }
//     csv_size = m_arrivalVector.size();// number of packets in file
//     m_scheduleVector.push_back(m_arrivalVector.at(0));
//     for (int i =1; i < csv_size; i++){
//        m_scheduleVector.push_back(m_arrivalVector.at(i) - m_arrivalVector.at(i-1)); //packet schedule time, store in vector m_scheduleVector
//     }
// }



void
PrintGnuplottablesToFile 
// (std::string dirname, ns3::Ptr<ListPositionAllocator> enbPositionAlloc, ns3::Ptr<ListPositionAllocator> uePositionAlloc)
(std::string dirname, NodeContainer enbNodes, NodeContainer ueNodeContainer)
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
  for (uint32_t i = 0; i< enbNodes.GetN(); i++)
  {
    ns3::Vector vec = enbNodes.Get(i)->GetObject<MobilityModel>()->GetPosition();
    outFile << i << ',' << vec.x << ',' << vec.y << ',' << vec.z << std::endl;
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
  for (uint32_t i = 0; i< ueNodeContainer.GetN(); i++)
  {
    Ptr<Node> nd = ueNodeContainer.Get(i);
    ns3::Vector vec = nd->GetObject<MobilityModel>()->GetPosition();
    outFile << i << ',' << vec.x << ',' << vec.y << ',' << vec.z << std::endl;
  }
  outFile.close();
}


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
  void packetRead(void);
  void ScheduleTx (void);
  void SendPacket (void);
  

  Ptr<Socket>           m_socket;
  Address               m_peer;
  uint32_t              m_packetSize;
  uint32_t              m_nPackets;
  DataRate              m_dataRate;
  EventId               m_sendEvent;
  bool                  m_running;
  uint32_t              m_packetsSent;
  std::vector<float>    m_arrivalVector;//store packet arrival time
  std::vector<float>    m_scheduleVector;//store packet schedule time, used to schedule packet send
  std::vector<uint32_t> sizeVector;//store packet size
  uint32_t              csv_size;//csv entry
  // const uint8_t*        m_filler;

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
  // m_filler = (const uint8_t*) FILLERSTRING.substr(0,m_packetSize).c_str();
  LoadCSV();//load CSV data from file
}

void
MyApp::StartApplication (void)
{
  //#ifdef DEBUG
  std::cout << "Start Application: " << std::endl;
  //#endif
  m_running = true;
  //m_packetsSent = 0;
  m_packetsSent = 1;//+++ new
  m_socket->Bind ();
  m_socket->Connect (m_peer);//initiate connection with remote peer using socket
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
//Helper method to change packe size 
void 
MyApp::ChangePacketSize(uint32_t packetSize) {
    m_packetSize = packetSize;
}

void MyApp::packetRead()
{
//Helper method to load csv packet information into vectors
    float x = 0.0;
    int x1 = 0;
    //adjust path string to host computer.
    // CsvReader csv ("path to directory where file is/CSE-UDP1-Large.csv", ',');
    CsvReader csv = CsvReader("~/src/ns-allinone-3.35/ns-3.35/build/scratch/CSE-UDP1-Large.csv", ',');
    while (csv.FetchNextRow()) {  
        csv.GetValue(0, x); //read time value
        m_arrivalVector.push_back(x);//store in vector V1
        csv.GetValue(1, x1); //read packet size value
        sizeVector.push_back(x1);//store in vector V3
    }
    csv_size = m_arrivalVector.size();// number of packets in file
    m_scheduleVector.push_back(m_arrivalVector.at(0));
    for (uint32_t i =1; i < csv_size; i++){
       m_scheduleVector.push_back(m_arrivalVector.at(i) - m_arrivalVector.at(i-1)); //packet schedule time, store in vector m_scheduleVector
    }
}


//Helper method to load csv packet information into vectors 
void
MyApp::LoadCSV(){//csv entry
    float x = 0.0;
    int x1 = 0;
    //adjust path string to host computer.
    // CsvReader csv ("/Users/rafaapaza/ns-allinone-3.35/ns-3.35/scratch/CSVRead/CSE-UDP1-Large.csv", ',');

    // CsvReader csv("\\\\wsl.localhost\\\\Ubuntu-20.04\\\\home\\\\schimpfen\\\\src\\\\ns-allinone-3.35\\\\ns-3.35\\\\scratch\\\\CSE-UDP1-LARGE.csv",',');
    CsvReader csv("/home/schimpfen/src/ns-allinone-3.35/ns-3.35/scratch/CSE-UDP1-LARGE.csv",',');
    while (csv.FetchNextRow()) {  
        csv.GetValue(0, x); //read time value
        m_arrivalVector.push_back(x);//store in vector V1
        csv.GetValue(1, x1); //read packet size value
        sizeVector.push_back(x1);//store in vector V3
    }
    csv_size = m_arrivalVector.size();// number of packets in file
    m_scheduleVector.push_back(m_arrivalVector.at(0));
    for (uint32_t i =1; i < csv_size; i++){
       m_scheduleVector.push_back(m_arrivalVector.at(i) - m_arrivalVector.at(i-1)); //packet schedule time, store in vector m_scheduleVector
    }
}

void
MyApp::SendPacket (void)
{
  SeqTsHeader seqTs;
  seqTs.SetSeq (m_packetsSent);
  m_packetSize = sizeVector.at(m_packetsSent);
  char *buffer = new char[m_packetSize];
  for(uint32_t i = 0; i < m_packetSize-1; i++){
    buffer[i] = FILLERSTRING[i];
  }
  buffer[m_packetSize-1] = '\0';
  // buffer = FILLERSTRING.substr(0,m_packetSize-1).c_str();
  Ptr<Packet> packet = Create<Packet> ((const uint8_t *)buffer, m_packetSize);
  m_socket->Send (packet);
  // this ->ChangePacketSize();//packet size update
  if (m_packetsSent < csv_size)
    {
      ScheduleTx ();
    }
  std::cout << "SendApp Time: " << Simulator::Now ().GetSeconds () << "\t" << "\t Sent Packet: " 
            << m_packetsSent << "\t Pkt Size: " << m_packetSize << "\t" << "to :" 
            << InetSocketAddress::ConvertFrom(m_peer).GetIpv4 () <<"\n";
}

bool variableBurst;
bool toggle2 = true;
int i_var = 0;//packet counter

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
        // if(i_var <= csv_size) { 
            Time tNext (Seconds (m_scheduleVector.at(m_packetsSent)));//schedule packet send
            m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
            m_packetsSent++;           
      // }
    }
}

int main (int argc, char *argv[])
{
  uint16_t uePerEnb = 1;
  uint16_t scenario = 0;
  Time simTime = MilliSeconds (2500);
  // Time appEndTime = MilliSeconds (1000);
  double distance = 60.0;
  Time interPacketInterval = MilliSeconds (100);
  bool useCa = false;
  bool disableDl = true;
  bool disableUl = false;
  bool disablePl = false;
  bool tracing = false;
  bool sectorized = true;
  double enbTxPower = 43.0; // typical eNB TX power, in dBm
  double ueTxPower = 24.0; // MAX TX POWER default, in dBm
  double uasTxPower = 24.0; // Separate TX power for UAS, in dBm, as UE
  std::string handoverMetric = "RSRQ";

  scenario = 3;
  // simTime = MilliSeconds (0);
  tracing = true;
  

  // Command line arguments
  CommandLine cmd (__FILE__);
  cmd.AddValue ("uePerEnb", "Number of Loading UEs attached to each eNodeB", uePerEnb);
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
  cmd.AddValue("sectorized", "Split Cells into 3 cell-sectors", sectorized);
  cmd.AddValue("enbTxPower", "Transmit Power of eNodeB", enbTxPower);
  cmd.AddValue("ueTxPower", "Transmit Power of UE", ueTxPower);
  cmd.AddValue("uasTxPower", "Transmit Power of UAS", uasTxPower);
  cmd.Parse (argc, argv);

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();


  // parse again so you can override default values from the command line
  cmd.Parse(argc, argv);

  std::string algo;
  for(std::size_t i = 0; i < handoverMetric.length(); i++){
    handoverMetric[i] = std::tolower(handoverMetric[i]);
  }
  if (handoverMetric == "rsrp"){algo = "ns3::A3RsrpHandoverAlgorithm";}
  else if (handoverMetric == "rsrq"){algo = "ns3::A2A4RsrqHandoverAlgorithm";}
  else if (handoverMetric == "none") {algo = "ns3::NoOpHandoverAlgorithm";}
  else{NS_LOG_ERROR("INVALID HANDOVER METRIC");}

  if (useCa)
   {
     Config::SetDefault ("ns3::LteHelper::UseCa", BooleanValue (useCa));
     Config::SetDefault ("ns3::LteHelper::NumberOfComponentCarriers", UintegerValue (2));
     Config::SetDefault ("ns3::LteHelper::EnbComponentCarrierManager", StringValue ("ns3::RrComponentCarrierManager"));
   }
  
  Config::SetDefault("ns3::LteEnbPhy::TxPower", DoubleValue(enbTxPower));
  Config::SetDefault("ns3::LteUePhy::TxPower", DoubleValue(ueTxPower));
  
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
  NodeContainer enbUeNodes;
  enbNodes.Create (18);
  enbUeNodes.Create(18*uePerEnb);
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
          // to prevent SpectrumPropagationLossModel error, nodes cannot be placed identically
          Vector vecModifierEnb = positions[i];
          Vector vecModifierUe = positions[i];
          double angle = 60 + j*120;
          double offsetEnb = 1.0;
          double offsetUe = 10.;
          vecModifierEnb.x += offsetEnb*std::cos(M_PI*angle/180.);
          vecModifierEnb.y += offsetEnb*std::sin(M_PI*angle/180.);

          uint32_t nodeNumMin = (j + i*3)*uePerEnb;
          uint32_t nodeNumMax = ((j + i*3)+1)*uePerEnb;
          createBSSMobility(enbNodes.Get(nodeNumMin / uePerEnb),vecModifierEnb);
          for (uint32_t k = nodeNumMin; k < nodeNumMax; k++){
            vecModifierUe.x += offsetUe*(k-nodeNumMin)*std::cos(M_PI*angle/180.);
            vecModifierUe.y += offsetUe*(k-nodeNumMin)*std::sin(M_PI*angle/180.);
            createBSSMobility(enbUeNodes.Get(k), vecModifierUe);
          }
      }
  }


  PrintGnuplottablesToFile ("scratchlogs/uavsim", enbNodes, enbUeNodes);

  //#ifdef DEBUG
  std::cout<< "Adding Mobility" << std::endl;
  //#endif
  for(uint32_t i=0; i<ueNodes.GetN(); i++){
    ns3::Time t_uas = createUASMobility(ueNodes.Get(i), scenario, simTime) + MilliSeconds(500);
    // if (t_uas > simTime) {simTime = t_uas;}
    simTime = t_uas;
  }

  //#ifdef DEBUG
  std::cout << "HANDOVER ALGORITH: " << algo << std::endl;
  //#endif
  lteHelper->SetHandoverAlgorithmType(algo);

  //------------------------------------------------------------//
  //-------------Install LTE-V2V Channel Modelling--------------//
  //------------------------------------------------------------//
  //#ifdef DEBUG
  std::cout << "LTE Models: ";
  std::cout << "Set Path Loss.....";
  //#endif
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::ThreeGppV2vUrbanPropagationLossModel"));
  lteHelper->SetPathlossModelAttribute("ChannelConditionModel", StringValue("ns3::ThreeGppUmaChannelConditionModel"));
  // ThreeGppV2vUrbanPropagationLossModelv2v = ThreeGppV2vUrbanPropagationLossModel();
  // lteHelper->SetPathlossModelType(v2v.GetTypeId());
  //#ifdef DEBUG
  std::cout <<"Set Spectrum Channel.....";
  //#endif
  //#ifdef DEBUG
  std::cout << "Set Fading.....";
  //#endif


  // Install EnB / UE Devices, include EnB Antenna Sectorization
  // Install LTE Devices to the nodes
  
  //#ifdef DEBUG
  std::cout << "NETDEVICE INSTALLS: ";
  

  std::cout << "EnB.....";
  //#endif
  NetDeviceContainer enbDevs;
  if (sectorized){
    for(int i = 0; i < 18; i += 3)
    {
      //#ifdef DEBUG
      std::cout << std::endl << "Sectorizing nodes " << i << " - " << i+2 << "\t";
      //#endif
      lteHelper->SetEnbAntennaModelType ("ns3::CosineAntennaModel");
      lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (0));
      lteHelper->SetEnbAntennaModelAttribute ("HorizontalBeamwidth", DoubleValue (120));
      lteHelper->SetEnbAntennaModelAttribute ("MaxGain", DoubleValue (0.0));
      //#ifdef DEBUG
      std::cout << "Install antenna " << i << "...";
      //#endif
      enbDevs.Add ( lteHelper->InstallEnbDevice (enbNodes.Get (i)));


      lteHelper->SetEnbAntennaModelType ("ns3::CosineAntennaModel");
      lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (360/3));
      lteHelper->SetEnbAntennaModelAttribute ("HorizontalBeamwidth", DoubleValue (120));
      lteHelper->SetEnbAntennaModelAttribute ("MaxGain", DoubleValue (0.0));
      //#ifdef DEBUG
      std::cout << "....." << i+1 << "...";
      //#endif
      enbDevs.Add ( lteHelper->InstallEnbDevice (enbNodes.Get (i + 1)));


      lteHelper->SetEnbAntennaModelType ("ns3::CosineAntennaModel");
      lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (2*360/3));
      lteHelper->SetEnbAntennaModelAttribute ("HorizontalBeamwidth", DoubleValue (120));
      lteHelper->SetEnbAntennaModelAttribute ("MaxGain", DoubleValue (0.0));
      //#ifdef DEBUG
      std::cout << "....." << i+2 << "...";
      //#endif
      enbDevs.Add ( lteHelper->InstallEnbDevice (enbNodes.Get (i + 2)));  
      //#ifdef DEBUG
      std::cout << std::endl;
      //#endif
    }
  }
  else {
    //#ifdef DEBUG
    std::cout << std::endl << "No Cell sectorization Specified" << std::endl;
    //#endif
    lteHelper->SetEnbAntennaModelType("ns3::IsotropicAntennaModel");
    lteHelper->InstallEnbDevice(enbNodes);
  }


  //#ifdef DEBUG
  std::cout << "ENB UE.....";
  //#endif
  NetDeviceContainer enbUeDevs = lteHelper->InstallUeDevice(enbUeNodes);
  // EpsBearer loadBearer = EpsBearer(EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
  // lteHelper->ActivateDataRadioBearer(enbUeDevs, loadBearer);
  //#ifdef DEBUG
  std::cout << "UAS UE.....";
  //#endif
  Config::SetDefault("ns3::LteUePhy::TxPower", DoubleValue(uasTxPower));
  NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);
  // EpsBearer uasBearer = EpsBearer(EpsBearer::GBR_MC_VIDEO);
  // lteHelper->ActivateDataRadioBearer(ueDevs, uasBearer);
  
  //#ifdef DEBUG
  std::cout << "DONE" << std::endl;
  //#endif


  

  // Install the IP stack on the UEs
  //#ifdef DEBUG
  std::cout << std::endl << "installing UE Nodes....." << std::endl;
  //#endif
  internet.Install (ueNodes); internet.Install (enbUeNodes);
  Ipv4InterfaceContainer ueIpIface;
  Ipv4InterfaceContainer enbUeIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueDevs));
  enbUeIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (enbUeDevs));
  
  //#ifdef DEBUG
  std::cout << "UE IPV4 ROUTING: ";
  //#endif
  for (uint32_t i = 0; i < 18; i++)
    {
      //Assign IP address to UEs, and install applications
      // Set the default gateway for the UE
      //#ifdef DEBUG
      std::cout << i << "...";
      //#endif
      Ptr<Ipv4StaticRouting> enbUeStaticRouting = ipv4RoutingHelper.GetStaticRouting (enbUeNodes.Get (i)->GetObject<Ipv4> ());
      enbUeStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    } 
    for (uint32_t i = 0; i < ueNodes.GetN(); i++){
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNodes.Get(i)->GetObject<Ipv4>());
      ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);
    }
    //#ifdef DEBUG
    std::cout << ".....Done!" << std::endl;
    //#endif
  
  //#ifdef DEBUG
  std::cout << "ATTACH LOADING UEs: ";
  //#endif
  for (uint32_t i = 0; i < enbUeDevs.GetN(); i++){
      //Attach each EnB-Loading UE to associated EnB
      lteHelper->Attach (enbUeDevs.Get(i), enbDevs.Get(i/uePerEnb));
      //#ifdef DEBUG
      std::cout << i << "...";
      //#endif
    } 
    //#ifdef DEBUG
    std::cout << ".....Done!" << std::endl;
    //#endif



  // Attach UAS-UE to nearest EnB
  lteHelper->AttachToClosestEnb(ueDevs.Get(0), enbDevs);

  //#ifdef DEBUG
  std::cout << "Installing UDP Echo Client/Server" << std::endl;
  //#endif

  // Install and start applications on UEs and remote host
  uint16_t dlPort = 1100;
  uint16_t ulPort = 2000;
  Address remoteSinkAddr (InetSocketAddress(remoteHostAddr, ulPort)); ulPort++;
  ApplicationContainer clientApps;
  ApplicationContainer serverApps;



  //------------------------------------------------------------//
  //--------------------Install UDP Sockets---------------------//
  //------------------------------------------------------------//

  // Install UAS UDP Sockets 

  //Establish remote host packet sink
  PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", remoteSinkAddr);
  // PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
  serverApps.Add (ulPacketSinkHelper.Install (remoteHost));

  // ++ulPort;
  // Address sinkAddress (InetSocketAddress (remoteHostAddr, ulPort));
  // PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
  // serverApps.Add (ulPacketSinkHelper.Install (remoteHost));
  
  //#ifdef DEBUG
  std::cout << "UAS Client...";
  // //#endif
  Ptr<Socket> ns3UdpSocket = Socket::CreateSocket (ueNodes.Get (0), UdpSocketFactory::GetTypeId ()); //Server at UE create socket
  Ptr<MyApp> app2 = CreateObject<MyApp> ();
  // app2->Setup (ns3UdpSocket, remoteSinkAddr, 1400, 2511, DataRate ("1Mbps"));//+++UE socket, peer address, pkt size 1400
  app2->Setup (ns3UdpSocket, remoteSinkAddr, 1400, 10000, DataRate ("1Mbps"));//+++UE socket, peer address, pkt size 1400
  ueNodes.Get (0)->AddApplication (app2);//+++connect app1 to ue: internetIpIfaces.GetAddress(0)
  app2->SetStartTime (MilliSeconds (10));
  app2->SetStopTime (simTime);
  ulPort++;
  



  // Set LTE Loading UDP Sockets
  for (uint32_t u = 0; u < enbUeNodes.GetN (); ++u)
    {

      if (!disableDl)
        {
          PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
          serverApps.Add (dlPacketSinkHelper.Install (enbUeNodes.Get (u)));

          UdpClientHelper dlClient (enbUeIpIface.GetAddress (u), dlPort);
          dlClient.SetAttribute ("Interval", TimeValue (interPacketInterval));
          dlClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
          clientApps.Add (dlClient.Install (remoteHost));
        }

      if (!disableUl)
        {          
          PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
          serverApps.Add (packetSinkHelper.Install (enbUeNodes.Get (u)));

          UdpClientHelper client (enbUeIpIface.GetAddress (u), ulPort);
          client.SetAttribute ("Interval", TimeValue (interPacketInterval));
          client.SetAttribute ("MaxPackets", UintegerValue (1000000));
          
          clientApps.Add (client.Install (enbUeNodes.Get (u)));
          ++ulPort;
        }
    }


  serverApps.Start (MilliSeconds (50)); serverApps.Stop  (simTime);
  clientApps.Start (MilliSeconds (50)); clientApps.Stop  (simTime);


  /**************** FLOW MONITOR can track packet statistics at Layer 3 ********************/
    /**************** FLOW MONITOR SETUP ********************/
  Ptr<FlowMonitor> flowmon;
  FlowMonitorHelper flowmonHelper;
  
  if (tracing) {
    flowmon = flowmonHelper.InstallAll();
    flowmon->CheckForLostPackets ();
  }
  

  
  //--------------------------------------//
  //----------SIMULATION LOGGING----------//
  //--------------------------------------//
  AsciiTraceHelper ascii;
  if(tracing)
  { 
    //#ifdef DEBUG
    std::cout << "Installing Traces" << std::endl;
    //#endif
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

    // Application layer Traces
    Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::PacketSink/Rx", MakeBoundCallback(&RcvPacket, callbackFile));
    // Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::UdpEchoClient/RxWithAddresses", MakeBoundCallback(&RcvPacketWAddr, callbackFile));
    // Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::UdpEchoServer/RxWithAddresses", MakeBoundCallback(&RcvPacketWAddr, callbackFile));
    Config::ConnectWithoutContext("/NodeList/*/$ns3::Ipv4L3Protocol/Rx", MakeBoundCallback(&ipRxTrace, callbackFile));
    // Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::PacketSocketServer/Rx", MakeBoundCallback(&RcvPacket, callbackFile));
    // IPV4 / UDP Traces
    // std::string udpClientPrefix = "/NodeList/*/ApplicationList/*/$ns3::UdpEchoClient/";
    // std::string udpServerPrefix = "/NodeList/*/ApplicationList/*/$ns3::UdpEchoServer/";
    // std::string udpSocketPrefix = "/NodeList/*/$ns3::UdpL4Protocol/SocketList/*/";
    // std::string ipPrefix = "/NodeList/*/$ns3::Ipv4L3Protocol/";

    // Config::ConnectWithoutContext(udpClientPrefix + "Rx", MakeBoundCallback(&UdpRxTrace, callbackFile));
    // Config::ConnectWithoutContext(udpClientPrefix + "Tx", MakeBoundCallback(&UdpTxTrace, callbackFile));
    // Config::ConnectWithoutContext(udpSocketPrefix + "Drop", MakeBoundCallback(&udpDropTrace, callbackFile));
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
    Config::ConnectWithoutContext(enbRrcPrefix + "HandoverStart", MakeBoundCallback(&HandoverStartTrace, callbackFile));
    Config::ConnectWithoutContext(enbRrcPrefix + "HandoverEndOk", MakeBoundCallback(&HandoverEndOkTrace, callbackFile));
    Config::ConnectWithoutContext(ueRrcPrefix + "HandoverEndError", MakeBoundCallback(&HandoverEndErrorTrace, callbackFile));
    // Config::ConnectWithoutContext(enbRrcPrefix + "ConnectionTimeout", MakeCallback(&RrcTimeoutTrace));
    Config::ConnectWithoutContext(ueRrcPrefix + "StateTransition", MakeBoundCallback(&StateTransitionTrace, callbackFile));
    Config::ConnectWithoutContext(ueRrcPrefix + "InitialCellSelectionEndError", MakeBoundCallback(&InitCellSelectErrTrace, callbackFile));
    Config::ConnectWithoutContext(ueRrcPrefix + "ConnectionTimeout", MakeBoundCallback(&ConnectionTimeoutTrace, callbackFile));


    // //MAC and PHY Traces
    uint32_t uasMin = ueNodes.Get(0)->GetId();
    uint32_t uasMax = ueNodes.Get(ueNodes.GetN()-1)->GetId();
    std::string uasNodeStr = std::to_string(uasMin) + "-" + std::to_string(uasMax);
    std::string uasPhyPrefix ="/NodeList/" + uasNodeStr + "/DeviceList/" + "*" + "/$ns3::LteUeNetDevice/ComponentCarrierMapUe/*/LteUePhy/"; 
    std::string uePhyPrefix = "/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/ComponentCarrierMapUe/*/LteUePhy/";
    std::string lteSpectrumPrefixDl =  "/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/ComponentCarrierMapUe/*/LteUePhy/DlSpectrumPhy/" ;
    std::string lteSpectrumPrefixUl =  "/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/ComponentCarrierMapUe/*/LteUePhy/UlSpectrumPhy/" ;
    std::string lteMacPrefix = "/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/ComponentCarrierMapUe/*/LteUeMac/";

    Config::ConnectWithoutContext(lteMacPrefix + "RaResponseTimeout", MakeBoundCallback(&RaResponseTimeoutTrace, callbackFile));
    Config::ConnectWithoutContext(uasPhyPrefix + "ReportUeMeasurements", MakeBoundCallback(&UeMeasTrace, callbackFile));
    Config::ConnectWithoutContext(uasPhyPrefix + "ReportCurrentCellRsrpSinr", MakeBoundCallback(&CellRsrpSinrTrace, callbackFile));
    Config::ConnectWithoutContext(lteSpectrumPrefixDl + "RxEndError", MakeBoundCallback(&RxEndErrorTrace, callbackFile));
    Config::ConnectWithoutContext(lteSpectrumPrefixUl + "RxEndError", MakeBoundCallback(&RxEndErrorTrace, callbackFile));
    // Config::ConnectWithoutContext("dl", MakeCallback(&HandoverEndOkTrace));

    Config::ConnectWithoutContext("/NodeList/*/$ns3::MobilityModel/CourseChange", MakeBoundCallback(&UASCourseChange, callbackFile));
    //#ifdef DEBUG
    std::cout << "Done!" << std::endl;
    //#endif
    }
  
  //#ifdef DEBUG
  std::cout << "UAS ADDR: " << ueNodes.Get(0)->GetId() << std::endl;
  for(uint32_t i = 0; i < ueNodes.GetN(); i++){
    Ptr<Ipv4> uasIp = ueNodes.Get(i)->GetObject<Ipv4>();
    std::cout << ueNodes.Get(i)->GetId() << ": " << std::endl;
    for (uint32_t j = 0; j < uasIp->GetNInterfaces(); j++){
      std::cout << "\t" << uasIp->GetAddress(j,0) << "," << std::endl;
    }
  }
  
  std::cout << "ENB ADDRs: " << std::endl;
  for(uint32_t i = 0; i < enbNodes.GetN(); i++)
  {
    Ptr<Ipv4> ipObj = enbNodes.Get(i)->GetObject<Ipv4>();
    std::cout << enbNodes.Get(i)->GetId() << ": " << std::endl;
    for (uint32_t j = 0; j < ipObj->GetNInterfaces(); j++){
    std::cout << "\t" << ipObj->GetAddress(j,0) << ", " << std::endl;
    }
  } std::cout << std::endl << "ENB UE ADDRs: " << std::endl;
  
  for(uint32_t i = 0; i < enbUeNodes.GetN(); i++)
  {
    std::cout << enbUeNodes.Get(i)->GetId() << std::endl;
    Ptr<Ipv4> ipObj = enbUeNodes.Get(i)->GetObject<Ipv4>();
    for (uint32_t j = 0; j < ipObj->GetNInterfaces(); j++){
      std::cout << "\t" << ipObj->GetAddress(j,0) << ", " << std::endl;
    }
    
  } std::cout << std::endl;



  std::cout << "running (" << simTime.As(Time::Unit::MS) << " s)";
  //#endif 

  Simulator::Stop (simTime);
  Simulator::Run ();
  //#ifdef DEBUG
  std::cout << ".....Done!" << std::endl;
  //#endif

  /*GtkConfigStore config;
  config.ConfigureAttributes();*/
  Simulator::Destroy ();

  

  if(tracing){
    flowmon->SerializeToXmlFile(PREFIX + "uavsim.flowmon", true, true);

    std::ofstream touchfile; std::fstream readfile;
    touchfile.open(callbackFile, std::ios_base::openmode::_S_app);
    if(!touchfile) {
      std::cout << "Cannot open " << callbackFile;
     exit(1);}
    touchfile << std::endl << "]";
    touchfile.close();
    }
  return 0;
}
