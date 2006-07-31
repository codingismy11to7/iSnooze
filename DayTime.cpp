// Copyright (c) 2004, Steven Scott (progoth@gmail.com)
//
// This file is part of iSnooze.
//
// iSnooze is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// iSnooze is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with iSnooze; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "StdAfx.h"
#include ".\daytime.h"
#include <iomanip>

using namespace DayTime;

//const TCHAR *DayTime::DayNames[7] = { _T("Sun"), _T("Mon"), _T("Tue"), _T("Wed"), _T("Thu"), _T("Fri"), _T("Sat") };

TimeAndDays::TimeAndDays( int hour, int minute, BYTE days, bool enabled /*= true*/ )
{
	this->day = days | ( enabled ? ALARM_ENABLED : 0);
	setTime( hour, minute );
}

TimeAndDays::TimeAndDays( const SYSTEMTIME &st, bool enabled /*= true*/ )
{
	this->day = Win2DayTime[ st.wDayOfWeek ] | ( enabled ? ALARM_ENABLED : 0);
	setTime( st.wHour, st.wMinute );
}

void TimeAndDays::setTime( int hour, int minute )
{
	time = 0;
	time |= (hour & 0x1f ) << 6;
	time |= (minute & 0x3f );
}

bool TimeAndDays::operator==( const TimeAndDays &o ) const
{
	return ((time == o.time) && (day == o.day));
}

bool TimeAndDays::operator < ( const TimeAndDays& o ) const
{
	return ( time < o.time );
}

bool TimeAndDays::operator > ( const TimeAndDays& o ) const
{
	return ( time > o.time );
}

tstring TimeAndDays::getString() const
{
	tstringstream str;
	int hour, minute;
	getTime( hour, minute );
	if( !( day & ALARM_ENABLED ) )
		str << _T("(X) ");
	str << hour << _T(":") << std::setw(2) << std::setfill(_T('0')) << minute << _T(" ");
	if( (0x7f & day) == ALL_DAYS )
		str << DayInfo::ShortDayNames()[DayInfo::FirstDayOfWeek()] << _T("-") << DayInfo::ShortDayNames()[(DayInfo::FirstDayOfWeek()+6)%7];
	else if( (0x7f & day) == WEEKDAYS )
		str << DayInfo::ShortDayNames()[1] << _T("-") << DayInfo::ShortDayNames()[5];
	else
	{
		if( day & SUNDAY ) str << /*DayNames*/ DayInfo::ShortDayNames()[0] << _T(" ");
		if( day & MONDAY ) str << /*DayNames*/ DayInfo::ShortDayNames()[1] << _T(" ");
		if( day & TUESDAY ) str << /*DayNames*/ DayInfo::ShortDayNames()[2] << _T(" ");
		if( day & WEDNESDAY ) str << /*DayNames*/ DayInfo::ShortDayNames()[3] << _T(" ");
		if( day & THURSDAY ) str << /*DayNames*/ DayInfo::ShortDayNames()[4] << _T(" ");
		if( day & FRIDAY ) str << /*DayNames*/ DayInfo::ShortDayNames()[5] << _T(" ");
		if( day & SATURDAY ) str << /*DayNames*/ DayInfo::ShortDayNames()[6] << _T(" ");
	}
	tstring ret( str.str() );
	if( ret[ret.size()-1] == _T(' ') )
		ret.resize(ret.size()-1);
	return ret;
}

void DayTime::contToBin( const std::vector<TimeAndDays> &in, std::string &out )
{
	out.resize( 3 * in.size() );
	std::vector<TimeAndDays>::const_iterator i, end = in.end();
	int j = 0;
	for( i = in.begin(); i != end; ++i )
	{
		(*i).getToBinary( (BYTE*)out.c_str() + (3 * j++) );
	}
}

//template<typename T>
void DayTime::binToCont( const std::string &in, std::vector<TimeAndDays> &out )
{
	TimeAndDays t;
	for( ULONG i = 0; i < in.size(); i += 3 )
	{
		t.setFromBinary( in.c_str() + i );
		out.push_back( t );
	}
}

void DayTime::addSorted( const TimeAndDays &tm, std::list< TimeAndDays* > &list )
{
	std::list< TimeAndDays* >::iterator i, end = list.end();

	if( !tm.isEnabled() )
	{
		list.push_back( new TimeAndDays( tm ) );
		return;
	}
	for( i = list.begin(); i != end; ++i )
	{
		if( (**i) > tm )
		{
			list.insert( i, new TimeAndDays( tm ) );
			break;
		}
	}
	if( i == end )
		list.push_back( new TimeAndDays( tm ) );
}

void DayTime::cleanList( std::list< TimeAndDays* > &list )
{
	std::list< TimeAndDays* >::iterator i, end = list.end();
	for( i = list.begin(); i != end; ++i )
		delete (*i);
	list.clear();
}
