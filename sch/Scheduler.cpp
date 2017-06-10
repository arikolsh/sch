#include "Scheduler.h"
#include <iostream>
#include <sstream>
using namespace std;
Scheduler::Scheduler(int defaultWeight, int quantum) :
	_defaultWeight(defaultWeight), _flowsData(quantum), _lastReceivedPacketWeight(defaultWeight), _currentTime(0), _currentFlowIndex(0), _isEOF(false)
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

void Scheduler::start()
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
			_currentTime = _lastReceivedPacket.getArrivalTime();
			continue;
		}
		Packet packet = _flowsData.getNextPacketToSend(_currentFlowIndex);
		
		_outputFile << _currentTime << ": " << packet.getID() << endl; //send packet

		_currentTime += packet.getLength();
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
