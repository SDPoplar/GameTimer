#include "GameTimer.h"
using namespace SeaDrip::GameTimer;

#define     WSTR_MAINWND_CLASS_NAME     TEXT( "SD_GAMETIMER_MAINWND_CLASS" )
#define     INT_WND_WIDTH               480
#define     INT_WND_HEIGHT              80

#define     INT_MAX_FILENAME_LEN        128

LRESULT PaintWnd( MainWnd *pWnd )
{
    PAINTSTRUCT ps;
    HWND hWnd = pWnd->GetWindow();
    BeginPaint( hWnd, &ps );
    pWnd->OnPaint( GetDC( hWnd ) );
    EndPaint( hWnd, &ps );
    return 0;
}

RECT CreateWndRect( void )
{
    RECT rect;
    rect.top = 0;
    rect.left = 0;
    rect.right = INT_WND_WIDTH;
    rect.bottom = INT_WND_HEIGHT;

    return rect;
}

void ToggleDataMgrTickStatus( DataManager *datamgr )
{
    if( !datamgr || !datamgr->DataValid() )
    {
        return;
    }
    DataItem *d = datamgr->GetCurrentItem();
    if( !d || datamgr->IsTicking() )
    {
        datamgr->EndTick();
        d->ResetIndex();
    }
    else
    {
        datamgr->BeginTick();
    }
}

void ChooseData( DataManager *datamgr, short volume )
{
    if( !datamgr || datamgr->IsTicking() )
    {
        return;
    }
    if( volume > 0 )
    {
        datamgr->Front( true );
    }
    else
    {
        datamgr->Next( true );
    }
}

std::wstring LoadFirstDropFile( HDROP files )
{
    wchar_t buff[ INT_MAX_FILENAME_LEN ];
    DragQueryFile( files, 0, buff, INT_MAX_FILENAME_LEN );
    return std::wstring( buff );
}

LRESULT CALLBACK GameTimerWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    GameTimer *engine = GameTimer::GetInstance();
    switch( msg )
    {
        case WM_QUIT:
        case WM_DESTROY:
            engine->Quit();
            return 0;
        case WM_LBUTTONDOWN:
            engine->GetWindow()->StartMove( LOWORD( lParam ), HIWORD( lParam ) );
            return 0;
        case WM_LBUTTONUP:
            engine->GetWindow()->EndMove();
            return 0;
        case WM_MOUSEMOVE:
            engine->GetWindow()->MoveTo( LOWORD( lParam ), HIWORD( lParam ) );
            return 0;
        case WM_PAINT:
            return PaintWnd( engine->GetWindow() );
        case WM_DROPFILES:
            if( !engine->GetDataManager()->LoadDataByFileName( LoadFirstDropFile( ( HDROP )wParam ) ) )
            {
                MessageBox( hWnd, TEXT( "无效的数据文件" ), TEXT( "载入文件失败" ), MB_OK );
            }
            SetWindowText( hWnd, engine->GetDataManager()->GetDataName().c_str() );
            engine->GetWindow()->MarkWindowNeedRedraw();
            return 0;
        case WM_TIMER:
            engine->Tick();
            return 0;
        case WM_LBUTTONDBLCLK:
            ToggleDataMgrTickStatus( engine->GetDataManager() );
            engine->GetWindow()->MarkWindowNeedRedraw();
            return 0;
        case WM_MOUSEWHEEL:
            ChooseData( engine->GetDataManager(), HIWORD( wParam ) );
            engine->GetWindow()->MarkWindowNeedRedraw();
            return 0;
        default:
            return DefWindowProc( hWnd, msg, wParam, lParam );
    }
}

MainWnd::MainWnd( int left, int top ) : m_nLeft( left ), m_nTop( top ), m_bMoving( false )
{
    this->m_colorBackground = RGB( 47, 179, 255 );
}

HWND MainWnd::GetWindow( void )
{
    ErrorWhen( !this->validWindow() && !this->buildWindow(), ErrorCode::CANNOT_CREATE_WINDOW );
    return this->m_hMainWnd;
}

bool MainWnd::validWindow( void )
{
    return this->m_hMainWnd && IsWindow( this->m_hMainWnd );
}

