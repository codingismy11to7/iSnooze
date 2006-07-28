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

#pragma once

#include <sstream>

#ifdef _UNICODE
#include "wrterror.h"
typedef std::wstring tstring;
typedef wruntime_error trterror;
typedef std::wstringstream tstringstream;
#define ERRMSG(x) (x).error()
#else
typedef std::string tstring;
typedef std::runtime_error trterror;
typedef std::stringstream tstringstream;
#define ERRMSG(x) (x).what()
#endif

