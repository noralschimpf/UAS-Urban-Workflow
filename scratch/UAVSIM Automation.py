import os, shutil, subprocess
import matplotlib.pyplot as plt, pandas as pd, numpy as np
import tqdm

def main():
    # Global parameters
    exec_path = "/home/schimpfen/src/ns-allinone-3.35/ns-3.35"
    log_orig_path = "{}/scratchlogs/uavsim".format(exec_path)
    os.chdir(exec_path)

    # Static Parameters
    tracing = "true"
    handover = "None"
    sectorized = "true"
    exec_list = ["./waf", "--run", f"sample-lte --tracing={tracing} --handover={handover} --sectorized={sectorized}"]

    # Experimental Parameters
    uasPTx = [1., 5., 13., 17., 21.]
    enbUePTx = [1., 13., 21.]
    enbUeNodes = [1, 2]
    uePckInterval_ms = [100., 75., 50., 25]
    mobility = [1, 2, 3, 4, 5]

    # iterate scenarios
    for uas in uasPTx:
        for nodes in enbUeNodes:
            for intv in uePckInterval_ms:
                for mob in tqdm.tqdm(mobility):
                    varParams = f' --uasTxPower={uas} --interPacketInterval={intv} --uePerEnb={nodes} --scenario={mob}'
                    exec_list[-1] = exec_list[-1] + " " +  varParams
                    subprocess.run(exec_list)
                    simname = "uavsim" + "".join(varParams)
                    if not os.path.isdir(f"{log_orig_path}/{simname}"): os.makedirs(f"{log_orig_path}/{simname}")
                    files = [x for x in os.listdir(log_orig_path) if not os.path.isdir(f"{log_orig_path}/{x}")]
                    for file in files: 
                        if ".json" in file: 
                            lines = open(f"{log_orig_path}/{file}", 'r').readlines()
                            nocomma = True; i=-1
                            while (nocomma):
                                newline = lines[i]
                                if i==-1 and not (']' in newline): newline = newline + "]"
                                if '},' in newline: 
                                    newline = newline.replace('},', '}')
                                    lines[i] = newline
                                    nocomma = False
                                i-=1
                            open(f"{log_orig_path}/{simname}/{file}", 'w').writelines(lines)
                        else:
                            shutil.move(f"{log_orig_path}/{file}", f"{log_orig_path}/{simname}/{file}")

if __name__ == '__main__': main()