#include "SchedulerData.h"
#include <sstream>
#include <iostream>
#include <fstream>
using namespace std;

SchedulerData::SchedulerData(int quantum) : _quantum(quantum) {}

void SchedulerData::addPacket(Packet& packet, int weight)
{
	if (packet.getID() < 0) { return; } //not valid packet, drop
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

Packet SchedulerData::nextPacketToSend_DRR(int& currFlow)
{
	while (true) {
		while (get<0>(_allFlows[currFlow]).empty()) // Skip empty queues
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

// Pass a path (like "log.txt") to write to a file or "stdout" to print to screen
bool fileIsOpen;
void SchedulerData::logSchedulerData(string out) const
{
	ostringstream stream;
	stream << "\n================ The Scheduler's data ===================" << endl;
	for (int i = 0; i < _allFlows.size(); i++)
	{
		stream << "flow " << i << ": ";
		auto flow_queue = get<0>(_allFlows[i]);
		auto flow_weight = get<1>(_allFlows[i]);
		auto flow_credit = get<2>(_allFlows[i]);
		stream << "Weight = " << flow_weight << ", Credit = " << flow_credit;
		stream << ", Queue size = " << flow_queue.size() << ", Packets in queue: ";
		while (!flow_queue.empty())
		{
			stream << flow_queue.front() << " ";
			flow_queue.pop();
		}
		stream << endl;
	}
	if (out == "stdout")
	{
		cout << stream.str() << endl;
	}
	else
	{
		fstream outFile;
		if (!fileIsOpen)
		{
			outFile.open(out, fstream::out);
		}
		else
		{
			outFile.open(out);
			outFile.seekg(0, ios::end);
		}
		outFile << stream.str();
		outFile.close();
	}
}
