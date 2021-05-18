//    This is a wireless channel simulation for Castalia Simulator based on Van Roy et. al channel model.
//    Copyright (C) 2020  Vinicius Correa Ferreira
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef _WIRELESSCHANNEL_H
#define _WIRELESSCHANNEL_H

#include "WirelessChannelMessages_m.h"
#include "WirelessChannelTemporal.h"
#include "VirtualMobilityManager.h"
#include "VirtualMac.h"
#include "BaselineBANMac.h"
#include "CastaliaModule.h"

#include "time.h"
#include <list>

using namespace std;

class PathLossElement {
public:
    int BodyPosition;
    simtime_t lastObservationTime;
};

class GoswamiChannel: public CastaliaModule {
private:

    /*--- variables corresponding to .ned file's parameters ---*/
    int numOfNodes;

    double xFieldSize;
    double yFieldSize;
    double zFieldSize;

    // variables corresponding to Wireless Channel module parameters
    struct PathLossParameters {
        double pathLossExponent;    // the path loss exponent
        double PLd0;                // Power loss at a reference distance d0 (in dBm)
        double d0 = 5;             // reference distance (in centimeters)
        double sigma;               // std of a zero-mean Gaussian RV
    };

    //Initialize the pathLoss parameters
    struct PathLossParameters pathLossMap[2];

    bool onlyStaticNodes;
    double receiverSensitivity;
    double maxTxPower;			// this is derived, by reading all the Tx power levels
    int numNodes;
    int hubID;
    /*--- other class member variables ---*/
    NodeLocation_type *nodeLocation;		// an array (numOfNodes long) that gives the
    // location for each node.
    double signalDeliveryThreshold;

    vector <vector <int>> nodesAffectedByTransmitter;	// an array of lists (numOfNodes long). The list
    // at array element i holds the node IDs that are
    // affected when node i transmits.

protected:
    virtual void initialize(int);
    virtual void handleMessage(cMessage * msg);
    virtual void finishSpecific();

    void readIniFileParameters(void);

    double calculatePathLoss(int src, int dst);

};

#endif				//_WIRELESSCHANNEL_H
