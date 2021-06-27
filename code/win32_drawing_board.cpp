/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Alexander Baskakov $
   ======================================================================== */

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <algorithm>

//#include "../..//win32_code_templates/win32codetemplates.h"
#include "../../win32_code_templates/win32codetemplates.cpp"


#define global_variable static;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef i32 bool32;

typedef uint8_t ui8;
typedef uint16_t ui16;
typedef uint32_t ui32;
typedef uint64_t ui64;

typedef float real32;
typedef double real64;

#define BYTES_PER_PIXEL 4

global_variable bool GlobalRunning = false;

global_variable ui32 WindowDefaultPosX = 100;
global_variable ui32 WindowDefaultPosY = 100;
global_variable ui32 WindowRectWidth = 1000;
global_variable ui32 WindowRectHeight = 700;

global_variable ui32 ClientWindowRectWidth = 0;
global_variable ui32 ClientWindowRectHeight = 0;

global_variable ui32 CursorPosX = 0;
global_variable ui32 CursorPosY = 0;
global_variable bool LeftMouseButtonPressedStatus = false;
global_variable bool LeftMouseButtonReleasedStatus = false;
global_variable bool RightMouseButtonPressedStatus = false;
global_variable bool RightMouseButtonReleasedStatus = false;


global_variable HWND WindowHandle = { };
global_variable void* BackbufferBitmapMemory = 0;
global_variable HDC DeviceContextHandle = 0;
global_variable BITMAPINFO BitmapInfo = { };

//////////////////////////////////////////

//////////////////////////////////////////

//TODO: Use DirectInput to get rid of windows message input lag
//TODO: Handle drawing save to bmp format

void DrawCircleToBackffer(ui32 X, ui32 Y, i32 TargetRadius, Pixel32RGB* Pixel)
{
    real32 Theta = 0.0;
    real32 ThetaStep = 1;
    real32 CircleX = X;
    real32 CircleY = Y;

    real32 DestX = 0;
    real32 DestY = 0;
    
    for(i32 Radius = TargetRadius; Radius >= 0; Radius--)
    {
        Theta = 0.0;
        while(Theta <= 360)
        {
            DestX = CircleX + Radius * cos(Theta);
            DestY = CircleY + Radius * sin(Theta);
        
            Win32_DrawPixelToBitmap(BackbufferBitmapMemory, ClientWindowRectWidth,
                                    DestX, DestY, Pixel);
            Theta += ThetaStep;
        }
    }
}

void ClearCircleInBackbuffer(ui32 X, ui32 Y, i32 TargetRadius)
{
    real32 Theta = 0.0;
    real32 ThetaStep = 0.1;
    real32 CircleX = X;
    real32 CircleY = Y;

    real32 DestX = 0;
    real32 DestY = 0;

    Pixel32RGB Pixel = { };
    for(i32 Radius = TargetRadius; Radius >= 0; Radius--)
    {
        Theta = 0.0;
        while(Theta <= 360)
        {
            DestX = CircleX + Radius * cos(Theta);
            DestY = CircleY + Radius * sin(Theta);
        
            Win32_DrawPixelToBitmap(BackbufferBitmapMemory, ClientWindowRectWidth,
                                    DestX, DestY, &Pixel);
            Theta += ThetaStep;
        }
    }
}

