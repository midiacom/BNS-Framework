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

#include "MobanMobilityManager.h"

Define_Module(MobanMobilityManager);

void MobanMobilityManager::initialize() {
    VirtualMobilityManager::initialize();

    // Posture sequence and its changing time
//    statusSequence.push("STANDING");
//    statusSequence.push("WALKING");
//    statusSequence.push("RUNNING");
//    statusSequence.push("WALKING");
//    statusSequence.push("STANDING");
//    statusChangeTimeSequence.push(0);
//    statusChangeTimeSequence.push(120);
//    statusChangeTimeSequence.push(240);
//    statusChangeTimeSequence.push(360);
//    statusChangeTimeSequence.push(480);


    // NED file parameters
    // only for test purposes the status is initialized as a static variable here in cc
    // area size
    xlen = network->par("field_x");
    ylen = network->par("field_y");
    zlen = network->par("field_z");


    const char* inputBodyPlacement = par("bodyPlacement");
    bodyPlacement = (string)inputBodyPlacement;
    if (bodyPlacement == "None"){
        //initial position
        loc1_x = nodeLocation.x;
        loc1_y = nodeLocation.y;
        loc1_z = nodeLocation.z;

        // destination of linear movement
        loc2_x = par("xCoorDestination");
        loc2_y = par("yCoorDestination");
        loc2_z = par("zCoorDestination");

        // location variable
        loc_x = loc1_x;
        loc_y = loc1_y;
        loc_z = loc1_z;

        movementRange = (double) par("movementRange");

        // movement control variables
        direction = 1;
        tmp = 0;

        //initialize location
        setLocation(loc_x, loc_y, loc_z, 0, 0, nodePlacement);

    } else {
        // initialize position based on bodyPlacement
        initPosition(bodyPlacement);

    }

    // linear speed - for WALKING or RUNNING
    walkingFreq = (double) par("walkingFreq")/1000.0;
    runningFreq = (double) par("runningFreq")/1000.0;

    // area of random movement
    rand_area = par("randomArea");

    // update interval for the movement - influences on wireless channel notification
    updateInterval = par("updateInterval");
    updateInterval = updateInterval / 1000;

    // initialize postures
    statusSequence.push("WALKING");
    statusChangeTimeSequence.push(0);
    updateStatus();

    // schedule update timer
    scheduleAt(simTime() + updateInterval,
               new MobilityManagerMessage("Location update message", MOBILITY_PERIODIC));

}

void MobanMobilityManager::handleMessage(cMessage * msg) {
    int msgKind = msg->getKind();
    switch (msgKind) {

        case MOBILITY_PERIODIC: {

            moveNode();
            notifyWirelessChannel();

            scheduleAt(simTime() + updateInterval,
                       new MobilityManagerMessage("Location update message", MOBILITY_PERIODIC));
            break;
        }

        case UPDATE_STATUS: {
            updateStatus();
            break;
        }

        default: {
            trace() << "WARNING: Unexpected message " << msgKind;
        }
    }

    delete msg;
    msg = NULL;
}

void MobanMobilityManager::moveNode(){

    if ( status == "WALKING" || status == "RUNNING") {
        if ( nodePlacement == 1 || nodePlacement == 2 || nodePlacement == 4 || nodePlacement == 5 ||
        nodePlacement == 8 || nodePlacement == 9 || nodePlacement == 10 || nodePlacement == 11 ) {

            zigZagMovement();

        }

        //groupMovement();
    }
    //randomMovement();

    checkMovementBoundaries();
}

void MobanMobilityManager::groupMovement() {
    // must move nodes using groupSpeed
    // to be developed
}

// Move node inside rand_area around it's loc
// if rand_area is not defined the node can moves to the full extent of the simulation area
void MobanMobilityManager::randomMovement() {

    double d_x = genk_intrand(0, xlen);
    double d_y = genk_intrand(0, ylen);
    double d_z = genk_intrand(0, zlen);

    if (rand_area > 0) {
        // the movement is confined in a circle around the point
        d_x = 2*rand_area*rand()/RAND_MAX - rand_area;
        d_y = 2*rand_area*rand()/RAND_MAX - rand_area;
        d_z = 2*rand_area*rand()/RAND_MAX - rand_area;
    }

    loc_x += d_x;
    loc_y += d_y;
    loc_z += d_z;
}


// Periodic movement between loc1 and loc2
void MobanMobilityManager::zigZagMovement() {

    if(tmp >= distance){
        direction = -1*direction;
        tmp = 0;
    }

    loc_x += direction*incr_x;
    loc_y += direction*incr_y;
    loc_z += direction*incr_z;

    tmp = tmp + desloc;

}


