/****************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2010                         *
 *  Developed at the ATP lab, Networked Systems research theme              *
 *  Author(s): Athanassios Boulis, Yuriy Tselishchev                        *
 *  This file is distributed under the terms in the attached LICENSE file.  *
 *  If you do not find this file, copies can be found by writing to:        *
 *                                                                          *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
 *      Attention:  License Inquiry.                                        *
 *                                                                          *
 ****************************************************************************/

#include "BaselineBANMac.h"

Define_Module(BaselineBANMac);

void BaselineBANMac::startup() {
	isHub = par("isHub");
	if (isHub) {
		connectedHID =  SELF_MAC_ADDRESS % (2<<16); // keep the 16 LS bits as a short address
		connectedNID = connectedHID; // default value, usually overwritten
		trace() << "Selected random HID " << connectedHID;		
		currentFreeConnectedNID = 1; // start assigning connected NID from ID 1
		allocationSlotLength = (double) par("allocationSlotLength")/1000.0; // convert msec to sec
		beaconPeriodLength = par("beaconPeriodLength");
		RAP1Length = par("RAP1Length");
		currentFirstFreeSlot = RAP1Length +1;
		setTimer(SEND_BEACON,0);
		lastTxAccessSlot = new AccessSlot[256];
		reqToSendMoreData = new int[256];
		for (int i=0; i<256; i++) {lastTxAccessSlot[i].scheduled=0; lastTxAccessSlot[i].polled=0; reqToSendMoreData[i]=0;}
	} else {
	    isRelay = par("isRelay");
		connectedHID = UNCONNECTED;
 		connectedNID = UNCONNECTED;
		unconnectedNID = 300 + SELF_MAC_ADDRESS;    //we select random unconnected NID
		trace() << "Selected random unconnected NID " << unconnectedNID;
		scheduledAccessLength = par("scheduledAccessLength");
		scheduledAccessPeriod = par("scheduledAccessPeriod");
		pastSyncIntervalNominal = false;
		macState = MAC_SETUP;
		SInominal = -1;

	}

	relayPar = par("relayPar");
	neighborhood.setOrdenador(relayPar);
	pMIFS = (double) par("pMIFS")/1000.0;
	pSIFS = (double) par("pSIFS")/1000.0;
	ackTimeout = (double) par("ackTimeout")/1000.0;
	pTimeSleepToTX = (double) par("pTimeSleepToTX")/1000.0;
	isRadioSleeping = false;
	phyLayerOverhead = par("phyLayerOverhead");
	phyDataRate = par("phyDataRate");
	priority = getParentModule()->getParentModule()->getSubmodule("Application")->par("priority");
	mClockAccuracy = par("mClockAccuracy");
	enhanceGuardTime = par("enhanceGuardTime");
	enhanceMoreData = par("enhanceMoreData");
	pollingEnabled = par("pollingEnabled");
	naivePollingScheme = par("naivePollingScheme");
	enableRAP = par("enableRAP");
	sendIAckPoll = false;	// only used by Hub, but must be initialized for all
	currentSlot = -1;		// only used by Hub
	nextFuturePollSlot = -1;	// only used by Hub

	contentionSlotLength = (double) par("contentionSlotLength")/1000.0; // convert msec to sec;
	maxPacketTries = par("maxPacketTries");

	CW = CWmin[priority];
	CWdouble = false;
	backoffCounter = 0;

	packetToBeSent = NULL;
	currentPacketTransmissions = 0;
	currentPacketCSFails = 0;
	waitingForACK = false;
	futureAttemptToTX = false;
	attemptingToTX = false;
	isPollPeriod = false;

 	scheduledTxAccessStart = UNCONNECTED;
 	scheduledTxAccessEnd = UNCONNECTED;
 	scheduledRxAccessStart = UNCONNECTED;
 	scheduledRxAccessEnd = UNCONNECTED;

	declareOutput("Data pkt breakdown");
	declareOutput("Mgmt & Ctrl pkt breakdown");
	declareOutput("pkt TX state breakdown");
	declareOutput("Beacons received");
	declareOutput("Beacons sent");
	declareOutput("var stats");
    declareOutput("MAC Relay");
    declareOutput("Polls given");
}

