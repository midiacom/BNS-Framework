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

#ifndef _MOBILITYMODULE_H_
#define _MOBILITYMODULE_H_

#include "MobilityManagerMessage_m.h"
#include "VirtualMobilityManager.h"

using namespace std;

class RandomMobilityManager: public VirtualMobilityManager {
 private:
	/*--- The .ned file's parameters ---*/
	int xlen;
	int ylen;
	int zlen;
	double updateInterval;
	double loc1_x;
	double loc1_y;
	double loc1_z;
	double speed;
	double radius;

	/*--- Custom class parameters ---*/
	double d_x;
	double d_y;
	double d_z;
	double incr_x;
	double incr_y;
	double incr_z;
	double distance;

 protected:
	void initialize();
	void randomMovement();
	void handleMessage(cMessage * msg);
};

#endif
