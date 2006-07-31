#include "StdAfx.h"
#include ".\dayinfo.h"

DayInfo *DayInfo::inst = NULL;

DayInfo::DayInfo()
{
	TCHAR buf[1024];

	GetLocaleInfo( GetUserDefaultLCID(), LOCALE_SABBREVDAYNAME7, buf, 1024 );
	m_short_day_names.push_back( buf );
	GetLocaleInfo( GetUserDefaultLCID(), LOCALE_SABBREVDAYNAME1, buf, 1024 );
	m_short_day_names.push_back( buf );
	GetLocaleInfo( GetUserDefaultLCID(), LOCALE_SABBREVDAYNAME2, buf, 1024 );
	m_short_day_names.push_back( buf );
	GetLocaleInfo( GetUserDefaultLCID(), LOCALE_SABBREVDAYNAME3, buf, 1024 );
	m_short_day_names.push_back( buf );
	GetLocaleInfo( GetUserDefaultLCID(), LOCALE_SABBREVDAYNAME4, buf, 1024 );
	m_short_day_names.push_back( buf );
	GetLocaleInfo( GetUserDefaultLCID(), LOCALE_SABBREVDAYNAME5, buf, 1024 );
	m_short_day_names.push_back( buf );
	GetLocaleInfo( GetUserDefaultLCID(), LOCALE_SABBREVDAYNAME6, buf, 1024 );
	m_short_day_names.push_back( buf );

	GetLocaleInfo( GetUserDefaultLCID(), LOCALE_SDAYNAME7, buf, 1024 );
	m_long_day_names.push_back( buf );
	GetLocaleInfo( GetUserDefaultLCID(), LOCALE_SDAYNAME1, buf, 1024 );
	m_long_day_names.push_back( buf );
	GetLocaleInfo( GetUserDefaultLCID(), LOCALE_SDAYNAME2, buf, 1024 );
	m_long_day_names.push_back( buf );
	GetLocaleInfo( GetUserDefaultLCID(), LOCALE_SDAYNAME3, buf, 1024 );
	m_long_day_names.push_back( buf );
	GetLocaleInfo( GetUserDefaultLCID(), LOCALE_SDAYNAME4, buf, 1024 );
	m_long_day_names.push_back( buf );
	GetLocaleInfo( GetUserDefaultLCID(), LOCALE_SDAYNAME5, buf, 1024 );
	m_long_day_names.push_back( buf );
	GetLocaleInfo( GetUserDefaultLCID(), LOCALE_SDAYNAME6, buf, 1024 );
	m_long_day_names.push_back( buf );

	GetLocaleInfo( GetUserDefaultLCID(), LOCALE_IFIRSTDAYOFWEEK, buf, 1024 );
	m_fdow = ( 1 + _tstoi( buf ) ) % 7;  // sunday is first day of week for me
}