void BaselineBANMac::timerFiredCallback(int index) {
	switch (index) {

		case CARRIER_SENSING: {
			if (!canFitTx()) {
				attemptingToTX = false;
				currentPacketCSFails++;
				break;
			}
			CCA_result CCAcode = radioModule->isChannelClear();
			if (CCAcode == CLEAR) {
				if (backoffCounter > 0){
                    backoffCounter--;
                    setTimer(CARRIER_SENSING, contentionSlotLength);
				}
				else {
					sendPacket();
				}
			} else if (CCAcode == BUSY) {
				/* spec states that we wait until the channel is not busy
				 * we cannot simply do that, we have to have periodic checks
				 * we arbitrarily choose 3*contention slot = 1.08 msec
				 * The only way of failing because of repeated busy signals
				 * is to eventually not fit in the current RAP
				 */
				setTimer(CARRIER_SENSING, 3*contentionSlotLength);
            } else if (CCAcode == CS_NOT_VALID_YET) {
                //Clear Channel Assesment (CCA) pin is not valid yet
                setTimer(CARRIER_SENSING, 3*contentionSlotLength);
            } else {
            //Clear Channel Assesment (CCA) pin is not valid at all (radio is sleeping?)
                trace() << "ERROR: isChannelClear() called when radio is not ready";
                toRadioLayer(createRadioCommand(SET_STATE, RX));
            }
			break;
		}

		case START_ATTEMPT_TX: {
			futureAttemptToTX = false;
			attemptTX();
			break;
		}

		case ACK_TIMEOUT: {
			trace() << "ACK timeout fired";
			waitingForACK = false;

			// double the Contention Window, after every second fail.
			CWdouble ? CWdouble=false : CWdouble=true;
			if ((CWdouble) && (CW < CWmax[priority])) CW *=2;

			// check if we reached the max number and if so delete the packet
			if (currentPacketTransmissions + currentPacketCSFails == maxPacketTries) {
				// collect statistics
				if (packetToBeSent != NULL){
                    //CASE OF CONN REQ E ASSIGNMENT ARE NULL - NOT DATA PACKETS - THIS CASE SHOULD BE TREATED
                    if (packetToBeSent->getFrameType() == DATA) {
                        collectOutput("Data pkt breakdown", "Failed, No Ack");
                    } else collectOutput("Mgmt & Ctrl pkt breakdown", "Failed, No Ack");
                    cancelAndDelete(packetToBeSent);
                    packetToBeSent = NULL;
				}
				currentPacketTransmissions = 0;
				currentPacketCSFails = 0;
			}
			attemptTX();
			break;
		}

		case START_SLEEPING: {
			trace() << "State from "<< macState << " to MAC_SLEEP";
			macState = MAC_SLEEP;
			toRadioLayer(createRadioCommand(SET_STATE,SLEEP));   isRadioSleeping = true;
			isPollPeriod = false;
			break;
		}

		case START_SCHEDULED_TX_ACCESS: {
			trace() << "State from "<< macState << " to MAC_FREE_TX_ACCESS (scheduled)";
			macState = MAC_FREE_TX_ACCESS;
			endTime = getClock() + (scheduledTxAccessEnd - scheduledTxAccessStart) * allocationSlotLength;
			if (beaconPeriodLength > scheduledTxAccessEnd)
				setTimer(START_SLEEPING, (scheduledTxAccessEnd - scheduledTxAccessStart) * allocationSlotLength);
			attemptTX();
			break;
		}

		case START_SCHEDULED_RX_ACCESS: {
			trace() << "State from "<< macState << " to MAC_FREE_RX_ACCESS (scheduled)";
			macState = MAC_FREE_RX_ACCESS;
			toRadioLayer(createRadioCommand(SET_STATE,RX));  isRadioSleeping = false;
			if (beaconPeriodLength > scheduledRxAccessEnd)
				setTimer(START_SLEEPING, (scheduledRxAccessEnd - scheduledRxAccessStart) * allocationSlotLength);
			break;
		}

		case START_POSTED_ACCESS: {
			trace() << "State from "<< macState << " to MAC_FREE_RX_ACCESS (post)";
			macState = MAC_FREE_RX_ACCESS;
			toRadioLayer(createRadioCommand(SET_STATE,RX));  isRadioSleeping = false;
			// reset the timer for sleeping as needed
			if ((postedAccessEnd-1) != beaconPeriodLength &&
				postedAccessEnd != scheduledTxAccessStart && postedAccessEnd != scheduledRxAccessStart){
				// we could set the timer with the following ways:
				//setTimer(START_SLEEPING, frameStartTime + ((postedAccessEnd-1) * allocationSlotLength) - getClock());
				//setTimer(START_SLEEPING, (postedAccessEnd-postedAccessStart)* allocationSlotLength);
				// but this is simpler, since the duration is always 1 slot
				setTimer(START_SLEEPING, allocationSlotLength);
			}else cancelTimer(START_SLEEPING);
			break;
		}

		case WAKEUP_FOR_BEACON: {
			trace() << "State from "<< macState << " to MAC_BEACON_WAIT";
			macState = MAC_BEACON_WAIT;
			toRadioLayer(createRadioCommand(SET_STATE,RX));  isRadioSleeping = false;
			isPollPeriod = false;
			break;
		}

		case SYNC_INTERVAL_TIMEOUT: {
			pastSyncIntervalNominal = true;
			syncIntervalAdditionalStart = getClock();
			break;
		}

		case START_SETUP: {
			macState = MAC_SETUP;
			break;
		}

		// The rest of the timers are specific to a Hub
		case SEND_BEACON: {
			trace() << "BEACON SEND, next beacon in " << beaconPeriodLength * allocationSlotLength;
			trace() << "State from "<< macState << " to MAC_RAP";
			macState = MAC_RAP;
			// We should provide for the case of the Hub sleeping. Here we ASSUME it is always ON!
			setTimer(SEND_BEACON, beaconPeriodLength * allocationSlotLength);
			setTimer(HUB_SCHEDULED_ACCESS, RAP1Length * allocationSlotLength);
			// the hub has to set its own endTime
			endTime = getClock() + RAP1Length * allocationSlotLength;

			BaselineBeaconPacket * beaconPkt = new BaselineBeaconPacket("BaselineBAN beacon",MAC_LAYER_PACKET);
			setHeaderFields(beaconPkt,N_ACK_POLICY,MANAGEMENT,BEACON,BROADCAST_NID);

			beaconPkt->setAllocationSlotLength((int)(allocationSlotLength*1000000));
			beaconPkt->setBeaconPeriodLength(beaconPeriodLength);
			beaconPkt->setRAP1Length(RAP1Length);
			beaconPkt->setByteLength(BASELINEBAN_BEACON_SIZE);

			toRadioLayer(beaconPkt);
			toRadioLayer(createRadioCommand(SET_STATE,TX));  isRadioSleeping = false;

			// read the long comment in sendPacket() to understand why we add 2*pMIFS
			setTimer(START_ATTEMPT_TX, (TX_TIME(beaconPkt->getByteLength()) + 2*pMIFS));
			futureAttemptToTX = true;

			collectOutput("Beacons sent");
			// keep track of the current slot and the frame start time
			frameStartTime = getClock();
			currentSlot = 1;
			setTimer(INCREMENT_SLOT, allocationSlotLength);
			// free slots for polls happen after RAP and scheduled access
			nextFuturePollSlot = currentFirstFreeSlot;
			// if implementing a naive polling scheme, we will send a bunch of future polls in the fist free slot for polls
			if (naivePollingScheme && pollingEnabled && nextFuturePollSlot <= beaconPeriodLength)
				setTimer(SEND_FUTURE_POLLS, (nextFuturePollSlot-1) * allocationSlotLength);
			break;
		}

		case SEND_FUTURE_POLLS: {
			trace() << "State from "<< macState << " to MAC_FREE_TX_ACCESS (send Future Polls)";
			macState = MAC_FREE_TX_ACCESS;
			// when we are in a state that we can TX, we should *always* set endTime
			endTime = getClock() + allocationSlotLength;

			// The current slot is used to TX the future polls, so we have 1 less slot available
			int availableSlots = beaconPeriodLength - (currentSlot-1) -1;
			if (availableSlots <= 0) break;

			int totalRequests = 0;
			// Our (immediate) polls should start one slot after the current one.
			int nextPollStart = currentSlot +1;
			for (int nid=0; nid<256; nid++) {
			    totalRequests += reqToSendMoreData[nid];
			}

			if (totalRequests == 0) {
			    break;
			}

			for (int i=0; i<256; i++) {
				if (reqToSendMoreData[i] > 0) {
					// a very simple assignment scheme. It can leave several slots unused
					int slotsGiven = floor(((float)reqToSendMoreData[i]/(float)totalRequests)*availableSlots);
					trace() << "REQ["<<i<<"]= "<<reqToSendMoreData[i]<<", total REQ= "<<totalRequests<<", available slots= "<<availableSlots;
					if (slotsGiven == 0) continue;
					TimerInfo t; t.NID=i; t.slotsGiven=slotsGiven; t.endSlot=nextPollStart + slotsGiven -1;
					hubPollTimers.push(t);
					reqToSendMoreData[i] = 0; // reset the requested resources
					// create the future POLL packet and buffer it
					BaselineMacPacket *pollPkt = new BaselineMacPacket("BaselineBAN Future Poll", MAC_LAYER_PACKET);
					setHeaderFields(pollPkt,N_ACK_POLICY,MANAGEMENT,POLL,i);
					pollPkt->setSequenceNumber(nextPollStart);
					pollPkt->setFragmentNumber(0);
					pollPkt->setMoreData(1);
					pollPkt->setByteLength(BASELINEBAN_HEADER_SIZE);
					trace() << "Created future POLL for NID:" << i << ", for slot "<< nextPollStart;
					nextPollStart += slotsGiven;
					//collectOutput("Polls given", nid);
					MgmtBuffer.push(pollPkt);
				}
			}
			// the first poll will be send one slot after the current one.
			if (!hubPollTimers.empty()) setTimer(SEND_POLL, allocationSlotLength);
			// TX all the future POLL packets created
			attemptTX();
			break;
		}

		case SEND_POLL: {
			if (hubPollTimers.empty()) {trace() << "WARNING: timer SEND_POLL with hubPollTimers NULL"; break;}
			trace() << "State from "<< macState << " to MAC_FREE_RX_ACCESS (Poll)";
			macState = MAC_FREE_RX_ACCESS;
			// we set the state to RX but we also need to send the POLL message.
			TimerInfo t = hubPollTimers.front();
			int slotsGiven = t.slotsGiven;
			BaselineMacPacket *pollPkt = new BaselineMacPacket("BaselineBAN Immediate Poll", MAC_LAYER_PACKET);
			setHeaderFields(pollPkt,N_ACK_POLICY,MANAGEMENT,POLL,t.NID);
			pollPkt->setSequenceNumber(t.endSlot);
			pollPkt->setFragmentNumber(0);
			pollPkt->setMoreData(0);
			pollPkt->setByteLength(BASELINEBAN_HEADER_SIZE);
			toRadioLayer(pollPkt);
			toRadioLayer(createRadioCommand(SET_STATE,TX)); isRadioSleeping = false;

			collectOutput("var stats", "poll slots given", t.slotsGiven);
			trace() << "POLL for NID: "<< t.NID <<", ending at slot: "<< t.endSlot << ", lasting: " << t.slotsGiven << " slots";
			hubPollTimers.pop();
			// if there is another poll then it will come after this one, so scheduling the timer is easy
			if (hubPollTimers.size() > 0) setTimer(SEND_POLL, slotsGiven * allocationSlotLength);
			break;
		}

		case INCREMENT_SLOT: {
			currentSlot++;
			if (currentSlot < beaconPeriodLength) setTimer(INCREMENT_SLOT, allocationSlotLength);
			break;
		}

		case HUB_SCHEDULED_ACCESS: {
			trace() << "State from "<< macState << " to MAC_FREE_RX_ACCESS (hub)";
			macState = MAC_FREE_RX_ACCESS;
			// we should look at the schedule and setup timers to get in and out
			// of MAC_FREE_RX_ACCESS MAC_FREE_TX_ACCESS and finally MAC_SLEEP
			break;
		}
	}
}


