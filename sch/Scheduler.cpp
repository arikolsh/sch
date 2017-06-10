#include "Scheduler.h"
#include <iostream>
#include <sstream>
using namespace std;
Scheduler::Scheduler(int defaultWeight, int quantum) :
	_defaultWeight(defaultWeight), _flowsData(quantum)
{
}

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
		cout << "Error: failed to open file " << inPath << endl;
		return false;
	}
	if (!_outputFile.is_open()) {
		cout << "Error: failed to open file " << outPath << endl;
		return false;
	}
	return true;
}

void Scheduler::start()
{
	Packet lastPacketReceived; //first packet with time 0 and id -1
	int lastWeightReceived = _defaultWeight;
	// scheduler runs in while loop until finishing file and sending all packets
	while (true)
	{
		getPacketsUpToCurrentTime(lastPacketReceived, lastWeightReceived);
		Packet packet = _flowsData.getNextPacketToSend(_currentFlowIndex);
		_currentTime += packet.getLength();
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

void Scheduler::getPacketsUpToCurrentTime(Packet& lastReceivedPacket, int& lastReceivedPacketWeight)
{
	if (_isEOF || _currentTime < lastReceivedPacket.getArrivalTime())
	{
		return;
	}
	do
	{	// add last packet to data struct until current time allows
		_flowsData.addPacket(lastReceivedPacket, lastReceivedPacketWeight);
		lastReceivedPacket = receivePacket(lastReceivedPacketWeight);

	} while (!_isEOF && lastReceivedPacket.getArrivalTime() <= _currentTime);
}