//check's if movement goes beyond simulation field and restricts it to its boundaries
void MobanMobilityManager::checkMovementBoundaries() {

    if (loc_x > 0 && loc_x < xlen) {
        nodeLocation.x = loc_x;
    } else if (loc_x > xlen) {
        nodeLocation.x = xlen;
    } else {
        nodeLocation.x = 0;
    }

    if (loc_y > 0 && loc_y < ylen) {
        nodeLocation.y = loc_y;
    } else if (loc_y > ylen) {
        nodeLocation.y = ylen;
    } else {
        nodeLocation.y = 0;
    }

    if (loc_z > 0 && loc_z < zlen) {
        nodeLocation.z = loc_z;
    } else if (loc_z > zlen) {
        nodeLocation.z = zlen;
    } else {
        nodeLocation.z = 0;
    }

    trace() << "New node location: " << nodeLocation.x << " , " << nodeLocation.y << " , " << nodeLocation.z;
}

// Update's posture based on queue sequence and timing
void MobanMobilityManager::updateStatus() {
    if(statusChangeTimeSequence.front() == 0){
        trace() << "Initializing posture status";
    }
    status = statusSequence.front();
    setSpeed();
    statusSequence.pop();
    statusChangeTimeSequence.pop();

    trace() << "Status set as: " << status;
    if (!statusSequence.empty()){
        trace() << "Next update in: " << statusChangeTimeSequence.front() << " s to : " << statusSequence.front();
        scheduleAt(statusChangeTimeSequence.front(), new MobilityManagerMessage("Status update message", UPDATE_STATUS));
    }
}

// Set's up node speed based on posture status
void MobanMobilityManager::setSpeed() {
    if(status == "WALKING") {
        trace() << "wakingFreq: " << walkingFreq << " , movementRange: " << movementRange;
        speed = walkingFreq*movementRange;
    } else if( status == "RUNNING"){
        trace() << "runningFreq: " << runningFreq << " , movementRange: " << movementRange;
        speed = runningFreq*movementRange;
    } else {
        trace() << "Not walking nor running,";
        speed = 0;
    }
    trace() << "Speed updated to: " << speed;
    // one step of linear movement for an update interval
//    distance = sqrt(pow(loc1_x - loc2_x, 2) + pow(loc1_y - loc2_y, 2) +
//                    pow(loc1_z - loc2_z, 2));
    distance = movementRange;
    if (speed > 0 && distance > 0) {
        desloc = speed*updateInterval;
        double desloc_ratio = desloc/distance;
        incr_x = (loc2_x - loc1_x) * desloc_ratio;
        incr_y = (loc2_y - loc1_y) * desloc_ratio;
        incr_z = (loc2_z - loc1_z) * desloc_ratio;
    }

    trace() << "incr_x, incr_y, incr_z: " << incr_x << " , " << incr_y << " , " << incr_z;
}

// Initialize node coordinates based on body placement string
void MobanMobilityManager::initPosition(string bodyPlacement) {
    np = nodePlacementMap.find(bodyPlacement);

    if(np != nodePlacementMap.end()){
        nodePlacement = np->second;
        nl = nodeLocationMap.find(nodePlacement);
        loc_x = std::get<0>(nl->second);
        loc_y = std::get<1>(nl->second);
        loc_z = std::get<2>(nl->second);

        if (nodePlacement == 1 || nodePlacement == 4){
            movementRange = 40;
        }

        if (nodePlacement == 2 || nodePlacement == 5){
            movementRange = 100;
        }
        if (nodePlacement == 8 || nodePlacement == 10){
            movementRange = 100;
        }
        if (nodePlacement == 9 || nodePlacement == 11){
            movementRange = 200;
        }

        trace() << "Movement range updated to: " << movementRange;

        int dir = 1;
        if (nodePlacement == 4 || nodePlacement == 5
        || nodePlacement == 8 || nodePlacement == 9){
            dir = -1;
        }

        loc1_x = loc_x;
        loc2_x = loc_x;

        loc1_y = loc_y;
        loc2_y = loc_y;

        loc1_z = loc_z + dir*movementRange/2;
        loc2_z = loc_z - dir*movementRange/2;

        // movement control variables
        direction = 1;
        tmp = movementRange/2;

        setLocation(loc_x, loc_y, loc_z, 0, 0, nodePlacement);
        trace() << "Body placement found: " << bodyPlacement << " nodePlacement: " << nodePlacement;
        trace() << "Location initialized as (x, y, z): " << loc_x << " , " << loc_y << " , " << loc_z;

    } else {
        trace() << "WARNING: Body placement not found. The node's x, y, z coordinates must be provided.";
    }


}
