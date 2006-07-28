// Copyright (c) 2004, Steven Scott (progoth@gmail.com)
//
// This file is part of iTunesAlarm.
//
// iTunesAlarm is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// iTunesAlarm is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with iTunesAlarm; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "StdAfx.h"
#include ".\daytime.h"
#include <iomanip>

using namespace DayTime;

TimeAndDays::TimeAndDays( int hour, int minute, BYTE days )
{
	this->day = days;
	setTime( hour, minute );
}

TimeAndDays::TimeAndDays( const SYSTEMTIME &st )
{
	this->day = Win2DayTime[ st.wDayOfWeek ];
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
	str << hour << _T(":") << std::setw(2) << std::setfill(_T('0')) << minute << _T(" ");
	if( day == ALL_DAYS )
		str << _T("Every day");
	else if( day == WEEKDAYS )
		str << _T("Mon-Fri");
	else
	{
		if( day & SUNDAY ) str << "Sun ";
		if( day & MONDAY ) str << "Mon ";
		if( day & TUESDAY ) str << "Tue ";
		if( day & WEDNESDAY ) str << "Wed ";
		if( day & THURSDAY ) str << "Thu ";
		if( day & FRIDAY ) str << "Fri ";
		if( day & SATURDAY ) str << "Sat ";
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
