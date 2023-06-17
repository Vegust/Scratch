//
// Created by Vegust on 17.06.2023.
//
#include <Windows.h>
#include <Xinput.h>
#include <xaudio2.h>
#include "core_types.h"

#include <bit>

struct FWin32ImageBuffer
{
	BITMAPINFO BitmapInfo{};
	void* BitmapMemory{};
	int BitmapWidth{};
	int BitmapHeight{};
	int BytesPerPixel = 4;
};

struct FWin32AudioData
{
	IXAudio2* AudioInterface;
	IXAudio2MasteringVoice* MasteringVoice;
	WAVEFORMATEX Format;
};

struct FWin32WindowDimension
{
	int Width;
	int Height;
};

static bool bRunning{};
static FWin32ImageBuffer GlobalBackBuffer{};
static FWin32AudioData GlobalAudioData{};

static FWin32WindowDimension Win32GetWindowDimension(HWND Window)
{
	FWin32WindowDimension Result{};
	RECT ClientTRect;
	GetClientRect(Window, &ClientTRect);
	Result.Width = ClientTRect.right - ClientTRect.left;
	Result.Height = ClientTRect.bottom - ClientTRect.top;
	return Result;
}

static void PlayDebugSound(const FWin32AudioData* AudioData)
{
	// WAVEFORMATEXTENSIBLE wfx{};
	// XAUDIO2_BUFFER buffer{};
	if (AudioData->AudioInterface && AudioData->MasteringVoice)
	{
		OutputDebugString("Sound played\n");
	}
}

static void Win32RenderAnimation(
	const FWin32ImageBuffer* Buffer,
	int32 XOffset,
	int32 YOffset)
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
			*Pixel = ((G << 8) | B);
			++Pixel;
		}

		Row += Pitch;
	}
#pragma clang unsafe_buffer_usage end
}

static void Win32InitAudioData(FWin32AudioData* AudioData)
{
	if (XAudio2Create(
		&(AudioData->AudioInterface), 0, XAUDIO2_DEFAULT_PROCESSOR) == S_OK)
	{
		if (AudioData->AudioInterface->CreateMasteringVoice(
			&(AudioData->MasteringVoice)) == S_OK)
		{
			// https://learn.microsoft.com/en-us/windows/win32/api/mmreg/ns-mmreg-waveformatex
			auto& Format = AudioData->Format;
			Format.wFormatTag = WAVE_FORMAT_PCM;
			Format.nChannels = 2;
			Format.nSamplesPerSec = 44100;
			Format.wBitsPerSample = 16;
			Format.nBlockAlign = (Format.nChannels * Format.wBitsPerSample) / 8;
			Format.cbSize = 0;
			OutputDebugStringA("Audio Initialized\n");
		}
	}
}

static void Win32ResizeDIBSection(
	FWin32ImageBuffer* Buffer,
	int Width,
	int Height)
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

	auto BitmapMemorySize =
		static_cast<SIZE_T>(Buffer->BytesPerPixel * Width * Height);
	Buffer->BitmapMemory = VirtualAlloc(
		nullptr, BitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

enum class EKeyAction : uint8
{
	Up,
	Down,
	DownRepeated
};

enum EModifierKeyFlags : uint8
{
	Alt = 0x01
};

static bool HandleInput(
	uint64 VKCode,
	EKeyAction Action,
	EModifierKeyFlags Modifiers)
{
	bool bHandled = true;
	if (Action == EKeyAction::Down)
	{
		switch (VKCode)
		{
			case 'W':
			{
				OutputDebugStringA("Pressed W\n");
				break;
			}
			case 'A':
			{
				OutputDebugStringA("Pressed A\n");
				break;
			}
			case 'S':
			{
				OutputDebugStringA("Pressed S\n");
				break;
			}
			case 'D':
			{
				OutputDebugStringA("Pressed D\n");
				break;
			}
			case VK_ESCAPE:
			{
				OutputDebugStringA("Pressed Escape\n");
				break;
			}
			case VK_SPACE:
			{
				OutputDebugStringA("Pressed Space\n");
				PlayDebugSound(&GlobalAudioData);
				break;
			}
			case VK_F4:
			{
				if (!!(Modifiers & EModifierKeyFlags::Alt))
				{
					// bRunning = false;
				}
				bHandled = false;
				break;
			}
			default:
			{
				bHandled = false;
				break;
			}
		}
	}

	return bHandled;
}

static void Win32DrawBufferToWindow(
	const FWin32ImageBuffer* Buffer,
	HDC DeviceContext,
	int Width,
	int Height)
{
	StretchDIBits(
		DeviceContext,
		0,
		0,
		Width,
		Height,
		0,
		0,
		Buffer->BitmapWidth,
		Buffer->BitmapHeight,
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
		//		case WM_SIZE:
		//		{
		//			FWin32WindowDimension WDim =
		// Win32GetWindowDimension(Window);
		//			Win32ResizeDIBSection(&GlobalBackBuffer,
		// WDim.Width,WDim.Height); 			break;
		//		}
		case WM_DESTROY:
		case WM_CLOSE:
		{
			bRunning = false;
			break;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			FWin32WindowDimension Dimension = Win32GetWindowDimension(Window);
			Win32DrawBufferToWindow(
				&GlobalBackBuffer,
				DeviceContext,
				Dimension.Width,
				Dimension.Height);
			EndPaint(Window, &Paint);
			break;
		}
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			const uint64 VKCode = WParam;
			const bool bDown = (LParam & (1 << 31)) == 0;
			const bool bWasDown = (LParam & (1 << 30)) != 0;
			EKeyAction Action = !bDown ? EKeyAction::Up
									   : (bWasDown ? EKeyAction::DownRepeated
												   : EKeyAction::Down);
			EModifierKeyFlags Modifiers{};
			Modifiers = static_cast<EModifierKeyFlags>(
				Modifiers | ((LParam & (1 << 29)) != 0));
			if (!HandleInput(VKCode, Action, Modifiers))
			{
				// Fallback to Windows if we didn't handle input (Alt + F4 case)
				Result = DefWindowProcA(Window, Message, WParam, LParam);
			}
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
			Win32InitAudioData(&GlobalAudioData);
			Win32ResizeDIBSection(&GlobalBackBuffer, 1200, 720);

			int32 XOffset = 0;
			int32 YOffset = 0;
			bRunning = true;
			while (bRunning)
			{
				MSG Message;
				while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
				{
					if (Message.message == WM_QUIT)
					{
						bRunning = false;
					}

					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}

				Win32RenderAnimation(&GlobalBackBuffer, XOffset, YOffset);
				HDC DeviceContext = GetDC(WindowHandle);
				FWin32WindowDimension Dimension =
					Win32GetWindowDimension(WindowHandle);
				Win32DrawBufferToWindow(
					&GlobalBackBuffer,
					DeviceContext,
					Dimension.Width,
					Dimension.Height);

				++XOffset;
				++YOffset;
			}
		}
	}

	return (0);
}
