#include "GameTimer.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow )
{
    SeaDrip::GameTimer::GameTimer *engine = SeaDrip::GameTimer::GameTimer::GetInstance();
    try
    {
        SeaDrip::GameTimer::Initer *initer = new SeaDrip::GameTimer::Initer( hInstance, nCmdShow );
        engine->Init( initer );
        engine->Run();
        return 0;
    }
    catch( SeaDrip::GameTimer::ErrorCode e )
    {
        return ( int )e;
    }
}
