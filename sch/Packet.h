#pragma once
#include <string>

class Packet
{
	long _ID; /* packet id */
	long _arrivalTime; /* packet arrival time */
	int _length; /* packet length */
	std::string _flowID; /* flow id */
public:
	explicit Packet();
	explicit Packet(long id, long time, int length, std::string flowID);
	long getID() const;
	long getArrivalTime() const;
	int getLength() const;
	std::string getFlowID() const;
	/* tostring method will generate <time: pktID> */
	friend std::ostream & operator<<(std::ostream & str, Packet const & p);
};


