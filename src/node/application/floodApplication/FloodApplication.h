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

#ifndef _FLOODAPPLICATION_H_
#define _FLOODAPPLICATION_H_

#include "VirtualApplication.h"
#include "FloodPacket_m.h"

using namespace std;

enum ThroughputTestTimers {
	SEND_PACKET = 1
};

class FloodApplication: public VirtualApplication {
 private:
	double packet_rate;
	string recipientAddress;
	double startupDelay;

	/* Data created to flooding app*/
	int numSenders;
	int numNodes;
	map<string, double> receivedPackets;

	float packet_spacing;
	int dataSN;


 protected:
	void startup();
	void fromNetworkLayer(ApplicationPacket *, const char *, double, double);
	void handleRadioControlMessage(RadioControlMessage *);
	void timerFiredCallback(int);
};

#endif
