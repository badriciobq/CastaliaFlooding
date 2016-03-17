/****************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2011                         *
 *  Developed at the ATP lab, Networked Systems research theme              *
 *  Author(s): Athanassios Boulis, Yuriy Tselishchev                        *
 *  This file is distributed under the terms in the attached LICENSE file.  *
 *  If you do not find this file, copies can be found by writing to:        *
 *                                                                          *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
 *      Attention:  License Inquiry.                                        *
 *                                                                          *
 ****************************************************************************/

#include "FloodApplication.h"
#include <sstream>

Define_Module(FloodApplication);

void FloodApplication::startup()
{
	packet_rate = par("packet_rate");
	recipientAddress = par("nextRecipient").stringValue();
	startupDelay = par("startupDelay");
	numSenders = par("numSenders");	


	/* Get the number of nodes that will be simulated */	
	numNodes = getParentModule()->getParentModule()->par("numNodes");

	packet_spacing = packet_rate > 0 ? 1 / float (packet_rate) : -1;

	/* Loop to set nodes that will be sender of messages */
	for(int i=0; i < numSenders; ++i)
	{
		int nodeID = atoi(SELF_NETWORK_ADDRESS);
		if( nodeID == i)
		{
			setTimer(SEND_PACKET, packet_spacing + startupDelay);	
		}
	}

	dataSN = 0;
	declareOutput("Packets received per node");
}

void FloodApplication::fromNetworkLayer(ApplicationPacket * rcvPacket,
		const char *source, double rssi, double lqi)
{
	FloodPacket *pkt = static_cast<FloodPacket *>(rcvPacket);

	int pktOrigin = pkt->getSourceID();
	int pktDestination = pkt->getDestinationID();
	int nodeID = atoi(SELF_NETWORK_ADDRESS);
	
	// If I already received packet, not to do
	if ( receivedPackets.find(pkt->getPacketID()) != receivedPackets.end() )
	{

		trace() << "Node[" << nodeID << "] already has the packet " << pktOrigin;
		return;
	}
	else
	{

		trace() << "Node[" << nodeID << "] put packet in the buffer " << pktOrigin;
	}
	
	// I did not get the packet, so, I put it in my list of received
	receivedPackets.insert( pair<string, double>( pkt->getPacketID(), simTime().dbl() ) );

	if (pktDestination == nodeID) {
		trace() << "Node[" << nodeID << "] Received packet from node " << pktOrigin;
		collectOutput("Packets received per node", atoi(source));
	// Packet has to be forwarded to the next hop recipient
	} else {
		
		trace() << "Node[" << nodeID << "] forward packet from " << pktOrigin;
		ApplicationPacket* fwdPacket = rcvPacket->dup();
		// Reset the size of the packet, otherwise the app overhead will keep adding on
		fwdPacket->setByteLength(0);
		toNetworkLayer(fwdPacket, BROADCAST_NETWORK_ADDRESS);
	}
}

void FloodApplication::timerFiredCallback(int index)
{
	switch (index) {
		case SEND_PACKET:{
			
			/* Get own network address */
			int nodeID = atoi(SELF_NETWORK_ADDRESS);
			
			/* Select a destination randomicaly amoung all nodes in the network */
			int destination = intrand(numNodes);

			/* Ensure that not select my own address */
			while(destination == nodeID)
				destination = intrand(numNodes);

	
			trace() << "Node [" << nodeID << "] sendind data to: " << destination;
	
			FloodPacket *pkt = new FloodPacket("Flood Packet", APPLICATION_PACKET);
			pkt->setSourceID(nodeID);
			pkt->setDestinationID(destination);

			stringstream ss;
			ss << nodeID << "" << simTime().dbl();
			pkt->setPacketID(ss.str().c_str());
			
			/* Send packet to all neighbour nodes */
			toNetworkLayer(pkt, BROADCAST_NETWORK_ADDRESS);
			break;
		}
	}
}

// This method processes a received carrier sense interupt. Used only for demo purposes
// in some simulations. Feel free to comment out the trace command.
void FloodApplication::handleRadioControlMessage(RadioControlMessage *radioMsg)
{
	switch (radioMsg->getRadioControlMessageKind()) {
		case CARRIER_SENSE_INTERRUPT:
			trace() << "CS Interrupt received! current RSSI value is: " << radioModule->readRSSI();
                        break;
	}
}

