#include "Scheduler.h"
#include <iostream>
#include <sstream>
using namespace std;

Scheduler::Scheduler(string schedulerType, int default_weight, int quantum) : 
	_defaultWeight(default_weight), _schedulerType(schedulerType), _flowsData(quantum){}

Scheduler::~Scheduler()
{
	_inputFile.close();
	_outputFile.close();
}

bool Scheduler::init(const string&  inPath, const string&  outPath)
{
	_inputFile = ifstream(inPath);
	_outputFile = ofstream(outPath);
	if (!_inputFile.is_open()) {
		cout << "Error: failed to open input file: " << inPath << endl;
		return false;
	}
	if (!_outputFile.is_open()) {
		cout << "Error: failed to open output file: " << outPath << endl;
		return false;
	}
	return true;
}

void Scheduler::run()
{
	Packet packetToSend, lastPacketReceived; //first packet with time 0 and id -1
	int lastWeightReceived = _defaultWeight;
	while (true) // scheduler runs in while loop until finishing file and sending all packets
	{
		_flowsData.logSchedulerData("logSchedulerData.txt"); // Pass a path (like "log.txt") to write to a file or "stdout" to print to screen
		getPacketsUpToCurrentTime(lastPacketReceived, lastWeightReceived);
		if (_schedulerDone)
		{
			return;
		}
		if (_schedulerType == "DRR")  // DRR scheduling
		{
			packetToSend = _flowsData.nextPacketToSend_DRR(_currentFlowIndex);
			_outputFile << _currentTime << ": " << packetToSend.getID() << endl; //send packet
			_currentTime += packetToSend.getLength(); // increment time after each packet is sent
		}
		else // RR scheduling
		{
			sendPackets_WRR(_currentFlowIndex);
		}

	}
}

void Scheduler::getPacketsUpToCurrentTime(Packet& lastPacketReceived, int& lastWeightReceived)
{
	if (_isEOF)
	{
		_schedulerDone = _flowsData.empty(); // reached EOF and all queues are empty --> scheduler is done!
		return;
	}
	if (_currentTime < lastPacketReceived.getArrivalTime() && !_flowsData.empty())
	{
		return;
	}
	if (_flowsData.empty() && _currentTime < lastPacketReceived.getArrivalTime())
	{
		_currentTime = lastPacketReceived.getArrivalTime();
	}
	do
	{	// add last packet to data struct until current time allows
		_flowsData.addPacket(lastPacketReceived, lastWeightReceived);
		lastPacketReceived = receivePacket(lastWeightReceived);

	} while (!_isEOF && lastPacketReceived.getArrivalTime() <= _currentTime);
}

int packets_sent = 0;
void Scheduler::sendPackets_WRR(int& currFlow)
{
	if (_flowsData._totalPackets == 0) {
		return; // All queues are empty --> Nothing to send
	}
	while (get<0>(_flowsData._allFlows[currFlow]).empty()) // Skip empty queues
	{
		currFlow = (currFlow + 1) % _flowsData._allFlows.size();
		packets_sent = 0;
	}
	queue<Packet>& flowQueue = get<0>(_flowsData._allFlows[currFlow]); // The queue that should be served
	int weight = get<1>(_flowsData._allFlows[currFlow]);
	while (!flowQueue.empty() && packets_sent < weight)
	{
		Packet packetToSend = flowQueue.front(); //get packet from queue
		flowQueue.pop(); //pop packet from queue
		packets_sent++;
		_flowsData._totalPackets--;
		_outputFile << _currentTime << ": " << packetToSend.getID() << endl; //send packet
		_currentTime += packetToSend.getLength(); // increment time after each packet is sent
	}
	if (packets_sent == weight) // Finished serving this flow
	{
		currFlow = (currFlow + 1) % _flowsData._allFlows.size(); // Go to next flow
		packets_sent = 0;
	}
}


Packet Scheduler::receivePacket(int& weight)
{
	string line;
	Packet packet;
	if (!getline(_inputFile, line)) //reached eof
	{
		_isEOF = true;
		return packet;
	}
	stringstream lineStream(line);
	string param;
	vector<string> packetParams;

	// split line to vector with ' ' delimeter
	while (getline(lineStream, param, ' '))
	{
		packetParams.push_back(param);
	}

	// construct flowID: 'Sadd Sport Dadd Dport'
	stringstream stream;
	stream << packetParams[2] << " " << packetParams[3] << " " << packetParams[4] << " " << packetParams[5];
	string flowID = stream.str();

	long pktID = stol(packetParams[0]);
	long pktTime = stol(packetParams[1]);
	int pktLen = stoi(packetParams[6]);
	weight = packetParams.size() > 7 ? stoi(packetParams[7]) : _defaultWeight;
	packet = Packet(pktID, pktTime, pktLen, flowID);
	return packet;
}