void BaselineBANMac::fromNetworkLayer(cPacket *pkt, int dst) {
	BaselineMacPacket *BaselineBANDataPkt = new BaselineMacPacket("BaselineBAN data packet",MAC_LAYER_PACKET);
	encapsulatePacket(BaselineBANDataPkt,pkt);
	setHeaderFields(BaselineBANDataPkt,I_ACK_POLICY,DATA,RESERVED,dst);
	trace() << "Packet received from network layer, making frame. Frame type: " << BaselineBANDataPkt->getFrameType() << " from: " << BaselineBANDataPkt->getSource() << " to: " << BaselineBANDataPkt->getDestination();

    int nextHop = neighborhood.getRelay(dst);

	if (nextHop == connectedHID || nextHop == dst || nextHop == BROADCAST_NID || neighborhood.getOrdenador() == 0) {
		if (bufferPacket(BaselineBANDataPkt)) {
			attemptTX();
		} else {
			trace() << "WARNING BaselineBAN MAC buffer overflow";
			collectOutput("Data pkt breakdown", "Fail, buffer overflow");
		}
	} else {
        trace() << "Next hop is: " << nextHop << " is it my hub? " << (nextHop == connectedHID);
		BaselineMacPacket *BaselineBANRelayPkt = new BaselineMacPacket("BaselineBAN data packet", MAC_LAYER_PACKET);
		encapsulateRelayedPacket(BaselineBANRelayPkt, BaselineBANDataPkt);
		setHeaderFields(BaselineBANRelayPkt, I_ACK_POLICY, DATA, RESERVED, nextHop);
		BaselineBANRelayPkt->setBANSecurity(1);
		if (bufferPacket(BaselineBANRelayPkt)) {
			attemptTX();
			collectOutput("MAC Relay", "Sensor sending");
		} else {
			trace() << "WARNING BaselineBAN MAC buffer overflow";
			collectOutput("Data pkt breakdown", "Fail, buffer overflow");
		}
	}
}

