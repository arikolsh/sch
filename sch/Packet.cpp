
#include "Packet.h"

Packet::Packet(long id, long time, int length, std::string flowID) :
	_ID(id), _arrivalTime(time), _length(length), _flowID(flowID)
{
}

long Packet::getID() const
{
	return _ID;
}

long Packet::getArrivalTime() const
{
	return _arrivalTime;
}

int Packet::getLength() const
{
	return _length;
}

std::string Packet::getFlowID() const
{
	return  _flowID;
}

std::ostream& operator<<(std::ostream& str, Packet const& p)
{
	str << p._ID; //return id
	return str;
}
