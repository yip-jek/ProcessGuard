#pragma once

#include <string>
#include "log.h"

class ProcessGuard
{
public:
	ProcessGuard(const std::string& cfg_file);
	~ProcessGuard();

	static std::string Version();

public:
	void Run();

private:
	void LoadConfig();
	void Init();
	void InitProcRun();
	void InitCommand();
	void GuardProcess();
	bool CheckProcessDead();
	bool CheckTimeUp();
	void RestartProcess();

private:
	Log*        m_pLog;
	std::string m_sCfgFile;

private:
	unsigned int m_chktime;
	long long    m_recTime;
	std::string  m_sProcName;
	std::string  m_sFeatures;
	std::string  m_sCommand;
	std::string  m_sProcRun;
};

