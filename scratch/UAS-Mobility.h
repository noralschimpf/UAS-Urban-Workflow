#ifndef UAS_MOBILITY_H
#define UAS_MOBILITY_H

// Environment Layout
static double buildingSizeX = 250 - 3.5 * 2 - 3; // m
static double buildingSizeY = 433 - 3.5 * 2 - 3; // m
static double streetWidth = 20; // m
static double buildingHeight = 10; // m
static uint32_t numBuildingsX = 2;
static uint32_t numBuildingsY = 2;
static double maxAxisX = (buildingSizeX + streetWidth) * numBuildingsX;
static double maxAxisY = (buildingSizeY + streetWidth) * numBuildingsY;

// 3GPP defines that the maximum speed in urban scenario is 60 km/h
double vScatt = 60 / 3.6;

// ns3::Ptr<MobilityModel> createUASMobility(int scenario, ns3::Time duration);
void createUASMobility(ns3::Ptr<ns3::Node> node, int scenario, ns3::Time duration);
void createBSSMobility(ns3::Ptr<ns3::Node> node, ns3::Vector v);
// int main();


#endif