from __future__ import division
import sys
import os
import matplotlib.pyplot as plt
try:
    from xml.etree import cElementTree as ElementTree
except ImportError:
    from xml.etree import ElementTree

def parse_time_ns(tm):
    if tm.endswith('ns'):
        return float(tm[:-2])
    raise ValueError(tm)



## FiveTuple
class FiveTuple(object):
    ## class variables
    ## @var sourceAddress 
    #  source address
    ## @var destinationAddress 
    #  destination address
    ## @var protocol 
    #  network protocol
    ## @var sourcePort 
    #  source port
    ## @var destinationPort 
    #  destination port
    ## @var __slots_ 
    #  class variable list
    __slots_ = ['sourceAddress', 'destinationAddress', 'protocol', 'sourcePort', 'destinationPort']
    def __init__(self, el):
        '''! The initializer.
        @param self The object pointer.
        @param el The element.
        '''
        self.sourceAddress = el.get('sourceAddress')
        self.destinationAddress = el.get('destinationAddress')
        self.sourcePort = int(el.get('sourcePort'))
        self.destinationPort = int(el.get('destinationPort'))
        self.protocol = int(el.get('protocol'))
        
## Histogram
class Histogram(object):
    ## class variables
    ## @var bins
    #  histogram bins
    ## @var nbins
    #  number of bins
    ## @var number_of_flows
    #  number of flows
    ## @var __slots_
    #  class variable list
    __slots_ = 'bins', 'nbins', 'number_of_flows'
    def __init__(self, el=None):
        '''! The initializer.
        @param self The object pointer.
        @param el The element.
        '''
        self.bins = []
        if el is not None:
            #self.nbins = int(el.get('nBins'))
            for bin in el.findall('bin'):
                self.bins.append( (float(bin.get("start")), float(bin.get("width")), int(bin.get("count"))) )

## Flow
class Flow(object):
    ## class variables
    ## @var flowId
    #  delay ID
    ## @var delayMean
    #  mean delay
    ## @var packetLossRatio
    #  packet loss ratio
    ## @var rxBitrate
    #  receive bit rate
    ## @var txBitrate
    #  transmit bit rate
    ## @var fiveTuple
    #  five tuple
    ## @var packetSizeMean
    #  packet size mean
    ## @var probe_stats_unsorted
    #  unsirted probe stats
    ## @var hopCount
    #  hop count
    ## @var flowInterruptionsHistogram
    #  flow histogram
    ## @var rx_duration
    #  receive duration
    ## @var __slots_
    #  class variable list
    __slots_ = ['flowId', 'delayMean', 'packetLossRatio', 'rxBitrate', 'txBitrate',
                'fiveTuple', 'packetSizeMean', 'probe_stats_unsorted',
                'hopCount', 'flowInterruptionsHistogram', 'rx_duration', 
                'delayHist', 'jitterHist', 'txBytes', 'rxBytes', 'txPackets', 'rxPackets',
                'packetsDropped', 'bytesDropped', 'lostPackets', 'delaySum', 'jitterSum']
    def __init__(self, flow_el):
        '''! The initializer.
        @param self The object pointer.
        @param flow_el The element.
        '''
        self.flowId = int(flow_el.get('flowId'))
        rxPackets, rxBytes = float(flow_el.get('rxPackets')), float(flow_el.get('rxBytes'))
        txPackets, txBytes = float(flow_el.get('txPackets')), float(flow_el.get('txBytes'))
        self.txBytes, self.rxBytes, self.txPackets, self.rxPackets = txBytes, rxBytes, txPackets, rxPackets

        self.packetsDropped, self.bytesDropped = flow_el.get('packetsDropped'), flow_el.get('bytesDropped')
        self.lostPackets = flow_el.get('lostPackets')

        self.delaySum, self.jitterSum = 1e-9*float(flow_el.get('delaySum').replace("+","").replace("ns","")), 1e-9*float(flow_el.get('jitterSum'.replace("+","")).replace("ns",""))

        tx_duration = (parse_time_ns (flow_el.get('timeLastTxPacket')) - parse_time_ns(flow_el.get('timeFirstTxPacket')))*1e-9
        rx_duration = (parse_time_ns (flow_el.get('timeLastRxPacket')) - parse_time_ns(flow_el.get('timeFirstRxPacket')))*1e-9
        self.rx_duration = rx_duration
        self.probe_stats_unsorted = []
        if rxPackets:
            self.hopCount = float(flow_el.get('timesForwarded')) / rxPackets + 1
        else:
            self.hopCount = -1000
        if rxPackets:
            self.delayMean = float(flow_el.get('delaySum')[:-2]) / rxPackets * 1e-9
            self.packetSizeMean = float(flow_el.get('rxBytes')) / rxPackets
        else:
            self.delayMean = None
            self.packetSizeMean = None
        if rx_duration > 0:
            self.rxBitrate = float(flow_el.get('rxBytes'))*8 / rx_duration
        else:
            self.rxBitrate = None
        if tx_duration > 0:
            self.txBitrate = float(flow_el.get('txBytes'))*8 / tx_duration
        else:
            self.txBitrate = None
        lost = float(flow_el.get('lostPackets'))
        #print "rxBytes: %s; txPackets: %s; rxPackets: %s; lostPackets: %s" % (flow_el.get('rxBytes'), txPackets, rxPackets, lost)
        if rxPackets == 0:
            self.packetLossRatio = None
        else:
            self.packetLossRatio = (lost / (rxPackets + lost))

        interrupt_hist_elem = flow_el.find("flowInterruptionsHistogram")
        if interrupt_hist_elem is None:
            self.flowInterruptionsHistogram = None
        else:
            self.flowInterruptionsHistogram = Histogram(interrupt_hist_elem)
        
        del_hist = flow_el.find("delayHistogram")
        if del_hist is None: self.delayHist = None 
        else: self.delayHist = Histogram(del_hist)

        jit_hist = flow_el.find("jitterHistogram")
        if jit_hist is None: self.jitterHist = None
        else: self.jitterHist = Histogram(jit_hist)

