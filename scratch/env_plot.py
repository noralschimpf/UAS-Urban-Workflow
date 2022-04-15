from matplotlib.collections import PathCollection
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as patch
from matplotlib.collections import PatchCollection
import os

def main():
    os.chdir("scratchlogs/uavsim")
    df_bd = pd.read_csv("buildings.txt");

    fig, ax = plt.subplots(1,1);
    rects = []
    for idx, row in df_bd.iterrows():
        # "building,rectFromX,rectFromY,rectToX,rectToY"
        rects = rects + [patch.Rectangle((row['rectFromX'],row['rectFromY']), row['rectToX']-row['rectFromX'], row['rectToY']-row['rectFromY'], fill=True, alpha=.5)]
        ax.text((row['rectFromX']+row['rectToX'])/2, (row['rectFromY']+row['rectToY'])/2, str(row['building']), ha='center',va='center')
    
    pc = PatchCollection(rects, alpha=.5)
    ax.add_collection(pc)

    margin=20
    ax.set_xlim(df_bd['rectFromX'].min()-margin, df_bd['rectToX'].max()+margin)
    ax.set_ylim(df_bd['rectFromY'].min()-margin, df_bd['rectToY'].max()+margin)
    
    fig.savefig("env.png")
    fig.clf(); ax.cla(); plt.close()

if __name__ == '__main__': main()