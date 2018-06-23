#pragma once

#include <string>

namespace SeaDrip
{
    namespace GameTimer
    {
        class MainWnd
        {
            public:
                MainWnd( int left = 320, int top = 180 );
                void RegistWndClass( HINSTANCE hInstance );
                HWND GetWindow( void );
                void Show( int nCmdShow = SW_SHOW );
                void OnPaint( HDC hDc );

                void StartMove( int cursorX, int cursorY );
                void EndMove();
                void MoveTo( int x, int y );

                void MarkWindowNeedRedraw( void );

            protected:
                bool validWindow( void );
                bool buildWindow( void );

                /*
                    Paint Window
                */
                void PaintDataInfo( HDC hDc, std::wstring dataName, std::wstring author );

                void PaintSeconds( HDC hDc, const int &nStarted, const int &nSeconds );
                void PaintCurrentSkillName( HDC hDc, const std::wstring &skillName );
                void PaintCurrentSkillComment( HDC hDc, const std::wstring &skillComment );
                //  void PaintComingSkills( HDC hDc,  );

                HINSTANCE m_hInstance;
                HWND m_hMainWnd;

                int m_nLeft;
                int m_nTop;

                bool m_bMoving;
                int m_nCursorX;
                int m_nCursorY;

                COLORREF m_colorBackground;
        };
    };
};
