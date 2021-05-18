/****************************************************************************
 *   Copyright (C) <2018>  <Vinicius C Ferreira>                            * 
 *   Developed at the Midiacom Laboratory - Universidade Federal Fluminense *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation, either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *    
 * This program is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 * GNU General Public License for more details.                             *
 ****************************************************************************/

#include "RandomMobilityManager.h"

Define_Module(RandomMobilityManager);

void RandomMobilityManager::initialize()
{
	VirtualMobilityManager::initialize();
	xlen = network->par("field_x");
	ylen = network->par("field_y");
	zlen = network->par("field_z");
	speed = par("speed");
	radius = par("radius");
	updateInterval = par("updateInterval");
	updateInterval = updateInterval / 1000;
	loc1_x = nodeLocation.x;
	loc1_y = nodeLocation.y;
	loc1_z = nodeLocation.z;
	
	setLocation(loc1_x, loc1_y, loc1_z);
	scheduleAt(simTime() + updateInterval,
	new MobilityManagerMessage("Periodic location update message", MOBILITY_PERIODIC));
}

void RandomMobilityManager::randomMovement()
{
	// if radius is not defined the node can moves to the full extent of the simulation area
	if (radius == 0) {
		d_x = genk_intrand(0, xlen);
		d_y = genk_intrand(0, ylen);
		d_z = genk_intrand(0, zlen);
	// the movement is confined in a circle around the original deployment point
	} else {
		d_x = loc1_x + 2*radius*rand()/RAND_MAX - radius;
		d_y = loc1_y + 2*radius*rand()/RAND_MAX - radius;
		d_z = loc1_z + 2*radius*rand()/RAND_MAX - radius;
	}	
	distance = sqrt(pow(nodeLocation.x - d_x, 2) + pow(nodeLocation.y - d_y, 2) +
	 pow(nodeLocation.z - d_z, 2));
	if (speed > 0 && distance > 0) {
		double tmp = (distance / speed) / updateInterval;
		incr_x = (d_x - nodeLocation.x) / tmp;
		incr_y = (d_y - nodeLocation.y) / tmp;
		incr_z = (d_z - nodeLocation.z) / tmp;
	}
}	
	

void RandomMobilityManager::handleMessage(cMessage * msg)
{
	int msgKind = msg->getKind();
	switch (msgKind) {

		case MOBILITY_PERIODIC:
        {
            randomMovement();
            if (nodeLocation.x + incr_x > 0 && nodeLocation.x + incr_x < xlen) {
                nodeLocation.x += incr_x;
            } else if (nodeLocation.x + incr_x > xlen) {
                nodeLocation.x = xlen;
            } else {
                nodeLocation.x = 0;
            }

            if (nodeLocation.y + incr_y > 0 && nodeLocation.y + incr_y < ylen) {
                nodeLocation.y += incr_y;
            } else if (nodeLocation.y + incr_y > ylen) {
                nodeLocation.y = ylen;
            } else {
                nodeLocation.y = 0;
            }

            if (nodeLocation.z + incr_z > 0 && nodeLocation.z + incr_z < zlen) {
                nodeLocation.z += incr_z;
            } else if (nodeLocation.z + incr_z > zlen) {
                nodeLocation.z = zlen;
            } else {
                nodeLocation.z = 0;
            }

			notifyWirelessChannel();
			scheduleAt(simTime() + updateInterval,
				new MobilityManagerMessage("Periodic location update message", MOBILITY_PERIODIC));

			trace() << "changed location(x:y:z) to " << nodeLocation.x << 
					":" << nodeLocation.y << ":" << nodeLocation.z;
			break;
		}

		default:{
			trace() << "WARNING: Unexpected message " << msgKind;
		}
	}

	delete msg;
	msg = NULL;
}