bool MainWnd::buildWindow( void )
{
    this->m_hMainWnd = CreateWindowEx( WS_EX_ACCEPTFILES, WSTR_MAINWND_CLASS_NAME, TEXT( "GAME TIMER" ),
        WS_POPUP, this->m_nLeft, this->m_nTop, INT_WND_WIDTH, INT_WND_HEIGHT,
        NULL, NULL, this->m_hInstance, NULL );
    return !!this->m_hMainWnd;
}

void MainWnd::RegistWndClass( HINSTANCE hInstance )
{
    this->m_hInstance = hInstance;

    WNDCLASS wc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    //  wc.hbrBackground = ( HBRUSH )GetStockObject( WHITE_BRUSH );
    wc.hbrBackground = ( HBRUSH )CreateSolidBrush( this->m_colorBackground );
    wc.hCursor = LoadCursor( NULL, IDC_SIZEALL );
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.lpfnWndProc = GameTimerWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WSTR_MAINWND_CLASS_NAME;
    wc.lpszMenuName = NULL;
    wc.style = CS_DROPSHADOW | CS_DBLCLKS;
    ErrorWhen( !RegisterClass( &wc ), ErrorCode::CANNOT_INIT_WNDCLS );
}

void MainWnd::Show( int nCmdShow )
{
    HWND hWnd = this->GetWindow();
    ShowWindow( hWnd, nCmdShow );
    UpdateWindow( hWnd );
    SetWindowPos( this->m_hMainWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
}

void MainWnd::OnPaint( HDC hDc )
{
    GameTimer *engine = GameTimer::GetInstance();
    DataManager *datamgr = engine->GetDataManager();
    if( !datamgr->DataValid() )
    {
        this->PaintDataInfo( hDc, TEXT( "欢迎使用SD::GameTimer" ), TEXT( "拖放.sdgtj文件以加载数据" ) );
        return;
    }

    DataItem *cd = datamgr->GetCurrentItem();
    if( !cd )
    {
        this->PaintDataInfo( hDc, datamgr->GetDataName() + TEXT( " - " ) + datamgr->GetDataAuthor(), TEXT( "No data" ) );
        if( datamgr->IsTicking() )
        {
            datamgr->EndTick();
        }
        return;
    }
    if( !datamgr->IsTicking() )
    {
        this->PaintDataInfo( hDc, cd->GetItemName(), cd->GetComment() );
        return;
    }

    int delay = datamgr->GetCurrentDelay();
    SkillRecord *cs = cd->GetCurrentItem();
    if( !cs )
    {
        return;
    }
    this->PaintSeconds( hDc, delay, cs->SecondsDiff( delay ) );
    this->PaintCurrentSkillName( hDc, cs->GetItemName() );
    this->PaintCurrentSkillComment( hDc, cs->GetComment() );
}

void MainWnd::StartMove( int cursorX, int cursorY )
{
    this->m_nCursorX = cursorX;
    this->m_nCursorY = cursorY;
    this->m_bMoving = true;
}

void MainWnd::EndMove( void )
{
    this->m_bMoving = false;
}

void MainWnd::MoveTo( int cursorX, int cursorY )
{
    if( !this->m_bMoving )
    {
        return;
    }
    this->m_nLeft += ( cursorX - this->m_nCursorX );
    this->m_nTop += ( cursorY - this->m_nCursorY );
    MoveWindow( this->m_hMainWnd, this->m_nLeft, this->m_nTop, INT_WND_WIDTH, INT_WND_HEIGHT, true );
}

void MainWnd::MarkWindowNeedRedraw()
{
    RECT rect = CreateWndRect();
    //  GetWindowRect( this->m_hMainWnd, &rect );
    InvalidateRect( this->m_hMainWnd, &rect, true );
}

HFONT CreateWndDefFont( int nWidth, int nHeight )
{
    return CreateFont( nHeight, nWidth, 0, 0, FW_NORMAL, false, false, false,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH, TEXT( "Microsoft Yahei" ) );
}

void MainWnd::PaintDataInfo( HDC hDc, std::wstring dataName, std::wstring author )
{
    SetTextColor( hDc, RGB( 255, 255, 255 ) );
    SetBkMode( hDc, TRANSPARENT );

    RECT rect;
    HFONT font, oldFont;

    rect.left = 10;
    rect.right = 300;
    rect.top = 10;
    rect.bottom = 40;
    font = CreateWndDefFont( 12, 24 );
    oldFont = ( HFONT )SelectObject( hDc, font );
    DrawText( hDc, dataName.c_str(), -1, &rect, DT_SINGLELINE | DT_LEFT );
    font = ( HFONT )SelectObject( hDc, oldFont );
    DeleteObject( font );

    rect.left = 10;
    rect.right = 320;
    rect.top = 50;
    rect.bottom = 80;
    font = CreateWndDefFont( 9, 18 );
    oldFont = ( HFONT )SelectObject( hDc, font );
    DrawText( hDc, author.c_str(), -1, &rect, DT_SINGLELINE | DT_LEFT );
    font = ( HFONT )SelectObject( hDc, oldFont );
    DeleteObject( font );
}

void MainWnd::PaintSeconds( HDC hDc, const int &nStarted, const int &nSecond )
{
    SetBkMode( hDc, TRANSPARENT );

    HPEN pen, oldPen;
    HBRUSH brush, oldBrush;
    pen = CreatePen( PS_SOLID, 1, RGB( 51, 136, 255 ) );
    oldPen = ( HPEN )SelectObject( hDc, pen );
    brush = CreateSolidBrush( RGB( 45, 121, 218 ) );
    oldBrush = ( HBRUSH )SelectObject( hDc, brush );
    Rectangle( hDc, 0, 0, 80, 80 );
    pen = ( HPEN )SelectObject( hDc, oldPen );
    DeleteObject( pen );
    brush = ( HBRUSH )SelectObject( hDc, oldBrush );
    DeleteObject( brush );

    wchar_t s[ 16 ] = { 0 };
    swprintf_s( s, TEXT( "%d" ), nSecond );
    COLORREF c = RGB( 255, 255, 255 );
    if( nSecond < 5 )
    {
        c = RGB( 243, 213, 59 );
    }
    if( nSecond < 3 )
    {
        c = RGB( 255, 64, 31 );
    }
    SetTextColor( hDc, c );
    RECT rect;
    rect.left = 0;
    rect.bottom = 50;
    rect.top = 10;
    rect.right = 80;
    HFONT font = CreateWndDefFont( 16, 32 ), oldFont;
    oldFont = ( HFONT )SelectObject( hDc, font );
    DrawText( hDc, s, -1, &rect, DT_CENTER );
    font = ( HFONT )SelectObject( hDc, oldFont );
    DeleteObject( font );

    if( nStarted < 0 )
    {
        swprintf_s( s, TEXT( "即将开始" ) );
    }
    else
    {
        swprintf_s( s, TEXT( "%02d:%02d" ), nStarted / 60, nStarted % 60 );
    }

    SetTextColor( hDc, RGB( 255, 255, 255 ) );
    rect.left = 0;
    rect.bottom = 70;
    rect.top = 50;
    rect.right = 80;
    font = CreateWndDefFont( 8, 16 );
    oldFont = ( HFONT )SelectObject( hDc, font );
    DrawText( hDc, s, -1, &rect, DT_CENTER );
    font = ( HFONT )SelectObject( hDc, oldFont );
    DeleteObject( font );
}

void MainWnd::PaintCurrentSkillName( HDC hDc, const std::wstring &name )
{
    SetTextColor( hDc, RGB( 255, 255, 255 ) );
    SetBkMode( hDc, TRANSPARENT );

    RECT rect;
    rect.left = 90;
    rect.bottom = 40;
    rect.top = 10;
    rect.right = 320;
    HFONT font = CreateWndDefFont( 12, 24 ), oldFont;
    oldFont = ( HFONT )SelectObject( hDc, font );
    DrawText( hDc, name.c_str(), -1, &rect, DT_LEFT | DT_SINGLELINE );
    font = ( HFONT )SelectObject( hDc, oldFont );
    DeleteObject( font );
}

void MainWnd::PaintCurrentSkillComment( HDC hDc, const std::wstring &comment )
{
    SetTextColor( hDc, RGB( 255, 255, 255 ) );
    SetBkMode( hDc, TRANSPARENT );

    RECT rect;
    rect.left = 90;
    rect.bottom = 70;
    rect.top = 50;
    rect.right = 300;
    HFONT font = CreateWndDefFont( 9, 18 ), oldFont;
    oldFont = ( HFONT )SelectObject( hDc, font );
    DrawText( hDc, comment.c_str(), -1, &rect, DT_LEFT | DT_SINGLELINE );
    font = ( HFONT )SelectObject( hDc, oldFont );
    DeleteObject( font );
}

/*
void MainWnd::PaintComingSkills()
{}
*/
