#pragma once
#include "SchedulerData.h"
#include <fstream>

using namespace std;

class Scheduler
{
public:

	explicit Scheduler(string schedulerType, int default_weight, int quantum);
	~Scheduler();
	bool init(const string& inPath, const std::string& outPath);
	void start();
	/* get packet from file and put in appropriate queue in dataStruct. if reached EOF update isEOF to true.
	 * put weight of packet in weight: if not first packet of flow weight will be default weight. this is okay because
	 * the actual weight of the flow was already determined and put in the flows Data structure. */
	Packet receivePacket(int& weight);
	/* read packets from file up to given time */
	void getPacketsUpToCurrentTime(Packet& lastReceivedPacket, int& lastReceivedPacketWeight);

private:
	SchedulerData _flowsData; // This class object holds all the flows (and additional data) that served the scheduler
	int _defaultWeight;
	int _currentTime = 0;
	int _currentFlowIndex = 0;
	string _schedulerType; // "RR" or "DRR"
	ifstream _inputFile;
	ofstream _outputFile;
	bool _isEOF = false;
	bool _schedulerDone = false; // Set to true when reached EOF and all queues are empty
};
