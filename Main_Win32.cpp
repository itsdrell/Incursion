#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "Engine/Math/MathUtils.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Game.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Remove all OpenGL references out of Main_Win32.cpp once you have a Renderer
//
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this macro later to a more central place, e.g. Engine/Core/EngineCommon.hpp
//
#define UNUSED(x) (void)(x);


// These are the input stuff
#define g_theLetterP 0x50
#define g_theLetterO 0x4F
#define g_theLetterT 0x54
#define g_theLetterI 0x49
#define g_theSpaceKey 0x20

#define g_theLetterE 0x45
#define g_theLetterS 0x53
#define g_theLetterF 0x46

#define g_theLeftArrowKey 0x25
#define g_theRightArrowKey 0x27
#define g_theUpArrowKey 0x26

#define g_theLetterN 0x4E
#define g_theF1Key 0x70

#define g_theLetterZ 0x5A // This is for my screen debug view:)

//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move these constants to GameCommon.hpp or elsewhere
// 
constexpr float CLIENT_ASPECT = 1.0f; // We are requesting a 1:1 aspect (square) window area


									  //-----------------------------------------------------------------------------------------------
									  // #SD1ToDo: Move each of these items to its proper place, once that place is established
									  // 
//bool g_isQuitting = false;						// ...becomes App::m_isQuitting
HWND g_hWnd = nullptr;							// ...becomes WindowContext::m_windowHandle
HDC g_displayDeviceContext = nullptr;			// ...becomes WindowContext::m_displayContext
HGLRC g_openGLRenderingContext = nullptr;		// ...becomes RendererOpenGL::m_glRenderingContext
const char* APP_NAME = "Win32 OpenGL Test App";	// ...becomes ???

bool g_isDebugging = false;

												//-----------------------------------------------------------------------------------------------
												// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
												// This function is called by Windows whenever we ask it for notifications
												// #SD1ToDo: We will move this function to a more appropriate place when we're ready
												//
LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
	unsigned char asKey = (unsigned char) wParam;
	switch( wmMessageCode )
	{
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
	case WM_CLOSE:		
	{
		g_theApp->m_isQuitting = true;
		return 0; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
	case WM_KEYDOWN:
	{
		
		if( asKey == VK_ESCAPE )
		{
			g_theApp->m_isQuitting = true;
			return 0; // "Consumes" this message (tells Windows "okay, we handled it")
		}

		g_theInput->OnKeyPressed( asKey );
		break;
	}

	// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
	case WM_KEYUP:
	{
 		
// 		if(asKey == g_theLetterZ)
// 		{
// 			g_isDebugging = !g_isDebugging;
// 		}


		g_theInput->OnKeyReleased( asKey );
		break;
	}
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
}


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: We will move this function to a more appropriate place later on...
//
void CreateOpenGLWindow( HINSTANCE applicationInstanceHandle, float clientAspect )
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast< WNDPROC >( WindowsMessageHandlingProcedure ); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle( NULL );
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT( "Simple Window Class" );
	RegisterClassEx( &windowClassDescription );

	// #SD1ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );
	float desktopWidth = (float)( desktopRect.right - desktopRect.left );
	float desktopHeight = (float)( desktopRect.bottom - desktopRect.top );
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if( clientAspect > desktopAspect )
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * clientAspect;
	}

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int) clientMarginX;
	clientRect.right = clientRect.left + (int) clientWidth;
	clientRect.top = (int) clientMarginY;
	clientRect.bottom = clientRect.top + (int) clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

	WCHAR windowTitle[ 1024 ];
	MultiByteToWideChar( GetACP(), 0, APP_NAME, -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[ 0 ] ) );
	g_hWnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		applicationInstanceHandle,
		NULL );

	ShowWindow( g_hWnd, SW_SHOW );
	SetForegroundWindow( g_hWnd );
	SetFocus( g_hWnd );

	g_displayDeviceContext = GetDC( g_hWnd );

	HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );
	SetCursor( cursor );

	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	memset( &pixelFormatDescriptor, 0, sizeof( pixelFormatDescriptor ) );
	pixelFormatDescriptor.nSize = sizeof( pixelFormatDescriptor );
	pixelFormatDescriptor.nVersion = 1;
	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits = 24;
	pixelFormatDescriptor.cDepthBits = 24;
	pixelFormatDescriptor.cAccumBits = 0;
	pixelFormatDescriptor.cStencilBits = 8;

	int pixelFormatCode = ChoosePixelFormat( g_displayDeviceContext, &pixelFormatDescriptor );
	SetPixelFormat( g_displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor );
	g_openGLRenderingContext = wglCreateContext( g_displayDeviceContext );
	wglMakeCurrent( g_displayDeviceContext, g_openGLRenderingContext );

	glLineWidth( 1.5f );
	glEnable( GL_BLEND );
	glEnable( GL_LINE_SMOOTH );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}


