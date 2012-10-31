#include "Encoder.h"


CEncoder::CEncoder(void)
{
	zDProcCallback.client = 0;
	zDProcCallback.zDProcCallbackfuc = 0;
	decoder = new CDecoder;
}


CEncoder::~CEncoder(void)
{
	if(decoder)
		delete decoder;
}

void CEncoder::SetProccessCallback(ZDProcCallbackFunction zDProcCallbackfuc,void *client)
{
	this->zDProcCallback.zDProcCallbackfuc = zDProcCallbackfuc;
	this->zDProcCallback.client = client;
}