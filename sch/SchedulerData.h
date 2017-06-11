#pragma once
#include <vector>
#include "queue"
#include "Packet.h"
#include <map>

class SchedulerData
{
public:
	explicit SchedulerData(int quantum);
	bool isEmpty() const { return _totalPackets <= 0; }
	void resetAllFlowsCredit();
	void clear();
	/* list of flow pairs, every tuple contains queues of packets and the flow weight and current credit */
	std::vector<std::tuple<std::queue<Packet>, int, int>> _allFlowTuples;
	/* map of all flows currently handled by the manager. key is flowID and value is the index
	* of its specific packet queue in _allFlowTuples. */
	std::map<std::string, int> _flows;
	void SchedulerData::addPacket(Packet& packet, int weight);
	std::vector<Packet> getPacketsToSend_DRR(int& currFlow);
	long _totalPackets = 0;
	int _quantum;
};
