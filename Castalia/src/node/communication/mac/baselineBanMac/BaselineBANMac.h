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

#ifndef BaselineBAN_MAC_MODULE_H
#define BaselineBAN_MAC_MODULE_H

#include <vector>
#include <complex>
#include "VirtualMac.h"
#include "BaselineMacPacket_m.h"

#define TX_TIME(x) (phyLayerOverhead + x)*1/(1000*phyDataRate/8.0) //x are in BYTES
#define UNCONNECTED -1

// Guard time used to start RX earlier and allow less time to finish TX
#define GUARD_TIME (pastSyncIntervalNominal ? allocationSlotLength / 10.0 + extraGuardTime() : allocationSlotLength/10.0)
// Guard time used to delay the start of a transmission
#define GUARD_TX_TIME (pastSyncIntervalNominal ? extraGuardTime() : 0.0)
// We discovered that ending a TX we should really guard for 2*GT instead of 1*GT.
// We offer this option with the enhanceGuardTime parameter
#define GUARD_FACTOR (enhanceGuardTime ? 2.0 : 1.0)

#define SLEEP2TX (isRadioSleeping ? pTimeSleepToTX : 0.0)

#define MAC_SELF_ADDRESS self
#define MGMT_BUFFER_SIZE 16  // hold maximum of 16 management packets


typedef std::complex<double> Complex;

enum MacStates {
	MAC_SETUP = 1000,
	MAC_RAP = 1001,
	MAC_FREE_TX_ACCESS = 1002,
	MAC_FREE_RX_ACCESS = 1003,
	MAC_BEACON_WAIT = 1009,
	MAC_SLEEP = 1010,
	MAC_STARTUP = 1004
};

enum Timers {
	CARRIER_SENSING = 1,
	START_ATTEMPT_TX = 2,
	ACK_TIMEOUT = 3,
	START_SLEEPING = 4,
	START_SCHEDULED_TX_ACCESS = 5,
	START_SCHEDULED_RX_ACCESS = 6,
	WAKEUP_FOR_BEACON = 7,
	SYNC_INTERVAL_TIMEOUT = 8,
	SEND_BEACON = 9,
	HUB_SCHEDULED_ACCESS = 10,
	START_SETUP = 11,
	START_POSTED_ACCESS = 12,
	SEND_FUTURE_POLLS = 13,
	SEND_POLL = 14,
	INCREMENT_SLOT = 15,
	WAIT_SETUP = 16,
	START_RELAY = 17,
	B_MOVE = 18,
	B_MOVE_REFRESH = 19
};

static int CWmin[8] = { 16, 16, 8, 8, 4, 4, 2, 1 };
static int CWmax[8] = { 64, 32, 32, 16, 16, 8, 8, 4};

struct TimerInfo {
	int NID;
	int slotsGiven;
	int endSlot;
};

struct slotAssign_t {
	int NID;
	int startSlot;
	int endSlot;
};

struct AccessSlot {
	int scheduled;
	int polled;
};

class Neighbor {

private:
    int address;
    // size of rssis and contancs queue
    int size;
    std::vector<double> rssis;
    std::vector<double> contacts;
    bool relayCapable;
    bool hub;
    //bool myRelay; not used in the opportunistic approach

public:
    explicit Neighbor() {};
    Neighbor(int rssiSize, int address, double rssi, double lastSeen, bool relayCapable, bool hub);
    void updateNeighbor(double rssi, double lastSeen, bool relayCapable, bool hub);
    std::vector<double> getRSSIs() {return rssis;}
    void insertRSSI(double rssi);
    double getLastRSSI(void) {return this->rssis.back();}
    std::vector<double> getContacts(void) {return this->contacts;}
    double getLastContact(void) {return this->contacts.back();}
    void insertContact(double lastSeen);
    int getAddress(void) {return address;}
    void setAddress(int address) {this->address = address;}
    bool getRelayCapable(void) {return relayCapable;}
    void setRelayCapable(bool relayCapable) {this->relayCapable = relayCapable;}
    bool getHubFlag(void) {return hub;}
    void setHubFlag(bool isHub) {this->hub = isHub;}
    void setSize(int rssiSize) {this->size = rssiSize;}
    int getSize(void) {return this->size;}

};

// neighborhood management - store every new neighbor, its  for future relay purposes.
class Neighborhood {
    public:

