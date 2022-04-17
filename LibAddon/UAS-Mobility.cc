#include "ns3/buildings-module.h"
#include "ns3/mobility-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include <fstream>
#include "ns3/uniform-planar-array.h"
#include "ns3/three-gpp-spectrum-propagation-loss-model.h"
#include "ns3/three-gpp-v2v-propagation-loss-model.h"
#include "ns3/three-gpp-channel-model.h"

#include "UAS-Mobility.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("UASMobility");

// static Ptr<ChannelConditionModel> m_condModel;
// static Ptr<ThreeGppPropagationLossModel> m_propagationLossModel; //!< the PropagationLossModel object

void createUASMobility(ns3::Ptr<ns3::Node> node, int scenario, ns3::Time duration)
{
  Ptr<MobilityModel> uasMob;
  // set the mobility model
  double vTx = vScatt;
  Time t_count = Seconds (0.0);

  if(scenario==0)
    { 
    uasMob = CreateObject<RandomWalk2dMobilityModel>();
    uasMob->SetAttribute ("Bounds", RectangleValue (Rectangle (-5, 5, -5, 5)));
    uasMob->SetAttribute ("Speed", StringValue ("ns3::ConstantRandomVariable[Constant=0.1]"));
    }
  else
    {
    uasMob = CreateObject<WaypointMobilityModel> ();  
    }

  switch(scenario)
    {
    case 1:
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, Vector (maxAxisX / 2 - streetWidth / 2, 1.0, 1.5)));
      t_count += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      uasMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (t_count, Vector (maxAxisX / 2 - streetWidth / 2, maxAxisY / 2 - streetWidth / 2, 1.5)));
      t_count += Seconds ((maxAxisX - streetWidth) / 2 / vTx);    
      break;

    case 2:
      break;

    case 3:
      break;

    case 4:
      break;

    case 5:
      break;

    default:
      break;

    }


  node->AggregateObject (uasMob);
  // return uasMob;
}


void createBSSMobility(ns3::Ptr<ns3::Node> node, ns3::Vector v)
{
  Ptr<MobilityModel> bssMob;  
  // set the mobility model
  bssMob = CreateObject<ConstantPositionMobilityModel>();
  bssMob->SetPosition(v);
  node->AggregateObject(bssMob);
}

// static void createUrbanEnvironment()
// {
//       std::vector<Ptr<Building> > buildingVector;
//   for (uint32_t buildingIdX = 0; buildingIdX < numBuildingsX; ++buildingIdX)
//     {
//       for (uint32_t buildingIdY = 0; buildingIdY < numBuildingsY; ++buildingIdY)
//         {
//           Ptr < Building > building;
//           building = CreateObject<Building> ();

//           building->SetBoundaries (Box (buildingIdX * (buildingSizeX + streetWidth),
//                                         buildingIdX * (buildingSizeX + streetWidth) + buildingSizeX,
//                                         buildingIdY * (buildingSizeY + streetWidth),
//                                         buildingIdY * (buildingSizeY + streetWidth) + buildingSizeY,
//                                         0.0, buildingHeight));
//           building->SetNRoomsX (1);
//           building->SetNRoomsY (1);
//           building->SetNFloors (1);
//           buildingVector.push_back (building);
//         }
//     }
// }


int main()
{
  // Evaluate model
  ns3::Ptr<MobilityModel> a;
  createUASMobility(a, 1, Seconds(60.));
  createBSSMobility(a, Vector(1.,0.,0.));
  // createUrbanEnvironment();
  return 0;
}