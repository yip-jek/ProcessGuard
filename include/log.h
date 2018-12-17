#pragma once

#include <string>
#include <vector>
#include "def.h"
#include "exception.h"
#include "basefile.h"

class Log
{
private:
	Log();
	virtual ~Log();

public:
	// 日志缓冲区大小
	static const unsigned int MAX_BUFFER_SIZE = 4096;

public:
	// 获取多少次，就要释放多少次
	// 获取日志实例
	static Log* Instance();
	// 释放日志实例
	static void Release();

	// 设置日志ID
	static bool SetLogID(long long log_id);

	// 重置日志文件大小
	static bool ResetFileSize(unsigned long long fsize);

	// 设置日志文件名称前缀
	static void SetLogFilePrefix(const std::string& log_prefix);

	// 设置日志文件最大行数
	static void SetFileMaxLine(unsigned long long max_line);

	// 导入日志头
	static void ImportHeaders(std::vector<std::string>& vec_headers);

public:
	// 初始化
	void Init();

	// 设置日志路径
	void SetPath(const std::string& path);

	// 输出日志...
	bool Output(const char* format, ...);

private:
	// 检查最大文件条件
	bool CheckFileMaximum(std::string& tail);

	// 打开新的日志
	void OpenNewLogger();

	// 写入文件头
	void WriteLogHeaders();

private:
	static int                      _sInstances;				// 实例计数器
	static Log*                     _spLogger;					// 实例指针
	static long long                _sLogID;					// 日志ID
	static unsigned long long       _sMaxFileLine;				// 日志文件最大行数
	static unsigned long long       _sMaxLogFileSize;			// 最大日志文件大小
	static std::string              _sLogFilePrefix;			// 日志文件名称前缀
	static std::vector<std::string> _svLogHeaders;				// 日志头

private:
	std::string              m_sLogPath;				// 日志路径
	BaseFile                 m_bfLogger;				// 日志文件类
	unsigned long long       m_llLineCount;				// 文件行计数
};

class AutoLogger
{
public:
	AutoLogger(): m_pLogger(Log::Instance())
	{}

	virtual ~AutoLogger()
	{ Log::Release(); }

public:
	Log* Get() const
	{ return m_pLogger; }

private:
	Log*	m_pLogger;
};

