#include "Scheduler.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

using namespace std;

Scheduler::Scheduler(string schedulerType, int defaultWeight, int quantum) : 
	_defaultWeight(defaultWeight),	_schedulerType(schedulerType), _flowsData(quantum), 
	_isEOF(false), _lastReceivedPacketWeight(defaultWeight), _currentTime(0), _currentFlowIndex(0) {}

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
	//check if file not empty
	_lastReceivedPacket = getPacketFromFile(_lastReceivedPacketWeight);
	if (_isEOF) { return false; } //empty file
	//rewind state of input file stream
	_inputFile.clear();
	_inputFile.seekg(0);
	if (!_outputFile.is_open()) {
		cout << "Error: failed to open file " << outPath << endl;
		return false;
	}
	return true;
}

void Scheduler::run()
{
	_lastReceivedPacket = Packet(); //default constructor -> id is -1, will be dropped in addPacket (first iteration)
	while (true) {
		//get packets from file until(including) current time
		while (_lastReceivedPacket.getArrivalTime() <= _currentTime && !_isEOF)
		{
			//add last packet from previous getPacketFromFile
			_flowsData.addPacket(_lastReceivedPacket, _lastReceivedPacketWeight);
			//get next packet from file, update _lastReceivedPacketWeight and _isEOF
			_lastReceivedPacket = getPacketFromFile(_lastReceivedPacketWeight);
		}
		if (_flowsData.isEmpty())
		{
			if (_isEOF) { break; } //reached end of file and no more packets to send
			_flowsData.resetAllFlowsCredit();
			_currentFlowIndex = 0;
			_currFlowChanged = true;
			_currentTime = _lastReceivedPacket.getArrivalTime();
			_flowsData.clear();
			continue;
		}
		vector<Packet> packetsToSend = _schedulerType == "DRR" ? 
			_flowsData.getPacketsToSend_DRR(_currentFlowIndex) : getPacketsToSend_WRR(_currentFlowIndex);
		for (int i = 0; i < packetsToSend.size(); i++)
		{
			_outputFile << _currentTime << ": " << packetsToSend[i].getID() << endl; //send packet
			_currentTime += packetsToSend[i].getLength();
		}
	}
}

Packet Scheduler::getPacketFromFile(int& weight)
{
	string line;
	Packet packet; //default constructor -> invalid packet
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
	packet = Packet(pktID, pktTime, pktLen, flowID); //-> valid packet
	return packet;
}

vector<Packet> Scheduler::getPacketsToSend_WRR(int& currFlow)
{
	vector<Packet> packetsToSend;
	while (get<0>(_flowsData._allFlowTuples[currFlow]).empty()) // Skip empty queues
	{
		currFlow = (currFlow + 1) % _flowsData._allFlowTuples.size();
		_packetsSent = 0;
	}
	queue<Packet>& flowQueue = get<0>(_flowsData._allFlowTuples[currFlow]); // The queue that should be served
	int weight = get<1>(_flowsData._allFlowTuples[currFlow]);
	while (!flowQueue.empty() && _packetsSent < weight)
	{
		packetsToSend.push_back(flowQueue.front()); //Add packet from queue to the vector of packets to be sent
		flowQueue.pop(); //pop packet from queue
		_packetsSent++;
		_flowsData._totalPackets--;
	}
	if (_packetsSent == weight) // Finished serving this flow
	{
		currFlow = (currFlow + 1) % _flowsData._allFlowTuples.size(); // Go to next flow
		_packetsSent = 0;
	}
	return packetsToSend;
}
