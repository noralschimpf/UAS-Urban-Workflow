from cProfile import label
from matplotlib.collections import PathCollection
import pandas as pd
import numpy as np
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

    # Read and plot UE and EnB locations
    df_ue = pd.read_csv("ues.txt")
    ax.scatter(df_ue['X'], df_ue['Y'], label="UEs")
    
    df_enb = pd.read_csv("enbs.txt")
    ax.scatter(df_enb['X'], df_enb['Y'], label="ENBs", alpha=.25)


     
    # Track X-Y Coordinate System relative to predefined building and axis measures
    buildingSizeX, streetWidth, buildingSizeY, buildingHeight = 250-3.5 * 2 - 3, 20, 433 - 3.5 * 2 - 3, 10
    maxAxisX, maxAxisY = 4000, 2000
    numBuildingsX, numBuildingsY = maxAxisX / (buildingSizeX + streetWidth), maxAxisY / (buildingSizeX + streetWidth)
    
    # Predefined Scenario Routes from mobility update (branch: nora)
    routes = [[
                (0 + 1*buildingSizeX + 0*streetWidth,0 + 1*buildingSizeX + 0*streetWidth,  maxAxisX - 1*buildingSizeX - 0*streetWidth, 0 + 1*buildingSizeX + 0*streetWidth), 
                (0 + 1*buildingSizeY + 0*streetWidth, 0 + 1*buildingSizeY + 0*streetWidth, 0 + 3*buildingSizeY + 2*streetWidth, 0 + 1*buildingSizeY + 0*streetWidth)
                ],
                [
                (0 + 1*buildingSizeX + 0.5*streetWidth, maxAxisX - 1*buildingSizeX - 0.5*streetWidth, maxAxisX - 1*buildingSizeX - 0.5*streetWidth, 0 + 1*buildingSizeX + 0.5*streetWidth, 0 + 1*buildingSizeX + 0.5*streetWidth),
                (0 + 1*buildingSizeY + 0.5*streetWidth, 0 + 1*buildingSizeY + 0.5*streetWidth, 0 + 2*buildingSizeY + 1.5*streetWidth, 0 + 2*buildingSizeY + 1.5*streetWidth, 0 + 1*buildingSizeY + 0.5*streetWidth)
                ],                
                [
                (0 + 3*buildingSizeX + 2.5*streetWidth, 0 + 3*buildingSizeX + 2.5*streetWidth, 0 + 7.5*buildingSizeX + 7*streetWidth, 0 + 3*buildingSizeX + 2.5*streetWidth),
                (0 + 2.5*buildingSizeY + 2*streetWidth, 0 + 0.5*buildingSizeY + 0*streetWidth, 0 + 2.5*buildingSizeY + 2*streetWidth, 0 + 2.5*buildingSizeY + 2*streetWidth)
                ],
                [
                (maxAxisX - 1.5*buildingSizeX - 1*streetWidth,0 + 2.5*buildingSizeX + 2*streetWidth, maxAxisX - 1.5*buildingSizeX - 1*streetWidth),
                (0 + 2.5*buildingSizeY + 2*streetWidth, maxAxisY - 1.5*buildingSizeY + 1*streetWidth, 0 + 2.5*buildingSizeY + 2*streetWidth)
                ],
                [
                (0 + 0.5*buildingSizeX + 0*streetWidth, 0 + 0.5*buildingSizeX + 0*streetWidth, 0 + 2.5*buildingSizeX + 2*streetWidth, 0 + 2.5*buildingSizeX + 2*streetWidth, 0 + 4.5*buildingSizeX + 4*streetWidth, 0 + 4.5*buildingSizeX + 4*streetWidth, 0 + 6.5*buildingSizeX + 6*streetWidth, 0 + 6.5*buildingSizeX + 6*streetWidth, 0 + 8.5*buildingSizeX + 8*streetWidth, 0 + 8.5*buildingSizeX + 8*streetWidth, 0 + 10.5*buildingSizeX + 10*streetWidth,  0 + 10.5*buildingSizeX + 10*streetWidth, 0 + 12.5*buildingSizeX + 12*streetWidth, 0 + 12.5*buildingSizeX + 12*streetWidth, 0 + 14.5*buildingSizeX + 14*streetWidth, 0 + 14.5*buildingSizeX + 14*streetWidth),
                (maxAxisY - 0.5*buildingSizeY - 0*streetWidth,  0 + 0.5*buildingSizeY + 0*streetWidth,  0 + 0.5*buildingSizeY + 0*streetWidth, maxAxisY - 0.5*buildingSizeY - 0*streetWidth, maxAxisY - 0.5*buildingSizeY - 0*streetWidth, 0 + 0.5*buildingSizeY + 0*streetWidth, 0 + 0.5*buildingSizeY + 0*streetWidth, maxAxisY - 0.5*buildingSizeY - 0*streetWidth, maxAxisY - 0.5*buildingSizeY - 0*streetWidth, 0 + 0.5*buildingSizeY + 0*streetWidth,  0 + 0.5*buildingSizeY + 0*streetWidth, maxAxisY - 0.5*buildingSizeY - 0*streetWidth, maxAxisY - 0.5*buildingSizeY - 0*streetWidth, 0 + 0.5*buildingSizeY + 0*streetWidth, 0 + 0.5*buildingSizeY + 0*streetWidth, maxAxisY - 0.5*buildingSizeY - 0*streetWidth)
                ]
                ]
    polys = []

    # Plot each UAS Route
    for idx, route in enumerate(routes):
        plt.plot(route[0], route[1], label="Route {}".format(idx), marker="o", linestyle='-', linewidth=3, alpha=.5)
            # for i in range(len(route)-1):
                # plt.plot(route[0][i], route[1][i], route[0][i+1], route[1][i+1], label="Route {}".format(idx), marker="o", linestyle='-', linewidth=3)
            # a = np.stack((route[0], route[1])).T
            # print(a.shape)
            # polys = polys + [patch.Polygon(a, label="Route {}".format(idx), fill=False)]
            
    # pcPoly = PatchCollection(polys)
    # ax.add_collection(pcPoly)
    
    margin=100; legendPad = 800
    # ax.set_xlim(df_bd['rectFromX'].min()-margin, df_bd['rectToX'].max()+margin)
    # ax.set_ylim(df_bd['rectFromY'].min()-margin, df_bd['rectToY'].max()+margin))
    ax.set_xlim(0-margin, maxAxisX+margin + legendPad); ax.set_ylim(0-margin, maxAxisY+margin)
    ax.set_title("Physical Environment Layout (t=0)", alpha=.5)
    fig.tight_layout()

    ax.legend()
    fig.savefig("env.png")
    fig.clf(); ax.cla(); plt.close()

if __name__ == '__main__': main()