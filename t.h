#pragma once

#ifdef _UNICODE
#include "wrterror.h"
#define tstring std::wstring
#define trterror wruntime_error
#else
#define tstring std::string
#define trterror std::runtime_error
#endif

