#pragma once

#include <string>
#include "log.h"

class ProcessGuard
{
public:
	ProcessGuard();
	~ProcessGuard();

	static std::string Version();

public:
	void Run();

private:
	Log* m_pLog;
};

