#include "regmap.h"

RegMap::RegMap( HKEY root, const tstring &key )
: m_root(root), m_isval(false), m_type(KEY), m_create(false), m_key(key)
{
}

RegMap::RegMap( const tstring &key, HKEY root, bool create )
: m_root(root), m_key(key), m_isval(false), m_type(KEY), m_create(create)
{
}

void RegMap::clean()
{
	if( m_isval )
	{
		switch( m_type )
		{
		case SZ:
			delete m_val.str;
			break;
		case BIN:
			delete[] m_val.bin->data;
			delete m_val.bin;
			break;
		case MV_SZ:
			delete m_val.strarray;
			break;
		}
	}
	else
	{
		std::map<tstring,RegMap*>::iterator end = m_regcache.end();
		for( std::map<tstring,RegMap*>::iterator i = m_regcache.begin(); i != end; ++i )
			delete i->second;
		m_regcache.clear();
	}
}

RegMap& RegMap::operator[]( const TCHAR *item ) const
{
    // I'm spending way too much time trying to get caching to work right,
    // and to update after a value was changed.  screw it
	/*if( m_regcache.find( item ) != m_regcache.end() )
    {
        if( m_regcache[item]->m_create )
        {
            delete m_regcache[item];
            m_regcache.erase( item );
        }
        else
		    return *(m_regcache[item]);
    }*/
	HKEY key;
    LONG hr;
    if( _tcslen( item ) )
    {
        tstring newkey;
        if( !m_key.empty() ) newkey = m_key + _T("\\") + item;
        else newkey = item;
        hr = RegOpenKeyEx( m_root, newkey.c_str(), 0, KEY_QUERY_VALUE | KEY_READ, &key );
        if( hr == ERROR_SUCCESS )
        {
            RegCloseKey( key );
            if( m_regcache.find( item ) != m_regcache.end() )
                delete m_regcache[item];
            m_regcache[item] = new RegMap( newkey, m_root );
            return *(m_regcache[item]);
        }
    }

	hr = RegOpenKeyEx( m_root, m_key.c_str(), 0, KEY_QUERY_VALUE | KEY_READ, &key );
	if( hr == ERROR_SUCCESS )
	{
		ULONG newbufsize, bufsize = 512;
		unsigned char *buf;
		ULONG type;
		do {
			bufsize *= 2;
			buf = new unsigned char[bufsize];
			newbufsize = bufsize;
			hr = RegQueryValueEx( key, item, NULL, &type, buf, &newbufsize );
			if( hr != ERROR_SUCCESS )
				delete[] buf;
		} while( hr == ERROR_MORE_DATA );
		if( hr == ERROR_SUCCESS )
		{
			RegCloseKey( key );
            if( m_regcache.find( item ) != m_regcache.end() )
                delete m_regcache[item];
            tstring newkey;
            if( !m_key.empty() ) newkey = m_key + _T("\\") + item;
            else newkey = item;
			switch( type )
			{
			case REG_BINARY:
				binary nb;
				nb.data = buf;
				nb.length = newbufsize;
				m_regcache[item] = new RegMap( nb, newkey.c_str(), m_root );
				return *(m_regcache[item]);
			case REG_DWORD:
				m_regcache[item] = new RegMap( ((DWORD*)buf)[0], newkey.c_str(), m_root );
				delete[] buf;
				return *(m_regcache[item]);
			case REG_SZ:
				m_regcache[item] = new RegMap( (TCHAR*)buf, newkey.c_str(), m_root );
				delete[] buf;
				return *(m_regcache[item]);
			case REG_MULTI_SZ:
				{
					TCHAR *str = (TCHAR*)buf;
					std::vector<tstring> out;
					while( *str != _T('\0') )
					{
						out.push_back( str );
						str += _tcslen( str );
					}
					m_regcache[item] = new RegMap( out, newkey.c_str(), m_root );
					delete[] buf;
					return *(m_regcache[item]);
				}
			default:
				delete[] buf;
				throw trterror( _T("Type not supported (yet)") );
			}
		}
		RegCloseKey( key );
	}
	//else
	{
        if( m_regcache.find( item ) != m_regcache.end() )
            delete m_regcache[item];
        tstring newkey;
        if( !m_key.empty() ) newkey = m_key + _T("\\") + item;
        else newkey = item;
		m_regcache[item] = new RegMap( newkey, m_root, true );
		return *(m_regcache[item]);
	}

	throw trterror( _T("this key doesn't exist") );
}

