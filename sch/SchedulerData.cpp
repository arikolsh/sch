#include "SchedulerData.h"
using namespace std;
SchedulerData::SchedulerData(int quantum) : _quantum(quantum)
{
}

void SchedulerData::addPacket(Packet& packet, int weight)
{
	if (packet.getID() < 0) //not valid packet, drop
	{
		return;
	}
	// construct packet
	Packet packetReceived = packet;
	if (_flows.find(packetReceived.getFlowID()) != _flows.end()) // flow already in dataStruct
	{
		int flowIndex = _flows.find(packetReceived.getFlowID())->second;
		get<0>(_allFlowTuples[flowIndex]).push(packetReceived); //put packet in queue
	}
	else  // encountered new flow, init new queue for this flow
	{
		queue<Packet> flowQueue;
		flowQueue.push(packetReceived);
		tuple <queue<Packet>, int, int> flowTuple = make_tuple(flowQueue, weight, 0);
		_allFlowTuples.push_back(flowTuple);
		_flows[packetReceived.getFlowID()] = _allFlowTuples.size() - 1; // put flow index in map
	}
}

Packet SchedulerData::getNextPacketToSend(int& currFlow)
{
	while (true) {
		while (get<0>(_allFlowTuples[currFlow]).empty())
		{
			currFlow = (currFlow + 1) % _allFlowTuples.size();
		}
		auto flowQueue = get<0>(_allFlowTuples[currFlow]);
		Packet packet = flowQueue.front(); //get packet from queue
		int weight = get<1>(_allFlowTuples[currFlow]);
		int& credit = get<2>(_allFlowTuples[currFlow]);
		credit += weight*_quantum;
		if (packet.getLength() <= credit)
		{ //able to send
			flowQueue.pop(); //pop packet from queue
			_totalPackets--;
			if (flowQueue.empty())
			{
				credit = 0; //clear credit if queue got empty
			}
			return packet;
		}
		currFlow = (currFlow + 1) % _allFlowTuples.size(); // Go to next flow
	}

}


