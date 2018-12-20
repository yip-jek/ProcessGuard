#pragma once

enum ERROR_CODE_DEF
{
	// Log error code
	LG_INIT_FAIL             = -1000001,
	LG_OPEN_FILE_FAIL        = -1000002,

	// Config error code
	CFG_FILE_INVALID         = -1001001,
	CFG_OPEN_FILE_FAIL       = -1001002,
	CFG_UNREGISTER_ITEM      = -1001003,
	CFG_ITEM_NOT_FOUND       = -1001004,
	CFG_VALUE_INVALID        = -1001005,

	// ProcessGuard error code
	PG_GSIGNAL_INIT_FAILED   = -2000001,
};

