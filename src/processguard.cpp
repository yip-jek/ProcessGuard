#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "processguard.h"
#include "config.h"
#include "def.h"
#include "gsignal.h"
#include "pubstr.h"
#include "simpletime.h"

ProcessGuard::ProcessGuard(const std::string& cfg_file)
:m_pLog(Log::Instance())
,m_sCfgFile(cfg_file)
,m_chktime(0)
,m_recTime(0)
{
}

ProcessGuard::~ProcessGuard()
{
	Log::Release();
}

std::string ProcessGuard::Version()
{
	return ("ProcessGuard version 2.0.1 released. Compiled at " __TIME__ " on " __DATE__);
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
	Config cfg(m_sCfgFile);
	cfg.RegisterItem("COMMON", "CHECK_TIME");
	cfg.RegisterItem("COMMON", "PROCESS_NAME");
	cfg.RegisterItem("COMMON", "FEATURES");
	cfg.RegisterItem("COMMON", "PROCESS_RUN");
	cfg.ReadConfig();

	m_chktime   = cfg.GetCfgUIntVal("COMMON", "CHECK_TIME");
	m_sProcName = cfg.GetCfgValue("COMMON", "PROCESS_NAME");
	m_sProcRun  = cfg.GetCfgValue("COMMON", "PROCESS_RUN");

	m_pLog->Output("[COMMON] CHECK_TIME   = [%u]", m_chktime);
	m_pLog->Output("[COMMON] PROCESS_NAME = [%s]", m_sProcName.c_str());
	m_pLog->Output("[COMMON] PROCESS_RUN  = [%s]", m_sProcRun.c_str());

	// 相关特征值可为空
	try
	{
		m_sFeatures = cfg.GetCfgValue("COMMON", "FEATURES");
		m_pLog->Output("[COMMON] FEATURES     = [%s]", m_sFeatures.c_str());
	}
	catch ( const Exception& ex )
	{
		m_sFeatures.clear();
	}

	m_pLog->Output("Load config OK.");
}

void ProcessGuard::Init()
{
	if ( m_chktime <= 0 )
	{
		throw Exception(PG_INIT_FAILED, "The check time is invalid: [%u] [FILE:%s, LINE:%d]", m_chktime, __FILE__, __LINE__);
	}

	if ( m_sProcName.empty() )
	{
		throw Exception(PG_INIT_FAILED, "The process name is a blank! [FILE:%s, LINE:%d]", __FILE__, __LINE__);
	}

	InitProcRun();
	InitCommand();

	m_recTime = SimpleTime::CurrentTime();
	m_pLog->Output("Init OK.");
}

void ProcessGuard::InitProcRun()
{
	if ( m_sProcRun.empty() )
	{
		throw Exception(PG_INIT_FAILED, "The process run is a blank! [FILE:%s, LINE:%d]", __FILE__, __LINE__);
	}

	m_sProcRun = "nohup " + m_sProcRun + " >/dev/null 2>&1 &";
	m_pLog->Output("Process run: [%s]", m_sProcRun.c_str());
}

void ProcessGuard::InitCommand()
{
	std::vector<std::string> vec_feature;
	PubStr::Str2StrVector(m_sFeatures, ",", vec_feature);

	m_sCommand = "ps -ef | grep " + m_sProcName + " | ";

	const int VEC_SIZE = vec_feature.size();
	for ( int i = 0; i < VEC_SIZE; ++i )
	{
		m_sCommand += ("grep " + vec_feature[i] + " | ");
	}

	m_sCommand += "grep -v grep | wc -l";
	m_pLog->Output("Command: [%s]", m_sCommand.c_str());
}

void ProcessGuard::GuardProcess()
{
	if ( CheckTimeUp() )
	{
		if ( CheckProcessDead() )
		{
			m_pLog->Output("# The process is dead!");
			RestartProcess();

			if ( CheckProcessDead() )
			{
				m_pLog->Output("### Restart the process failed !");
			}
			else
			{
				m_pLog->Output(">>> Restart the process successfully.");
			}
		}
		else
		{
			m_pLog->Output("* The process is working good.");
		}
	}
}

bool ProcessGuard::CheckProcessDead()
{
	m_pLog->Output("[CHECK] Is process [%s] still alive ?", m_sProcName.c_str());

	FILE* fp_pipe = popen(m_sCommand.c_str(), "r");
	if ( NULL == fp_pipe )
	{
		throw Exception(PG_CHK_PROC_FAILED, "Popen(r:\"%s\") failed: (%d) %s [FILE:%s, LINE:%d]", m_sCommand.c_str(), errno, strerror(errno), __FILE__, __LINE__);
	}

	char buffer[512] = "";
	fgets(buffer, 512, fp_pipe);
	pclose(fp_pipe);

	int proc_count = 0;
	PubStr::Str2Int(buffer, proc_count);
	return (0 == proc_count);
}

bool ProcessGuard::CheckTimeUp()
{
	const long long NOW_TIME = SimpleTime::CurrentTime();
	if ( NOW_TIME - m_recTime >= m_chktime )
	{
		m_pLog->Output("***** Check point *****");

		m_recTime = NOW_TIME;
		return true;
	}

	return false;
}

void ProcessGuard::RestartProcess()
{
	m_pLog->Output("==> Try to restart the process: [%s]", m_sProcName.c_str());

	system(m_sProcRun.c_str());
}

