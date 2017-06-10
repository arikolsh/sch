#include <ostream>
#include <iostream>
#include <string>
#include <vector>
#include "Scheduler.h"
#include <iostream>

using namespace std;

/* Command-line arguments:
* 0) The program name: "sch.exe" (string)
* 1) The scheduler type: "RR" or "DRR" (string)
* 2) input_file name (string)
* 3) output_file name (string)
* 4) default_weight (integer) - for the flows
* 5) quantum (integer) - 0 for "RR" or the quantum size in bytes for "DRR" scheduler
* Examples:   sch.exe DRR input_file.txt output_file.txt 10 64	 OR	  sch.exe RR input_file.txt output_file.txt 1 0 */
bool validCmdArgs(int argc, char** argv, string &schedulerType, string &input_file, string &output_file, int &default_weight, int &quantum);

int main(int argc, char* argv[])
{
	string schedulerType, input_file, output_file; 
	int default_weight, quantum;
	if (!validCmdArgs(argc, argv, schedulerType, input_file, output_file, default_weight, quantum)) { return 1; }
	Scheduler scheduler(schedulerType, default_weight, quantum);
	if (!scheduler.init(input_file, output_file)) {	return 0; }
	scheduler.start();
	cout << "Scheduler is done handling all packets!" << endl;
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
	try
	{
		schedulerType = argv[1], input_file = argv[2], output_file = argv[3];
		default_weight = stoi(argv[4]), quantum = stoi(argv[5]);
	}
	catch (...) //catch all exceptions
	{
		cout << "Failed to extract command-line arguments"  << endl;
		return false;
	}
	return true;
}