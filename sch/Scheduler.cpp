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
	// Try to read first line in file to make sure it isn't empty: 
	int receivedPacketWeight = _defaultWeight;
	Packet lastPacket = receivePacket(receivedPacketWeight);
	if (_isEOF)
	{
		return;
	}
	// scheduler runs in while loop until finishing file and sending all packets
	while (true)
	{
		if (_flowsData.empty()) {
			if (_isEOF)
			{
				return;
			}
			if (lastPacket.getArrivalTime() > _currentTime)
			{
				_currentTime = lastPacket.getArrivalTime();
				_currentFlowIndex = 0;
			}
		}

		getPacketsUpToTime(lastPacket, receivedPacketWeight);
		Packet packet = _flowsData.getNextPacketToSend(_currentFlowIndex);
		_currentTime += packet.getLength();
		_outputFile << _currentTime << ": " << packet << endl;
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

void Scheduler::getPacketsUpToTime(Packet& lastReceivedPacket, int& lastReceivedPacketWeight)
{
	if (_isEOF)
	{
		return;
	}
	int lastPacketTime = lastReceivedPacket.getArrivalTime();
	if (_currentTime < lastPacketTime)
	{
		//cant read yet
		return;
	}
	do
	{	// add last packet to data struct until current time allows
		_flowsData.addPacket(lastReceivedPacket, lastReceivedPacketWeight);
		lastReceivedPacket = receivePacket(lastReceivedPacketWeight);

	} while (!_isEOF && lastReceivedPacket.getArrivalTime() <= _currentTime);
}
