/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2020, University of Padova, Dep. of Information Engineering, SIGNET lab
 *
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
 *
 */

/**
* This is an example on how to configure the channel model classes to simulate
* a vehicular environment.
* The channel condition is determined using the model specified in [1], Table 6.2-1.
* The pathloss is determined using the model specified in [1], Table 6.2.1-1.
* The model for the fast fading is the one described in 3GPP TR 38.901 v15.0.0,
* the model parameters are those specified in [1], Table 6.2.3-1.
*
* This example generates the output file 'example-output.txt'. Each row of the
* file is organized as follows:
* Time[s] TxPosX[m] TxPosY[m] RxPosX[m] RxPosY[m] ChannelState SNR[dB] Pathloss[dB]
* We also provide a bash script which reads the output file and generates two
* figures:
* (i) map.gif, a GIF representing the simulation scenario and vehicle mobility;
* (ii) snr.png, which represents the behavior of the SNR.
*
* [1] 3GPP TR 37.885, v15.3.0
*/

#include "ns3/buildings-module.h"
#include "ns3/mobility-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include <fstream>
#include "ns3/uniform-planar-array.h"
#include "ns3/lte-module.h"
#include "ns3/config-store.h"
#include <ns3/buildings-propagation-loss-model.h>
#include <ns3/buildings-helper.h>
#include <ns3/radio-environment-map-helper.h>
#include <iomanip>
#include <string>
#include <vector>
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("UAVSim");


int
main (int argc, char *argv[])
{
  //Set up helpers
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  PointToPointHelper p2ph;
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);
  MobilityHelper mobility;
  Ipv4AddressHelper ipv4h;

  Ptr<Node> pgw = epcHelper->GetPgwNode ();

  // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

   // Create the Internet
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (10)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  // interface 0 is localhost, 1 is the p2p device
//  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  // create lte nodes
  NodeContainer enbUeNodes;
  NodeContainer uavUeNode;
  NodeContainer enbNodes;

  // the following node configuration corresponds to a 8 sq. km (2 km x 4 km)
  // rather than the initially specified 16 sq. km

  enbNodes.Create(18); // 6 stations with 3 nodes apiece
  enbUeNodes.Create (18);  // one UE attached per enbUeNode for traffic sim
  uavUeNode.Create(1);  // single UAV UE node

  // create a grid of buildings
  double buildingSizeX = 250 - 3.5 * 2 - 3; // m
  double buildingSizeY = 433 - 3.5 * 2 - 3; // m
  double streetWidth = 20; // m
  double buildingHeight = 10; // m
  double maxAxisX = 4000;
  double maxAxisY = 2000;
  uint32_t numBuildingsX = maxAxisX / (buildingSizeX + streetWidth);
  uint32_t numBuildingsY = maxAxisY / (buildingSizeX + streetWidth);

  std::vector<Ptr<Building> > buildingVector;
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

  std::vector<Vector> positions{Vector(0,0,30),Vector(1600,0,30),Vector(3200,0,30),Vector(800,1380,30),Vector(2400,1380,30),Vector(4000,1380,30)};

  Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator> ();
  Ptr<ListPositionAllocator> enbUePositionAlloc = CreateObject<ListPositionAllocator> ();



  for(int i = 0; i < 6; i++)
  {
      for(int j = 0; j < 3; j++)
      {
            enbPositionAlloc -> Add(positions[i]);
      }

      enbUePositionAlloc -> Add(positions[i]);
  }

  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

  mobility.SetPositionAllocator(enbPositionAlloc);
  mobility.Install(enbNodes);

  BuildingsHelper::Install (enbNodes);


  mobility.SetPositionAllocator(enbUePositionAlloc);
  mobility.Install(enbUeNodes);

}
