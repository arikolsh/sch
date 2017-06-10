#include <ostream>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include "Packet.h"
#include "SchedulerData.h"
#include "Scheduler.h"

using namespace std;
int main(int argc, char* argv[])
{

	//todo: parse & check arguments from cmd
	Scheduler scheduler(1, 0);
	bool isSuccess = scheduler.init("inp.txt", "out.txt");
	if (!isSuccess)
	{
		return 0;
	}
	scheduler.start();
}


