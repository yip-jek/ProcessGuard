#include "timer.h"
#include "pubstr.h"

const char* const IntervalTimer::S_MONTH_TYPE_W  = "MONTH";
const char* const IntervalTimer::S_MONTH_TYPE_F  = "YYYYMM";
const char* const IntervalTimer::S_DAY_TYPE_W    = "DAY";
const char* const IntervalTimer::S_DAY_TYPE_F    = "YYYYMMDD";
const char* const IntervalTimer::S_HOUR_TYPE_W   = "HOUR";
const char* const IntervalTimer::S_HOUR_TYPE_F   = "YYYYMMDDHH";
const char* const IntervalTimer::S_MINUTE_TYPE_W = "MINUTE";
const char* const IntervalTimer::S_MINUTE_TYPE_F = "YYYYMMDDHHMI";

IntervalTimer::IntervalTimer()
:m_bRunning(false)
,m_iType(IT_UNKNOWN)
{
}

IntervalTimer::~IntervalTimer()
{
}

bool IntervalTimer::IsIntervalTypeValid(IntervalType type)
{
	switch ( type )
	{
	case IT_MONTH:
	case IT_DAY:
	case IT_HOUR:
	case IT_MINUTE:
		return true;
	default:	// Unknown type !
		return false;
	}
}

void IntervalTimer::Start()
{
	// 是否正在运行？
	if ( m_bRunning )
	{
		return;
	}

	// 无效的时间间隔类型？
	if ( !IsIntervalTypeValid(m_iType) )
	{
		return;
	}

	m_bRunning = true;
}

void IntervalTimer::Stop()
{
	m_bRunning = false;
}

bool IntervalTimer::SetIntervalType(IntervalType type)
{
	if ( IsIntervalTypeValid(type) )
	{
		m_iType = type;
		return true;
	}

	m_iType = IT_UNKNOWN;
	return false;
}

bool IntervalTimer::SetIntervalType(const std::string& str_type)
{
	const std::string TYPE = PubStr::TrimUpperB(str_type);

	if ( TYPE == S_MONTH_TYPE_W || TYPE == S_MONTH_TYPE_F )
	{
		m_iType = IT_MONTH;
	}
	else if ( TYPE == S_DAY_TYPE_W || TYPE == S_DAY_TYPE_F )
	{
		m_iType = IT_DAY;
	}
	else if ( TYPE == S_HOUR_TYPE_W || TYPE == S_HOUR_TYPE_F )
	{
		m_iType = IT_HOUR;
	}
	else if ( TYPE == S_MINUTE_TYPE_W || TYPE == S_MINUTE_TYPE_F )
	{
		m_iType = IT_MINUTE;
	}
	else
	{
		m_iType = IT_UNKNOWN;
		return false;
	}

	return true;
}

void IntervalTimer::Register(TimeInterface* pTimer)
{
	if ( pTimer != NULL )
	{
		m_sTimers.insert(pTimer);
	}
}

void IntervalTimer::Unregister(TimeInterface* pTimer)
{
	if ( pTimer != NULL && !m_sTimers.empty() )
	{
		for ( std::set<TimeInterface*>::iterator it = m_sTimers.begin(); it != m_sTimers.end(); ++it )
		{
			if ( pTimer == *it )
			{
				m_sTimers.erase(it);
				break;
			}
		}
	}
}

void IntervalTimer::Run()
{
	while ( m_bRunning )
	{
		if ( CheckTimeInterval() )
		{
			SendNotification();
		}

		sleep(1);
	}
}

bool IntervalTimer::CheckTimeInterval()
{
	if ( !m_sRecordTime.IsValid() )
	{
		m_sRecordTime = SimpleTime::Now();
		return false;
	}

	switch ( m_iType )
	{
	case IT_MONTH:
	case IT_DAY:
	case IT_HOUR:
	case IT_MINUTE:
		return true;
	default:	// Unknown type !
		return false;
	}
}

void IntervalTimer::SendNotification()
{
	if ( !m_sTimers.empty() )
	{
		for ( std::set<TimeInterface*>::iterator it = m_sTimers.begin(); it != m_sTimers.end(); ++it )
		{
			(*it)->Notify();
		}
	}
}

