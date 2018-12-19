#include "log.h"
#include <iostream>
#include <stdio.h>
#include "simpletime.h"
#include "pubstr.h"
#include "basedir.h"

int Log::_sInstances = 0;
Log* Log::_spLogger = NULL;		// single log pointer

long long Log::_sLogID = 0;
unsigned long long Log::_sMaxFileLine = 0;
unsigned long long Log::_sMaxLogFileSize = 10*1024*1024;	// default log file size 10M

std::string Log::_sLogFilePrefix = "LOG";			// default log file prefix
std::vector<std::string> Log::_svLogHeaders;		// 日志头

Log::Log()
:m_llLineCount(0)
{
}

Log::~Log()
{
}

Log* Log::Instance()
{
	if ( NULL == _spLogger )
	{
		std::cout << "[LOG] Creating logger instance ..." << std::endl;
		_spLogger = new Log();
	}

	// 计数加 1
	++_sInstances;
	return _spLogger;
}

void Log::Release()
{
	if ( _spLogger != NULL )
	{
		// 计数减 1
		--_sInstances;

		// 计数器归 0，则释放资源
		if ( _sInstances <= 0 )
		{
			std::cout << "[LOG] Releasing logger instance ..." << std::endl;

			delete _spLogger;
			_spLogger = NULL;
		}
	}
}

bool Log::SetLogID(long long log_id)
{
	if ( log_id > 0 )
	{
		_sLogID = log_id;
		return true;
	}

	return false;
}

bool Log::ResetFileSize(unsigned long long fsize)
{
	if ( fsize > 0 )
	{
		_sMaxLogFileSize = fsize;
		return true;
	}

	return false;
}

void Log::SetLogFilePrefix(const std::string& log_prefix)
{
	if ( !log_prefix.empty() )
	{
		_sLogFilePrefix = log_prefix;
	}
}

void Log::SetFileMaxLine(unsigned long long max_line)
{
	// 设置日志文件最大行数
	// >0，有效
	// =0，无效
	_sMaxFileLine = max_line;
}

void Log::ImportHeaders(std::vector<std::string>& vec_headers)
{
	_svLogHeaders.swap(vec_headers);
}

void Log::Init()
{
	if ( m_sLogPath.empty() )
	{
		throw Exception(LG_INIT_FAIL, "[LOG] The log path is not configured! [FILE:%s, LINE:%d]", __FILE__, __LINE__);
	}

	OpenNewLogger();
}

void Log::SetPath(const std::string& path)
{
	if ( path.empty() )
	{
		throw Exception(LG_FILE_PATH_EMPTY, "[LOG] The log path is empty! [FILE:%s, LINE:%d]", __FILE__, __LINE__);
	}

	// 自动建日志路径
	if ( !BaseDir::CreateFullPath(path) )
	{
		throw Exception(LG_FILE_PATH_INVALID, "[LOG] The log path is invalid: %s [FILE:%s, LINE:%d]", path.c_str(), __FILE__, __LINE__);
	}

	m_sLogPath = path;
	BaseDir::DirWithSlash(m_sLogPath);
}

bool Log::Output(const char* format, ...)
{
	if ( !m_bfLogger.IsOpen() )
	{
		return false;
	}

	char buf[MAX_BUFFER_SIZE] = "";
	va_list arg_ptr;
	va_start(arg_ptr, format);
	vsnprintf(buf, sizeof(buf), format, arg_ptr);
	va_end(arg_ptr);

	std::string str_out = SimpleTime::Now().LLTimeStamp() + std::string("\x20\x20") + buf;
	m_bfLogger.Write(str_out);
	++m_llLineCount;

	// Maximum file ?
	if ( CheckFileMaximum(str_out) )
	{
		m_bfLogger.Write(str_out);

		OpenNewLogger();
	}

	return true;
}

bool Log::CheckFileMaximum(std::string& tail)
{
	// Max line ?
	if ( _sMaxFileLine > 0 && m_llLineCount >= _sMaxFileLine )
	{
		tail = "\n\n\x20\x20\x20\x20( MAXIMUM FILE LINE )";
		return true;
	}

	// Max file size ?
	if ( m_bfLogger.FileSize() >= _sMaxLogFileSize )
	{
		tail = "\n\n\x20\x20\x20\x20( MAXIMUM FILE SIZE )";
		return true;
	}

	return false;
}

void Log::OpenNewLogger()
{
	if ( m_bfLogger.IsOpen() )
	{
		m_bfLogger.Close();
	}

	int log_id = 0;
	std::string fullLogPath;
	const std::string DAY_TIME = SimpleTime::Now().DayTime8();

	do
	{
		PubStr::SetFormatString(fullLogPath, "%s%s_%lld_%s_%04d.log", m_sLogPath.c_str(), _sLogFilePrefix.c_str(), _sLogID, DAY_TIME.c_str(), log_id++);

	} while ( BaseFile::IsFileExist(fullLogPath) );

	if ( !m_bfLogger.Open(fullLogPath) )
	{
		throw Exception(LG_OPEN_FILE_FAIL, "[LOG] Can not open log file: %s [FILE:%s, LINE:%d]", fullLogPath.c_str(), __FILE__, __LINE__);
	}

	WriteLogHeaders();
}

void Log::WriteLogHeaders()
{
	std::string str_out;
	const unsigned int V_HEADER_SIZE = _svLogHeaders.size();
	for ( unsigned int u = 0; u < V_HEADER_SIZE; ++u )
	{
		str_out = SimpleTime::Now().LLTimeStamp() + std::string("\x20\x20") + _svLogHeaders[u];
		m_bfLogger.Write(str_out);
	}

	m_llLineCount = V_HEADER_SIZE;
}
