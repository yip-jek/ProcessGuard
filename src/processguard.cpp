#include <unistd.h>
#include "processguard.h"
#include "config.h"
#include "def.h"
#include "gsignal.h"

ProcessGuard::ProcessGuard(const std::string& cfg_file)
:m_pLog(Log::Instance())
,m_sCfgFile(cfg_file)
{
}

ProcessGuard::~ProcessGuard()
{
	Log::Release();
}

std::string ProcessGuard::Version()
{
	return ("ProcessGuard version 1.9.0 released. Compiled at " __TIME__ " on " __DATE__);
}

void ProcessGuard::Run()
{
	if ( !GSignal::Init(m_pLog) )
	{
		throw Exception(PG_GSIGNAL_INIT_FAILED, "Init setting signal failed! [FILE:%s, LINE:%d]", __FILE__, __LINE__);
	}

	LoadConfig();

	Init();

	while ( GSignal::IsRunning() )
	{
		GuardProcess();

		sleep(1);
	}
}

void ProcessGuard::LoadConfig()
{
	Config cfg;
	cfg.RegisterItem();
	cfg.ReadConfig();

}

void ProcessGuard::Init()
{
	m_pLog->Output("Init OK.");
}

void ProcessGuard::GuardProcess()
{
	if ( CheckTimeUp() )
	{
		if ( CheckProcessDead() )
		{
			RestartProcess();

			if ( CheckProcessDead() )
			{
				m_pLog->Output("### Restart the process [%s] failed !", );
			}
			else
			{
				m_pLog->Output(">>> Restart the process [%s] succeed.", );
			}
		}
		else
		{
			m_pLog->Output("The process [%s] is working good.", );
		}
	}
}

bool ProcessGuard::CheckProcessDead()
{
	m_pLog->Output("[CHECK] The process [%s] is dead.", );
}

bool ProcessGuard::CheckTimeUp()
{
	m_pLog->Output("***** Check point *****");
}

void ProcessGuard::RestartProcess()
{
	m_pLog->Output("> Try to restart the process: [%s]", );
}

