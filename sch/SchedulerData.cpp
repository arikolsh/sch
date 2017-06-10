#include "SchedulerData.h"
using namespace std;
SchedulerData::SchedulerData(int quantum) : _quantum(quantum)
{
}

void SchedulerData::resetAllFlowsCredit()
{
	for (int i = 0; i < _allFlowTuples.size(); i++)
	{
		get<2>(_allFlowTuples[i]) = 0;
	}
}

void SchedulerData::clear()
{
	_flows.clear();
	_allFlowTuples.clear();
}

void SchedulerData::addPacket(Packet& packet, int weight)
{
	if (packet.getID() < 0) //not valid packet, drop
	{
		return;
	}
	//cout << "adding packet: " << packet << endl;
	Packet packetReceived = packet;
	if (_flows.find(packetReceived.getFlowID()) != _flows.end()) // flow already in dataStruct
	{
		int flowIndex = _flows.find(packetReceived.getFlowID())->second;
		queue<Packet>& queue = get<0>(_allFlowTuples[flowIndex]);
		queue.push(packetReceived); //put packet in queue
	}
	else  // encountered new flow, init new queue for this flow
	{
		queue<Packet> flowQueue;
		flowQueue.push(packetReceived);
		tuple <queue<Packet>, int, int> flowTuple = make_tuple(flowQueue, weight, 0);
		_allFlowTuples.push_back(flowTuple);
		_flows[packetReceived.getFlowID()] = _allFlowTuples.size() - 1; // put flow index in map
	}

	_totalPackets++;
}

Packet SchedulerData::getNextPacketToSend(int& currFlow)
{
	auto MAX = [](auto a, auto b) { return (a > b) ? a : b; };
	while (true) {
		while (get<0>(_allFlowTuples[currFlow]).empty()) //traverse flows until found non empty flow
		{
			get<2>(_allFlowTuples[currFlow]) = 0; //second time we arrived to this flow, zero credit
			currFlow = (currFlow + 1) % _allFlowTuples.size();
		}
		// found non-empty flow
		queue<Packet>& flowQueue = get<0>(_allFlowTuples[currFlow]);
		Packet packet = flowQueue.front(); //get packet from queue
		int weight = get<1>(_allFlowTuples[currFlow]);
		int& credit = get<2>(_allFlowTuples[currFlow]);
		credit += weight*_quantum;
		if (packet.getLength() <= credit)
		{ //able to send
			flowQueue.pop(); //pop packet from queue
			_totalPackets--;
			credit = MAX(credit - packet.getLength(), 0);
			if (flowQueue.empty() || credit < (flowQueue.front()).getLength())
			{
				if (flowQueue.empty()) { credit = 0; }
				currFlow = (currFlow + 1) % _allFlowTuples.size(); // Go to next flow
			}
			return packet;
		}
		// credit not enough for current flow, proceed to next flow
		currFlow = (currFlow + 1) % _allFlowTuples.size(); // Go to next flow
	}
}

vector<Packet> SchedulerData::getNextPacketsToSend(int& currFlow)
{
	vector<Packet> packetsToSend;
	auto MAX = [](auto a, auto b) { return (a > b) ? a : b; };
	while (true) {
		while (get<0>(_allFlowTuples[currFlow]).empty()) //traverse flows until found non empty flow
		{
			get<2>(_allFlowTuples[currFlow]) = 0; //second time we arrived to this flow, zero credit
			currFlow = (currFlow + 1) % _allFlowTuples.size();
		}
		// found non-empty flow
		queue<Packet>& flowQueue = get<0>(_allFlowTuples[currFlow]);
		Packet packet = flowQueue.front(); //get packet from queue
		int weight = get<1>(_allFlowTuples[currFlow]);
		int& credit = get<2>(_allFlowTuples[currFlow]);
		credit += weight*_quantum;
		while (packet.getLength() <= credit) {
			flowQueue.pop(); //pop packet from queue
			_totalPackets--;
			credit = MAX(credit - packet.getLength(), 0);
			packetsToSend.push_back(packet);
			if (flowQueue.empty())
			{
				credit = 0;
				break;
			}
			packet = flowQueue.front();
		}
		// credit not enough for current flow, proceed to next flow
		currFlow = (currFlow + 1) % _allFlowTuples.size(); // Go to next flow
		if (packetsToSend.size() > 0) { return packetsToSend; }
	}
}

