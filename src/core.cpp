#include "GameTimer.h"
using namespace SeaDrip::GameTimer;

/*
    Initer ====================================================================
*/

Initer::Initer( HINSTANCE hInstance, int nCmdShow )
{
    this->m_hInstnace = hInstance;
    this->m_nCmdShow = nCmdShow;
}

HINSTANCE Initer::GetInstance( void )
{
    return this->m_hInstnace;
}

int Initer::GetCmdShow( void )
{
    return this->m_nCmdShow;
}

/*
    GameTimer ====================================================================
*/

GameTimer *GameTimer::gp_instance = NULL;

GameTimer::GameTimer() : m_bRunning( false )
{
    GameTimer::gp_instance = this;
    this->m_pMainWnd = new MainWnd();
    this->m_pDataMgr = new DataManager();
}

GameTimer *GameTimer::GetInstance( void )
{
    if( GameTimer::gp_instance == NULL )
    {
        new GameTimer();
    }
    return GameTimer::gp_instance;
}

void GameTimer::Init( Initer *initer )
{
    HINSTANCE projInstance = initer->GetInstance();
    this->m_hProjInstance = projInstance;
    this->m_pMainWnd->RegistWndClass( projInstance );
    this->m_pMainWnd->Show( initer->GetCmdShow() );
}

void GameTimer::Run( void )
{
    this->m_bRunning = true;
    SetTimer( this->GetWindow()->GetWindow(), 0, 500, NULL );
    MSG msg;
    while( m_bRunning )
    {
        if( GetMessage( &msg, NULL, 0, 0 ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }
}

void GameTimer::Quit( void )
{
    this->m_bRunning = false;
}

HINSTANCE GameTimer::GetProjInstance( void )
{
    return this->m_hProjInstance;
}

MainWnd *GameTimer::GetWindow( void )
{
    return this->m_pMainWnd;
}

DataManager *GameTimer::GetDataManager( void )
{
    return this->m_pDataMgr;
}

void GameTimer::Tick()
{
    DataManager *mgr = this->GetDataManager();
    if( !mgr || !mgr->IsTicking() )
    {
        return;
    }
    DataItem *d = mgr->GetCurrentItem();
    if( !d )
    {
        return;
    }
    int delay = mgr->GetCurrentDelay();
    SkillRecord *cs;
    while( ( cs = d->GetCurrentItem() ) && ( cs->SecondsDiff( delay ) < 0 ) )
    {
        d->Next();
    }
    this->GetWindow()->MarkWindowNeedRedraw();
}
