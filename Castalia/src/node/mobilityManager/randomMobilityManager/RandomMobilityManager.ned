//****************************************************************************
//*   Copyright (C) <2018>  <Vinicius C Ferreira>                            * 
//*   Developed at the Midiacom Laboratory - Universidade Federal Fluminense *
//*   This program is free software: you can redistribute it and/or modify   *
//*   it under the terms of the GNU General Public License as published by   *
//*   the Free Software Foundation, either version 3 of the License, or      *
//*   (at your option) any later version.                                    *
//*                                                                          *    
//* This program is distributed in the hope that it will be useful,          *
//* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
//* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
//* GNU General Public License for more details.                             *
//****************************************************************************/

package node.mobilityManager.randomMobilityManager;

simple RandomMobilityManager like node.mobilityManager.iMobilityManager {
 parameters:
	bool collectTraceInfo = default (false);
	double updateInterval = default (1000);
	double speed = default (1);
	double radius = default (0);

}
