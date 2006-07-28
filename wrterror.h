#ifndef _WRTERROR_H
#define _WRTERROR_H

#include <exception>
#include "utf8.h"

class wruntime_error : std::runtime_error
{
public:
	wruntime_error( const std::wstring &in )
		: std::runtime_error( utf8::w2n(in) ), m_errorMsg(in)
	{}

	wruntime_error( const wruntime_error &in )
		: std::runtime_error( in.what() ), m_errorMsg(in.m_errorMsg)
	{}

	wruntime_error& operator=( const wruntime_error &o )
	{
		std::runtime_error::operator=( o );
		m_errorMsg = o.m_errorMsg;
		return *this;
	}

	std::wstring const& error() const
	{
		return m_errorMsg;
	}

private:
	std::wstring m_errorMsg;
};

#endif