
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


int
main (int argc, char *argv[])
{
  uint16_t numNodePairs = 2;
  Time simTime = MilliSeconds (1100);
  double distance = 60.0;
  Time interPacketInterval = MilliSeconds (100);
  bool useCa = false;
  bool disableDl = false;
  bool disableUl = false;
  bool disablePl = false;

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
  //p2ph.EnablePcapAll("lena-simple-epc");

  Simulator::Stop (simTime);
  Simulator::Run ();

  /*GtkConfigStore config;
  config.ConfigureAttributes();*/

  Simulator::Destroy ();
  return 0;
}
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
#include "ns3/three-gpp-spectrum-propagation-loss-model.h"
#include "ns3/three-gpp-v2v-propagation-loss-model.h"
#include "ns3/three-gpp-channel-model.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ThreeGppV2vChannelExample");

static Ptr<ThreeGppPropagationLossModel> m_propagationLossModel; //!< the PropagationLossModel object
static Ptr<ThreeGppSpectrumPropagationLossModel> m_spectrumLossModel; //!< the SpectrumPropagationLossModel object
static Ptr<ChannelConditionModel> m_condModel; //!< the ChannelConditionModel object

/**
 * Perform the beamforming using the DFT beamforming method
 * \param thisDevice the device performing the beamforming
 * \param thisAntenna the antenna object associated to thisDevice
 * \param otherDevice the device towards which point the beam
 */
static void
DoBeamforming (Ptr<NetDevice> thisDevice, Ptr<PhasedArrayModel> thisAntenna, Ptr<NetDevice> otherDevice)
{
  PhasedArrayModel::ComplexVector antennaWeights;

  // retrieve the position of the two devices
  Vector aPos = thisDevice->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();
  Vector bPos = otherDevice->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();

  // compute the azimuth and the elevation angles
  Angles completeAngle (bPos,aPos);

  PhasedArrayModel::ComplexVector bf = thisAntenna->GetBeamformingVector (completeAngle);
  thisAntenna->SetBeamformingVector (bf);
}

/**
 * Compute the average SNR
 * \param txMob the tx mobility model
 * \param rxMob the rx mobility model
 * \param txPsd the PSD of the transmitting signal
 * \param noiseFigure the noise figure in dB
 */
static void
ComputeSnr (Ptr<MobilityModel> txMob, Ptr<MobilityModel> rxMob, Ptr<const SpectrumValue> txPsd, double noiseFigure)
{
  Ptr<SpectrumValue> rxPsd = txPsd->Copy ();

  // check the channel condition
  Ptr<ChannelCondition> cond = m_condModel->GetChannelCondition (txMob, rxMob);

  // apply the pathloss
  double propagationGainDb = m_propagationLossModel->CalcRxPower (0, txMob, rxMob);
  NS_LOG_DEBUG ("Pathloss " << -propagationGainDb << " dB");
  double propagationGainLinear = std::pow (10.0, (propagationGainDb) / 10.0);
  *(rxPsd) *= propagationGainLinear;

  // apply the fast fading and the beamforming gain
  rxPsd = m_spectrumLossModel->CalcRxPowerSpectralDensity (rxPsd, txMob, rxMob);
  NS_LOG_DEBUG ("Average rx power " << 10 * log10 (Sum (*rxPsd) * 180e3) << " dB");

  // create the noise psd
  // taken from lte-spectrum-value-helper
  const double kT_dBm_Hz = -174.0; // dBm/Hz
  double kT_W_Hz = std::pow (10.0, (kT_dBm_Hz - 30) / 10.0);
  double noiseFigureLinear = std::pow (10.0, noiseFigure / 10.0);
  double noisePowerSpectralDensity =  kT_W_Hz * noiseFigureLinear;
  Ptr<SpectrumValue> noisePsd = Create <SpectrumValue> (txPsd->GetSpectrumModel ());
  (*noisePsd) = noisePowerSpectralDensity;

  // compute the SNR
  NS_LOG_DEBUG ("Average SNR " << 10 * log10 (Sum (*rxPsd) / Sum (*noisePsd)) << " dB");

  // print the SNR and pathloss values in the snr-trace.txt file
  std::ofstream f;
  f.open ("example-output.txt", std::ios::out | std::ios::app);
  f << Simulator::Now ().GetSeconds () << " " // time [s]
    << txMob->GetPosition ().x << " "
    << txMob->GetPosition ().y << " "
    << rxMob->GetPosition ().x << " "
    << rxMob->GetPosition ().y << " "
    << cond->GetLosCondition () << " " // channel state
    << 10 * log10 (Sum (*rxPsd) / Sum (*noisePsd)) << " " // SNR [dB]
    << -propagationGainDb << std::endl; // pathloss [dB]
  f.close ();
}

