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

//#pragma once
#ifndef _DAYTIME_H
#define _DAYTIME_H

#include <vector>
#include <list>
#include "t.h"

namespace DayTime {

	
	const unsigned char SUNDAY    =  1;
	const unsigned char MONDAY    =  2;
	const unsigned char TUESDAY   =  4;
	const unsigned char WEDNESDAY =  8;
	const unsigned char THURSDAY  = 16;
	const unsigned char FRIDAY    = 32;
	const unsigned char SATURDAY  = 64;
	const unsigned char ALL_DAYS  = 127;
	const unsigned char WEEKDAYS  = 62;

	const unsigned char ALARM_ENABLED = 128;

	const unsigned char Win2DayTime[] = {SUNDAY,MONDAY,TUESDAY,WEDNESDAY,THURSDAY,FRIDAY,SATURDAY};

	//extern const TCHAR *DayNames[7];

	class TimeAndDays
	{
	public:
		TimeAndDays():time(0), day(ALARM_ENABLED){}
		TimeAndDays( int hour, int minute, BYTE days, bool enabled = true );
		TimeAndDays( const SYSTEMTIME &st, bool enabled = true );

		void setTime( int hour, int minute );

		void getTime( int &hour, int &minute ) const
		{
			minute = time & 0x3f;
			hour = (time >> 6) & 0x1f;
		}


		void getToBinary( BYTE *data ) const
		{
			((unsigned short*)data)[0] = 0x07ff & time;
			((unsigned char*)data)[2]  = day;
		}

		void setFromBinary( const char *data )
		{
			time = 0x07ff & ((unsigned short*)data)[0];
			day  = ((unsigned char*)data)[2];
		}

		bool isIncluded( const SYSTEMTIME &o ) const
		{
			if( !( day & Win2DayTime[o.wDayOfWeek] ) ) return false;
			int hour, minute; getTime( hour, minute );
			return ( (minute == o.wMinute) && (hour == o.wHour) );
		}

		bool isEnabled() const
		{
			return (day & ALARM_ENABLED) != 0;
		}

		void isEnabled( bool e )
		{
			if( e )
				day |= ALARM_ENABLED;
			else
				day &= ~ALARM_ENABLED;
		}

		tstring getString() const;

		unsigned short time;
		unsigned char day;


		bool operator==(const TimeAndDays& o) const;

		// only compares Time, fine for my purposes
		bool operator < ( const TimeAndDays& o ) const;
		bool operator > ( const TimeAndDays& o ) const;
	};

	typedef std::vector< TimeAndDays > AlarmList;

	extern void contToBin( const AlarmList &in, std::string &out );
	extern void binToCont( const std::string &in, AlarmList &out );

	extern void addSorted( const TimeAndDays &tm, std::list< TimeAndDays* > &list );
	extern void cleanList( std::list< TimeAndDays* > &list );

};

#endif
