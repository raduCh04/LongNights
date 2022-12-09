#include <Windows.h>

static bool s_isRunning;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        OutputDebugStringA("Window created");

        return (0);
    } break;

    case WM_DESTROY:
    {
        s_isRunning = false;
        OutputDebugStringA("Window destroyed");

        return (0);
    } break;
    
    case WM_SIZE:
    {
        OutputDebugStringA("Window sized");

        return (0);
    } break;

    case WM_QUIT:
    {
        s_isRunning = false;
        PostQuitMessage(0);
    } break;
    
    default:
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI 
WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    WNDCLASSEX WindowClass = {};
    WindowClass.hInstance = hInstance;
    WindowClass.lpfnWndProc = WndProc;
    WindowClass.hCursor = nullptr;
    WindowClass.hIcon = LoadIcon(hInstance, IDI_ERROR);
    WindowClass.hIcon = LoadIcon(hInstance, IDI_ERROR);
    WindowClass.style = CS_VREDRAW | CS_HREDRAW;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.hbrBackground = nullptr;
    WindowClass.cbWndExtra = 0;
    WindowClass.lpszClassName = "Hello";
    WindowClass.lpszMenuName = 0;

    if (RegisterClassEx(&WindowClass))
    {
        HWND WindowHandle = CreateWindowEx(0, 
        WindowClass.lpszClassName, "MainWindow", 
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
        200, 200, 640, 480, 0, 0, hInstance, 0);

        if (WindowHandle)
        {
            s_isRunning = true;

            while (s_isRunning)
            {
                MSG message;
                BOOL messageResult = PeekMessage(&message, WindowHandle, 0, 0, PM_REMOVE);

                if (messageResult > 0)
                {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }
                
                
            }
            
        }
        else
        {
            OutputDebugStringA("Failed Window Handle");
            return (EXIT_FAILURE);
        }
        
    }
    else
    {
        OutputDebugStringA("Failed register class");
        return (EXIT_FAILURE);
    }
    

    return (EXIT_SUCCESS);
}