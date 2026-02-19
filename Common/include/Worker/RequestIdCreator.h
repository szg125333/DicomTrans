#pragma once
class RequestIdCreator
{
private:
	static int requestId;
public:
	static int generateRequestId()
	{ 
		return ++requestId; 
	}
};
int RequestIdCreator::requestId = 0;