//-----------------------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
//
// void RunMessagePump()
// {
// 	MSG queuedMessage;
// 	for( ;; )
// 	{
// 		const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
// 		if( !wasMessagePresent )
// 		{
// 			break;
// 		}
// 
// 		TranslateMessage( &queuedMessage );
// 		DispatchMessage( &queuedMessage ); // This tells Windows to call our "WindowsMessageHandlingProcedure" function
// 	}
// }


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Remove this function and replace it with TheApp::Update()
//
void Update()
{
	//float deltaSeconds = (1.f / 60.f);
	//Sleep(16);

	//g_theApp->Update(deltaSeconds);
}




//-----------------------------------------------------------------------------------------------
// Some simple OpenGL example drawing code.
// This is the graphical equivalent of printing "Hello, world."
// #SD1ToDo: Remove this function and replace it with TheApp::Render()
//
void Render()
{
	// Establish a 2D (orthographic) drawing coordinate system: (0,0) bottom-left to (1000,1000) top-right
	//glLoadIdentity();
	//glOrtho( 0.f, 1000.f, 0.f, 1000.f, 0.f, 1.f );
	
	// Clear all screen (backbuffer) pixels to medium-blue
	//glClearColor( 0.f, 0.f, 0.0f, 0.f ); 
	//glClear( GL_COLOR_BUFFER_BIT );

	// Would be cool for debug
	if(g_isDebugging == true)
	{
		glLoadIdentity();
		glOrtho( -1000.f, 2000.f, -1000.f, 2000.f, 0.f, 1.f );
		glBegin( GL_LINES );
		glVertex2f( 0.f,0.f);
		glVertex2f( 0.f,1000.f);

		glVertex2f( 0.f,1000.f);
		glVertex2f( 1000.f,1000.f);

		glVertex2f( 1000.f,1000.f);
		glVertex2f( 1000.f, 0.f);

		glVertex2f( 1000.f,0.f);
		glVertex2f( 0.f,0.f);
	}
	

	/*g_theApp->Render();*/
	
	// "Present" the backbuffer by swapping the front (visible) and back (working) screen buffers
	//SwapBuffers( g_displayDeviceContext ); // Note: call this once at the end of each frame
}


//-----------------------------------------------------------------------------------------------
// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
//
void RunFrame()
{
	g_theApp->RunFrame();			
	g_theApp->Render();			// #SD1ToDo: ...becomes with g_theApp->Render();
	/*SwapBuffers( g_displayDeviceContext ); // Note: call this once at the end of each frame*/
}


//-----------------------------------------------------------------------------------------------
void Initialize( HINSTANCE applicationInstanceHandle )
{
	CreateOpenGLWindow( applicationInstanceHandle, CLIENT_ASPECT );
	g_theApp = new App();	// #SD1ToDo: create an App class and instance it here
}


//-----------------------------------------------------------------------------------------------
void Shutdown()
{
	// Destroy the global App instance
	delete g_theApp;			// #SD1ToDo: make sure you delete and null out the App when done
	g_theApp = nullptr;
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	UNUSED( commandLineString );
	Initialize( applicationInstanceHandle );

	// Program main loop; keep running frames until it's time to quit
	while( !g_theApp->m_isQuitting) // #SD1ToDo: ...becomes:  !g_theApp->IsQuitting()
	{
		RunFrame();
	}

	Shutdown();
	return 0;
}


