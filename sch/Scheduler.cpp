#include "Scheduler.h"
#include <iostream>
using namespace std;
Scheduler::Scheduler(int weight, int quantum) :
	_weight(weight), _quantum(quantum)
{
}

Scheduler::~Scheduler()
{
	_inputFile.close();
	_outputFile.close();
}

bool Scheduler::init(const std::string&  inPath, const std::string&  outPath)
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
	_flowsData = SchedulerData();
	return true;
}

void Scheduler::start()
{
	
}
