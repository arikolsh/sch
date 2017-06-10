#include "Scheduler.h"
#include <iostream>

using namespace std;
bool validCmdArgs(int argc, char** argv, string &schedulerType, string &input_file, string &output_file, int &default_weight, int &quantum);
int main(int argc, char* argv[])
{
	string schedulerType, input_file, output_file;
	int default_weight, quantum;
	/*if (!validCmdArgs(argc, argv, schedulerType, input_file, output_file, default_weight, quantum))
	{
		return 1;
	}*/   //todo: uncomment
	Scheduler scheduler(30, 40);
	bool isSuccess = scheduler.init("inp3.txt", "out.txt");
	if (!isSuccess)
	{
		return 1;
	}
	scheduler.start();
}

bool validCmdArgs(int argc, char** argv, string &schedulerType, string &input_file, string &output_file, int &default_weight, int &quantum)
{
	if (argc != 6)
	{
		cout << "This program requires exactly 6 Command-line arguments:" << endl;
		cout << "For example: \n1) sch.exe  DRR  input_file.txt  output_file.txt  10  64 \n2) sch.exe  RR   input_file.txt  output_file.txt  1   0" << endl;
		return false;
	}
	try
	{
		schedulerType = argv[1], input_file = argv[2], output_file = argv[3];
		default_weight = stoi(argv[4]), quantum = stoi(argv[5]);
	}
	catch (...) //catch all exceptions
	{
		cout << "Failed to extract command-line arguments" << endl;
		return false;
	}
	return true;
}


