#define UNICODE
#define _UNICODE


#include <Windows.h>
#include <Windowsx.h> // For GET_X_PARAM()/GET_Y_PARAM() macro
#include <Psapi.h>    // For GetProcessMemoryInfo()

#include <gl/gl.h>
#include "external/glext.h"
#include "external/wglext.h"

#define DQN_IMPLEMENTATION
#define DQN_WIN32_IMPLEMENTATION
#include "../../dqn.h"

FILE_SCOPE bool globalRunning = true;

FILE_SCOPE LRESULT CALLBACK Win32MainProcCallback(HWND window, UINT msg,
                                                  WPARAM wParam, LPARAM lParam)
{
	const LRESULT MESSAGE_HANDLED = 0;
	LRESULT result                = MESSAGE_HANDLED;
	switch (msg)
	{
		case WM_CLOSE:
		{
			globalRunning = false;
		}
		break;

		default:
		{
			result = DefWindowProcW(window, msg, wParam, lParam);
		}
		break;
	}

	return result;
}

FILE_SCOPE void Win32ProcessMessages(HWND window)
{
	MSG msg;
	while (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
	{
		switch (msg.message)
		{
			case WM_COMMAND:
			{
			}
			break;

			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			{
				bool isDown = (msg.message == WM_LBUTTONDOWN || msg.message == WM_RBUTTONDOWN);
				(void)isDown;
				if (msg.message == WM_LBUTTONDOWN || msg.message == WM_LBUTTONUP)
				{
				}
				else if (msg.message == WM_RBUTTONDOWN || msg.message == WM_RBUTTONUP)
				{
				}
			}
			break;

			case WM_MOUSEMOVE:
			{
				LONG height;
				DqnWin32_GetClientDim(window, NULL, &height);
				f32 mouseX = (f32)(GET_X_LPARAM(msg.lParam));
				f32 mouseY = (f32)(height - GET_Y_LPARAM(msg.lParam));
				(void)mouseX; (void)mouseY;
			}
			break;

			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP:
			{
				bool isDown = (msg.message == WM_KEYDOWN);
				switch (msg.wParam)
				{
					case VK_UP:    break;
					case VK_DOWN:  break;
					case VK_LEFT:  break;
					case VK_RIGHT: break;

					case '1': break;
					case '2': break;
					case '3': break;
					case '4': break;

					case 'Q': break;
					case 'W': break;
					case 'E': break;
					case 'R': break;

					case 'A': break;
					case 'S': break;
					case 'D': break;
					case 'F': break;

					case 'Z': break;
					case 'X': break;
					case 'C': break;
					case 'V': break;

					case VK_ESCAPE:
					{
						if (isDown) globalRunning = false;
					}
					break;

					default: break;
				}
			}
			break;

			default:
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			};
		}
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	////////////////////////////////////////////////////////////////////////////
	// App Init
	////////////////////////////////////////////////////////////////////////////
	// Main Loop Config
	const f32 TARGET_FRAMES_PER_S = 60.0f;
	f32 targetSecondsPerFrame     = 1 / TARGET_FRAMES_PER_S;
	f64 frameTimeInS              = 0.0f;

	// Window Config
	const char WINDOW_TITLE_A[] = u8"MinimalWin32OpenGL";
	const char WINDOW_CLASS_A[] = u8"MinimalWin32OpenGLClass";

	// UTF-8 Version
	wchar_t windowTitleW[DQN_ARRAY_COUNT(WINDOW_TITLE_A)] = {};
	wchar_t windowClassW[DQN_ARRAY_COUNT(WINDOW_CLASS_A)] = {};
	DQN_ASSERT(DqnWin32_UTF8ToWChar(WINDOW_TITLE_A, windowTitleW, DQN_ARRAY_COUNT(windowTitleW)));
	DQN_ASSERT(DqnWin32_UTF8ToWChar(WINDOW_CLASS_A, windowClassW, DQN_ARRAY_COUNT(windowClassW)));

	const u32 BUFFER_WIDTH  = 800;
	const u32 BUFFER_HEIGHT = 600;

	(void)nShowCmd;
	(void)lpCmdLine;
	(void)hPrevInstance;

	////////////////////////////////////////////////////////////////////////////
	// Setup OpenGL
	////////////////////////////////////////////////////////////////////////////
	HWND mainWindow;
	{
		////////////////////////////////////////////////////////////////////////
		// Create Temp Win32 Window For Temp OGL Rendering Context
		////////////////////////////////////////////////////////////////////////
		WNDCLASSEXW windowClass = {
		    sizeof(WNDCLASSEX),
		    CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		    Win32MainProcCallback,
		    0, // int cbClsExtra
		    0, // int cbWndExtra
		    hInstance,
		    LoadIcon(NULL, IDI_APPLICATION),
		    LoadCursor(NULL, IDC_ARROW),
		    GetSysColorBrush(COLOR_3DFACE),
		    L"", // LPCTSTR lpszMenuName
		    windowClassW,
		    NULL, // HICON hIconSm
		};

		// Register and create tmp window
		HWND tmpWindow;
		{
			if (!RegisterClassExW(&windowClass))
			{
				DqnWin32_DisplayLastError("RegisterClassEx() failed.");
				return -1;
			}

			tmpWindow =
			    CreateWindowExW(0, windowClass.lpszClassName, windowTitleW, 0, CW_USEDEFAULT,
			                    CW_USEDEFAULT, 0, 0, NULL, NULL, hInstance, NULL);

			if (!tmpWindow)
			{
				DqnWin32_DisplayLastError("CreateWindowEx() failed.");
				return -1;
			}
		}

		////////////////////////////////////////////////////////////////////////
		// Setup A Temp OGL Context To Use For Getting Extended Functionality
		////////////////////////////////////////////////////////////////////////
		HDC tmpDeviceContext = GetDC(tmpWindow);
		HGLRC tmpOGLRenderingContext;
		{
			PIXELFORMATDESCRIPTOR tmpDesiredPixelFormat = {};
			tmpDesiredPixelFormat.nSize                 = sizeof(tmpDesiredPixelFormat);
			tmpDesiredPixelFormat.nVersion              = 1;
			tmpDesiredPixelFormat.iPixelType            = PFD_TYPE_RGBA;
			tmpDesiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
			tmpDesiredPixelFormat.cColorBits = 32;
			tmpDesiredPixelFormat.cAlphaBits = 8;
			tmpDesiredPixelFormat.iLayerType = PFD_MAIN_PLANE;

			// ChoosePixelFormat finds the closest matching pixelFormat available on the machine
			i32 tmpSuggestedPixelFormatIndex =
			    ChoosePixelFormat(tmpDeviceContext, &tmpDesiredPixelFormat);

			// Create the PixelFormat based on the closest matching one
			PIXELFORMATDESCRIPTOR tmpSuggestedPixelFormat = {};
			DescribePixelFormat(tmpDeviceContext, tmpSuggestedPixelFormatIndex,
			                    sizeof(PIXELFORMATDESCRIPTOR), &tmpSuggestedPixelFormat);

			// Set it and assign the OpenGL context to our program
			if (!SetPixelFormat(tmpDeviceContext, tmpSuggestedPixelFormatIndex,
			                    &tmpSuggestedPixelFormat))
			{
				DqnWin32_DisplayLastError("SetPixelFormat() failed");
				return -1;
			}

			tmpOGLRenderingContext = wglCreateContext(tmpDeviceContext);
			if (!wglMakeCurrent(tmpDeviceContext, tmpOGLRenderingContext))
			{
				DqnWin32_DisplayLastError("wglMakeCurrent() failed");
				return -1;
			}
		}

		////////////////////////////////////////////////////////////////////////
		// Load WGL Functions For Creating Modern OGL Windows
		////////////////////////////////////////////////////////////////////////
		PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB    = (PFNWGLCHOOSEPIXELFORMATARBPROC)   (wglGetProcAddress("wglChoosePixelFormatARB"));
		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)(wglGetProcAddress("wglCreateContextAttribsARB"));
		{
			if (!wglChoosePixelFormatARB)
			{
				DqnWin32_DisplayLastError("wglGetProcAddress() failed");
				return -1;
			}

			if (!wglCreateContextAttribsARB)
			{
				DqnWin32_DisplayLastError("wglGetProcAddress() failed");
				return -1;
			}
		}

		////////////////////////////////////////////////////////////////////////
		// Create Window Using Modern OGL Functions
		////////////////////////////////////////////////////////////////////////
		{
			// NOTE: Regarding Window Sizes
			// If you specify a window size, e.g. 800x600, Windows regards the 800x600
			// region to be inclusive of the toolbars and side borders. So in actuality,
			// when you blit to the screen blackness, the area that is being blitted to
			// is slightly smaller than 800x600. Windows provides a function to help
			// calculate the size you'd need by accounting for the window style.
			RECT windowRect   = {};
			windowRect.right  = BUFFER_WIDTH;
			windowRect.bottom = BUFFER_HEIGHT;

			const bool HAS_MENU_BAR  = false;
			const DWORD WINDOW_STYLE = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
			AdjustWindowRect(&windowRect, WINDOW_STYLE, HAS_MENU_BAR);

			mainWindow =
			    CreateWindowExW(0, windowClass.lpszClassName, windowTitleW, WINDOW_STYLE,
			                    CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left,
			                    windowRect.bottom - windowRect.top, NULL, NULL, hInstance, NULL);
			if (!mainWindow)
			{
				DqnWin32_DisplayLastError("CreateWindowEx() failed.");
				return -1;
			}

			const i32 DESIRED_PIXEL_FORMAT[] = {WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			                                    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			                                    WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
			                                    WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
			                                    WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
			                                    WGL_COLOR_BITS_ARB,     32,
			                                    WGL_ALPHA_BITS_ARB,     8,
			                                    WGL_DEPTH_BITS_ARB,     24,
			                                    WGL_STENCIL_BITS_ARB,   8,
			                                    WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
			                                    WGL_SAMPLES_ARB,        4,
			                                    0};

			HDC deviceContext = GetDC(mainWindow);
			i32 suggestedPixelFormatIndex;
			u32 numFormatsSuggested;
			bool wglResult =
			    wglChoosePixelFormatARB(deviceContext, DESIRED_PIXEL_FORMAT, NULL, 1,
			                            &suggestedPixelFormatIndex, &numFormatsSuggested);
			if (!wglResult || numFormatsSuggested == 0)
			{
				DQN_WIN32_ERROR_BOX("wglChoosePixelFormatARB() failed", NULL);
				return -1;
			}

			PIXELFORMATDESCRIPTOR suggestedPixelFormat = {};
			DescribePixelFormat(deviceContext, suggestedPixelFormatIndex,
			                    sizeof(suggestedPixelFormat), &suggestedPixelFormat);
			if (!SetPixelFormat(deviceContext, suggestedPixelFormatIndex, &suggestedPixelFormat))
			{
				DqnWin32_DisplayLastError("SetPixelFormat() Modern OGL Creation failed");
				return -1;
			}

			const i32 OGL_MAJOR_MIN = 4, OGL_MINOR_MIN = 5;
			const i32 CONTEXT_ATTRIBS[] = {WGL_CONTEXT_MAJOR_VERSION_ARB, OGL_MAJOR_MIN,
			                               WGL_CONTEXT_MINOR_VERSION_ARB, OGL_MINOR_MIN,
			                               WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			                               0};

			HGLRC oglRenderingContext =
			    wglCreateContextAttribsARB(deviceContext, 0, CONTEXT_ATTRIBS);
			if (!oglRenderingContext)
			{
				DQN_WIN32_ERROR_BOX("wglCreateContextAttribsARB() failed", NULL);
				return -1;
			}

			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(tmpOGLRenderingContext);
			ReleaseDC(tmpWindow, tmpDeviceContext);
			DestroyWindow(tmpWindow);

			if (!wglMakeCurrent(deviceContext, oglRenderingContext))
			{
				DqnWin32_DisplayLastError("wglMakeCurrent() Modern OGL Creation failed");
				return -1;
			}

			ReleaseDC(mainWindow, deviceContext);

			// TODO: Load all the OGL function pointers we need here
		}
	}

	while (globalRunning)
	{
		f64 startFrameTimeInS = DqnTimer_NowInS();
		Win32ProcessMessages(mainWindow);

		////////////////////////////////////////////////////////////////////////
		// Rendering
		////////////////////////////////////////////////////////////////////////
		if (1)
		{
			HDC deviceContext = GetDC(mainWindow);
			glViewport(0, 0, BUFFER_WIDTH, BUFFER_HEIGHT);
			glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			SwapBuffers(deviceContext);
			ReleaseDC(mainWindow, deviceContext);
		}

		////////////////////////////////////////////////////////////////////////
		// Frame Limiting
		////////////////////////////////////////////////////////////////////////
		if (1)
		{
			f64 workTimeInS = DqnTimer_NowInS() - startFrameTimeInS;
			if (workTimeInS < targetSecondsPerFrame)
			{
				DWORD remainingTimeInMs =
				    (DWORD)((targetSecondsPerFrame - workTimeInS) * 1000);
				Sleep(remainingTimeInMs);
			}
		}

		frameTimeInS        = DqnTimer_NowInS() - startFrameTimeInS;
		f32 msPerFrame      = 1000.0f * (f32)frameTimeInS;
		f32 framesPerSecond = 1.0f / (f32)frameTimeInS;

		////////////////////////////////////////////////////////////////////////
		// Misc
		////////////////////////////////////////////////////////////////////////
		// Update title bar
		if (1)
		{
			const f32 titleUpdateFrequencyInS  = 0.1f;
			LOCAL_PERSIST f32 titleUpdateTimer = titleUpdateFrequencyInS;
			titleUpdateTimer += (f32)frameTimeInS;
			if (titleUpdateTimer > titleUpdateFrequencyInS)
			{
				titleUpdateTimer = 0;

				// Get Win32 reported mem usage
				PROCESS_MEMORY_COUNTERS memCounter = {};
				GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter));

				// Create UTF-8 buffer string
				const char formatStr[]       = "%s - dev - %5.2f ms/f - %5.2f fps - working set mem %'dkb - page file touched mem %'dkb";
				const u32 windowTitleBufSize = DQN_ARRAY_COUNT(formatStr) + DQN_ARRAY_COUNT(WINDOW_TITLE_A) + 32;
				char windowTitleBufA[windowTitleBufSize] = {};

				// Form UTF-8 buffer string
				Dqn_sprintf(windowTitleBufA, formatStr, WINDOW_TITLE_A, msPerFrame, framesPerSecond,
				            (u32)(memCounter.WorkingSetSize / 1024.0f),
				            (u32)(memCounter.PagefileUsage / 1024.0f));

				// Convert to wchar_t for windows
				wchar_t windowTitleBufW[windowTitleBufSize] = {};
				DQN_ASSERT(DqnWin32_UTF8ToWChar(windowTitleBufA, windowTitleBufW, windowTitleBufSize));
				SetWindowTextW(mainWindow, windowTitleBufW);
			}
		}
	}

	return 0;
}
