#include <iostream>
#include <string.h>
#include <unistd.h>
#include "pubstr.h"
#include "log.h"
#include "config.h"
#include "processguard.h"

int main(int argc, char* argv[])
{
	// DAEMON_FLAG: 守护进程标志位（'1'-有效）
	// LOG_ID     ：日志 ID
	// CFG_FILE   ：配置文件
	if ( argc != 4 )
	{
		std::cerr << "[usage] " << argv[0] << "DAEMON_FLAG LOG_ID CFG_FILE" << std::endl;
		return -1;
	}

	// Daemon process ?
	bool is_daemon = (strcmp(argv[1], "1") == 0);
	if ( is_daemon )
	{
		pid_t f_pid = fork();
		if ( f_pid < 0 )    // fork error !
		{
			std::cerr << "Process fork error: " << f_pid << std::endl;
			return -1;
		}
		else if ( f_pid > 0 )   // Parent process: end !
		{
			std::cout << "[DAEMON] Parent process [pid=" << getpid() << "] end" << std::endl;
			std::cout << "[DAEMON] Child process [pid=" << f_pid << "] start" << std::endl;
			return 0;
		}
	}

	// Log configuration
	long long log_id = 0;
	if ( !PubStr::Str2LLong(argv[2], log_id) )
	{
		std::cerr << "[ERROR] Unknown log ID: '" << argv[2] << "'" << std::endl;
		return -1;
	}

	LogStrategy log_strategy;
	log_strategy.log_prefix = "PGuard";
	log_strategy.log_id     = log_id;

	std::string str_head;
	PubStr::SetFormatString(str_head, "%s: PID=[%d]", (is_daemon?"守护进程":"一般进程"), getpid());
	log_strategy.log_headers.push_back(str_head);
	log_strategy.log_headers.push_back(ProcessGuard::Version());

	AutoLogger aLog;
	Log* pLog = aLog.Get();

	try
	{
		// Read CFG
		Config cfg;
		cfg.SetCfgFile(argv[3]);
		cfg.RegisterItem("LOG", "PATH");
		cfg.RegisterItem("LOG", "MAX_SIZE");
		cfg.RegisterItem("LOG", "MAX_LINE");
		cfg.RegisterItem("LOG", "INTERVAL");
		cfg.ReadConfig();

		log_strategy.log_path = cfg.GetCfgValue("LOG", "PATH");
		log_strategy.max_size = cfg.GetCfgLongVal("LOG", "MAX_SIZE");
		log_strategy.max_line = cfg.GetCfgLongVal("LOG", "MAX_LINE");
		log_strategy.interval = cfg.GetCfgValue("LOG", "INTERVAL");

		pLog->Init(log_strategy);
		std::cout << ProcessGuard::Version() << std::endl;

		ProcessGuard pg;
		pg.Run();
	}
	catch ( Exception& ex )
	{
		std::cerr << "[ERROR] " << ex.What() << ", ERROR_CODE: " << ex.ErrorCode() << std::endl;
		pLog->Output("[ERROR] %s, ERROR_CODE: %d", ex.What().c_str(), ex.ErrorCode());
		return -2;
	}
	catch ( ... )
	{
		std::cerr << "[ERROR] Unknown error! [FILE:" << __FILE__ << ", LINE:" << __LINE__ << "]" << std::endl;
		pLog->Output("[ERROR] Unknown error! [FILE:%s, LINE:%d]", __FILE__, __LINE__);
		return -3;
	}

	std::cout << argv[0] << " quit !" << std::endl;
	pLog->Output("%s quit !", argv[0]);
	return 0;
}

