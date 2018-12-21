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
	void GuardProcess();
	bool CheckProcessDead();
	bool CheckTimeUp();
	void RestartProcess();

private:
	Log*        m_pLog;
	std::string m_sCfgFile;

private:
};

