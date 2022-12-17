#include <Windows.h>
#include <stdint.h>
#include <Xinput.h>
#include <stdio.h>

#define internal static
#define local_presist static
#define global_variable static

struct win32_screen_buffer
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int BytesPerPixel;
};

struct win32_window_dimension
{
    int Width;
    int Height;
};
global_variable bool s_isRunning;
global_variable win32_screen_buffer s_Buffer;

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return (0);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return (0);
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal void
Win32LoadXInput(void)
{
    HMODULE XInputLibrary = LoadLibraryA("xinput1_3.dll");
    if (XInputLibrary)
    {
        XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
    }
}


internal win32_window_dimension
GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    
    return (Result);
}

internal void
RenderWeirdGradient(win32_screen_buffer *Buffer, int XOffset, int YOffset)
{
    int Pitch = (Buffer->Width)*(Buffer->BytesPerPixel);
    uint8_t *Row = (uint8_t *)Buffer->Memory;
    for (size_t i = 0; i < Buffer->Height; i++)
    {
        uint32_t *Pixel = (uint32_t *)Row;
        for (size_t j = 0; j < Buffer->Width; j++)
        {
            uint8_t Blue = (j + XOffset);
            uint8_t Green = (i + YOffset);
            
            *Pixel++ = ((Green << 8) | Blue);
        }
        Row += Pitch;
    }
}


internal void
Win32ResizeDIBSection(win32_screen_buffer *Buffer, int Width, int Height)
{
    if (Buffer->Memory)
    { 
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE); 
    }
    
    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;
    
    Buffer->Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biSizeImage = 0;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;
    
    int BitmapMemorySize = (Buffer->Width * Buffer->Height)*Buffer->BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    
    RenderWeirdGradient(&s_Buffer,100, 0);
}

internal void
Win32UpdateWindow(win32_screen_buffer *Buffer, HDC DeviceContext, int WindowWidth, int WindowHeight, int X, int Y)
{
    StretchDIBits(DeviceContext, 
                  0, 0, Buffer->Width, Buffer->Height,
                  0, 0, WindowWidth, WindowHeight,
                  Buffer->Memory, 
                  &Buffer->Info, 
                  DIB_RGB_COLORS, SRCCOPY);
}


internal LRESULT CALLBACK 
WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_SIZE:
        {
            win32_window_dimension Dimension = GetWindowDimension(hWnd);
            Win32ResizeDIBSection(&s_Buffer, Dimension.Width, Dimension.Height);
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
        
        case WM_SYSKEYDOWN:
        {

        } break;
        case WM_SYSKEYUP:
        {

        } break;
        case WM_KEYDOWN:
        {

        } break;
        case WM_KEYUP:
        {
            uint32_t VKCode = wParam;
            bool WasDown = ((lParam & (1 << 30) != 0));
            bool IsDown = ((lParam & (1 << 31) == 0));

            if (VKCode == 'W')
            {}
            else if (VKCode == 'A')
            {}
            else if (VKCode == 'S')
            {}
            else if (VKCode == 'D')
            {}
            else if (VKCode == 'Q')
            {}
            else if (VKCode == 'E')
            {}
            else if (VKCode == VK_UP)
            {}
            else if (VKCode == VK_DOWN)
            {}
            else if (VKCode == VK_LEFT)
            {}
            else if (VKCode == VK_RIGHT)
            {}
            else if (VKCode == VK_ESCAPE)
            {}
            else if (VKCode == VK_SPACE)
            {}

            //lParam & (1 << 30);
        } break;
        case WM_QUIT:
        {
            s_isRunning = false;
            PostQuitMessage(0);
        } break;
        
        case WM_PAINT:
        {
            win32_window_dimension Dimension = GetWindowDimension(hWnd);
            PAINTSTRUCT ps;
            HDC handleDevice = BeginPaint(hWnd, &ps);
            int X = ps.rcPaint.left;
            int Y = ps.rcPaint.top;
            int Width = ps.rcPaint.right - ps.rcPaint.left;
            int Height = ps.rcPaint.bottom - ps.rcPaint.top;
            Win32UpdateWindow(&s_Buffer, handleDevice, Dimension.Width, Dimension.Height, X, Y);
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
    Win32LoadXInput();

    WNDCLASSEXA WindowClass = {};
    WindowClass.hInstance = hInstance;
    WindowClass.lpfnWndProc = WndProc;
    WindowClass.hCursor = nullptr;
    WindowClass.hIcon = LoadIcon(hInstance, IDI_ERROR);
    WindowClass.hIcon = LoadIcon(hInstance, IDI_ERROR);
    WindowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.hbrBackground = nullptr;
    WindowClass.cbWndExtra = 0;
    WindowClass.lpszClassName = "Hello";
    WindowClass.lpszMenuName = nullptr;
    
    if (RegisterClassExA(&WindowClass))
    {
        HWND WindowHandle = CreateWindowEx(0, WindowClass.lpszClassName, "MainWindow", 
                                           WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
                                           200, 200, 1280, 720, 0, 0, hInstance, 0);
        
        if (WindowHandle)
        {
            HDC DeviceContext = GetDC(WindowHandle);

            int XOffset = 0;
            int YOffset = 0;

            s_isRunning = true;
            
            while (s_isRunning)
            {
                MSG message;
                
                while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
                {
                    if (message.message == WM_QUIT)
                    {
                        s_isRunning = false;
                    }
                    
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }
                for (DWORD ControllerIndex = 0; 
                    ControllerIndex < XUSER_MAX_COUNT; 
                    ++ControllerIndex)
                {
                    XINPUT_STATE ControllerState;

                    // Simply get the state of the controller from XInput.
                    if (XInputGetState( ControllerIndex, &ControllerState ) == ERROR_SUCCESS)
                    {
                        XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;

                        bool Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                        bool Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                        bool Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                        bool Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                        bool LeftShoulder = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                        bool RightShoulder = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                        bool AButton = (Pad->wButtons & XINPUT_GAMEPAD_A);
                        bool BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
                        bool XButton = (Pad->wButtons & XINPUT_GAMEPAD_X);
                        bool YButton = (Pad->wButtons & XINPUT_GAMEPAD_Y);
                        bool Start = (Pad->wButtons & XINPUT_GAMEPAD_START);
                        bool Back = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
                        
                        int16_t StickX = Pad->sThumbLX;
                        int16_t StickY = Pad->sThumbLY;
                    }
                    else
                    {

                    }
                }
                
                RenderWeirdGradient(&s_Buffer, XOffset, YOffset);
                
                win32_window_dimension Dimension = GetWindowDimension(WindowHandle);
                Win32UpdateWindow(&s_Buffer, DeviceContext, Dimension.Width, Dimension.Height, 0, 0);
                ReleaseDC(WindowHandle, DeviceContext);
                
                ++XOffset;
                --YOffset;
            }   
        }
        else
        {
            return (EXIT_FAILURE);
        }
    }
    else
    {
        
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}