void BaselineBANMac::fromRadioLayer(cPacket *pkt, double rssi, double lqi) {
	// if the incoming packet is not BaselineBAN, return (VirtualMAC will delete it)
	BaselineMacPacket * BaselineBANPkt = dynamic_cast<BaselineMacPacket*>(pkt);
	if (BaselineBANPkt == NULL) return;

    // update Neighborhood
//    if (connectedHID != UNCONNECTED && BaselineBANPkt->getBAN() == connectedHID){
//        neighborhood.updateNeighborhood(BaselineBANPkt->getSource(), rssi, simTime().dbl(), true, true);
//    } else {
//        neighborhood.updateNeighborhood(BaselineBANPkt->getSource(), rssi, simTime().dbl(), true, false);
//    }


    // filter the incoming BaselineBAN packet
    if (!isPacketForMe(BaselineBANPkt)) return;

	/* Handle data packets */
	if (BaselineBANPkt->getFrameType() == DATA) {
		if (isNotDuplicatePacket(BaselineBANPkt)){
            // checks if it is a relay packet
            if (BaselineBANPkt->getBANSecurity() == 1) {
                trace() << "Relay - frame received.";
                if (isHub) {
                    // just decapsulate and keep the processing - CHECK IF IT IS SENDING THE ACK TO THE RIGHT NODE!
                    collectOutput("MAC Relay", "Hub reception");
                    BaselineMacPacket *BaselineDataPkt = dynamic_cast<BaselineMacPacket*>(decapsulateRelayedPacket(BaselineBANPkt));
                    trace() << "Relay - Packet received from: " << BaselineDataPkt->getSource() << " relayed by: " << BaselineBANPkt->getSource();
                    toNetworkLayer(decapsulatePacket(BaselineDataPkt));
                } else if (isRelay){
                    // decapsulate, reencapsulate and send it to hub
					BaselineMacPacket *BaselineDataPkt = dynamic_cast<BaselineMacPacket*>(decapsulateRelayedPacket(BaselineBANPkt));
					BaselineMacPacket *BaselineBANRelayPkt = new BaselineMacPacket("BaselineBAN data packet", MAC_LAYER_PACKET);
					encapsulateRelayedPacket(BaselineBANRelayPkt, BaselineDataPkt);
					setHeaderFields(BaselineBANRelayPkt,I_ACK_POLICY,DATA,RESERVED,BaselineDataPkt->getDestination());
					BaselineBANRelayPkt->setBANSecurity(1);
					trace() << "Relay - Relaying frame from: " << BaselineDataPkt->getSource() << " to: " << BaselineDataPkt->getDestination();
					if (bufferPacket(BaselineBANRelayPkt)) {
						attemptTX();
						collectOutput("MAC Relay", "Packet Relayed");
					} else {
						trace() << "WARNING BaselineBAN MAC buffer overflow";
						collectOutput("Data pkt breakdown", "Fail, buffer overflow");
					}
                }
            } else {
                toNetworkLayer(decapsulatePacket(BaselineBANPkt));
                trace() << "The frame received is a DATA one, sending to network layer.";
            }
		} else {
			trace() << "The frame received is a duplicate one - dropping.";
		}
		/* if this pkt requires a block ACK we should send it,
		 * by looking what packet we have received */

		// NOT IMPLEMENTED
		if (BaselineBANPkt->getAckPolicy() == B_ACK_POLICY){
		}
		if (BaselineBANPkt->getMoreData() > 0){
            handlePost(BaselineBANPkt);
		}
	}

	/* If the packet received (Data or Mgmt) requires an ACK, we should send it now.
	 * While processing a data packet we might have flagged the need to send an I_ACK_POLL
	 */
	if (BaselineBANPkt->getAckPolicy() == I_ACK_POLICY) {
		BaselineMacPacket * ackPacket = new BaselineMacPacket("ACK packet",MAC_LAYER_PACKET);
		setHeaderFields(ackPacket,N_ACK_POLICY,CONTROL, (sendIAckPoll ? I_ACK_POLL : I_ACK), BaselineBANPkt->getSource());
		ackPacket->setByteLength(BASELINEBAN_HEADER_SIZE);
		// if we are unconnected set a proper BAN ID(the packet is for us since it was not filtered)
		if (connectedHID == UNCONNECTED) ackPacket->setBAN(BaselineBANPkt->getBAN());
		// set the appropriate fields if this an I_ACK_POLL
		if (sendIAckPoll) {
			// we are sending a future poll
			ackPacket->setMoreData(1);
			sendIAckPoll = false;
			if (!naivePollingScheme) {
				/* If this node was not given a future poll already, update the hubPollTimers
				 * and nextFuturePollSlot. Also if the hubPollTimers is empty, schedule the
				 * timer to send this first POLL [the one that the (future)I_ACK_POLL points to]
				 */
				if (hubPollTimers.empty() || hubPollTimers.back().NID != BaselineBANPkt->getSource() ) {
					trace() << "TEST: frameStartTime= "<<frameStartTime<<" poll from start= "<<(nextFuturePollSlot-1)*allocationSlotLength<<" timer= "<<frameStartTime + (nextFuturePollSlot-1)*allocationSlotLength - getClock();
					if (hubPollTimers.empty())
						setTimer(SEND_POLL, frameStartTime + (nextFuturePollSlot-1)*allocationSlotLength - getClock());
					TimerInfo t; t.NID=BaselineBANPkt->getSource(); t.slotsGiven=1; t.endSlot=nextFuturePollSlot;
					hubPollTimers.push(t);
					nextFuturePollSlot++;
					trace() << "TEST: nextFuturePollSlot= " << nextFuturePollSlot;
					lastTxAccessSlot[t.NID].polled = t.endSlot;
				}
			}
			int futurePollSlot = (naivePollingScheme ? nextFuturePollSlot : hubPollTimers.back().endSlot);
			trace() << "Future POLL at slot " << futurePollSlot <<" inserted in ACK packet";
			ackPacket->setSequenceNumber(futurePollSlot);
		}
		trace() << "transmitting ACK to: " << BaselineBANPkt->getSource();

		//toRadioLayer(ackPacket);
		//toRadioLayer(createRadioCommand(SET_STATE,TX)); isRadioSleeping = false;
		//Making the I-ACK pass through MGMT buffer + CSMA/CA mechanism to avoid collision.
        MgmtBuffer.push(ackPacket);
        attemptTX();

		/* Any future attempts to TX should be done AFTER we are finished TXing
		 * the I-ACK. To ensure this we set the appropriate timer and variable.
		 * BASELINEBAN_HEADER_SIZE is the size of the ack. 2*pMIFS is explained at sendPacket()
		 */
		setTimer(START_ATTEMPT_TX, (TX_TIME(BASELINEBAN_HEADER_SIZE) + 2*pMIFS) );
		futureAttemptToTX = true;
	}

	/* If this was a data packet, we have done all our processing
	 * (+ sending a possible I-ACK or I-ACK-POLL), so just return.
	 */
	if (BaselineBANPkt->getFrameType() == DATA) return;

	/* Handle management and control packets */
	switch(BaselineBANPkt->getFrameSubtype()) {
		case BEACON: {
            //updating neighbor list with hub flag set to true
            neighborhood.updateNeighborhood(RSSIsamples, BaselineBANPkt->getSource(), rssi, simTime().dbl(), false, true);

			BaselineBeaconPacket * BaselineBANBeacon = check_and_cast<BaselineBeaconPacket*>(BaselineBANPkt);
			simtime_t beaconTxTime = TX_TIME(BaselineBANBeacon->getByteLength()) + pMIFS;

			// store the time the frame starts. Needed for polls and posts, which only reference end allocation slot
			frameStartTime = getClock() - beaconTxTime;

			// get the allocation slot length, which is used in many calculations
			allocationSlotLength = BaselineBANBeacon->getAllocationSlotLength() / 1000000.0;
			SInominal = (allocationSlotLength/10.0 - pMIFS) / (2*mClockAccuracy);

			// a beacon is our synchronization event. Update relevant timer
			pastSyncIntervalNominal = false;
			setTimer(SYNC_INTERVAL_TIMEOUT, SInominal);

			beaconPeriodLength = BaselineBANBeacon->getBeaconPeriodLength();
			RAP1Length = BaselineBANBeacon->getRAP1Length();
			if (RAP1Length > 0) {
				trace() << "State from "<< macState << " to MAC_RAP";
				macState = MAC_RAP;
				endTime = getClock() + RAP1Length * allocationSlotLength - beaconTxTime;
			}
			collectOutput("Beacons received");
			trace() << "Beacon rx: reseting sync clock to " << SInominal << " secs";
			trace() << "           Slot= " << allocationSlotLength << " secs, beacon period= " << beaconPeriodLength << "slots";
			trace() << "           RAP1= " << RAP1Length << "slots, RAP ends at time: "<< endTime;

			/* Flush the Management packets buffer. Delete packetToBeSent if it is a management packet
			 * This is a design choice. It simplifies the flowcontrol and prevents rare cases where
			 * management packets are piled up. More complicated schemes chould be applied w.r.t.
			 * connection requests and connection assignments.
			 */
			if (packetToBeSent != NULL && packetToBeSent->getFrameType() != DATA ) {
				cancelAndDelete(packetToBeSent);
				packetToBeSent = NULL;
			}
			while(!MgmtBuffer.empty()) {
				cancelAndDelete(MgmtBuffer.front());
				MgmtBuffer.pop();
    		}

			if (connectedHID == UNCONNECTED) {
				// go into a setup phase again after this beacon's RAP
				setTimer(START_SETUP, RAP1Length * allocationSlotLength - beaconTxTime);
				trace() << "           (unconnected): go back in setup mode when RAP ends";

				/* We will try to connect to this BAN  if our scheduled access length
				 * is NOT set to unconnected (-1). If it is set to 0, it means we are
				 * establishing a sleeping pattern and waking up only to hear beacons
				 * and are only able to transmit in RAP periods.
				 */
				if (scheduledAccessLength >= 0) {
					// we are unconnected, and we need to connect to obtain scheduled access
					// we will create and send a connection request
					BaselineConnectionRequestPacket *connectionRequest = new BaselineConnectionRequestPacket("BaselineBAN connection request packet",MAC_LAYER_PACKET);

					// This block takes care of general header fields
					setHeaderFields(connectionRequest,I_ACK_POLICY,MANAGEMENT,CONNECTION_REQUEST, BaselineBANBeacon->getSource());
					// while setHeaderFields should take care of the HID field, we are currently unconnected.
					// We want to keep this state, yet send the request to the right hub.
					connectionRequest->setBAN(BaselineBANBeacon->getBAN());

					// This block takes care of connection request specific fields
					connectionRequest->setRecipientAddress(BaselineBANBeacon->getSenderAddress());
					connectionRequest->setSenderAddress(SELF_MAC_ADDRESS);
					// in this implementation our schedule always starts from the next beacon
					connectionRequest->setNextWakeup(BaselineBANBeacon->getSequenceNumber() + 1);
					connectionRequest->setWakeupInterval(scheduledAccessPeriod);
					//uplink request is simplified in this implementation to only ask for a number of slots needed
					connectionRequest->setUplinkRequest(scheduledAccessLength);
					connectionRequest->setByteLength(BASELINEBAN_CONNECTION_REQUEST_SIZE);

					// Management packets go in their own buffer, and handled by attemptTX() with priority
					MgmtBuffer.push(connectionRequest);
					trace() << "           (unconnected): created connection request";
				}
			/* else we are connected already and previous filtering
			 * made sure that this beacon belongs to our BAN
			 */
			} else  {
				// schedule a timer to wake up for the next beacon (it might be m periods away)

                setTimer(WAKEUP_FOR_BEACON, beaconPeriodLength * scheduledAccessPeriod * allocationSlotLength - beaconTxTime - GUARD_TIME );


				// if we have a schedule that does not start immediately after RAP, or our schedule
				// is not assigned yet, then go to sleep after RAP.
				if ((scheduledTxAccessStart == UNCONNECTED && RAP1Length < beaconPeriodLength)
								|| (scheduledTxAccessStart-1 > RAP1Length)) {
					setTimer(START_SLEEPING, RAP1Length * allocationSlotLength - beaconTxTime);
					trace() << "           --- start sleeping in: " << RAP1Length * allocationSlotLength - beaconTxTime << " secs";
				}
				// schedule the timer to go in scheduled TX access, IF we have a valid schedule
                if ( scheduledTxAccessEnd > scheduledTxAccessStart) {
                    setTimer(START_SCHEDULED_TX_ACCESS, (scheduledTxAccessStart-1) * allocationSlotLength - beaconTxTime + GUARD_TX_TIME);
                    trace() << "           --- start scheduled TX access in: " << (scheduledTxAccessStart-1) * allocationSlotLength - beaconTxTime + GUARD_TX_TIME << " secs";
                }
                    // we should also handle the case when we have a scheduled RX access. This is not implemented yet.
			}

            attemptTX();

			break;
		}

		case I_ACK_POLL: {
		    trace() << "Received an I_ACK_POLL frame.";
			handlePoll(BaselineBANPkt);
			// roll over to the ACK part
		}
		case I_ACK: {
		    trace() << "-------- ACK received from: " << BaselineBANPkt->getSource();
			waitingForACK = false;
			cancelTimer(ACK_TIMEOUT);

			if (packetToBeSent == NULL || currentPacketTransmissions == 0){
				trace() << "WARNING: Received I-ACK with packetToBeSent being NULL, or not TXed!";
				break;
			}
			// collect statistics
			if (currentPacketTransmissions == 1){
				if (packetToBeSent->getFrameType() == DATA)
					collectOutput("Data pkt breakdown", "Success, 1st try");
				else collectOutput("Mgmt & Ctrl pkt breakdown", "Success, 1st try");
			} else {
				if (packetToBeSent->getFrameType() == DATA)
					collectOutput("Data pkt breakdown", "Success, 2 or more tries");
				else collectOutput("Mgmt & Ctrl pkt breakdown", "Success, 2 or more tries");
			}
			cancelAndDelete(packetToBeSent);
			packetToBeSent = NULL;
			currentPacketTransmissions = 0;
			currentPacketCSFails = 0;
			CW = CWmin[priority];

			// we could handle future posts here (if packet not I_ACK_POLL and moreData > 0)
			attemptTX();
			break;
		}

		case B_ACK_POLL: {
			handlePoll(BaselineBANPkt);
			// roll over to the ACK part
		}
		case B_ACK: {
			waitingForACK = false;
			cancelTimer(ACK_TIMEOUT);
			cancelAndDelete(packetToBeSent);
			packetToBeSent = NULL;
			currentPacketTransmissions = 0;
			currentPacketCSFails = 0;
			CW = CWmin[priority];

			// we need to analyze the bitmap and see if some of the LACK packets need to be retxed

			attemptTX();
			break;
		}

		case CONNECTION_ASSIGNMENT: {
			BaselineConnectionAssignmentPacket *connAssignment = check_and_cast<BaselineConnectionAssignmentPacket*>(BaselineBANPkt);

			if (connAssignment->getStatusCode() == ACCEPTED || connAssignment->getStatusCode() == MODIFIED) {
                trace() << "Connection assignment received. Changing status to CONNECTED";
				connectedHID = connAssignment->getSource();
				connectedNID = connAssignment->getAssignedNID();
				// set anew the header fields of the packet to be sent
				if (packetToBeSent){
					packetToBeSent->setBAN(connectedHID);
					packetToBeSent->setSource(connectedNID);
				}
				// set the start and end times for the schedule
				scheduledTxAccessStart = connAssignment->getUplinkRequestStart();
				scheduledTxAccessEnd = connAssignment->getUplinkRequestEnd();
				trace() << "connected as NID " << connectedNID << "  --start TX access at slot: " << scheduledTxAccessStart << ", end at slot: " << scheduledTxAccessEnd;

			} // else we don't need to do anything - request is rejected
			else trace() << "Connection Request REJECTED, status code: " << connAssignment->getStatusCode();

			break;
		}

		case DISCONNECTION: {
			connectedHID = UNCONNECTED;
			connectedNID = UNCONNECTED;
			break;
		}

		case CONNECTION_REQUEST: {
			BaselineConnectionRequestPacket *connRequest = check_and_cast<BaselineConnectionRequestPacket*>(BaselineBANPkt);
			/* The ACK for the connection req packet is handled by the general code.
			 * Here we need to create the connection assignment packet and decide
			 * when to send it. We treat management packets that need ack, similar
			 * to data packets, but with higher priority. They have their own buffer.
			 */
			BaselineConnectionAssignmentPacket *connAssignment = new BaselineConnectionAssignmentPacket("BaselineBAN connection assignment",MAC_LAYER_PACKET);
			setHeaderFields(connAssignment,I_ACK_POLICY,MANAGEMENT,CONNECTION_ASSIGNMENT, connRequest->getSource());
			// this is the unconnected NID that goes in the header. Used for addressing
			//connAssignment->setNID(connRequest->getNID());
			// the full ID of the requesting node needs to be included in the assignment
			int fullAddress = connRequest->getSenderAddress();
			connAssignment->setRecipientAddress(fullAddress);
			connAssignment->setByteLength(BASELINEBAN_CONNECTION_ASSIGNMENT_SIZE);

			/* Check if the request is on an already active assignment. If a node misses the
			 * connection assignment packet, it will eventually send another request.
			 * Here we guard against needless waste of resources, by giving again the old
			 * resources. This works well if nodes are requesting the same  number of slots.
			 * If variable number of slots is requested then we need a way to free resources.
			 * CURRENTLY we just give back the old requested resources!!
			 */
			map<int, slotAssign_t>::iterator iter = slotAssignmentMap.find(fullAddress);
			if (iter != slotAssignmentMap.end()){
				// the req has been processed *successfully* before, assign old resources
				connAssignment->setStatusCode(ACCEPTED);
				// this is the assigned NID and it is part of the payload
				connAssignment->setAssignedNID(iter->second.NID);
				connAssignment->setUplinkRequestStart(iter->second.startSlot);
				connAssignment->setUplinkRequestEnd(iter->second.endSlot);
				trace() << "Connection request seen before! Assigning stored NID and resources ...";
				trace() << "Connection request from NID " << connRequest->getSource() << " (full addr: " << fullAddress <<") Assigning connected NID " << iter->second.NID;
			} else {
				// the request has not been processed before, try to assign new resources
				if (connRequest->getUplinkRequest() > beaconPeriodLength - (currentFirstFreeSlot-1)) {
					connAssignment->setStatusCode(REJ_NO_RESOURCES);
					// can not accomodate request
				} else if (currentFreeConnectedNID > 255) {
					connAssignment->setStatusCode(REJ_NO_NID);
				} else {
					// update the slotAssignmentMap
					slotAssign_t newAssignment;
					newAssignment.NID = fullAddress;
					newAssignment.startSlot = currentFirstFreeSlot;
					newAssignment.endSlot = currentFirstFreeSlot + connRequest->getUplinkRequest();
					slotAssignmentMap[fullAddress] = newAssignment;
					// construct the rest of the connection assignment packet
					connAssignment->setStatusCode(ACCEPTED);
					// this is the new assigned NID and it is part of the payload
					connAssignment->setAssignedNID(newAssignment.NID);
					connAssignment->setUplinkRequestStart(newAssignment.startSlot);
					connAssignment->setUplinkRequestEnd(newAssignment.endSlot);
					trace() << "Connection request from NID " << connRequest->getSource() << " (full addr: " << fullAddress <<") Assigning connected NID " << newAssignment.NID;
					// hub keeps track of the assignments
					lastTxAccessSlot[currentFreeConnectedNID].scheduled = newAssignment.endSlot -1;
					currentFirstFreeSlot += connRequest->getUplinkRequest();
					currentFreeConnectedNID++;
				}
			}
			MgmtBuffer.push(connAssignment);

			// transmission will be attempted after we are done sending the I-ACK
			trace() << "Conn assignment created, wait for " << (TX_TIME(BASELINEBAN_HEADER_SIZE) + 2*pMIFS) << " to attempTX";
			break;
		}

		case RELAY_REQUEST:
			trace() << "Processing Relay Request.";

		case T_POLL:
			// just read the time values from the payload, update relevant variables
			// and roll over to handle the POLL part (no break)
		case POLL: {
			handlePoll(BaselineBANPkt);
			break;
		}
		case ASSOCIATION:
		case DISASSOCIATION:
		case PTK:
		case GTK: {
			trace() << "WARNING: unimplemented packet subtype in [" << BaselineBANPkt->getName() << "]";
			break;
		}
	}
}

