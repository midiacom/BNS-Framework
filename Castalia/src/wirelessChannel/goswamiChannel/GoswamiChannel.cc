//    This is a wireless channel simulation for Castalia Simulator based on Goswami et. al (doi:10.1049/htl.2016.0005)
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

#include "GoswamiChannel.h"

Define_Module(GoswamiChannel);

void GoswamiChannel::initialize(int stage) {
//    if (stage == 0) {
//        readIniFileParameters();
//        return;
//    }

    readIniFileParameters();
    /* variable to report initialization run time */
    clock_t startTime;
    startTime = clock();

    /* initialize nodeLocation */
    nodeLocation = new NodeLocation_type[numOfNodes];
    if (nodeLocation == NULL)
        opp_error("Could not allocate array nodeLocation\n");

    cTopology *topo;	// temp variable to access initial location of the nodes
    topo = new cTopology("topo");
    topo->extractByNedTypeName(cStringTokenizer("node.Node").asVector());

    for (int i = 0; i < numOfNodes; i++) {
        VirtualMobilityManager *nodeMobilityModule =
                check_and_cast<VirtualMobilityManager*>
                        (topo->getNode(i)->getModule()->getSubmodule("MobilityManager"));

        // This module was designed to work with BaselineBANMac - it needs the mac layer hub node id to perform its functions
        VirtualMac *virtualMacModule = check_and_cast<VirtualMac*>
                (topo->getNode(i)->getModule()->getSubmodule("Communication")->getSubmodule("MAC"));

        BaselineBANMac* baselineBanMac = dynamic_cast<BaselineBANMac*>(virtualMacModule);
//        if (baselineBanMac){
//            opp_error("\n GoswamiChannel initialize: Error in getting a valid baselineBANMac module.");
//        }
        if (baselineBanMac->getIsHub()) {
            hubID = i;
        }
    }

    trace() << "GoswamiChannel initialized. NodeLocation concluded. Network Hub ID is: " << hubID;

    delete(topo);


    for (int i = 0; i < numOfNodes; i++){
        vector<int> v;
        nodesAffectedByTransmitter.push_back(v);
    }


    // Log normal parameters from Goswami et al:(doi: 10.1049/htl.2016.0005) of a Normal subject using channel 6
    //LOS Channel
    pathLossMap[0].PLd0 = 36;
    pathLossMap[0].d0 = 5;
    pathLossMap[0].pathLossExponent = 3.2;
    pathLossMap[0].sigma = 7.9;

    //NLOS Channel
    pathLossMap[1].PLd0 = 48;
    pathLossMap[1].d0 = 5;
    pathLossMap[1].pathLossExponent = 2.3;
    pathLossMap[1].sigma = 6.26;


    trace() << "Time for Goswami Channel module initialization: " <<
            (double)(clock() - startTime) / CLOCKS_PER_SEC << "secs";
}


// readIniParameters
void GoswamiChannel::readIniFileParameters(void) {
    DebugInfoWriter::setDebugFileName(
            getParentModule()->par("debugInfoFileName").stringValue());

    onlyStaticNodes = par("onlyStaticNodes");
    signalDeliveryThreshold = par("signalDeliveryThreshold");

    numOfNodes = getParentModule()->par("numNodes");
    xFieldSize = getParentModule()->par("field_x");
    yFieldSize = getParentModule()->par("field_y");
    zFieldSize = getParentModule()->par("field_z");
    maxTxPower = 6.0; // power used in the experiments

}


/*****************************************************************************
 * This is where the main work is done by processing all the messages received
 *****************************************************************************/