## ProbeFlowStats
class ProbeFlowStats(object):
    ## class variables
    ## @var probeId
    #  probe ID
    ## @var packets
    #  network packets
    ## @var bytes
    #  bytes
    ## @var delayFromFirstProbe
    #  delay from first probe
    ## @var __slots_
    #  class variable list
    __slots_ = ['probeId', 'packets', 'bytes', 'delayFromFirstProbe']

## Simulation
class Simulation(object):
    ## class variables
    ## @var flows
    #  list of flows
    def __init__(self, simulation_el):
        '''! The initializer.
        @param self The object pointer.
        @param simulation_el The element.
        '''
        self.flows = []
        FlowClassifier_el, = simulation_el.findall("Ipv4FlowClassifier")
        flow_map = {}
        for flow_el in simulation_el.findall("FlowStats/Flow"):
            flow = Flow(flow_el)
            flow_map[flow.flowId] = flow
            self.flows.append(flow)
        for flow_cls in FlowClassifier_el.findall("Flow"):
            flowId = int(flow_cls.get('flowId'))
            flow_map[flowId].fiveTuple = FiveTuple(flow_cls)

        for probe_elem in simulation_el.findall("FlowProbes/FlowProbe"):
            probeId = int(probe_elem.get('index'))
            for stats in probe_elem.findall("FlowStats"):
                flowId = int(stats.get('flowId'))
                s = ProbeFlowStats()
                s.packets = int(stats.get('packets'))
                s.bytes = float(stats.get('bytes'))
                s.probeId = probeId
                if s.packets > 0:
                    s.delayFromFirstProbe =  parse_time_ns(stats.get('delayFromFirstProbeSum')) / float(s.packets)
                else:
                    s.delayFromFirstProbe = 0
                flow_map[flowId].probe_stats_unsorted.append(s)


