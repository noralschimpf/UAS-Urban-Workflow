#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

// NS_LOG_COMPONENT_DEFINE ("FinalProject");

// // See link for available trace sources, callback selection 
// // https://www.nsnam.org/docs/release/3.35/doxygen/_trace_source_list.html


// //QoS Metrics: Packet & Frame-level delay, loss, throughput
// //QoE Metrics: 
// //    PSNR: 10*log10(max pixel value / (MSE of actual v. received image))
// //    SSIM:  ((2*xMean*yMean + c1)*(2*xyCov + c2))/
// //            ((xMean^2 + yMean^2 + c1)*(xVar + yVar + c2))
// //            c1: (.01*(2^(bits/pixel)-1))^2    c2: (.03*(2^(bits/pixel)-1))^2
// //    VMAF: 



// static void
// CwndChange (uint32_t oldCwnd, uint32_t newCwnd)
// {
//   NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
// }

// // static void
// // RxDrop (Ptr<const Packet> p)
// // {
//   // NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
//   // std::cout << "RxDrop";
// // }

// static void
// UdpRxTrace (Ptr<const Packet> pck)
// {
//   std::cout << "UDP RX";
// }

void
IpDropTrace (const Ipv4Header header, Ptr<const Packet> pckt, Ipv4L3Protocol::DropReason dropReason, Ptr<Ipv4> ip, uint32_t interface)
{
  std::cout << "IP Drop";
}

// static void
// TxDropTrace(ns3::Ptr<const Packet> pckt)
// {
//   std::cout << "RLC TX Packet Drop";
// }

// static void
// HandoverStartTrace(const uint64_t imsi, const uint16_t cellId, const uint16_t rnti,const uint16_t targetCellId)
// {
//   std::cout << "RRC Handover Start";
// }


// static void
// HandoverEndOkTrace(const uint64_t imsi, const uint16_t cellId, const uint16_t rnti)
// {
//   std::cout << "RRC Handover End OK";
// }

// static void
// HandoverEndErrorTrace(const uint64_t imsi, const uint16_t cellId, const uint16_t rnti)
// {
//   std::cout << "RRC Handover End ERR";
// }

// static void
// RrcTimeoutTrace(uint64_t ismi, uint16_t cellId, uint16_t rnti)
// {
//   std::cout << "RRC Timeout";
// }

// static void
// StateTransitionTrace(uint64_t imsi, uint16_t cellId, uint16_t rnti, ns3::LteUeRrc::State oldstate, ns3::LteUeRrc::State newstate)
// {
//   std::cout << "RRC State Transition";
// }

// static void
// InitCellSelectErrTrace(uint64_t imsi, uint16_t cellId)
// {
//   std::cout << "Initial Cell Selection Error";
// }

// static void
// ConnectionTimeoutTrace(uint64_t imsi, uint16_t cellId, uint16_t rnti)
// {
//   std::cout << "RRC Connection Timeout";
// }

// static void
// RaResponseTimeoutTrace(uint64_t imsi, bool contention, uint8_t preambleTxCnt, uint8_t preambleTxMax)
// {
//   std::cout << "RA Response Timeout";
// }

// static void
// UeMeasTrace(uint16_t rnti, uint16_t cellId, double rsrp, double rsrq, uint8_t servingCell)
// {
//   std::cout << "UE PHY Measurement Report";
// }

// static void
// CellRsrpSinrTrace(uint16_t cellId, uint16_t rnti, double rsrp, double sinr, uint8_t compCarrierId)
// {
//   std::cout << "UE RSRP/SINR Report";
// }

// static void
// RxEndErrorTrace(ns3::Ptr<const Packet> pckt)
// {
//   std::cout << "Spectrum PHY RX End ERR";
// }