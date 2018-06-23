#include "GameTimer.h"
#include <fstream>
#include <ctime>
#include "UseJsonCpp.h"
using namespace SeaDrip::GameTimer;

/* Static ============================================================================ */

std::wstring StrToWStr( std::string s )
{
    wchar_t ret[ 128 ] = { 0 };
    MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, s.c_str(), s.length(), ret, 128 );
    //  ret[ s.length() ] = TEXT( '\0' );
    return std::wstring( ret );
}

/* NameRecord ======================================================================== */

std::wstring NameRecord::GetItemName( void )
{
    return this->m_wsName;
}

std::wstring NameRecord::GetComment( void )
{
    return this->m_wsComment;
}

/* SkillRecord ======================================================================= */

SkillRecord::SkillRecord( int delay, std::wstring name, std::wstring comment )
    : m_nDelay( delay )
{
    this->m_wsName = name;
    this->m_wsComment = comment;
}

int SkillRecord::SecondsDiff( int second )
{
    return this->m_nDelay - second;
}

/* DataItem ========================================================================== */

void DataItem::SetInfo( std::wstring name, std::wstring comment )
{
    this->m_wsName = name;
    this->m_wsComment = comment;
}

void DataItem::AddTimePoint( int delay, std::wstring name, std::wstring comment )
{
    this->m_list.push_back( new SkillRecord( delay, name, comment ) );
}

/* DataManager ======================================================================= */

DataManager::DataManager() : m_bValid( false ), m_timeStartFrom( 0 )
{}

void DataManager::ResetCurrentData( void )
{
    if( this->m_bValid )
    {
        this->m_list[ this->m_nIndex ]->ResetIndex();
    }
}

bool DataManager::LoadDataByFileName( std::wstring fileName )
{
    this->ClearList();

    std::ifstream f( fileName, std::ios::in );
    if( !f )
    {
        return false;
    }
    
    Json::CharReaderBuilder jsonBuilder;
    Json::Value jsonValue;
    std::string err;

    try
    {
        Json::parseFromStream( jsonBuilder, f, &jsonValue, &err );
    

        this->m_wsDataName = StrToWStr( jsonValue[ "title" ].asString() );
        this->m_wsDataAuthor = StrToWStr( jsonValue[ "author" ].asString() );

        DataItem *data;
        for( Json::Value &item : jsonValue[ "list" ] )
        {
            data = new DataItem();
            data->SetInfo( StrToWStr( item[ "name" ].asString() ), StrToWStr( item[ "comment" ].asString() ) );
            data->AddTimePoint( 0, TEXT( "开场" ), TEXT( "战斗倒计时" ) );
            for( Json::Value &tp : item[ "points" ] )
            {
                data->AddTimePoint( tp[ "delay" ].asInt(), StrToWStr( tp[ "name" ].asString() ), StrToWStr( tp[ "comment" ].asString() ) );
            }
            this->m_list.push_back( data );
        }
        this->m_nIndex = 0;
        this->m_bValid = !this->m_list.empty();

        return this->m_bValid;
    }
    catch( ... )
    {
        return false;
    }
}

std::wstring DataManager::GetDataAuthor( void )
{
    return this->m_wsDataAuthor;
}

std::wstring DataManager::GetDataName( void )
{
    return this->m_wsDataName;
}

bool DataManager::IsTicking( void )
{
    return ( this->m_timeStartFrom > 0 );
}

void DataManager::BeginTick( void )
{
    this->m_timeStartFrom = time( NULL ) + 5;
}

void DataManager::EndTick( void )
{
    this->m_timeStartFrom = 0;
}

bool DataManager::DataValid( void )
{
    return this->m_bValid && this->GetCurrentItem();
}

int DataManager::GetCurrentDelay( void )
{
    unsigned int now = time( NULL );
    return ( now > this->m_timeStartFrom )
        ? ( now - this->m_timeStartFrom )
        : -1 * ( this->m_timeStartFrom - now );
}