def main(argv):
    file_obj = open(argv[1])
    outdir = os.path.split('file_obj.name')[0] + '/'
    # print(file_obj); print(os.path.split(file_obj.name))
    print("Reading XML file ", end=" ")
 
    sys.stdout.flush()        
    level = 0
    sim_list = []
    for event, elem in ElementTree.iterparse(file_obj, events=("start", "end")):
        if event == "start":
            level += 1
        if event == "end":
            level -= 1
            if level == 0 and elem.tag == 'FlowMonitor':
                sim = Simulation(elem)
                sim_list.append(sim)
                elem.clear() # won't need this any more
                sys.stdout.write(".")
                sys.stdout.flush()
    print(" done.")


    for sim in sim_list:
        for flow in sim.flows:
            t = flow.fiveTuple
            proto = {6: 'TCP', 17: 'UDP'} [t.protocol]
            print("\nFlowID: %i (%s %s/%s --> %s/%i)" % \
                (flow.flowId, proto, t.sourceAddress, t.sourcePort, t.destinationAddress, t.destinationPort))

            print("\tThrougput Metrics")
            tx_bits, rx_bits = "None", "None"
            txByte, rxByte = "None", "None"
            txPack, rxPack = "None", "None"

            if not flow.txBitrate is None: tx_bits = "{:.2f} kbit/s".format(flow.txBitrate*1e-3)
            if not flow.rxBitrate is None: rx_bits = "{:.2f} kbit/s".format(flow.rxBitrate*1e-3)
            print("\t\tBitrate:\tTX {}\tRX {}".format(tx_bits, rx_bits))
            if not flow.txBytes is None: txByte = "{:.2f} kB".format(flow.txBytes*1e-3)
            if not flow.rxBytes is None: rxByte = "{:.2f} kB".format(flow.rxBytes*1e-3)
            print("\t\tTotal Bytes:\tTX {}\t\tRX {}".format(txByte, rxByte))
            if not flow.txPackets is None: txPack = "{}".format(flow.txPackets)
            if not flow.rxPackets is None: rxPack = "{}".format(flow.rxPackets)
            print("\t\tTotal Packets:\tTX {} \t\t\tRX {}".format(txPack, rxPack))



            print("\tLoss Metrics")
            packDrop, byteDrop = "None", "None"
            packLost, lossRate = "None", "None"
            if not flow.packetsDropped is None: packDrop = "{}".format(flow.packetsDropped)
            if not flow.bytesDropped is None: byteDrop = "{}".format(flow.bytesDropped)
            if not flow.lostPackets is None: packLost = "{}".format(flow.lostPackets)
            if not flow.packetLossRatio is None: lossRate = "{:.2} %".format(flow.packetLossRatio*100)
            print("\t\tDropped\t\tPackets: {}\t\tBytes: {}".format(packDrop, byteDrop))
            print("\t\tLost Packets:\t{}\t\t\tRatio: {}".format(packLost, lossRate))




            print("\tDelay Metrics")
            delSum, jitSum = "None", "None"
            # delMean, jitMean = "None", "None"
            # delStd, jitStd = "None", "None"
            
            if not flow.delaySum is None: delSum = "{:.2f} ms".format(flow.delaySum*1e3)
            if not flow.jitterSum is None: jitSum = "{:.2e} ms".format(flow.jitterSum*1e3)
            print("\t\tSum Delay {}\t\t\tSum Jitter: {}".format(delSum, jitSum))
            fig, ax = plt.subplots(1,1)
            if not flow.delayHist is None: 
                start, width, count = [], [], []
                for x in flow.delayHist.bins: start.append(x[0]*1e3); width.append(x[1]*1e3); count.append(x[2])
                ax.bar(x=start, height=count, width=width, label="Delay", alpha=.5)
            if not flow.jitterHist is None:
                start, width, count = [], [], []
                for x in flow.jitterHist.bins: start.append(x[0]*1e3); width.append(x[1]*1e3); count.append(x[2])
                ax.bar(x=start, height=count, width=width, label="Jitter", alpha=.5)
            ax.legend()
            title = "Flow" + str(flow.flowId) + " Delay and Jitter Histograms"
            ax.set_title(title); ax.set_xlabel("ms"); ax.set_ylabel("count")
            fig.savefig(outdir + title + ".png", dpi=300)
            fig.clf(); ax.cla(); plt.close()
            
            
            # else:
            #     print("\t\tTX bitrate: %.2f kbit/s" % (flow.txBitrate*1e-3,))

            # if flow.rxBitrate is None:
            #     print("\tRX bitrate: None")
            # else:
            #     print("\tRX bitrate: %.2f kbit/s" % (flow.rxBitrate*1e-3,))

            # if flow.delayMean is None:
            #     print("\tMean Delay: None")
            # else:
            #     print("\tMean Delay: %.2f ms" % (flow.delayMean*1e3,))

            # if flow.packetLossRatio is None:
            #     print("\tPacket Loss Ratio: None")
            # else:
            #     print("\tPacket Loss Ratio: %.2f %%" % (flow.packetLossRatio*100))


if __name__ == '__main__':
    main(sys.argv)