void RegMap::setValue( const RegMap &o )
{
    TCHAR *buf = new TCHAR[m_key.size() + 1];
    TCHAR *parent = new TCHAR[m_key.size() + 1];

    _tcscpy( buf, m_key.c_str() );
    _tcscpy( parent, buf );

    TCHAR *child;
    if( ( child = _tcsrchr( parent, _T('\\') ) ) == NULL )
    {
        child = buf;
        parent[0] = _T('\0');
    }
    else
    {
        *child = _T('\0');
        child++;
    }

    try
    {
        setValue( parent, child, o );
    }
    catch( ... )
    {
        delete[] buf;
        delete[] parent;
        throw;
    }

    delete[] buf;
    delete[] parent;
}

RegMap &RegMap::operator=( const RegMap &o )
{
    if( this == &o ) return *this;


    if( m_create )
    {
        setValue( o );
    }

	if( /*!m_isval && */!m_create )
	{
        //if( m_isval ) clean();
		//m_isval = o.m_isval;
		m_key = o.m_key;
		m_root = o.m_root;
		//m_regcache = o.m_regcache;
		/*if( m_isval )
		{
			m_type = o.m_type;
			switch( m_type )
			{
			case NUMBER:
				m_val.dword = o.m_val.dword;
				break;
			case SZ:
				m_val.str = new tstring( *o.m_val.str );
				break;
			case BIN:
				m_val.bin = new binary( *o.m_val.bin );
				break;
			case MV_SZ:
				m_val.strarray = new std::vector<tstring>( *o.m_val.strarray );
				break;
			}
		}*/
		return *this;
	}

	return *this;
}

void RegMap::setValue( const TCHAR *parent, const TCHAR *child, const RegMap &o )
{
	HKEY parentkey, key;
	LONG hr;
	hr = RegOpenKeyEx( m_root, parent, 0, KEY_WRITE | KEY_CREATE_SUB_KEY, &parentkey );
	if( hr != ERROR_SUCCESS )
	{
		TCHAR msg[1024];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, msg, 1023, NULL );
		msg[1023] = _T('\0');
		throw trterror( msg );
	}

	switch( o.getType() )
	{
	case KEY:
		{
			hr = RegCreateKeyEx( parentkey, child, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &key, NULL );
			if( hr != ERROR_SUCCESS )
			{
				RegCloseKey( parentkey );
				TCHAR msg[1024];
				FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, msg, 1023, NULL );
				msg[1023] = _T('\0');
				throw trterror( msg );
			}
			RegCloseKey( key );
			break;
		}
	case NUMBER:
		{
			hr = RegSetValueEx( parentkey, child, 0, REG_DWORD, (const BYTE*)&(o.m_val.dword), sizeof(DWORD) );
			if( hr != ERROR_SUCCESS )
			{
				RegCloseKey( parentkey );
				TCHAR msg[1024];
				FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, msg, 1023, NULL );
				msg[1023] = _T('\0');
				throw trterror( msg );
			}
			break;
		}
	case BIN:
		{
			hr = RegSetValueEx( parentkey, child, 0, REG_BINARY, (const BYTE*)o.m_val.bin->data, o.m_val.bin->length );
			if( hr != ERROR_SUCCESS )
			{
				RegCloseKey( parentkey );
				TCHAR msg[1024];
				FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, msg, 1023, NULL );
				msg[1023] = _T('\0');
				throw trterror( msg );
			}
			break;
		}
	case SZ:
		{
			hr = RegSetValueEx( parentkey, child, 0, REG_SZ, (const BYTE*)o.m_val.str->c_str(),
				sizeof(TCHAR) * (DWORD)(o.m_val.str->size() + 1) );
			if( hr != ERROR_SUCCESS )
			{
				RegCloseKey( parentkey );
				TCHAR msg[1024];
				FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, msg, 1023, NULL );
				msg[1023] = _T('\0');
				throw trterror( msg );
			}
			break;
		}
	case MV_SZ:
		{
			ULONG len = 0;
			std::vector<tstring>::iterator i, end = o.m_val.strarray->end();
			for( i = o.m_val.strarray->begin(); i != end; ++i )
				len += 1 + (ULONG)o.m_val.strarray->size();

			TCHAR *tbuf = new TCHAR[len+1];

			len = 0;
			for( i = o.m_val.strarray->begin(); i != end; ++i )
			{
				_tcscpy( tbuf + len, (*i).c_str() );
				len += 1 + (*i).size();
			}
			tbuf[len] = _T('\0');

			hr = RegSetValueEx( parentkey, child, 0, REG_MULTI_SZ, (const BYTE*)tbuf, sizeof(TCHAR) * (len + 1) );
			if( hr != ERROR_SUCCESS )
			{
				delete[] tbuf;
				RegCloseKey( parentkey );
				TCHAR msg[1024];
				FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, msg, 1023, NULL );
				msg[1023] = _T('\0');
				throw trterror( msg );
			}

			delete[] tbuf;

			break;
		}
	}
    RegCloseKey( parentkey );
}

