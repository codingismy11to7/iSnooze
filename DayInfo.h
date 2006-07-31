#pragma once

#include <vector>
#include "t.h"

class DayInfo
{
public:

	static std::vector<tstring> const& LongDayNames()
	{ return instance()->m_long_day_names; }

	static std::vector<tstring> const& ShortDayNames()
	{ return instance()->m_short_day_names; }

	static int FirstDayOfWeek()
	{ return instance()->m_fdow; }

	static void Destruct()
	{ delete instance(); DayInfo::inst = NULL; }

protected:
	DayInfo();

	static DayInfo *instance()
	{ if( !inst ) inst = new DayInfo; return inst; }

	static DayInfo *inst;

	std::vector< tstring > m_long_day_names;
	std::vector< tstring > m_short_day_names;
	
	int m_fdow;
};
