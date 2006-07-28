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

#ifndef _REGMAP_H
#define _REGMAP_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>
#include <string>
#include <map>
#include <vector>
#include <exception>
#include <algorithm>

#include "t.h"

typedef struct {
    std::string data;
} binary;

class RegMap
{
public:

	typedef enum { NUMBER, SZ, BIN, KEY, MV_SZ } RegType;


	RegMap( HKEY root = HKEY_LOCAL_MACHINE, const tstring &key = _T("") );
	RegMap( const tstring &key, HKEY root = HKEY_LOCAL_MACHINE, bool create = false );

    RegMap( const RegMap &o )
        : m_create(false), m_isval(false)
    { *this = o; }

	RegMap( DWORD dword, const TCHAR *key = NULL, HKEY root = HKEY_LOCAL_MACHINE )
		: m_isval(true), m_type(NUMBER), m_root(root), m_create(false)
	{ m_val.dword = dword; if( key != NULL ) m_key = key; }

	RegMap( const TCHAR *str, const TCHAR *key = NULL, HKEY root = HKEY_LOCAL_MACHINE )
		: m_isval(true), m_type(SZ), m_root(root), m_create(false)
	{ m_val.str = new tstring(str); if( key != NULL ) m_key = key; }

	RegMap( const binary &bin, const TCHAR *key = NULL, HKEY root = HKEY_LOCAL_MACHINE )
		: m_isval(true), m_type(BIN), m_root(root), m_create(false)
	{ m_val.bin = new binary(bin); if( key != NULL ) m_key = key; }

	RegMap( const std::vector<tstring> &strarray, const TCHAR *key = NULL, HKEY root = HKEY_LOCAL_MACHINE )
		: m_isval(true), m_type(MV_SZ), m_root(root), m_create(false)
	{ m_val.strarray = new std::vector<tstring>( strarray ); if( key != NULL ) m_key = key; }

	~RegMap()
    { clean(); }

	RegMap& operator[]( const TCHAR *item ) const;
	RegMap& operator[]( const tstring &item ) const
	{ return (*this)[item.c_str()]; }

	RegType getType() const
	{ return m_type; }

	operator tstring const&() const
	{ if( !m_isval || m_type != SZ ) throw trterror( _T("String requested for non-string item.") ); return *(m_val.str); }

	operator DWORD() const
	{ if( !m_isval || m_type != NUMBER ) throw trterror( _T("DWORD requested for non-DWORD item.") ); return m_val.dword; }

	operator long() const
	{ if( !m_isval || m_type != NUMBER ) throw trterror( _T("Long requested for non-DWORD item.") ); return m_val.dword; }

	operator int() const
	{ if( !m_isval || m_type != NUMBER ) throw trterror( _T("Int requested for non-DWORD item.") ); return m_val.dword; }

    operator bool() const
    { if( !m_isval || m_type != NUMBER ) throw trterror( _T("Bool requested for non-DWORD item.") ); return (m_val.dword != 0); }

	operator binary const&() const
	{ if( !m_isval || m_type != BIN ) throw trterror( _T("Binary requested for non-binary item.") ); return *(m_val.bin); }

	operator std::vector<tstring> const&() const
	{ if( !m_isval || m_type != MV_SZ ) throw trterror( _T("String array requested for non-string array item.") ); return *(m_val.strarray); }

	void keys( std::vector<tstring> &out ) const;

	bool has_key( const tstring &key ) const;

	void deleteValue( const TCHAR *val ) const;
	void deleteKey() const;

	RegMap &operator=( const RegMap &o );
    RegMap &operator=( const TCHAR *str )
    { setValue( str ); return *this; }
    RegMap &operator=( const tstring &str )
    { setValue( str.c_str() ); return *this; }
    RegMap &operator=( DWORD num )
    { setValue( num ); return *this; }
    RegMap &operator=( int num )
    { setValue( (DWORD)num ); return *this; }
    RegMap &operator=( bool b )
    { setValue( ((b)?(DWORD)1:(DWORD)0 ) ); return *this; }
    RegMap &operator=( const std::vector<tstring> &strarray )
    { setValue( strarray ); return *this; }
    RegMap &operator=( const binary &bin )
    { setValue( bin ); return *this; }

private:
	void setValue( const TCHAR *parent, const TCHAR *child, const RegMap &o );
	void setValue( const RegMap &o );
    void clean();

	HKEY m_root;
	tstring m_key;
	bool m_isval;
	bool m_create;

	RegType m_type;
	union {
		DWORD dword;
		tstring *str;
		binary *bin;
		std::vector<tstring> *strarray;
	} m_val;

	mutable std::map<tstring, RegMap*> m_regcache;
};

#endif
