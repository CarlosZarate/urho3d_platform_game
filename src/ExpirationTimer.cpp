#include "ExpirationTimer.h"



ExpirationTimer::ExpirationTimer(unsigned expirationTime, bool startActive) : 
expirationTime_(expirationTime)
{
	timer_.Reset();
}
ExpirationTimer::~ExpirationTimer
()
{

}

bool ExpirationTimer::Expired()
{
	return !Active();
}

bool ExpirationTimer::Active()
{
	return (timer_.GetMSec(false) < expirationTime_);
}

void ExpirationTimer::Reset()
{
	timer_.Reset();
}

void ExpirationTimer::SetExpirationTime(unsigned expirationTime)
{
	expirationTime_ = expirationTime;
	timer_.Reset();
}


unsigned ExpirationTimer::GetCurrentTime()
{
	return timer_.GetMSec(false);
}





