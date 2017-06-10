#pragma once
#include <string>
#include "SchedulerData.h"

class Scheduler
{
	int _weight;
	int _quantum;
	SchedulerData _flowsData;
	std::ifstream _inputFile;
	std::ofstream _outputFile;
public:
	explicit Scheduler(int weight, int quantum);
	~Scheduler();
	bool init(const std::string& inPath, const std::string& outPath);
	void start();
};
