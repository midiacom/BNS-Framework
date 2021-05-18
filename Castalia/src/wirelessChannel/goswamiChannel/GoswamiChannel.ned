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

package wirelessChannel.goswamiChannel;

// The wireless channel module simulates the wireless medium. Nodes sent packets to it
// and according to various conditions (fading, interference etc) it is decided which
// nodes can receive this packet.
// This wireless channel is using the LogNormal with Normal Polatization channel from doi:10.1109/TAP.2012.2231917

simple GoswamiChannel like wirelessChannel.iWirelessChannel {
 parameters:
	bool collectTraceInfo = default (false);
	bool onlyStaticNodes = default (true);		// if NO mobility, set it to true for greater efficiency

	double signalDeliveryThreshold = default (-100);
												// threshold in dBm above which, wireless channel module
												// is delivering signal messages to radio modules of
												// individual nodes

 gates:
 	output toNode[];
	input fromMobilityModule @ directIn;
	input fromNode[];
}
