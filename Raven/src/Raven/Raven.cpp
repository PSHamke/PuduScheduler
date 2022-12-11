#include "Raven.h"
#include "Log/Log.h"

void Raven::Handle()
{
	Log::Init();
	PS_CORE_INFO("Log");
}
