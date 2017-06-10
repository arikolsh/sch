#pragma once
#include <vector>
#include <queue>
#include "Packet.h"
#include <memory>
#include <map>
#include <fstream>

class SchedulerData
{
	//todo: setters and getters.. and put members in private
	long _totalPackets = 0;
	int _quantum;
public:
	explicit SchedulerData(int quantum);
	bool isEmpty() const { return _totalPackets <= 0; }
	void resetAllFlowsCredit();
	/* list of flow pairs, every tuple contains queues of packets and the flow weight and current credit */
	std::vector<std::tuple<std::queue<Packet>, int, int>> _allFlowTuples;
	/* map of all flows currently handled by the manager. key is flowID and value is the index
	* of its specific packet queue in _allFlowTuples. */
	std::map<std::string, int> _flows;
	void SchedulerData::addPacket(Packet& packet, int weight);
	Packet getNextPacketToSend(int& currFlow);
};
