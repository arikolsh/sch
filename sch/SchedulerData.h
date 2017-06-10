#pragma once
#include <vector>
#include <queue>
#include "Packet.h"
#include <map>

using namespace std;

class SchedulerData
{
public:
	explicit SchedulerData(int quantum);
	
	bool empty() const { return _totalPackets <= 0; };
	
	/* A vector that contains all flows -
	 * each flow is represented by a tuple of: 
	 * 1) the packets queue for this flow, 2) the flow weight and 3) the current credit */
	vector<tuple<queue<Packet>, int, int>> _allFlows;
	
	/* map of all flows currently handled by the manager. 
	 * key = flowID and value = index of this flow in _allFlows. */
	map<string, int> _flowsMap;
	
	void SchedulerData::addPacket(Packet& packet, int weight);
	
	Packet getNextPacketToSend(int& currFlow);

	void logSchedulerData(string out) const;

private:
	long _totalPackets = 0;
	int _quantum;
};
