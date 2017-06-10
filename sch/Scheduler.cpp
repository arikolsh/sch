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

void Scheduler::start()
{
	Packet packetToSend, lastPacketReceived; //first packet with time 0 and id -1
	int lastWeightReceived = _defaultWeight;
	while (true) // scheduler runs in while loop until finishing file and sending all packets
	{
		_flowsData.logSchedulerData("stdout"); // Pass a path (like "log.txt") to write to a file or "stdout" to print to screen
		cout << "lastPacketReceived: " << lastPacketReceived << endl;
		getPacketsUpToCurrentTime(lastPacketReceived, lastWeightReceived);
		if (_schedulerDone)
		{
			return;
		}
		packetToSend = _flowsData.getNextPacketToSend(_currentFlowIndex);
		cout << "packetToSend: " << packetToSend << endl;;
		_currentTime += packetToSend.getLength(); // increment time after each packet is sent
	}
}

void Scheduler::getPacketsUpToCurrentTime(Packet& lastPacketReceived, int& lastWeightReceived)
{
	if (_isEOF || _currentTime < lastPacketReceived.getArrivalTime())
	{
		if (_flowsData.empty()) // reached EOF and all queues are empty --> scheduler is done!
		{
			_schedulerDone = true;
		}
		return;
	}
	do
	{	// add last packet to data struct until current time allows
		_flowsData.addPacket(lastPacketReceived, lastWeightReceived);
		lastPacketReceived = receivePacket(lastWeightReceived);

	} while (!_isEOF && lastPacketReceived.getArrivalTime() <= _currentTime);
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