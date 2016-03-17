

cplusplus {{
	#include "ApplicationPacket_m.h"
}}

class ApplicationPacket;

packet FloodPacket extends ApplicationPacket {
	int sourceID;
	int destinationID;
	string packetID;
}

