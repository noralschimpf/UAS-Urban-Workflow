#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FinalProject");

static std::string PREFIX = "scratchlogs/uavsim/";
static std::string callbackFile = PREFIX + "uavsimCallbacks.json";

// // See link for available trace sources, callback selection 
// // https://www.nsnam.org/docs/release/3.35/doxygen/_trace_source_list.html


// //QoS Metrics: Packet & Frame-level delay, loss, throughput
// //QoE Metrics: 
// //    PSNR: 10*log10(max pixel value / (MSE argpass actual v. received image))
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

static void toJson(std::ostream *os, std::string outfile)
{
  *os << "}, " << std::endl;
  //convert ostream to string
  std::stringstream strstr;
  strstr << os->rdbuf();
  std::string args = strstr.str();

  //open JSON file
  std::ofstream of;
  of.open(outfile, std::ios_base::app);
  if (!of){NS_LOG_ERROR("CANNOT OPEN LOGFILE");}
  std::string tofile = args;
  
  bool isColon = true;
  for (uint16_t i=0; i < tofile.length(); i++)
  {
    if (tofile[i] == ',')
    {
      if(isColon){tofile[i] = ':';}
      isColon = !isColon;      
    }
  }
  tofile.insert(0,"{");
  of << tofile;
  of.close();
}

void IpDropTrace (std::string outfile, std::string context, const Ipv4Header header, Ptr<const Packet> pckt, Ipv4L3Protocol::DropReason dropReason, Ptr<Ipv4> ip, uint32_t interface)
{
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"IP Drop\", \"IP Addr\", " << ip->GetAddress(interface, 0) 
              << ", \"Drop Reason\", " << dropReason;
  toJson(&argpass, outfile);
}

void ipTxTrace (std::string outfile, Ptr<const Packet> pkt, Ptr<Ipv4> ip, uint32_t addr)
{
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"IP TX\", \"Addr\", " << addr <<std::endl;
  toJson(&argpass, outfile);
}

static void
UdpRxTrace (std::string outfile, Ptr<const Packet> pck)
{
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"UDP RX\", " << ", \"Size\", " << pck->GetSize();
  toJson(&argpass, outfile);
}
static void UdpTxTrace (std::string outfile, Ptr<const Packet> pkt)
{
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"UDP TX\", \"Size\", " << pkt->GetSize();
  toJson(&argpass, outfile);
}

static void udpDropTrace (std::string outfile, Ptr<const Packet> pkt)
{
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" <<  ", \"Event\", \"UDP DROP\", \"Size\", " << pkt->GetSize();
  toJson(&argpass, outfile);
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
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"RRC Handover Start\", \"IMSI\"" << imsi << ", \"RNTI\", " << rnti
          << ", \"Current Cell\", " << cellId << ", \"Target Cell\", " << targetCellId;
  toJson(&argpass, outfile);
}


static void
HandoverEndOkTrace(std::string outfile, const uint64_t imsi, const uint16_t cellId, const uint16_t rnti)
{
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"RRC Handover End OK\", "
          << "\"IMSI\"" << imsi << ", \"RNTI\", " << rnti << ", \"Cell\", " << cellId;
  toJson(&argpass, outfile);
}

static void
HandoverEndErrorTrace(std::string outfile, const uint64_t imsi, const uint16_t cellId, const uint16_t rnti)
{
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"RRC Handover End ERR\", \"IMSI\"" 
          << imsi << ", \"RNTI\", " << rnti << ", \"Cell\", " << cellId;
  toJson(&argpass, outfile);
}

// static void
// RrcTimeoutTrace(uint64_t ismi, uint16_t cellId, uint16_t rnti)
// {
//   std::cout << "RRC Timeout";
// }

std::string stateUnravel(ns3::LteUeRrc::State state)
{
  switch(state)
  {
    case 0: return "IDLE_START";
    case 1: return "IDLE_CELL_SEARCH";
    case 2: return "IDLE_WAIT_MIB_SIB1";
    case 3: return "IDLE_WAIT_MIB";
    case 4: return "IDLE_WAIT_SIB1";
    case 5: return "IDLE_CAMPED_NORMALLY";
    case 6: return "IDLE_WAIT_SIB2";
    case 7: return "IDLE_RANDOM_ACCESS";
    case 8: return "IDLE_CONNECTING";
    case 9: return "CONNECTED_NORMALLY";
    case 10: return "CONNECTED_HANDOVER";
    case 11: return "CONNECTED_PHY_PROBLEM";
    case 12: return "CONNECTED_REESTABLISHING";
    case 13: return "NUM_STATES";
    default: return "NO STATE FOUND";
    }
}

static void
StateTransitionTrace(std::string outfile, uint64_t imsi, uint16_t cellId, uint16_t rnti, ns3::LteUeRrc::State oldstate, ns3::LteUeRrc::State newstate)
{
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  std::string oldstr, newstr;
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"RRC State Transition\", \"IMSI\", " << imsi << ", "
          << "\"RNTI\", " << rnti << ", \"Cell\", " << cellId << ", \"Old State\", \"" 
          << stateUnravel(oldstate) << "\", \"New State\", \"" << stateUnravel(newstate) << "\"";
  toJson(&argpass, outfile);
}

static void
InitCellSelectErrTrace(std::string outfile, uint64_t imsi, uint16_t cellId)
{
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"Initial Cell Selection Error\", \"IMSI\"" << imsi << ", \"Cell ID\", " << cellId;
  toJson(&argpass, outfile);
}

static void
ConnectionTimeoutTrace(std::string outfile, uint64_t imsi, uint16_t cellId, uint16_t rnti, uint8_t ch)
{
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"RRC Connection Timeout\", \"IMSI\"" << imsi << ", \"RNTI\", " << rnti
          << ", \"Cell\", " << cellId << ", \"uint8_t\", " << ch;
  toJson(&argpass, outfile);
}

static void
RaResponseTimeoutTrace(std::string outfile, uint64_t imsi, bool contention, uint8_t preambleTxCnt, uint8_t preambleTxMax)
{
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"RA Response Timeout\", \"IMSI\", " << imsi << ", \"isContention\", " << contention
          << ", \"Preamble TX Count\", " << preambleTxCnt << ", \"Preamble TX Max\", " << preambleTxMax;
  toJson(&argpass, outfile);
}

static void
UeMeasTrace(std::string outfile, uint16_t rnti, uint16_t cellId, double rsrp, double rsrq, bool isServingCell, uint8_t compCarrierId)
{
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"UE PHY Measurement Report\", \"CellID\", " << cellId 
      << ", \"isServing\", " << isServingCell << ", \"RSRQ\", " << rsrq;
  toJson(&argpass, outfile);
}

static void
CellRsrpSinrTrace(std::string outfile, uint16_t cellId, uint16_t rnti, double rsrp, double sinr, uint8_t compCarrierId)
{
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"UE RSRP/SINR Report\", \"CellID\", " << cellId
          << ", \"RSRP\", " << rsrp << ", \"SINR\", " << sinr;
  toJson(&argpass, outfile);
}

static void
RxEndErrorTrace(std::string outfile, ns3::Ptr<const Packet> pckt)
{
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);

  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"Spectrum PHY RX End ERR\"";
  
  toJson(&argpass, outfile);
}