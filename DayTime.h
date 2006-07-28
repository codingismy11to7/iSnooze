#pragma once

#define WIN32_MEAN_AND_LEAN

#include <windows.h>
#include <string>
#include <vector>

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

	const unsigned char Win2DayTime[] = {SUNDAY,MONDAY,TUESDAY,WEDNESDAY,THURSDAY,FRIDAY,SATURDAY};

	typedef struct _TM {

		_TM():time(0), day(0){}
		_TM( int hour, int minute, BYTE days )
		{
			this->day = days;
			setTime( hour, minute );
		}

		inline void setTime( int hour, int minute )
		{
			time = 0;
			time |= (hour & 0x1f ) << 6;
			time |= (minute & 0x3f );
		}

		inline void getTime( int &hour, int &minute ) const
		{
			minute = time & 0x3f;
			hour = (time >> 6) & 0x1f;
		}

		inline void getToBinary( BYTE *data ) const
		{
			((unsigned short*)data)[0] = 0x07ff & time;
			((unsigned char*)data)[2]  =   0x7f &  day;
		}

		inline void setFromBinary( const char *data )
		{
			time = 0x07ff & ((unsigned short*)data)[0];
			day  =   0x7f & ((unsigned char*)data)[2];
		}

		inline bool isIncluded( const SYSTEMTIME &o )
		{
			if( !( day & Win2DayTime[o.wDayOfWeek] ) ) return false;
			int hour, minute; getTime( hour, minute );
			return ( (minute == o.wMinute) && (hour == o.wHour) );
		}

		inline bool operator==( const _TM &o )
		{ return (time == o.time && day == o.day); }

		inline bool operator!=( const _TM &o )
		{ return !( *this == o ); }

		unsigned short time;
		unsigned char day;

	} TimeAndDays;

	template<typename T>
	void contToBin( const T &in, std::string &out )
	{
		out.resize( 3 * in.size() );
		T::const_iterator i, end = in.end();
		int j = 0;
		for( i = in.begin(); i != end; ++i )
		{
			(*i).setBinary( (BYTE*)out.c_str() + (3 * j++) );
		}
	}

	template<typename T>
	void binToCont( const std::string &in, T &out )
	{
		Time t;
		for( int i = 0; i < in.size(); i += 3 )
		{
			t.setBinary( (const BYTE*)in.c_str() + i );
			out.push_back( t );
		}
	}

};