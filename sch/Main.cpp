#include <ostream>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include "Packet.h"
#include "SchedulerData.h"
#include "Scheduler.h"

using namespace std;
/*
 * get packet from file and put in appropriate queue in dataStruct. if reached EOF update isEOF to true.
 * put weight of packet in weight: if not first packet of flow weight will be default weight. this is okay because
 * the actual weight of the flow was already determined and put in the flows Data structure.
 */
Packet receivePacket(SchedulerData& flowsDataStruct, ifstream& inputFile, int defaultWeight, bool& isEOF, int& weight);
/* read packets from file up to given time */
void getPacketsUpToTime(int defaultWeight, SchedulerData flowsDataStruct, bool& isEOF, ifstream& inputFile,
	long currentTime, Packet& lastPacket, int& lastPacketWeight);

int main(int argc, char* argv[])
{

	//todo: parse & check arguments from cmd
	Scheduler scheduler(1, 0);
	bool isSuccess = scheduler.init("inp.txt", "out.txt");
	if (!isSuccess)
	{
		return 0;
	}
	scheduler.start();
}