    explicit Neighborhood() {};

    int getRelay(int);
    void updateNeighborhood(int rssiSize, int address, double rssi, double lastSeen, bool relayCapable, bool isMyHub);

    void setOrdenador(int ordenator) {this->ordenador = ordenator;}
    int getOrdenador() {return ordenador;}
    void setMyHub(int address) {this->myHub = address;}
    Neighbor getHub();

    private:

    int ordenador;

    int myHub;
    map <int , Neighbor> neighborMap;
    map <int, Neighbor>::iterator nb;

};

class BaselineBANMac : public VirtualMac {
	private:
	bool isHub = false;
	bool isRelay;
	int connectedHID; // Hub ID also used as BAN ID for nodes
	int connectedNID; // Node ID
	int unconnectedNID;
	int relay;
	int relayPar;

	double RSSIsamples;

	// neighborhood checkup and relay selection
	Neighborhood neighborhood = Neighborhood();

	double allocationSlotLength;
	int RAP1Length;
	int beaconPeriodLength;

	int scheduledTxAccessStart;
	int scheduledTxAccessEnd;
	int scheduledAccessLength;
	int scheduledAccessPeriod;

	int scheduledRxAccessStart;
	int scheduledRxAccessEnd;

	int polledAccessEnd;
	int postedAccessEnd;

    double pMIFS; // Minimum Interframe Separation Time - used in burst mode - ack not transmitted (20 us)
    double pSIFS; // Short Interframe Separation Time - (75 us)
    double pCCATime; // Time to access the channel
    double ackTimeout;
	int phyLayerOverhead;
	double phyDataRate;
	int priority;
    double contentionSlotLength;

	int maxPacketTries;
	int currentPacketTransmissions;
	int currentPacketCSFails;

	int CW;
	bool CWdouble;
	int backoffCounter;

	MacStates macState;

	bool pastSyncIntervalNominal;
	simtime_t syncIntervalAdditionalStart;

	BaselineMacPacket *packetToBeSent;
	simtime_t endTime; // the time when our right to TX ends. Covers RAP, scheduled and polled access
	simtime_t frameStartTime; // time the frame starts (when we receive the beacon - beacon TX time)

	double SInominal;
    double mClockAccuracy;
	bool enhanceGuardTime;
	bool enhanceMoreData;
	bool pollingEnabled;
	bool naivePollingScheme;
	bool isRadioSleeping;
    double pTimeSleepToTX;
	bool enableRAP;

	bool waitingForACK;
	bool futureAttemptToTX;
	bool attemptingToTX;

	bool isPollPeriod;

	// a buffer to store Management packets that require ack and possible reTX
	// these packets are treated like data packets, but with higher priority
	queue <BaselineMacPacket *>MgmtBuffer;

	// the rest of the variables are hub-specific. They get allocated/used only for the hub
	int currentSlot;
	bool sendIAckPoll;
	int currentFirstFreeSlot;
	int currentFreeConnectedNID;
	int nextFuturePollSlot;
	AccessSlot *lastTxAccessSlot;
	int *reqToSendMoreData;
	queue <TimerInfo> hubPollTimers;
	map <int, slotAssign_t> slotAssignmentMap;


	protected:
	void startup();
	void timerFiredCallback(int);
	void fromNetworkLayer(cPacket*, int);
	void fromRadioLayer(cPacket*,double,double);
	void finishSpecific();
	bool isPacketForMe(BaselineMacPacket *pkt);
	simtime_t extraGuardTime();
	void setHeaderFields(BaselineMacPacket *pkt, AcknowledgementPolicy_type ackPolicy, Frame_type frameType, Frame_subtype frameSubtype, int dst);
	void attemptTxInRAP();
	void attemptTX();
	bool canFitTx();
	void sendPacket();
	void handlePoll(BaselineMacPacket *pkt);
	void handlePost(BaselineMacPacket *pkt);
	void handleMoreDataAtHub(BaselineMacPacket *pkt);
	simtime_t timeToNextBeacon(simtime_t interval, int index, int phase);
	void encapsulateRelayedPacket(cPacket * pkt, cPacket * relayedPkt);
	cPacket *decapsulateRelayedPacket(cPacket * pkt);


    public:
        bool getIsHub() {return this->isHub;}
};

#endif // BaselineBAN_MAC_MODULE_H