/**
 * Generates a GNU-plottable file representig the buildings deployed in the
 * scenario
 * \param filename the name of the output file
 */
void
PrintGnuplottableBuildingListToFile (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_LOG_ERROR ("Can't open file " << filename);
      return;
    }
  uint32_t index = 0;
  for (BuildingList::Iterator it = BuildingList::Begin (); it != BuildingList::End (); ++it)
    {
      ++index;
      Box box = (*it)->GetBoundaries ();
      outFile << "set object " << index
              << " rect from " << box.xMin  << "," << box.yMin
              << " to "   << box.xMax  << "," << box.yMax
              << std::endl;
    }
}

int
main (int argc, char *argv[])
{
  double frequency = 28.0e9; // operating frequency in Hz
  double txPow_dbm = 30.0; // tx power in dBm
  double noiseFigure = 9.0; // noise figure in dB
  Time simTime = Seconds (40); // simulation time
  Time timeRes = MilliSeconds (10); // time resolution
  std::string scenario = "V2V-Urban"; // 3GPP propagation scenario, V2V-Urban or V2V-Highway
  double vScatt = 0; // maximum speed of the vehicles in the scenario [m/s]
  double subCarrierSpacing = 60e3; // subcarrier spacing in kHz
  uint32_t numRb = 275; // number of resource blocks

  CommandLine cmd (__FILE__);
  cmd.AddValue ("frequency", "operating frequency in Hz", frequency);
  cmd.AddValue ("txPow", "tx power in dBm", txPow_dbm);
  cmd.AddValue ("noiseFigure", "noise figure in dB", noiseFigure);
  cmd.AddValue ("scenario", "3GPP propagation scenario, V2V-Urban or V2V-Highway", scenario);
  cmd.Parse (argc, argv);

  // create the nodes
  NodeContainer nodes;
  nodes.Create (2);

  // create the tx and rx devices
  Ptr<SimpleNetDevice> txDev = CreateObject<SimpleNetDevice> ();
  Ptr<SimpleNetDevice> rxDev = CreateObject<SimpleNetDevice> ();

  // associate the nodes and the devices
  nodes.Get (0)->AddDevice (txDev);
  txDev->SetNode (nodes.Get (0));
  nodes.Get (1)->AddDevice (rxDev);
  rxDev->SetNode (nodes.Get (1));

  // create the antenna objects and set their dimensions
  Ptr<PhasedArrayModel> txAntenna = CreateObjectWithAttributes<UniformPlanarArray> ("NumColumns", UintegerValue (2), "NumRows", UintegerValue (2), "BearingAngle", DoubleValue (-M_PI / 2));
  Ptr<PhasedArrayModel> rxAntenna = CreateObjectWithAttributes<UniformPlanarArray> ("NumColumns", UintegerValue (2), "NumRows", UintegerValue (2), "BearingAngle", DoubleValue (M_PI / 2));

  Ptr<MobilityModel> txMob;
  Ptr<MobilityModel> rxMob;
  if (scenario == "V2V-Urban")
    {
      // 3GPP defines that the maximum speed in urban scenario is 60 km/h
      vScatt = 60 / 3.6;

      // create a grid of buildings
      double buildingSizeX = 250 - 3.5 * 2 - 3; // m
      double buildingSizeY = 433 - 3.5 * 2 - 3; // m
      double streetWidth = 20; // m
      double buildingHeight = 10; // m
      uint32_t numBuildingsX = 2;
      uint32_t numBuildingsY = 2;
      double maxAxisX = (buildingSizeX + streetWidth) * numBuildingsX;
      double maxAxisY = (buildingSizeY + streetWidth) * numBuildingsY;

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

      // set the mobility model
      double vTx = vScatt;
      double vRx = vScatt / 2;
      txMob = CreateObject<WaypointMobilityModel> ();
      rxMob = CreateObject<WaypointMobilityModel> ();
      Time nextWaypoint = Seconds (0.0);
      txMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (nextWaypoint, Vector (maxAxisX / 2 - streetWidth / 2, 1.0, 1.5)));
      nextWaypoint += Seconds ((maxAxisY - streetWidth) / 2 / vTx);
      txMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (nextWaypoint, Vector (maxAxisX / 2 - streetWidth / 2, maxAxisY / 2 - streetWidth / 2, 1.5)));
      nextWaypoint += Seconds ((maxAxisX - streetWidth) / 2 / vTx);
      txMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (nextWaypoint, Vector (0.0, maxAxisY / 2 - streetWidth / 2, 1.5)));
      nextWaypoint = Seconds (0.0);
      rxMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (nextWaypoint, Vector (maxAxisX / 2 - streetWidth / 2, 0.0, 1.5)));
      nextWaypoint += Seconds (maxAxisY / vRx);
      rxMob->GetObject<WaypointMobilityModel> ()->AddWaypoint (Waypoint (nextWaypoint, Vector (maxAxisX / 2 - streetWidth / 2, maxAxisY, 1.5)));

      nodes.Get (0)->AggregateObject (txMob);
      nodes.Get (1)->AggregateObject (rxMob);

      // create the channel condition model
      m_condModel = CreateObject<ThreeGppV2vUrbanChannelConditionModel> ();

      // create the propagation loss model
      m_propagationLossModel = CreateObject<ThreeGppV2vUrbanPropagationLossModel> ();
    }
  else if (scenario == "V2V-Highway")
    {
      // Two vehicles are travelling one behid the other with constant velocity
      // along the y axis. The distance between the two vehicles is 20 meters.

      // 3GPP defines that the maximum speed in urban scenario is 140 km/h
      vScatt = 140 / 3.6;
      double vTx = vScatt;
      double vRx = vScatt / 2;

      txMob = CreateObject<ConstantVelocityMobilityModel> ();
      rxMob = CreateObject<ConstantVelocityMobilityModel> ();
      txMob->GetObject<ConstantVelocityMobilityModel> ()->SetPosition (Vector (300.0, 20.0, 1.5));
      txMob->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (0.0, vTx, 0.0));
      rxMob->GetObject<ConstantVelocityMobilityModel> ()->SetPosition (Vector (300.0, 0.0, 1.5));
      rxMob->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (0.0, vRx, 0.0));

      nodes.Get (0)->AggregateObject (txMob);
      nodes.Get (1)->AggregateObject (rxMob);

      // create the channel condition model
      m_condModel = CreateObject<ThreeGppV2vHighwayChannelConditionModel> ();

      // create the propagation loss model
      m_propagationLossModel = CreateObject<ThreeGppV2vHighwayPropagationLossModel> ();
    }
  else
    {
      NS_FATAL_ERROR ("Unknown scenario");
    }

  m_condModel->SetAttribute ("UpdatePeriod", TimeValue (MilliSeconds (100)));

  m_propagationLossModel->SetAttribute ("Frequency", DoubleValue (frequency));
  m_propagationLossModel->SetAttribute ("ShadowingEnabled", BooleanValue (false));
  m_propagationLossModel->SetAttribute ("ChannelConditionModel", PointerValue (m_condModel));

  // create the channel model
  Ptr<ThreeGppChannelModel> channelModel = CreateObject<ThreeGppChannelModel> ();
  channelModel->SetAttribute ("Scenario", StringValue (scenario));
  channelModel->SetAttribute ("Frequency", DoubleValue (frequency));
  channelModel->SetAttribute ("ChannelConditionModel", PointerValue (m_condModel));

  // create the spectrum propagation loss model
  m_spectrumLossModel = CreateObjectWithAttributes<ThreeGppSpectrumPropagationLossModel> ("ChannelModel", PointerValue (channelModel));
  m_spectrumLossModel->SetAttribute ("vScatt", DoubleValue (vScatt));

  // initialize the devices in the ThreeGppSpectrumPropagationLossModel
  m_spectrumLossModel->AddDevice (txDev, txAntenna);
  m_spectrumLossModel->AddDevice (rxDev, rxAntenna);

  BuildingsHelper::Install (nodes);

  // set the beamforming vectors
  DoBeamforming (txDev, txAntenna, rxDev);
  DoBeamforming (rxDev, rxAntenna, txDev);

  // create the tx power spectral density
  Bands rbs;
  double freqSubBand = frequency;
  for (uint16_t n = 0; n < numRb; ++n)
    {
      BandInfo rb;
      rb.fl = freqSubBand;
      freqSubBand += subCarrierSpacing / 2;
      rb.fc = freqSubBand;
      freqSubBand += subCarrierSpacing / 2;
      rb.fh = freqSubBand;
      rbs.push_back (rb);
    }
  Ptr<SpectrumModel> spectrumModel = Create<SpectrumModel> (rbs);
  Ptr<SpectrumValue> txPsd = Create <SpectrumValue> (spectrumModel);
  double txPow_w = std::pow (10., (txPow_dbm - 30) / 10);
  double txPowDens = (txPow_w / (numRb * subCarrierSpacing));
  (*txPsd) = txPowDens;

  for (int i = 0; i < simTime / timeRes; i++)
    {
      Simulator::Schedule (timeRes * i, &ComputeSnr, txMob, rxMob, txPsd, noiseFigure);
    }

  // initialize the output file
  std::ofstream f;
  f.open ("example-output.txt", std::ios::out);
  f << "Time[s] TxPosX[m] TxPosY[m] RxPosX[m] RxPosY[m] ChannelState SNR[dB] Pathloss[dB]" << std::endl;
  f.close ();

  // print the list of buildings to file
  PrintGnuplottableBuildingListToFile ("buildings.txt");

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}