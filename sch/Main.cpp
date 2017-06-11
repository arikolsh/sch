#include "Scheduler.h"
#include <iostream>

using namespace std;

bool validCmdArgs(int argc, char** argv, string &schedulerType, string &input_file, string &output_file, int &default_weight, int &quantum);

int main(int argc, char* argv[])
{
	string schedulerType, input_file, output_file;
	int default_weight, quantum;
	
	if (!validCmdArgs(argc, argv, schedulerType, input_file, output_file, default_weight, quantum))
	{
		return 1;
	}

	// Create the Scheduler class object:
	Scheduler scheduler(schedulerType, default_weight, quantum);
	// Initialize the Scheduler - open files for read/write and make sure input file isn't empty:
	bool schedulerInitialized = scheduler.init(input_file, output_file);
	if (!schedulerInitialized)
	{
		return 1; // Error occurred (printed to screen) - cannot continue.
	}
	scheduler.run(); // Run the scheduler in "DRR" or "RR" mode (according to schedulerType provided)
	cout << "Done." << endl; 
	
	return 0;
}

bool validCmdArgs(int argc, char** argv, string &schedulerType, string &input_file, string &output_file, int &default_weight, int &quantum)
{
	if (argc != 6)
	{
		cout << "This program requires exactly 6 Command-line arguments:" << endl;
		cout << "For example: \n1) sch.exe  DRR  input_file.txt  output_file.txt  10  64 \n2) sch.exe  RR   input_file.txt  output_file.txt  1   0" << endl;
		return false;
	}
	try // Try to receive and parse user arguments
	{
		schedulerType = argv[1], input_file = argv[2], output_file = argv[3];
		default_weight = stoi(argv[4]), quantum = stoi(argv[5]);
		if (schedulerType != "RR" && schedulerType != "DRR")
		{
			cout << "Scheduler type can only be 'RR' or 'DRR'" << endl;
			return false;
		}
	}
	catch (...) //catch all exceptions
	{
		cout << "Failed to extract command-line arguments" << endl;
		return false;
	}
	return true;
}