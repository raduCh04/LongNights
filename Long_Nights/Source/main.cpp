#include <Windows.h>
#include <stdint.h>

#define internal static
#define local_presist static
#define global_variable static

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

global_variable bool s_isRunning;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
int BytesPerPixel = 4;

internal void
RenderWeirdGradient(int XOffset, int YOffset)
{
    int Width = BitmapWidth;
    int Height = BitmapHeight;
    int Pitch = Width*BytesPerPixel;
    uint8 *Row = (uint8 *)BitmapMemory;
    for (size_t i = 0; i < BitmapHeight; i++)
    {
        uint8 *Pixel = (uint8 *)Row;
        for (size_t j = 0; j < BitmapWidth; j++)
        {
            *Pixel = (uint8)i;
            ++Pixel;

            *Pixel = (uint8)j;
            ++Pixel;

            *Pixel = 0;
            ++Pixel;

            *Pixel = 0;
            ++Pixel;
        }
        Row += Pitch;
    }
}


internal void
Win32ResizeDIBSection(int Width, int Height)
{

    if (BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE); 
    }

    BitmapWidth = Width;
    BitmapHeight = Height;
    BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biSizeImage = 0;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize = (BitmapWidth * BitmapHeight)*BytesPerPixel;
    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    
    RenderWeirdGradient(0, 0);
    
}

internal void
Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int X, int Y, int Width, int Height)
{

    int WindowWidth = WindowRect->right - WindowRect->left;
    int WindowHeight = WindowRect->bottom - WindowRect->top;

    StretchDIBits(DeviceContext, 
    0, 0, BitmapWidth, BitmapHeight,
    0, 0, WindowWidth, WindowHeight,
    BitmapMemory, 
    &BitmapInfo, 
    DIB_RGB_COLORS, SRCCOPY);
}


LRESULT CALLBACK 
WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    
    switch (message)
    {
    case WM_SIZE:
    {
        RECT ClientRect;
        GetClientRect(hWnd, &ClientRect);
        int Width = ClientRect.right - ClientRect.left;
        int Height = ClientRect.bottom - ClientRect.top;
        Win32ResizeDIBSection(Width, Height);
        OutputDebugStringA("Window sized");

        return (0);
    } break;
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
    

    case WM_QUIT:
    {
        s_isRunning = false;
        PostQuitMessage(0);
    } break;
    
    case WM_PAINT:
    {
        RECT ClientRect;
        GetClientRect(hWnd, &ClientRect);
        PAINTSTRUCT ps;
        HDC handleDevice = BeginPaint(hWnd, &ps);
        int X = ps.rcPaint.left;
        int Y = ps.rcPaint.top;
        int Width = ps.rcPaint.right - ps.rcPaint.left;
        int Height = ps.rcPaint.bottom - ps.rcPaint.top;
        Win32UpdateWindow(handleDevice, &ClientRect, X, Y, Width, Height);
        //PatBlt(handleDevice, X, Y, Width, Height, BLACKNESS);
        EndPaint(hWnd, &ps);
    }
    
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
    WindowClass.lpszMenuName = nullptr;

    if (RegisterClassEx(&WindowClass))
    {
        HWND WindowHandle = CreateWindowEx(0, 
        WindowClass.lpszClassName, "MainWindow", 
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
        200, 200, 1280, 720, 0, 0, hInstance, 0);

        if (WindowHandle)
        {
            s_isRunning = true;

            while (s_isRunning)
            {
                MSG message;
                

                while (PeekMessage(&message, WindowHandle, 0, 0, PM_REMOVE))
                {
                    if (message.message == WM_QUIT)
                    {
                        s_isRunning = false;
                    }
                    
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