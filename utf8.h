#ifndef _UTF8_H

#define _UTF8_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <string>
#include <exception>

namespace utf8
{
	inline std::wstring n2w( const std::string &in )
	{
		//istream i; i << in; return i.data();
		std::wstring out;
		out.resize( in.size() );
		int chars = MultiByteToWideChar( CP_UTF8, 0, in.c_str(), (int)in.size(),
            (wchar_t*)out.c_str(), (int)in.size() + 1 );
		if( chars != out.size() ) out.resize( chars );
		return out;
	}

	inline std::string w2n( const std::wstring &in )
	{
		//ostream o; o << in; return o.data();
		std::string out;
		out.resize( in.size() * 3 );
		int bytes = WideCharToMultiByte( CP_UTF8, 0, in.c_str(), (int)in.size(),
            (char*)out.c_str(), (int)(in.size() * 3) + 1, NULL, NULL );
		if( bytes != out.size() ) out.resize( bytes );
		return out;
	}
}

#endif
