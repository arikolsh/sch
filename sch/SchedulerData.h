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
public:
	SchedulerData();
	/* list of flow pairs, every pair contains queues of packets and the flow weigth */
	std::vector<std::pair<std::queue<Packet>, int>> _flowQueueWeightPairs;
	/* map of all flows currently handled by the manager. key is flowID and value is the index
	* of its specific packet queue in flowPacketQueues. */
	std::map<std::string, int> _flows;
	void addPacket(long pktID, long pktTime, int pktLen, std::string flowID, int weight);
};
