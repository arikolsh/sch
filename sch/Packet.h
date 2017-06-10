#pragma once
#include <string>

class Packet
{
	/*packet id*/
	long _ID;
	/*arrival time*/
	long _arrivalTime;
	/*length*/
	int _length;
	/*flow id*/
	std::string _flowID;
public:
	explicit Packet();
	explicit Packet(long id, long time, int length, std::string flowID);
	long getID() const;
	long getArrivalTime() const;
	int getLength() const;
	std::string getFlowID() const;
	/*tostring method will generate <time: pktID> */
	friend std::ostream & operator<<(std::ostream & str, Packet const & p);

};