/* The specific finish function for BaselineBANMAC does needed cleanup when simulation ends*/
void BaselineBANMac::finishSpecific(){
	if (packetToBeSent != NULL) cancelAndDelete(packetToBeSent);
	while(!MgmtBuffer.empty()) {
		cancelAndDelete(MgmtBuffer.front());
		MgmtBuffer.pop();
    }
    if (isHub) {delete[] reqToSendMoreData; delete[] lastTxAccessSlot;}
}

/* A function to filter incoming BaselineBAN packets. Works for both hub or sensor as a receiver. */
bool BaselineBANMac::isPacketForMe(BaselineMacPacket *pkt) {

	if (connectedHID != UNCONNECTED) {
	    trace() << "packet received to: " << pkt->getDestination() << " my NID is: " << connectedNID << " and SELF_MAC_ADDRESS " << SELF_MAC_ADDRESS;

		if (pkt->getBAN() != connectedHID) return false; // packet from the wrong BAN.
		if (pkt->getDestination() == connectedNID || pkt->getDestination() == BROADCAST_NID ) return true;
        if (pkt->getDestination() == SELF_MAC_ADDRESS || pkt->getDestination() == BROADCAST_MAC_ADDRESS ) return true;
	}

	if ((connectedHID == UNCONNECTED) && ((unconnectedNID == pkt->getDestination()) || (pkt->getDestination() == BROADCAST_NID))){
		/* We need to check all cases of packets types. It is tricky because when unconnected
		 * two or more nodes can have the same NID. Some packets have a Recipient Address
		 * to distinguish the real destination. Others can be filtered just by type. Some
		 * like I-ACK we have to do more tests and still cannot be sure
		 */
		if (pkt->getFrameSubtype() == CONNECTION_ASSIGNMENT) {
			BaselineConnectionAssignmentPacket *connAssignment = check_and_cast<BaselineConnectionAssignmentPacket*>(pkt);
			if (connAssignment->getRecipientAddress() != SELF_MAC_ADDRESS) {
				// the packet is not for us, but the NID is the same, so we need to choose a new one.
				unconnectedNID = 1 + intrand(99);
				if (packetToBeSent) packetToBeSent->setSource(unconnectedNID);
				trace() << "Choosing NEW unconnectedNID = " << unconnectedNID;
				return false;
			}
		}
		// if we are unconnected it means that we not a hub, so we cannot process connection reqs
		if (pkt->getFrameSubtype() == CONNECTION_REQUEST) return false;

		// if we receive an ACK we need to check whether we have sent a packet to be acked.
		if ((pkt->getFrameSubtype() == I_ACK || pkt->getFrameSubtype() == I_ACK_POLL ||
			pkt->getFrameSubtype() == B_ACK || pkt->getFrameSubtype() == B_ACK_POLL)) {
			if (packetToBeSent == NULL || currentPacketTransmissions == 0)	{
				trace() << "While unconnected: ACK packet received with no packet to ack, renewing NID";
				unconnectedNID = 1 + intrand(99);
				if (packetToBeSent) packetToBeSent->setSource(unconnectedNID);
				trace() << "Choosing NEW unconnectedNID = " << unconnectedNID;
				return false;
			}
		}

		// for all other cases of HID == UNCONNECTED, return true
		return true;
	}

	// for all other cases return false
	return false;
}

