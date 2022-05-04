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
static long recv_counter = 0;
const std::string FILLERSTRING = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Commodo ullamcorper a lacus vestibulum sed arcu non odio euismod. Pellentesque sit amet porttitor eget dolor. Libero nunc consequat interdum varius sit amet mattis. Nibh cras pulvinar mattis nunc sed. Vulputate sapien nec sagittis aliquam malesuada bibendum arcu. Felis donec et odio pellentesque diam volutpat commodo sed. Porttitor eget dolor morbi non arcu. Viverra nam libero justo laoreet sit amet cursus sit amet. Praesent semper feugiat nibh sed. Suspendisse potenti nullam ac tortor vitae purus faucibus. Dui sapien eget mi proin sed libero enim sed faucibus. Donec et odio pellentesque diam volutpat commodo sed. Nam libero justo laoreet sit. In hac habitasse platea dictumst quisque sagittis purus sit. Donec et odio pellentesque diam. Ipsum dolor sit amet consectetur adipiscing elit duis tristique. Gravida neque convallis a cras semper auctor neque. Turpis massa sed elementum tempus egestas sed. Eleifend donec pretium vulputate sapien nec sagittis. Egestas dui id ornare arcu odio ut sem nulla. Nunc non blandit massa enim nec dui nunc mattis. Mauris a diam maecenas sed enim ut. Donec adipiscing tristique risus nec feugiat in fermentum. At risus viverra adipiscing at in. Amet purus gravida quis blandit turpis cursus in. Pharetra sit amet aliquam id diam maecenas ultricies mi. Enim praesent elementum facilisis leo vel fringilla est ullamcorper eget. Nec nam aliquam sem et. Nibh cras pulvinar mattis nunc sed blandit libero. Semper feugiat nibh sed pulvinar proin gravida hendrerit lectus a. Vulputate sapien nec sagittis aliquam malesuada. Sit amet cursus sit amet dictum sit amet justo donec.";
const char *buffer;
// const uint128_t bufSize = static_cast<const uint128_t>(FILLERSTRING.length());
// uint8_t *pckBuff[bufSize];
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

void
UASCourseChange(std::string outfile, Ptr<const MobilityModel> mob)
{
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  Vector pos = mob->GetPosition(); Vector vel = mob->GetVelocity();
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"UAS Course Change\", \"Position x\", " << pos.x
          << ", \"Position y\", " << pos.y << ", \"Position z\", " << pos.z << ", \"Velocity x\", " << vel.x
          << ", \"Velocity y\", " << vel.y << ", \"Velocity z\", " << vel.z ;
  toJson(&argpass, outfile);
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

// static void
// UdpRxTrace (std::string outfile, Ptr<const Packet> pck)
// {
//   std::stringbuf os;
//   std::ostream argpass = std::ostream(&os);
//   argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"UDP RX\", " << ", \"Size\", " << pck->GetSize();
//   toJson(&argpass, outfile);
// }
// static void UdpTxTrace (std::string outfile, Ptr<const Packet> pkt)
// {
//   std::stringbuf os;
//   std::ostream argpass = std::ostream(&os);
//   argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"UDP TX\", \"Size\", " << pkt->GetSize();
//   std::cout << "Packet TX: " << pkt << std::endl;
//   toJson(&argpass, outfile);
// }
static void ipRxTrace (std::string outfile, Ptr<const Packet> pck, Ptr<Ipv4> ip, uint32_t uin)
{
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"IP RX\", \"Source\", \"" << ip->GetAddress(1,0).GetAddress() << "\", \"Size\", " << pck->GetSize();
  toJson(&argpass, outfile);
}
// static void RcvPacketWAddr(std::string outfile, const Ptr<const Packet> pkt, const Address &src, const Address &dst)
// {
//   std::stringbuf os;
//   std::ostream argpass = std::ostream(&os);
//   argpass << "\"Time\", \"" << Simulator::Now() << "\", \"Event\", \"RX Packet\", \"Source\", \"" << (InetSocketAddress::ConvertFrom(src)).GetIpv4().Get()
//    << "\", \"Destination\", \"" << InetSocketAddress::ConvertFrom(dst).GetIpv4() <<  "\", \"Size\", " << pkt->GetSize();
//   toJson(&argpass, outfile);
// }

// static void udpDropTrace (std::string outfile, Ptr<const Packet> pkt)
// {
//   std::stringbuf os;
//   std::ostream argpass = std::ostream(&os);
//   argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" <<  ", \"Event\", \"UDP DROP\", \"Size\", " << pkt->GetSize();
//   toJson(&argpass, outfile);
// }


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
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"UE PHY Measurement Report\", \"RNTI\", " << rnti 
      << ", \"CellID\", " << cellId  << ", \"isServing\", " << isServingCell << ", \"Component Carrier ID\", " << compCarrierId
      << ", \"RSRP\", " << rsrp << ", \"RSRQ\", " << rsrq;
  toJson(&argpass, outfile);
}

