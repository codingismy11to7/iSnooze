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