/* A function to calculate the extra guard time, if we are past the Sync time nominal. */
simtime_t BaselineBANMac::extraGuardTime() {
	return (simtime_t) (getClock() - syncIntervalAdditionalStart) * mClockAccuracy;
}

/* A function to set the header fields of a packet. It works with both hub- and sensor-created packets */
void BaselineBANMac::setHeaderFields(BaselineMacPacket * pkt, AcknowledgementPolicy_type ackPolicy, Frame_type frameType, Frame_subtype frameSubtype, int dst) {
	if (isHub) {
		pkt->setBAN(connectedHID);
		pkt->setSource(connectedHID);
	} else {
		if (connectedNID != UNCONNECTED) {
			pkt->setBAN(connectedHID);
			pkt->setSource(connectedNID);
		} else {
			pkt->setBAN(UNCONNECTED);
			pkt->setSource(unconnectedNID);
		}
	}
	if (dst != BROADCAST_MAC_ADDRESS) {
		pkt->setDestination(dst);
	} else {
		pkt->setDestination(BROADCAST_NID);
	}
	pkt->setAckPolicy(ackPolicy);
	pkt->setFrameType(frameType);
	pkt->setFrameSubtype(frameSubtype);
	pkt->setMoreData(0);
	pkt->setSequenceNumber(currentSequenceNumber);
	currentSequenceNumber++;
	// if this is a data packet but NOT from the Hub then set its moreData flag
	// Hub needs to handle its moreData flag (signaling posts) separately
	if (frameType == DATA && !isHub){
		if (TXBuffer.size()!=0 || MgmtBuffer.size()!=0){
			// option to enhance BaselineBAN by sending how many more pkts we have
			if (enhanceMoreData) pkt->setMoreData(TXBuffer.size() + MgmtBuffer.size());
			// como o ack passa pelo mgmt buffer o setmoredata estava sendo ativado indevidamente.
			//else pkt->setMoreData(1);
		}
	}
}

/* TX in RAP requires contending for the channel (a carrier sensing scheme)
 * this function prepares an important variable and starts the process.
 * It is used by the more generic attemptTX() function.
 */
void BaselineBANMac::attemptTxInRAP() {
    double ifs_timer = pSIFS;
	if (backoffCounter == 0) {
	    if (packetToBeSent->getFrameType() == I_ACK){
	        ifs_timer = pMIFS;
            backoffCounter = 1 + intrand(CWmin[7]);
	    } else {
	        ifs_timer = pSIFS;
	        backoffCounter = 1+ intrand(CW);
	    }

	    trace() << "Packet to be sent is of type: " << packetToBeSent->getFrameType() << " Contention Window: " << CW;
	}
	trace() << "Starting CCA to transmit " << packetToBeSent->getName() << " in RAP, backoffCounter " << backoffCounter;
	attemptingToTX = true;
	setTimer(CARRIER_SENSING,ifs_timer);
}

/* This function will attempt to TX in all TX access states(RAP, scheduled)
 * It will check whether we need to retransmit the current packet, or prepare
 * a new packet from the MAC data buffer or the Management buffer to be sent.
 */
