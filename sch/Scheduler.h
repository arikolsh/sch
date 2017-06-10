#pragma once
#include <string>
#include "SchedulerData.h"

class Scheduler
{
	int _defaultWeight;
	SchedulerData _flowsData;
	std::ifstream _inputFile;
	std::ofstream _outputFile;
	bool _isEOF = false;
	int _currentTime = 0;
	int _currentFlowIndex = 0;
public:
	explicit Scheduler(int weight, int quantum);
	~Scheduler();
	bool init(const std::string& inPath, const std::string& outPath);
	void start();
	Packet receivePacket(int& weight);
	void getPacketsUpToTime(Packet& lastReceivedPacket, int& lastReceivedPacketWeight);
};
