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
/* get packet from file and put in appropriate queue in dataStruct. if reached EOF update isEOF to true.*/
void receivePacket(SchedulerData &flowsDataStruct, ifstream &inputFile, int defaultWeight, bool &isEOF);

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
	int quantum;
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
	// todo: need to receive packets as long as packet time is equal to current time. then dispatch packets from dataStruct until reached last received packet time
	// todo: need to put this in some kind of while (true) loop
	if (!isEOF) {
		receivePacket(flowsDataStruct, inputFile, defaultWeight, isEOF);
	}

	cout << flowsDataStruct._flowQueueWeightPairs[0].first.front() << endl; //debug
	cout << flowsDataStruct._flowQueueWeightPairs[0].first.front().getFlowID() << endl; //debug

	if (!isEOF) {
		receivePacket(flowsDataStruct, inputFile, defaultWeight, isEOF);
	}

	cout << flowsDataStruct._flowQueueWeightPairs[1].first.front() << endl; //debug
	cout << flowsDataStruct._flowQueueWeightPairs[1].first.front().getFlowID() << endl; //debug

	//todo: dispatchPacket() in some kind of loop


}

void receivePacket(SchedulerData &flowsDataStruct, ifstream &inputFile, int defaultWeight, bool& isEOF)
{
	string line;
	if (!getline(inputFile, line)) //reached eof
	{
		isEOF = true;
		return;
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
	int weight = packetParams.size() > 7 ? stoi(packetParams[7]) : defaultWeight;
	flowsDataStruct.addPacket(pktID, pktTime, pktLen, flowID, weight);
}
