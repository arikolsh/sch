#include "SchedulerData.h"
using namespace std;

SchedulerData::SchedulerData(int quantum) : _quantum(quantum) {}

void SchedulerData::addPacket(Packet& packet, int weight)
{
	if (packet.getID() < 0)	{ return; } //not valid packet, drop
	auto iter = _flowsMap.find(packet.getFlowID());
	if (iter != _flowsMap.end()) // flow already in dataStruct
	{
		int flowIndex = iter->second;
		get<0>(_allFlows[flowIndex]).push(packet); //push packet to existing queue
		_totalPackets++;
	}
	else  // encountered new flow, init new queue for this flow
	{
		queue<Packet> flowQueue; //init new queue for this flow
		flowQueue.push(packet);
		_allFlows.push_back(make_tuple(flowQueue, weight, 0));
		_flowsMap[packet.getFlowID()] = _allFlows.size() - 1; // add flow index to map
		_totalPackets++;
	}
}

Packet SchedulerData::getNextPacketToSend(int& currFlow)
{
	//todo: check, probably not correct
	while (true) {
		while (get<0>(_allFlows[currFlow]).empty())
		{
			get<2>(_allFlows[currFlow]) = 0; //second time we arrived to this flow, zero credit
			currFlow = (currFlow + 1) % _allFlows.size();
		}
		queue<Packet>& flowQueue = get<0>(_allFlows[currFlow]);
		Packet packet = flowQueue.front(); //get packet from queue
		int weight = get<1>(_allFlows[currFlow]);
		int& credit = get<2>(_allFlows[currFlow]);
		credit += weight*_quantum;
		if (packet.getLength() <= credit)
		{ //able to send
			flowQueue.pop(); //pop packet from queue
			_totalPackets--;
			return packet;
		}
		currFlow = (currFlow + 1) % _allFlows.size(); // Go to next flow
	}

}


