#pragma once

enum ERROR_CODE_DEF
{
	// Log error code
	LG_FILE_PATH_EMPTY       = -1000001,
	LG_FILE_PATH_INVALID     = -1000002,
	LG_INIT_FAIL             = -1000003,
	LG_OPEN_FILE_FAIL        = -1000004,

	// Config error code
	CFG_FILE_INVALID         = -1001001,
	CFG_OPEN_FILE_FAIL       = -1001002,
	CFG_UNREGISTER_ITEM      = -1001003,
	CFG_ITEM_NOT_FOUND       = -1001004,
	CFG_VALUE_INVALID        = -1001005,

	// ProcessGuard error code
	PG_GSIGNAL_INIT_FAILED   = -2000001,
};

