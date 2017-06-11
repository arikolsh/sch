#pragma once
#include "SchedulerData.h"
#include <fstream>

class Scheduler
{
public:
	explicit Scheduler(std::string schedulerType, int weight, int quantum);
	~Scheduler();
	bool init(const std::string& inPath, const std::string& outPath);
	void run();
	/* Get packet from file and put in appropriate queue in dataStruct. if reached EOF update isEOF to true.
	 * put weight of packet in weight: if not first packet of flow weight will be default weight. this is okay because
	 * the actual weight of the flow was already determined and put in the flows Data structure. */
	Packet getPacketFromFile(int& weight);
	/* Return up to "weight" packets from the flow that shpuld be currently served according to WRR algorithm */
	std::vector<Packet> getPacketsToSend_WRR(int& currFlow);

private:
	int _defaultWeight;
	std::string _schedulerType; // "RR" or "DRR"
	SchedulerData _flowsData; // SchedulerData class object that holds all flows and relevant data ("On-Line")
	std::ifstream _inputFile;
	std::ofstream _outputFile;
	bool _isEOF; // Set to true once reached end of input file
	Packet _lastReceivedPacket;
	int _lastReceivedPacketWeight;
	int _currentTime;
	int _currentFlowIndex; // Keep track of the flow that should be served next
	bool _currFlowChanged = false;
	int _packetsSent = 0; // In "RR" mode we send packets until _packetsSent = weight (or no more packets to send from flow)
};
