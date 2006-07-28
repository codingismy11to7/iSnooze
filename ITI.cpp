#include "StdAfx.h"
#include ".\iti.h"

ITI* ITI::m_inst = NULL;

ITI::~ITI()
{
}

void ITI::connect()
{
    if( FAILED(::CoCreateInstance( CLSID_iTunesApp, NULL, CLSCTX_LOCAL_SERVER, 
        IID_IiTunes, (void**)&m_iTunesApp )) )
        throw trterror( _T("couldn't start itunes interface") );
}

void ITI::disconnect()
{
    m_iTunesApp->Release();
}

void ITI::getPlaylists( std::vector<tstring> &out/*, std::vector<ITID> &outids*/ )
{
    IITSource *libSource;
    if( FAILED(m_iTunesApp->get_LibrarySource( &libSource )) )
        return;

    IITPlaylistCollection *playlists;
    if( FAILED(libSource->get_Playlists( &playlists )) )
    {
        libSource->Release();
        return;
    }

    long plsCount;
    if( FAILED(playlists->get_Count( &plsCount )) )
    {
        playlists->Release();
        libSource->Release();
        return;
    }

    IITPlaylist *pl;
    BSTR name;
    ITID id;
    for( long i = 1; i <= plsCount; i++ )
    {
        if( FAILED(playlists->get_Item( i, &pl )) )
        {
            playlists->Release();
            libSource->Release();
            return;
        }

        if( FAILED(pl->get_Name( &name )) )
        {
            pl->Release();
            playlists->Release();
            libSource->Release();
            return;
        }

        tstring t = (TCHAR*)name;
        SysFreeString( name );

        if( t != _T("Library") && t != _T("Party Shuffle") )
        {
            /*if( FAILED(pl->GetITObjectIDs( &id.sourceID, &id.playlistID, &id.trackID, &id.databaseID )) )
            {
                pl->Release();
                playlists->Release();
                libSource->Release();
                return;
            }*/

            out.push_back( t );
            //outids.push_back( id );
        }

        pl->Release();
    }
      
    playlists->Release();
    libSource->Release();
}

void ITI::playPlaylist( const tstring &plname, bool shuffle )
{
    /*IITPlaylist *pl;
    IITObject *tmp;
    if( FAILED(m_iTunesApp->GetITObjectByID( plid.sourceID, plid.playlistID, plid.trackID, plid.databaseID,
        &tmp )) )
        return;
    if( FAILED(tmp->QueryInterface( IID_IITPlaylist, (void**)&pl )) )
    {
        tmp->Release();
        return;
    }

    pl->put_Shuffle( shuffle );
    pl->PlayFirstTrack();

    pl->Release();
    tmp->Release();*/
    IITSource *libSource;
    if( FAILED(m_iTunesApp->get_LibrarySource( &libSource )) )
        return;

    IITPlaylistCollection *playlists;
    if( FAILED(libSource->get_Playlists( &playlists )) )
    {
        libSource->Release();
        return;
    }

    IITPlaylist *pl;
    BSTR tmp = SysAllocString( plname.c_str() );
    if( FAILED(playlists->get_ItemByName( tmp, &pl )) )
    {
        SysFreeString( tmp );
        playlists->Release();
        libSource->Release();
        return;
    }

    pl->put_Shuffle( ( (shuffle) ? -1 : 0 ) );
    m_iTunesApp->Stop();
    pl->PlayFirstTrack();

    pl->Release();
    SysFreeString( tmp );
    playlists->Release();
    libSource->Release();
}
