#pragma once

#include <Windows.h>

#include "GameTimerError.h"
#include "GameTimerWnd.h"
#include "GameTimerData.hpp"

namespace SeaDrip
{
    namespace GameTimer
    {
        class Initer
        {
            public:
                Initer( HINSTANCE hInstance, int nCmdShow );

                HINSTANCE GetInstance( void );
                int GetCmdShow( void );

            protected:
                HINSTANCE m_hInstnace;
                int m_nCmdShow;
        };

        class GameTimer
        {
            public:
                static GameTimer *GetInstance( void );
                void Init( Initer *initer );
                void Run( void );
                void Quit( void );

                void Tick( void );

                HINSTANCE GetProjInstance( void );
                MainWnd *GetWindow( void );
                DataManager *GetDataManager( void );

            protected:
                GameTimer( void );

                static GameTimer *gp_instance;

                bool m_bRunning;
                HINSTANCE m_hProjInstance;

                MainWnd *m_pMainWnd;
                DataManager *m_pDataMgr;
        };
    };
};
