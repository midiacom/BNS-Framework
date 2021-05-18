/****************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2010                         *
 *  Developed at the ATP lab, Networked Systems research theme              *
 *  Author(s): Yuriy Tselishchev                                            *
 *  This file is distributed under the terms in the attached LICENSE file.  *
 *  If you do not find this file, copies can be found by writing to:        *
 *                                                                          *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
 *      Attention:  License Inquiry.                                        *
 *                                                                          *  
 ****************************************************************************/

#ifndef _MOBILITYMODULE_H_
#define _MOBILITYMODULE_H_
#include <queue>
#include <string>
#include <map>
#include <tuple>
#include <typeinfo>
#include "MobilityManagerMessage_m.h"
#include "VirtualMobilityManager.h"

using namespace std;

class MobanMobilityManager: public VirtualMobilityManager {
private:
    /*--- The .ned file's parameters ---*/

    // Body placement and node location variables
    std::string bodyPlacement;
    int nodePlacement;

    // Posture status variables
    string status;
    std::queue<string> statusSequence;
    std::queue<double> statusChangeTimeSequence;


    // Simulation field size
    int xlen;
    int ylen;
    int zlen;


    // Node initial location
    double loc1_x;
    double loc1_y;
    double loc1_z;

    // Node destination in zigZagMovement() - from loc1 to loc2, and vice-versa
    double loc2_x;
    double loc2_y;
    double loc2_z;

    // Actual node location
    double loc_x;
    double loc_y;
    double loc_z;


    // Node speed variables and group speed for groupMovement
    double speed;
    double walkingFreq;
    double runningFreq;
    double groupSpeed;
    double movementRange;

    // Area of random movement
    double rand_area;

    /*-- support variables --*/
    int direction;
    double distance;
    double desloc;
    double incr_x;
    double incr_y;
    double incr_z;
    double tmp;
    // Interval to update position and notify wireless channel
    double updateInterval;

    // map for bodyplacement string to nodeplacement int
    // important to notify wireless channel in vanRoyChannel
    std::map<string, int> nodePlacementMap = {
            { "head", 0 },
            { "left-arm", 1 },
            { "left-hand", 2 },
            { "chest", 3 },
            { "right-arm", 4 },
            { "right-hand", 5 },
            { "center-waist", 6 },
            { "right-waist", 7 },
            { "left-leg", 8 },
            { "left-foot", 9 },
            { "right-leg", 10 },
            { "right-foot", 11 }
    };
    std::map<string, int>::iterator np;

    // map of node placement and x,y,z corresponding coordinates
    std::map<int, std::tuple<double, double, double>> nodeLocationMap = {
            {0,  std::make_tuple(65, 170, 100)},
            {1,  std::make_tuple(80, 130, 100)},
            {2,  std::make_tuple(80, 100, 100)},
            {3,  std::make_tuple(65, 130, 100)},
            {4,  std::make_tuple(50, 130, 100)},
            {5,  std::make_tuple(50, 100, 100)},
            {6,  std::make_tuple(65, 100, 100)},
            {7,  std::make_tuple(50, 100, 100)},
            {8,  std::make_tuple(50, 60, 100)},
            {9,  std::make_tuple(50, 20, 100)},
            {10,  std::make_tuple(80, 60, 100)},
            {11,  std::make_tuple(80, 20, 100)}

    };

    std::map<int, std::tuple<double, double, double>>::iterator nl;

protected:
    void initialize();
    void handleMessage(cMessage * msg);
    void moveNode();
    void zigZagMovement();
    void groupMovement();
    void randomMovement();
    void checkMovementBoundaries();
    void updateStatus();
    void setSpeed();
    void initPosition(string bodyPlacement);

public:
    string getBodyPlacement() {return bodyPlacement;}
    int getNodePlacement() { return nodePlacement;}

};

#endif
