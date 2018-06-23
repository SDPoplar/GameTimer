#include "GameTimer.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow )
{
    std::string cmd = lpszCmdLine;
    std::wstring errMsg;

    SeaDrip::GameTimer::GameTimer *engine = SeaDrip::GameTimer::GameTimer::GetInstance();
    try
    {
        SeaDrip::GameTimer::Initer *initer = new SeaDrip::GameTimer::Initer( hInstance, nCmdShow );
        engine->Init( initer );
        if( ( cmd != "" ) )
        {
            if( !engine->GetDataManager()->LoadDataByFileName( cmd, errMsg ) )
            {
                MessageBox( NULL, errMsg.c_str(), TEXT( "Game Timer" ), MB_OK );
            }
            else
            {
                engine->GetWindow()->MarkWindowNeedRedraw();
            }
        }
        engine->Run();
        return 0;
    }
    catch( SeaDrip::GameTimer::ErrorCode e )
    {
        return ( int )e;
    }
}