void BaselineBANMac::attemptTX() {
	// If we are not in an appropriate state, return
	if (macState != MAC_RAP && macState != MAC_FREE_TX_ACCESS) return;
	/* if we are currently attempting to TX or we have scheduled a future
	 * attemp to TX, or waiting for an ack, return
	 */
	if (waitingForACK || attemptingToTX || futureAttemptToTX ) return;

	if (packetToBeSent && currentPacketTransmissions + currentPacketCSFails < maxPacketTries) {
		if (macState == MAC_RAP && (enableRAP || packetToBeSent->getFrameType() != DATA)) attemptTxInRAP();
		if ((macState == MAC_FREE_TX_ACCESS) && (canFitTx())) sendPacket();
		return;
	}
	/* if there is still a packet in the buffer after max tries
	 * then delete it, reset relevant variables, and collect stats.
	 */
	if (packetToBeSent) {
		trace() << "Max TX attempts reached. Last attempt was a CS fail";
		if (currentPacketCSFails == maxPacketTries){
			if (packetToBeSent->getFrameType() == DATA)
				collectOutput("Data pkt breakdown", "Failed, Channel busy");
			else collectOutput("Mgmt & Ctrl pkt breakdown", "Failed, Channel busy");
		} else {
			if (packetToBeSent->getFrameType() == DATA)
				collectOutput("Data pkt breakdown", "Failed, No Ack");
			else collectOutput("Mgmt & Ctrl pkt breakdown", "Failed, No Ack");
		}
		cancelAndDelete(packetToBeSent);
		packetToBeSent = NULL;
		currentPacketTransmissions = 0;
		currentPacketCSFails = 0;
	}

	// Try to draw a new packet from the data or Management buffers.
	if (MgmtBuffer.size() !=0) {
		packetToBeSent = (BaselineMacPacket*)MgmtBuffer.front();  MgmtBuffer.pop();
		if (MgmtBuffer.size() > MGMT_BUFFER_SIZE)
			trace() << "WARNING: Management buffer reached a size of " << MgmtBuffer.size();
	} else if (connectedNID != UNCONNECTED && TXBuffer.size() != 0) {
		packetToBeSent = (BaselineMacPacket*)TXBuffer.front();   TXBuffer.pop();
	}
	// if we found a packet in any of the buffers, try to TX it.
	if (packetToBeSent){
		if (macState == MAC_RAP && (enableRAP || packetToBeSent->getFrameType() != DATA)) attemptTxInRAP();
		if ((macState == MAC_FREE_TX_ACCESS) && (canFitTx())) sendPacket();
	}
}

/* This function lets us know if a transmission fits in the time we have (scheduled or RAP)
 * It takes into account guard times too. A small issue exists with scheduled access:
 * Sleeping is handled at the timer code, which does not take into account the guard times.
 * In fact if we TX once then we'll stay awake for the whole duration of the scheduled slot.
 */
bool BaselineBANMac::canFitTx() {
	if (!packetToBeSent) return false;
	if ( endTime - getClock() - (GUARD_FACTOR * GUARD_TIME) - TX_TIME(packetToBeSent->getByteLength()) - pMIFS > 0) return true;
	return false;
}


/* Sends a packet to the radio and either waits for an ack or restart the attemptTX process
 */
void BaselineBANMac::sendPacket() {
	// we are starting to TX, so we are exiting the attemptingToTX (sub)state.
	attemptingToTX = false;

	// collect stats about the state we are TXing data packets
	if (packetToBeSent->getFrameType() == DATA) {
		if (macState == MAC_RAP) collectOutput("pkt TX state breakdown", "Contention");
		else {
			if (isPollPeriod) collectOutput("pkt TX state breakdown", "Poll");
			else collectOutput("pkt TX state breakdown", "Contention-free");
		}
	}

	if (packetToBeSent->getAckPolicy() == I_ACK_POLICY || packetToBeSent->getAckPolicy() == B_ACK_POLICY) {
		/* Need to wait for ACK. Here we explicitly take into account the clock drift, since the spec does
		 * not mention a rule for ack timeout. In other timers, GUARD time takes care of the drift, or the
		 * timers are just scheduled on the face value. We also take into account sleep->TX delay, which
		 * the BaselineBAN spec does not mention but it is important.
		 */
		//double ackTimeout = 2*(SLEEP2TX + TX_TIME(packetToBeSent->getByteLength()) + 4*pSIFS + TX_TIME(BASELINEBAN_HEADER_SIZE) * (1 + mClockAccuracy));
		trace() << "TXing[" << packetToBeSent->getName() << "], ACK_TIMEOUT in " << ackTimeout;
		setTimer(ACK_TIMEOUT, ackTimeout);
		waitingForACK = true;

		currentPacketTransmissions++;
		toRadioLayer(packetToBeSent->dup());
		toRadioLayer(createRadioCommand(SET_STATE,TX));  isRadioSleeping = false;

	} else { // no need to wait for ack
		/* Start the process of attempting to TX again. The spec does not provide details on this.
		 * Our choice is more fair for contention-based periods, since we contend for every pkt.
		 * If we are in a scheduled TX access state the following implication arises: The spec would
		 * expect us to wait the TX time + pMIFS, before attempting to TX again. Because the pMIFS
		 * value is conservative (larger than what the radio actually needs to TX), the radio would
		 * TX and then try to go back in RX. With the default values, our new SET_STATE_TX message
		 * would find the radio, in the midst of changing back to RX. This is not a serious problem,
		 * in our radio implementation we would go back to TX and just print a warning trace message.
		 * But some real radios may need more time (e.g., wait to finish the first TX->RX transision)
		 * For this reason we are waiting for 2*pMIFS just to be on the safe side. We do not account
		 * for the clock drift, since this should be really small for just a packet transmission.
		 */
		trace() << "TXing[" << packetToBeSent->getName() << "], no ACK required";
		setTimer(START_ATTEMPT_TX, SLEEP2TX + TX_TIME(packetToBeSent->getByteLength()) + 2*pMIFS);
		futureAttemptToTX = true;

		toRadioLayer(packetToBeSent); // no need to dup() we are only sending it once
		toRadioLayer(createRadioCommand(SET_STATE,TX));  isRadioSleeping = false;
		packetToBeSent = NULL; // do not delete the message, just make the packetToBeSent placeholded available
		currentPacketTransmissions = 0; // just a sefeguard, it should be zero.
	}
}

/* Implements the polling functionality needed to handle several
 * control packets: Poll, T-Poll, I-ACK+Poll, B-ACK+Poll
 */
void BaselineBANMac::handlePoll(BaselineMacPacket *pkt) {
	// check if this is an immediate (not future) poll
	if (pkt->getMoreData() == 0){
		macState = MAC_FREE_TX_ACCESS;
		trace() << "State from "<< macState << " to MAC_FREE_TX_ACCESS (poll)";
		isPollPeriod = true;
		int endPolledAccessSlot = pkt->getSequenceNumber();
		/* The end of the polled access time is given as the end of an allocation
		 * slot. We have to know the start of the whole frame to calculate it.
		 * NOTICE the difference in semantics with other end slots such as scheduled access
		 * scheduledTxAccessEnd where the end is the beginning of scheduledTxAccessEnd
		 * equals with the end of scheduledTxAccessEnd-1 slot.
		 */
		endTime = frameStartTime + endPolledAccessSlot * allocationSlotLength;
		// reset the timer for sleeping as needed
		if (endPolledAccessSlot != beaconPeriodLength &&
		   (endPolledAccessSlot+1) != scheduledTxAccessStart && (endPolledAccessSlot+1) != scheduledRxAccessStart){
			setTimer(START_SLEEPING, endTime - getClock());
		}else cancelTimer(START_SLEEPING);

		int currentSlotEstimate = round(SIMTIME_DBL(getClock()-frameStartTime)/allocationSlotLength)+1;
		if (currentSlotEstimate-1 > beaconPeriodLength) trace() << "WARNING: currentSlotEstimate= "<< currentSlotEstimate;
		collectOutput("var stats", "poll slots taken", (endPolledAccessSlot+1) - currentSlotEstimate );
		attemptTX();
	}
	// else treat this as a POST: a post of the polling message coming in the future
	else {
		// seqNum holds the allocation slot that the post will happen and fragNum the num of beacon periods in the future
		int postedAccessStart = pkt->getSequenceNumber();
		postedAccessEnd = postedAccessStart + 1; // all posts last one slot, end here is the beginning of the end slot
		simtime_t postTime = frameStartTime + (postedAccessStart-1 + pkt->getFragmentNumber()* beaconPeriodLength) * allocationSlotLength;
		trace() << "Future Poll received, postSlot= "<< postedAccessStart <<" waking up in " << postTime - GUARD_TIME - getClock();
		// if the post is the slot immediately after, then we have to check if we get a negative number for the timer
		if (postTime <= getClock() - GUARD_TIME) setTimer(START_POSTED_ACCESS, 0);
		else setTimer(START_POSTED_ACCESS, postTime - GUARD_TIME - getClock());
	}
}