LRESULT CALLBACK
WindowMessageHandlerProcedure(HWND WindowHandle, UINT Message,
           WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = { };

    switch(Message)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT PaintStruct;
            BeginPaint(WindowHandle, &PaintStruct);

            

            EndPaint(WindowHandle, &PaintStruct);
            OutputDebugStringA("WM_PAINT\n");
        }break;

        case WM_MOUSEMOVE:
        {
            //OutputDebugStringA("WM_MOUSEMOVE\n");
            CursorPosX = LOWORD(lParam);
            CursorPosY = HIWORD(lParam);
            // Buffer[256];
            //_snprintf_s(Buffer, sizeof(Buffer), "posX:%d, posY:%d", CursorPosX, CursorPosY);
            // OutputDebugStringA(Buffer);
        }break;

        case WM_LBUTTONDOWN:
        {
            LeftMouseButtonReleasedStatus = false;
            LeftMouseButtonPressedStatus = true;
            
        }break;

        case WM_LBUTTONUP:
        {
            LeftMouseButtonPressedStatus = false;
            LeftMouseButtonReleasedStatus = true;
        }break;


        case WM_RBUTTONDOWN:
        {
            RightMouseButtonReleasedStatus = false;
            RightMouseButtonPressedStatus = true;
        }break;

        case WM_RBUTTONUP:
        {
            RightMouseButtonPressedStatus = false;
            RightMouseButtonReleasedStatus = true;
        }break;
        
        
        case WM_SIZE:
        {
            RECT ClientRect = { };
            GetClientRect(WindowHandle, &ClientRect);
            ui32 ClientWidth = ClientRect.right - ClientRect.left;
            ui32 ClientHeight = ClientRect.bottom - ClientRect.top;

            ClientWindowRectWidth = ClientWidth;
            ClientWindowRectHeight = ClientHeight;
            
            BackbufferBitmapMemory = Win32_GetBitmapMemory(&BitmapInfo, BackbufferBitmapMemory,
                                                           BYTES_PER_PIXEL, ClientWindowRectWidth, ClientWindowRectHeight);

            OutputDebugStringA("WM_SIZE\n");
        }break;

        case WM_CLOSE:
        {
            GlobalRunning = false;
        }break;
        
        default:
        {
            Result = DefWindowProc(WindowHandle, Message, wParam, lParam);
        }break;
    }

    return Result;
}

int CALLBACK
WinMain(HINSTANCE WindowInstance, HINSTANCE PrevWindowInstance,
        LPSTR CommandLine, int LineArgs)
{   
    WNDCLASSA WindowClass = { };
    WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    WindowClass.lpfnWndProc = WindowMessageHandlerProcedure;
    WindowClass.hInstance = WindowInstance;
    WindowClass.hCursor = 0; //Check cursor
    WindowClass.lpszClassName = "DrawingBoardClassName";

    RegisterClass(&WindowClass);
    
    
    WindowHandle = CreateWindowEx(0,
                                  WindowClass.lpszClassName,
                                  "Win32DrawingBoard",
                                  WS_OVERLAPPEDWINDOW,
                                  WindowDefaultPosX, WindowDefaultPosY,
                                  WindowRectWidth, WindowRectHeight,
                                  NULL,
                                  NULL, //Menu
                                  WindowInstance,
                                  NULL);

    if(WindowHandle)
    {
        ShowWindow(WindowHandle, LineArgs);
        GlobalRunning = true;

        DeviceContextHandle = GetDC(WindowHandle);

        Pixel32RGB Pixel = { };
        Pixel.Red = 255;

        Pixel32RGB PixelBitmap = {0, 0, 0};

        Win32_ClearBitmapToColor(BackbufferBitmapMemory,
                                 ClientWindowRectWidth, ClientWindowRectHeight,
                                 &PixelBitmap);
        
        while(GlobalRunning)
        {
            MSG Message = { };
            while(PeekMessage(&Message, WindowHandle, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }

            
            if(LeftMouseButtonPressedStatus == true)
            {
                
                DrawCircleToBackffer(CursorPosX, CursorPosY, 10, &Pixel);
            }

            if(RightMouseButtonPressedStatus == true)
            {
                ClearCircleInBackbuffer(CursorPosX, CursorPosY, 20);
            }
            
            Win32_DrawDIBSectionToScreen(&DeviceContextHandle,
                                         0, 0, ClientWindowRectWidth, ClientWindowRectHeight,
                                         0, 0, ClientWindowRectWidth, ClientWindowRectHeight,
                                         BackbufferBitmapMemory, &BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
            
        }
    }

    Win32_ReleaseBitmapMemory(BackbufferBitmapMemory);
    
    return 0;
}
