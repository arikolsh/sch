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


	bool isEOF = false;
	Scheduler scheduler(1, 0);
	bool isSuccess = scheduler.init("inp.txt", "out.txt");
	if(!isSuccess)
	{
		return 0;
	}
	scheduler.start();


	bool debug2 = true;
	if (debug2) return 0;
	SchedulerData flowsDataStruct;

	bool debug1 = true;
	string inputPath;
	string outputPath;
	int defaultWeight;
	int quantum = 0;
	bool isEOF = false;
	inputPath = "inp.txt"; //debug1
	outputPath = "out.txt"; //debug1 
	defaultWeight = 1; //debug 1
	long currentTime = 0;
	Packet lastPacket;
	int lastPacketWeight = defaultWeight;
	int currentFlowIndex = 0;
	Packet packet;

	if (!debug1) {
		if (argc != 5)
		{
			cout << "Error: Wrong number of arguments" << endl;
			return -1;
		}
		inputPath = argv[1];
		outputPath = argv[2];
		defaultWeight = stoi(argv[3]);
		quantum = stoi(argv[4]);
	}


	SchedulerData flowsDataStruct(quantum);

	ifstream inputFile(inputPath);
	ofstream outputFile(outputPath);
	if (!inputFile.is_open()) {
		cout << "Error: failed to open file " << inputPath << endl;
		return -1;
	}
	if (!outputFile.is_open()) {
		cout << "Error: failed to open file " << outputPath << endl;
		return -1;
	}

	// Try to read first line in file to make sure it isn't empty: 
	lastPacket = receivePacket(flowsDataStruct, inputFile, defaultWeight, isEOF, lastPacketWeight);
	if (isEOF)
	{
		return 0;
	}

	// scheduler runs in while loop until finishing file and sending all packets
	while (true)
	{
		if (flowsDataStruct.empty()) {
			if (isEOF)
			{
				//close things if needed
				return 0;
			}
			if (lastPacket.getArrivalTime() > currentTime)
			{
				currentTime = lastPacket.getArrivalTime();
				currentFlowIndex = 0;
			}
		}

		getPacketsUpToTime(defaultWeight, flowsDataStruct, isEOF, inputFile, currentTime, lastPacket, lastPacketWeight);

		packet = flowsDataStruct.getNextPacketToSend(currentFlowIndex);

		currentTime += packet.getLength();

		outputFile << currentTime << ": " << packet << endl;

	}
}




Packet receivePacket(SchedulerData& flowsDataStruct, ifstream& inputFile, int defaultWeight, bool& isEOF, int& weight)
{
	string line;
	Packet packet;
	if (!getline(inputFile, line)) //reached eof
	{
		isEOF = true;
		return packet;
	}
	stringstream lineStream(line);
	string param;
	vector<string> packetParams;

	// split line to vector with ' ' delimeter
	while (getline(lineStream, param, ' '))
	{
		packetParams.push_back(param);
	}

	// construct flowID: 'Sadd Sport Dadd Dport'
	stringstream stream;
	stream << packetParams[2] << " " << packetParams[3] << " " << packetParams[4] << " " << packetParams[5];
	string flowID = stream.str();

	long pktID = stol(packetParams[0]);
	long pktTime = stol(packetParams[1]);
	int pktLen = stoi(packetParams[6]);
	weight = packetParams.size() > 7 ? stoi(packetParams[7]) : defaultWeight;
	packet = Packet(pktID, pktTime, pktLen, flowID);
	return packet;
}
void getPacketsUpToTime(int defaultWeight, SchedulerData flowsDataStruct, bool& isEOF, ifstream& inputFile,
	long currentTime, Packet& lastPacket, int& lastPacketWeight)
{
	if (isEOF)
	{
		return;
	}
	int lastPacketTime = lastPacket.getArrivalTime();
	if (currentTime < lastPacketTime)
	{
		//cant read yet
		return;
	}
	do
	{	// add last packet to data struct until current time allows
		flowsDataStruct.addPacket(lastPacket, lastPacketWeight);
		lastPacket = receivePacket(flowsDataStruct, inputFile, defaultWeight, isEOF, lastPacketWeight);

	} while (!isEOF && lastPacket.getArrivalTime() <= currentTime);
}