static void
CellRsrpSinrTrace(std::string outfile, uint16_t cellId, uint16_t rnti, double rsrp, double sinr, uint8_t compCarrierId)
{
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"UE RSRP/SINR Report\", \"RNTI\", " << rnti 
          << ", \"CellID\", " << cellId << ", \"RSRP\", " << rsrp << ", \"SINR\", " << sinr;
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



double PSNR (uint8_t *txBuffer, uint8_t *rxBuffer, uint32_t buffLen)
//    PSNR: 10*log10(max pixel value / (MSE argpass actual v. received image))
{
  std::vector<uint8_t> txVec(txBuffer[0], txBuffer[buffLen]);
  std::vector<uint8_t> rxVec(rxBuffer[0], rxBuffer[buffLen]);
  uint8_t maxVal = 0;
  for (uint32_t i = 0; i < buffLen; i++){
    if (rxBuffer[i] >= maxVal) {maxVal = rxBuffer[i];}
  }
  // std::vector<double> errSqr(rxVec.Size());
  double MSE = 0.;
  for (uint32_t i = 0; i < buffLen; i++){
    MSE += std::pow(((long double)txVec[i] - (long double)rxVec[i]),2);
  }
  MSE /= buffLen;
  double PSNR = 10*std::log10(maxVal / MSE);
  return PSNR;
}

void RcvPacket(std::string outfile, Ptr<const Packet> p, const Address &addr){
  std::stringbuf os;
  std::ostream argpass = std::ostream(&os);
  char application[20]="";
  int  currentSequenceNumber = 0;
  // currentSequenceNumber = recv_counter++;
  // IF source address matches sender address, increment packet count
    if(InetSocketAddress::ConvertFrom(addr).GetIpv4().Get() == Ipv4Address("10.1.3.1").Get()){
          currentSequenceNumber = recv_counter++;
          strcpy(application, "\"VIDEO\"");
  }else{
          strcpy(application, "\"eNB Loading\"");
  }
  Ipv4Address srcaddr = InetSocketAddress::ConvertFrom(addr).GetIpv4();
  
  
  uint8_t *rxBuffer = new uint8_t[p->GetSize()];
  uint8_t *txBuffer = new uint8_t[p->GetSize()];
  for (uint32_t i = 0; i < p->GetSize(); i++){
    txBuffer[i] = (uint8_t)FILLERSTRING[i];
  }
  // txBuffer = (uint8_t *)FILLERSTRING.substr(0,p->GetSize()).c_str();
  uint32_t size = p-> CopyData(rxBuffer, p->GetSize());
  double psnr = 0.0;
  if (srcaddr == Ipv4Address("7.0.0.2")){
    psnr = PSNR(txBuffer, rxBuffer, p->GetSize());
    
  }
  std::cout << "PACKET RECEIVED\tSRC: " << srcaddr << "\tSIZE " << size << "\tPSNR " << psnr << std::endl;

  argpass << "\"Time\", " << "\"" << Simulator::Now() << "\"" << ", \"Event\", \"APP Packet Received\", " 
          << "\"Application\", " << application << ", \"Sequence\", " << currentSequenceNumber << ", \"Size\", " << p->GetSize() 
          << ", \"Source addr\", \"" << srcaddr << "\", \"Source Port\", " << InetSocketAddress::ConvertFrom(addr).GetPort()
          << "PSNR: " << psnr;
  toJson(&argpass, outfile);
}