#include "SchedulerData.h"
using namespace std;
SchedulerData::SchedulerData() :
	_flowQueueWeightPairs(), _flows()
{
}

void SchedulerData::addPacket(long pktID, long pktTime, int pktLen, string flowID, int weight)
{
	// construct packet
	Packet packetReceived(pktID, pktTime, pktLen, flowID);
	if (_flows.find(flowID) != _flows.end()) // flow already in dataStruct
	{
		int flowIndex = _flows.find(flowID)->second;
		_flowQueueWeightPairs[flowIndex].first.push(packetReceived);
	}
	else  // encountered new flow, init new queue for this flow
	{
		queue<Packet> flowQueue;
		flowQueue.push(packetReceived);
		pair <queue<Packet>, int> flowPair = make_pair(flowQueue, weight);
		_flowQueueWeightPairs.push_back(flowPair);
		_flows[flowID] = _flowQueueWeightPairs.size() - 1; // put flow index in map
	}
}