void GoswamiChannel::handleMessage(cMessage * msg) {
    switch (msg->getKind()) {

        case WC_NODE_MOVEMENT:{
            /*****************************************************
             * A node notified the wireless channel that it moved
             * Update the nodeLocation and check if it is valid
             *****************************************************/

            WirelessChannelNodeMoveMessage *mobilityMsg = check_and_cast <WirelessChannelNodeMoveMessage*>(msg);
            int srcAddr = mobilityMsg->getNodeID();

            if (onlyStaticNodes)
                opp_error("Error: Rerceived WS_NODE_MOVEMENT msg, while onlyStaticNodes is TRUE");
            trace() << "WC_NODE_MOVEMENT message arrived for node: " << srcAddr;
            trace() << "prior node location (x,y,z): " << nodeLocation[srcAddr].x << " , " << nodeLocation[srcAddr].y << " , " << nodeLocation[srcAddr].z;
            nodeLocation[srcAddr].x = mobilityMsg->getX();
            nodeLocation[srcAddr].y = mobilityMsg->getY();
            nodeLocation[srcAddr].z = mobilityMsg->getZ();
            nodeLocation[srcAddr].phi = mobilityMsg->getPhi();
            nodeLocation[srcAddr].theta = mobilityMsg->getTheta();
            nodeLocation[srcAddr].nodePlacement = mobilityMsg->getNodePlacement();
            trace() << "New node location (x,y,z): " << nodeLocation[srcAddr].x << " , " << nodeLocation[srcAddr].y << " , " << nodeLocation[srcAddr].z;
            if ((nodeLocation[srcAddr].x < 0.0) ||
                (nodeLocation[srcAddr].y < 0.0) ||
                (nodeLocation[srcAddr].z < 0.0))
                opp_error("Wireless channel received faulty WC_NODE_MOVEMENT msg. We cannot have negative node coordinates");

            if (nodeLocation[srcAddr].x > xFieldSize)
                debug() << "WARNING at WC_NODE_MOVEMENT: node position out of bounds in X dimension!\n";
            if (nodeLocation[srcAddr].y > yFieldSize)
                debug() << "WARNING at WC_NODE_MOVEMENT: node position out of bounds in Y dimension!\n";
            if (nodeLocation[srcAddr].z > zFieldSize)
                debug() << "WARNING at WC_NODE_MOVEMENT: node position out of bounds in Z dimension!\n";

            break;
        }

        case WC_SIGNAL_START:{

            WirelessChannelSignalBegin *signalMsg =
                    check_and_cast <WirelessChannelSignalBegin*>(msg);
            int srcAddr = signalMsg->getNodeID();
            int receptioncount = 0;

            double currentPathLoss;

            for (int i = 0; i < numOfNodes; i++) {
                if (i == srcAddr) continue;
                double signalPower = signalMsg->getPower_dBm();
                currentPathLoss = calculatePathLoss(srcAddr, i);
                signalPower -= currentPathLoss;

                if (signalPower < signalDeliveryThreshold) {
                    collectOutput("Transmission failed","Below sensivity");
                    continue;
                }

                collectOutput("Transmission success","OK");

                WirelessChannelSignalBegin *signalMsgCopy = signalMsg->dup();
                signalMsgCopy->setPower_dBm(signalPower);
                send(signalMsgCopy, "toNode", i);
                nodesAffectedByTransmitter[srcAddr].push_back(i);
                receptioncount++;
            }

            if (receptioncount > 0)
                trace() << "signal from node[" << srcAddr << "] reached " <<
                        receptioncount << " other nodes";
            break;
        }

        case WC_SIGNAL_END:{
            WirelessChannelSignalEnd *signalMsg =
                    check_and_cast <WirelessChannelSignalEnd*>(msg);
            int srcAddr = signalMsg->getNodeID();


            /* Go through the list of nodes that were affected
             *  by this transmission. *it1 holds the node ID
             */

            for (int i = 0; i < nodesAffectedByTransmitter[srcAddr].size(); i++) {
                WirelessChannelSignalEnd *signalMsgCopy = signalMsg->dup();
                send(signalMsgCopy, "toNode", nodesAffectedByTransmitter[srcAddr][i]);
            }	//for it1

            /* Now that we are done processing the msg we delete the whole list
             * nodesAffectedByTransmitter[srcAddr], since srcAddr in not TXing anymore.
             */
            nodesAffectedByTransmitter[srcAddr].clear();
            break;
        }

        default:{
            opp_error("ERROR: Wireless Channel received unknown message kind=%i", msg->getKind());
            break;
        }
    }
    delete msg;
}

void GoswamiChannel::finishSpecific()
{

    /*****************************************************
     * Delete dynamically allocated arrays. Some allocate
     * lists of objects so they need an extra nested loop
     * to properly deallocate all these objects
     *****************************************************/

    /* delete nodeLocation */
    delete[]nodeLocation;

    //close the output stream that CASTALIA_DEBUG is writing to
    DebugInfoWriter::closeStream();
}

double GoswamiChannel::calculatePathLoss(int src, int dst) {

    int line_of_sight = 0; // LOS

    if (src == hubID ){
        if (nodeLocation[src].z - nodeLocation[dst].z > 0){
            line_of_sight = 1; //NLOS
        }
    } else if(dst == hubID){
        if (nodeLocation[dst].z - nodeLocation[src].z > 0){
            line_of_sight = 1; //NLOS
        }
    } else {
        //relative distance to hub
        double src_hub_z = nodeLocation[src].z - nodeLocation[hubID].z;
        double dst_hub_z = nodeLocation[dst].z - nodeLocation[hubID].z;
        if (src_hub_z*dst_hub_z < 0){
            line_of_sight = 1; // NLOS
        }
    }

    struct PathLossParameters currentPathLossParameters = pathLossMap[line_of_sight];


    float dist = sqrt((nodeLocation[src].x - nodeLocation[dst].x) * (nodeLocation[src].x - nodeLocation[dst].x) +
                        (nodeLocation[src].y - nodeLocation[dst].y) * (nodeLocation[src].y - nodeLocation[dst].y)
                        + (nodeLocation[src].z - nodeLocation[dst].z) * (nodeLocation[src].z - nodeLocation[dst].z));

    if (dist < currentPathLossParameters.d0){
        dist = currentPathLossParameters.d0;
    }

    float pathLoss = currentPathLossParameters.PLd0 +
            10.0 * currentPathLossParameters.pathLossExponent * log10(dist / currentPathLossParameters.d0) +
            normal(0, currentPathLossParameters.sigma);

    trace() << "------- GOSWAMI CHANNEL -----------------";
    trace() << "Src node: " << src << " Location: " << nodeLocation[src].x << " , " << nodeLocation[src].y << " , " <<
            nodeLocation[src].z;
    trace() << "Dst node: " << dst << " Location: " << nodeLocation[dst].x << " , " << nodeLocation[dst].y << " , " <<
            nodeLocation[dst].z;
    trace() << "Transmission is LOS (0) or NLOS (1): " << line_of_sight;
    trace() << "PathLoss is: " << pathLoss;

    return pathLoss;

}