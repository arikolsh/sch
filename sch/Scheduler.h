#pragma once
#include <string>
#include "SchedulerData.h"

class Scheduler
{
	int _defaultWeight;
	SchedulerData _flowsData;
	std::ifstream _inputFile;
	std::ofstream _outputFile;
	bool _isEOF;
	Packet _lastReceivedPacket;
	int _lastReceivedPacketWeight;
	int _currentTime;
	int _currentFlowIndex;
	bool _currFlowChanged=false;
public:

	explicit Scheduler(int weight, int quantum);
	~Scheduler();
	bool init(const std::string& inPath, const std::string& outPath);
	void start();
	/*
	* get packet from file and put in appropriate queue in dataStruct. if reached EOF update isEOF to true.
	* put weight of packet in weight: if not first packet of flow weight will be default weight. this is okay because
	* the actual weight of the flow was already determined and put in the flows Data structure.
	*/
	Packet getPacketFromFile(int& weight);
};
