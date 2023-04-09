#include <Windows.h>
#include <cstdint>

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

struct FWin32ImageBuffer
{
	BITMAPINFO BitmapInfo{};
	void* BitmapMemory{};
	int BitmapWidth{};
	int BitmapHeight{};
	int BytesPerPixel = 4;
};

static bool bRunning{};
static FWin32ImageBuffer GlobalBackBuffer{};

static void RenderAnimation(const FWin32ImageBuffer* Buffer, int32 XOffset, int32 YOffset)
{
#pragma clang unsafe_buffer_usage begin
	int32 Width = Buffer->BitmapWidth;
	int32 Pitch = Width * Buffer->BytesPerPixel;
	auto Row = reinterpret_cast<uint8*>(Buffer->BitmapMemory);
	for (int Y = 0; Y < Buffer->BitmapHeight; ++Y)
	{
		auto Pixel = reinterpret_cast<uint32*>(Row);
		for (int X = 0; X < Buffer->BitmapWidth; ++X)
		{
			/*
			 * Pixel in memory:	BB	GG	RR	aa
			 */
			auto B = static_cast<uint32>(static_cast<uint8>(X + XOffset));
			auto G = static_cast<uint32>(static_cast<uint8>(Y + YOffset));
			*Pixel = ((G << 8)|B);
			++Pixel;
		}
		
		Row += Pitch;
	}
#pragma clang unsafe_buffer_usage end
}

static void Win32ResizeDIBSection(FWin32ImageBuffer* Buffer, int Width, int Height)
{
	if (Buffer->BitmapMemory)
	{
		VirtualFree(Buffer->BitmapMemory, 0, MEM_RELEASE);
	}

	Buffer->BitmapWidth = Width;
	Buffer->BitmapHeight = Height;

	Buffer->BitmapInfo.bmiHeader.biSize = sizeof(Buffer->BitmapInfo.bmiHeader);
	Buffer->BitmapInfo.bmiHeader.biWidth = Buffer->BitmapWidth;
	Buffer->BitmapInfo.bmiHeader.biHeight = -Buffer->BitmapHeight;
	Buffer->BitmapInfo.bmiHeader.biPlanes = 1;
	Buffer->BitmapInfo.bmiHeader.biBitCount = 32;
	Buffer->BitmapInfo.bmiHeader.biCompression = BI_RGB;

	auto BitmapMemorySize = static_cast<SIZE_T>(Buffer->BytesPerPixel * Width * Height);
	Buffer->BitmapMemory =
		VirtualAlloc(nullptr, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

static void Win32DrawBufferToWindow(const FWin32ImageBuffer* Buffer, HDC DeviceContext, RECT WindowRect)
{
	int WindowWidth = WindowRect.right - WindowRect.left;
	int WindowHeight = WindowRect.bottom - WindowRect.top;

	StretchDIBits(
		DeviceContext,
		//		X,
		//		Y,
		//		Width,
		//		Height,
		//		X,
		//		Y,
		//		Width,
		//		Height,
		0,
		0,
		Buffer->BitmapWidth,
		Buffer->BitmapHeight,
		0,
		0,
		WindowWidth,
		WindowHeight,
		Buffer->BitmapMemory,
		&Buffer->BitmapInfo,
		DIB_RGB_COLORS,
		SRCCOPY);
}

static LRESULT CALLBACK
Win32Wndproc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
	LRESULT Result = 0;

	switch (Message)
	{
		case WM_SIZE:
		{
			RECT ClientRect;
			GetClientRect(Window, &ClientRect);
			LONG Height = ClientRect.bottom - ClientRect.top;
			LONG Width = ClientRect.right - ClientRect.left;
			Win32ResizeDIBSection(&GlobalBackBuffer, Width, Height);
			break;
		}
		case WM_DESTROY:
		{
			bRunning = false;
			break;
		}
		case WM_CLOSE:
		{
			bRunning = false;
			break;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			//			LONG Y = Paint.rcPaint.top;
			//			LONG X = Paint.rcPaint.left;
			//			LONG Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			//			LONG Width = Paint.rcPaint.right - Paint.rcPaint.left;
			Win32DrawBufferToWindow(&GlobalBackBuffer, DeviceContext, Paint.rcPaint);
			EndPaint(Window, &Paint);
			break;
		}
		default:
		{
			Result = DefWindowProcA(Window, Message, WParam, LParam);
			break;
		}
	}

	return Result;
}

int WIN32 WinMain(HINSTANCE Instance, HINSTANCE, LPSTR, int)
{
	WNDCLASS Window{};
	Window.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	Window.lpfnWndProc = Win32Wndproc;
	Window.hInstance = Instance;
	Window.lpszClassName = "ScratchWindowClass";

	if (RegisterClass(&Window))
	{
		HWND WindowHandle = CreateWindowExA(
			0,
			Window.lpszClassName,
			"Scratch Window",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			nullptr,
			nullptr,
			Instance,
			nullptr);
		if (WindowHandle)
		{
			int32 XOffset = 0;
			int32 YOffset = 0;
			bRunning = true;
			while (bRunning)
			{
				MSG Message;
				while(PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
				{
					if (Message.message == WM_QUIT)
					{
						bRunning = false;
					}
					
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}

				RenderAnimation(&GlobalBackBuffer, XOffset,YOffset);
				HDC DeviceContext = GetDC(WindowHandle);
				RECT ClientRect;
				GetClientRect(WindowHandle, &ClientRect);
				Win32DrawBufferToWindow(&GlobalBackBuffer, DeviceContext, ClientRect);
				
				++XOffset;
				++YOffset;
			}
		}
	}

	return (0);
}