// handles posts (data packets, with moreData flag == 1)
void BaselineBANMac::handlePost(BaselineMacPacket *pkt) {
	if (isHub) {
		if (pollingEnabled) handleMoreDataAtHub(pkt);
		// can we make this a separate class HubDecisionLayer:: ?? do we need too many variables from MAC?
		return;
	}
	// find the current slot, this is the starting slot of the post
	int postedAccessStart = (int)round(SIMTIME_DBL(getClock() - frameStartTime)/allocationSlotLength)+1;
	// post lasts for the current slot. This can be problematic, since we might go to sleep
	// while receiving. We need a post timeout.
	postedAccessEnd = postedAccessStart + 1;
	setTimer(START_POSTED_ACCESS, 0);
}

void BaselineBANMac::handleMoreDataAtHub(BaselineMacPacket *pkt) {
	// decide if this the last packet that node NID can send, keep how much more data it has
	int NID = pkt->getSource();
	/* If the packet we received is in the node's last TX access slot (scheduled or polled) then send a POLL.
	 * This means that we might send multiple polls (as we may receive multiple packets in the last slot
	 * but this is fine since all will point to the same time. Note that a node can only support one future
	 * poll (one timer for START_POSTED_ACCESS). Sending multiple polls (especially with I_ACK+POLL which
	 * do not cost anything extra compared to I_ACK) is beneficial bacause it increases the probability
	 * of the poll's reception. Also note that reqToSendMoreData[NID] will have the latest info (the info
	 * carried by the last packet with moreData received. Finally the lastTxAccessSlot[NID].polled does
	 * not need to be reset for a new beacon period. If we send a new poll, this variable will be updated,
	 * if we don't then we will not receive packets from that NID in the old slot so no harm done.
	 */
	if (currentSlot == lastTxAccessSlot[NID].scheduled || currentSlot == lastTxAccessSlot[NID].polled){
		if (nextFuturePollSlot <= beaconPeriodLength) {
			trace() << "Hub handles more Data ("<< pkt->getMoreData() <<")from NID: "<< NID <<" current slot: " << currentSlot;
			reqToSendMoreData[NID] = pkt->getMoreData();
			// if an ack is required for the packet the poll will be sent as an I_ACK_POLL
			if (pkt->getAckPolicy() == I_ACK_POLICY) sendIAckPoll = true;
			else {	// create a POLL message and send it.
					// Not implemeted here since currently all the data packets require I_ACK
			}
		}
	}
}
/* Not currently implemented. In the future useful if we implement the beacon shift sequences
 */
simtime_t BaselineBANMac::timeToNextBeacon(simtime_t interval, int index, int phase) {
	return interval;
}

void BaselineBANMac::encapsulateRelayedPacket(cPacket * pkt, cPacket * relayedPkt){
    BaselineMacPacket *macPkt = check_and_cast <BaselineMacPacket*>(pkt);
    //macPkt->setByteLength(macFrameOverhead);
    //macPkt->setKind(MAC_LAYER_PACKET);
    //macPkt->setSequenceNumber(currentSequenceNumber++);
    //macPkt->setSource(SELF_MAC_ADDRESS);
    // by default the packet created has its generic destination address to broadcast
    // a specific protocol can change this, and/or set more specific dest addresses
    //macPkt->setDestination(BROADCAST_MAC_ADDRESS);
    //macPkt->setBANSecurity(1);
    macPkt->encapsulate(relayedPkt);
}


cPacket *BaselineBANMac::decapsulateRelayedPacket(cPacket * pkt) {
    MacPacket *macPkt = check_and_cast <MacPacket*>(pkt);
    MacPacket *relayedPkt = check_and_cast <MacPacket*>(macPkt->decapsulate());
    return relayedPkt;
}


void Neighbor::updateNeighbor(double rssi, double lastSeen, bool relayCapable, bool hub) {
    insertRSSI(rssi);
    insertContact(lastSeen);
    setRelayCapable(relayCapable);
    setHubFlag(hub);
}

void Neighbor::insertRSSI(double rssi) {
    //this->rssis.erase(this->rssis.begin());
    //this->rssis.insert(rssi_index, rssi);
    this->rssis.push_back(rssi);
    if(this->rssis.size() > this->getSize() ){
        this->rssis.erase(this->rssis.begin());
    }
}

void Neighbor::insertContact(double lastSeen) {
    this->contacts.push_back(lastSeen);
    if (this->contacts.size() > this->size){
        contacts.erase(this->contacts.begin());
    }
}

Neighbor::Neighbor(int rssiSize, int address, double rssi, double lastSeen, bool relayCapable, bool hub) {
    setSize(rssiSize);
    setAddress(address);
    insertRSSI(rssi);
    insertContact(lastSeen);
    setRelayCapable(relayCapable);
    setHubFlag(hub);
}

void Neighborhood::updateNeighborhood(int rssiSize, int address, double rssi, double lastSeen, bool relayCapable, bool isMyHub) {
	nb = neighborMap.find(address);
	if (nb != neighborMap.end()) {
        nb->second.updateNeighbor(rssi, lastSeen, relayCapable, isMyHub);
    } else {
        Neighbor neighbor = Neighbor(rssiSize, address, rssi, lastSeen, relayCapable, isMyHub);
        neighborMap.insert({address, neighbor});
	}
	if (isMyHub) {this->setMyHub(address);}
}

Neighbor Neighborhood::getHub() {
    nb = neighborMap.find(this->myHub);
    if (nb != neighborMap.end()){
        return nb->second;
    }
}

int Neighborhood::getRelay(int address) {
	if (address == BROADCAST_NID || address == BROADCAST_MAC_ADDRESS){
		return BROADCAST_NID;
	}
	
	if (ordenador == 0){
	// Case 0 - do not relay
		return address;
	
	} else if (ordenador == 1){
	// Case 1 - send to best RSSI 
		int bestNeighbor;
		double bestRSSI = -DBL_MAX;
		for (nb = neighborMap.begin(); nb != neighborMap.end(); nb++){
			if (nb->second.getLastRSSI() >= bestRSSI){
				bestNeighbor = nb->second.getAddress();
				bestRSSI = nb->second.getLastRSSI();
			}
		}
		return bestNeighbor;

	} else if(ordenador == 2){
	// Case 2 - send to last seen.
		int bestNeighbor;
		double latestSeen = -DBL_MAX;
		for (nb = neighborMap.begin(); nb != neighborMap.end(); nb++){
			if (nb->second.getLastContact() >= latestSeen){
				bestNeighbor = nb->second.getAddress();
				latestSeen = nb->second.getLastContact();
			}
		}
		return bestNeighbor;

	} else {
		// not developed
	}
}
