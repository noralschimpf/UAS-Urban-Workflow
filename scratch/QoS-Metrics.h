#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FinalProject");

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



void IpDropTrace (std::string context, const Ipv4Header header, Ptr<const Packet> pckt, Ipv4L3Protocol::DropReason dropReason, Ptr<Ipv4> ip, uint32_t interface)
{
  std::cout << Simulator::Now() << ", IP Drop" << std::endl;
}

void ipTxTrace (Ptr<const Packet> pkt, Ptr<Ipv4> ip, uint32_t addr)
{
  std::cout << Simulator::Now() << ", IP TX" <<std::endl;
}

static void
UdpRxTrace (std::string outfile, Ptr<const Packet> pck)
{
  std::ofstream of;
  of.open(outfile, std::ios_base::openmode::_S_app);
  if(!of){NS_LOG_ERROR("CANNOT OPEN LOGFILE");}
  of << Simulator::Now() << ", UDP RX" << std::endl;
  of.close();
}
static void UdpTxTrace (std::string outfile, Ptr<const Packet> pkt)
{
  std::ofstream of;
  of.open(outfile, std::ios_base::openmode::_S_app);
  if(!of){NS_LOG_ERROR("CANNOT OPEN LOGFILE");}
  of << Simulator::Now() << ", UDP TX" << std::endl;
  of.close();
}

static void udpDropTrace (std::string outfile, Ptr<const Packet> pkt)
{
  std::ofstream of;
  of.open(outfile, std::ios_base::openmode::_S_app);
  if(!of){NS_LOG_ERROR("CANNOT OPEN LOGFILE");}
  of << Simulator::Now() <<  ", UDP DROP" << std::endl;
  of.close();
}


// void RxTrace (std::string context, Ptr<const Packet> pkt, const Address &a, const Address &b)
// {
//   std::cout << context << std::endl;
//   std::cout << "Size: " << pkt->GetSize()
//             << " From: " << InetSocketAddress::ConvertFrom(a).GetIpv4()
//             << " LocalAddr: " << InetSocketAddress::ConvertFrom(b).GetIpv4() << std::endl;
// }
// static void
// TxDropTrace(ns3::Ptr<const Packet> pckt)
// {
//   std::cout << "RLC TX Packet Drop";
// }

static void
HandoverStartTrace(std::string outfile, const uint64_t imsi, const uint16_t cellId, const uint16_t rnti,const uint16_t targetCellId)
{
  std::ofstream of;
  of.open(outfile, std::ios_base::openmode::_S_app);
  if(!of){NS_LOG_ERROR("CANNOT OPEN LOGFILE");}
  of << Simulator::Now() << ", RRC Handover Start" << std::endl;
  of.close();
}


static void
HandoverEndOkTrace(std::string outfile, const uint64_t imsi, const uint16_t cellId, const uint16_t rnti)
{
  std::ofstream of;
  of.open(outfile, std::ios_base::openmode::_S_app);
  if(!of){NS_LOG_ERROR("CANNOT OPEN LOGFILE");}
  of << Simulator::Now() << ", RRC Handover End OK" << std::endl;
  of.close();
}

static void
HandoverEndErrorTrace(std::string outfile, const uint64_t imsi, const uint16_t cellId, const uint16_t rnti)
{
  std::ofstream of;
  of.open(outfile, std::ios_base::openmode::_S_app);
  if(!of){NS_LOG_ERROR("CANNOT OPEN LOGFILE");}
  of << Simulator::Now() << ", RRC Handover End ERR" <<std::endl;
  of.close();
}

// static void
// RrcTimeoutTrace(uint64_t ismi, uint16_t cellId, uint16_t rnti)
// {
//   std::cout << "RRC Timeout";
// }

static void
StateTransitionTrace(std::string outfile, uint64_t imsi, uint16_t cellId, uint16_t rnti, ns3::LteUeRrc::State oldstate, ns3::LteUeRrc::State newstate)
{
  std::ofstream of;
  of.open(outfile, std::ios_base::openmode::_S_app);
  if(!of){NS_LOG_ERROR("CANNOT OPEN LOGFILE");}
  of << Simulator::Now() << ", RRC State Transition" << std::endl;
  of.close();
}

static void
InitCellSelectErrTrace(std::string outfile, uint64_t imsi, uint16_t cellId)
{
  std::ofstream of;
  of.open(outfile, std::ios_base::openmode::_S_app);
  if(!of){NS_LOG_ERROR("CANNOT OPEN LOGFILE");}
  of << Simulator::Now() << ", Initial Cell Selection Error" << std::endl;
  of.close();
}

static void
ConnectionTimeoutTrace(std::string outfile, uint64_t imsi, uint16_t cellId, uint16_t rnti, uint8_t ch)
{
  std::ofstream of;
  of.open(outfile, std::ios_base::openmode::_S_app);
  if(!of){NS_LOG_ERROR("CANNOT OPEN LOGFILE");}
  of << Simulator::Now() << ", RRC Connection Timeout" << std::endl;
  of.close();
}

static void
RaResponseTimeoutTrace(std::string outfile, uint64_t imsi, bool contention, uint8_t preambleTxCnt, uint8_t preambleTxMax)
{
  std::ofstream of;
  of.open(outfile, std::ios_base::openmode::_S_app);
  if(!of){NS_LOG_ERROR("CANNOT OPEN LOGFILE");}
  of << Simulator::Now() << ", RA Response Timeout" << std::endl;
  of.close();
}

static void
UeMeasTrace(std::string outfile, uint16_t rnti, uint16_t cellId, double rsrp, double rsrq, bool isServingCell, uint8_t compCarrierId)
{
  std::ofstream of;
  of.open(outfile, std::ios_base::openmode::_S_app);
  if(!of){NS_LOG_ERROR("CANNOT OPEN LOGFILE");}
  of << Simulator::Now() << ", UE PHY Measurement Report, CellID, " << cellId 
      << ", isServing, " << isServingCell << ", RSRQ, " << rsrq << std::endl;
  of.close();
}

static void
CellRsrpSinrTrace(std::string outfile, uint16_t cellId, uint16_t rnti, double rsrp, double sinr, uint8_t compCarrierId)
{
  std::ofstream of;
  of.open(outfile, std::ios_base::openmode::_S_app);
  if(!of){NS_LOG_ERROR("CANNOT OPEN LOGFILE");}
  of << Simulator::Now() << ", UE RSRP/SINR Report, CellID, " << cellId << ", RSRP, " << rsrp << ", SINR, " << sinr << std::endl;
  of.close();
}

static void
RxEndErrorTrace(std::string outfile, ns3::Ptr<const Packet> pckt)
{
  std::ofstream of;
  of.open(outfile, std::ios_base::openmode::_S_app);
  if(!of){NS_LOG_ERROR("CANNOT OPEN LOGFILE");}

  of << Simulator::Now() << ", Spectrum PHY RX End ERR"  << std::endl;
  
  of.close();
}