void RegMap::keys( std::vector<tstring> &out ) const
{
	HKEY key;
	LONG hr = RegOpenKeyEx( m_root, m_key.c_str(), 0,
        KEY_QUERY_VALUE | KEY_READ | KEY_ENUMERATE_SUB_KEYS, &key );
	if( hr != ERROR_SUCCESS )
	{
		TCHAR msg[1024];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, msg, 1023, NULL );
		msg[1023] = _T('\0');
		throw trterror( msg );
	}

    DWORD len = 16383;
	TCHAR buf[16383];
    ULONG i = 0;

    while( RegEnumValue( key, i++, buf, &len, NULL, NULL, NULL, NULL ) != ERROR_NO_MORE_ITEMS )
    {
        out.push_back( _tcslwr( buf ) );
        len = 16383;
    }

    i = 0;
	FILETIME ft;
	len = 16383;
	while( RegEnumKeyEx( key, i++, buf, &len, NULL, NULL, NULL, &ft ) != ERROR_NO_MORE_ITEMS )
	{
		out.push_back( _tcslwr( buf ) );
		len = 16383;
	}

	std::sort( out.begin(), out.end() );

	RegCloseKey( key );
}

bool RegMap::has_key( const tstring &key ) const
{
	TCHAR buf[16383];
	if( key.size() > 16382 )
		throw trterror( _T("has_key: value too long") );
	_tcscpy( buf, key.c_str() );
	std::vector<tstring> keylist;
	keys( keylist );
	return (std::find( keylist.begin(), keylist.end(), _tcslwr( buf ) ) != keylist.end());
}

void RegMap::deleteValue( const TCHAR *val ) const
{
	if( !has_key( val ) )
		throw trterror( tstring(val) + _T(" is not an existing value") );
	RegMap r = (*this)[val];
	if( r.getType() == KEY )
		throw trterror( tstring(val) + _T(" is a key, not a value") );

	HKEY key;
	LONG hr = RegOpenKeyEx( m_root, m_key.c_str(), 0, KEY_WRITE, &key );
	if( hr != ERROR_SUCCESS )
	{
		TCHAR msg[1024];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, msg, 1023, NULL );
		msg[1023] = _T('\0');
		throw trterror( msg );
	}
	RegDeleteValue( key, val );
	RegCloseKey( key );
}

void RegMap::deleteKey() const
{
	TCHAR *buf = new TCHAR[m_key.size() + 1];
	TCHAR *todel = new TCHAR[m_key.size() + 1];

	_tcscpy( buf, m_key.c_str() );
	while( buf[ _tcslen( buf ) - 1 ] == _T('\\') )
		buf[ _tcslen( buf ) - 1 ] = _T('\0');
	_tcscpy( todel, buf );

	TCHAR *p;
	if( ( p = _tcsrchr( buf, _T('\\') ) ) == NULL )
	{
		buf[0] = _T('\0');
	}
	else
	{
		*p = _T('\0');
        _tcscpy( todel, p + 1 );
	}

	HKEY key;
	LONG hr = RegOpenKeyEx( m_root, buf, 0, KEY_WRITE, &key );
	if( hr != ERROR_SUCCESS )
	{
		delete[] buf;
		delete[] todel;
		TCHAR msg[1024];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, msg, 1023, NULL );
		msg[1023] = _T('\0');
		throw trterror( msg );
	}
	
	RegDeleteKey( key, todel );
	RegCloseKey( key );

	delete[] buf;
	delete[] todel;
}

		/*TCHAR msg[1024];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, msg, 1023, NULL );
		throw wruntime_error( msg );*/
