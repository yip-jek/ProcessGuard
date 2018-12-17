#include <unistd.h>
#include "processguard.h"
#include "def.h"
#include "gsignal.h"

ProcessGuard::ProcessGuard()
:m_pLog(Log::Instance())
{
}

ProcessGuard::~ProcessGuard()
{
	Log::Release();
}

std::string ProcessGuard::Version()
{
	return ("ProcessGuard version 1.5.0 released. Compiled at " __TIME__ " on " __DATE__);
}

void ProcessGuard::Run()
{
	if ( !GSignal::Init(m_pLog) )
	{
		throw Exception(PG_GSIGNAL_INIT_FAILED, "Init setting signal failed! [FILE:%s, LINE:%d]", __FILE__, __LINE__);
	}

	int count = 0;
	while ( GSignal::IsRunning() )
	{
		m_pLog->Output("ProcessGuard is running ... %d", (++count));
		sleep(1);
	}
}

