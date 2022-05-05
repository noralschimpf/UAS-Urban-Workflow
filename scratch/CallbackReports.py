import os, json, subprocess
import matplotlib.pyplot as plt, numpy as np, seaborn as sns

def main():
    # TABLE For each mobility scenario: u/std/top5% jitter/delay/PSNR/RSRP/RSRQ, # packet drops, bitrate
    # FIXED PARAMTERS: UENodes (1/enb), PTx (13 dBm), interval (50 ms)
    os.chdir("/home/schimpfen/src/ns-allinone-3.35/ns-3.35/scratchlogs/uavsim")
    matchstr = "--uasTxPower=13.0 --interPacketInterval=50.0 --uePerEnb=1"
    dirs = [x for x in os.listdir() if matchstr in x]
    for dir in dirs:
        os.chdir(dir)
        scenario_str = dir.replace(matchstr, '').replace('--','')
        
        # LOAD CALLBACK DATA FROM JSON
        with open('uavsimCallbacks.json', 'r') as f:
            json_data = json.load(f)
        rsrp, sinr, psnr, drops = [], [], [], 0
        for entry in json_data:
            if entry['Event'] == 'UE RSRP/SINR Report': rsrp.append(entry['RSRP']); sinr.append(entry['SINR'])
            # elif entry['Event'] == 'APP Packet Received': if 'Source addr' == '7.0.0.3': 
            print("Metric\tavg\tstd\t5%")
            for name, arr in [("RSRP", rsrp), ("SINR", sinr)]:
                print(f"{name}\t{np.mean(arr)}\t{np.std(arr)}\t{np.quantile(arr,.05)}")

        # LOAD DELAYS FROM FLOWMON
        subprocess.run(["python3", "../../scratch/flowmon-parse-results.py", f"{dir}/uavsim.flowmon"])
            


        os.chdir('..')

    

    # Seaborn Lineplots: delay/jitter v. interval, drops v Ptx, PSNR v Ptx
if __name__ == '__main__': main()