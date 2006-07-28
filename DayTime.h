#pragma once

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

	typedef struct _TM {

		_TM():time(0), day(0){}
		_TM( int hour, int minute, BYTE days )
		{
			this->day = day;
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

		inline void getBinary( BYTE *data ) const
		{
			((unsigned short*)data)[0] = 0x07ff & time;
			((unsigned char*)data)[2]  =   0x7f &  day;
		}

		inline void setBinary( const BYTE *data )
		{
			time = 0x07ff & ((unsigned short*)data)[0];
			day  =   0x7f & ((unsigned char*)data)[2];
		}

		unsigned short time;
		unsigned char day;

	} Time;

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