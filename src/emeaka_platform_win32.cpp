#include <Windows.h>
#include <Xinput.h>
#include <dsound.h>

#include <cinttypes>

#include "emeaka.h"
#include "emeaka_platform.h"

#define RGB_TO_UINT32(r, g, b) (((uint8_t)(r) << 16) | ((uint8_t)(g) << 8) | (uint8_t)(b))

global_variable bool Running;

struct Win32OffscreenBuffer
{
	BITMAPINFO Info;
	void *Memory;
	int Height;
	int Width;
	int BytesPerPixel;
	int Pitch;
};

Win32OffscreenBuffer OffscreenBuffer;

struct Win32WindowDimensions
{
	int Width;
	int Height;
};

//Stubbing out XInput to allow for dynamic loading

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_GET_STATE(XInputGetStateType);
typedef X_INPUT_SET_STATE(XInputSetStateType);

X_INPUT_GET_STATE(XInputGetStateStub)
{
	return ERROR_DEVICE_NOT_CONNECTED;
}

X_INPUT_SET_STATE(XInputSetStateStub)
{
	return ERROR_DEVICE_NOT_CONNECTED;
}

global_variable XInputGetStateType *XInputGetState_ = XInputGetStateStub;
global_variable XInputSetStateType *XInputSetState_ = XInputSetStateStub;

#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_

internal void Win32LoadXInput(void)
{
	HMODULE xInputLib = LoadLibraryA("xinput1_4.dll");
	if (!xInputLib)
	{
		xInputLib = LoadLibraryA("xinput1_3.dll");
	}
	if (xInputLib)
	{
		XInputGetState = (XInputGetStateType *)GetProcAddress(xInputLib, "XInputGetState");
		if (!XInputGetState)
		{
			XInputGetState = XInputGetStateStub;
		}

		XInputSetState = (XInputSetStateType *)GetProcAddress(xInputLib, "XInputSetState");
		if (!XInputSetState)
		{
			XInputSetState = XInputSetStateStub;
		}
	}
	else
	{
		OutputDebugStringA("Failed to Load XInput 1.3\n");
	}
}

//End of Xinput Junk

//direct sound

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(DirectSoundCreateType);
DIRECT_SOUND_CREATE(DirectSoundCreateStub)
{
	return -1;
}
global_variable DirectSoundCreateType *DirectSoundCreate_ = DirectSoundCreateStub;
#define DirectSoundCreate DirectSoundCreate_

internal void Win32InitDSound(HWND window, int32_t samplesPerSecond, int32_t bufferSize)
{
	//Load
	HMODULE dSoundLib = LoadLibraryA("dsound.dll");
	if (dSoundLib)
	{
		DirectSoundCreate = (DirectSoundCreateType *)GetProcAddress(dSoundLib, "DirectSoundCreate");
		LPDIRECTSOUND directSound;
		if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &directSound, 0)))
		{
			WAVEFORMATEX waveFormat = {};
			waveFormat.wFormatTag = WAVE_FORMAT_PCM;
			waveFormat.nChannels = 2;
			waveFormat.wBitsPerSample = 16;
			waveFormat.nSamplesPerSec = samplesPerSecond;
			waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / 8;
			waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign * waveFormat.nSamplesPerSec;
			
			if (SUCCEEDED(directSound->SetCooperativeLevel(window, DSSCL_PRIORITY)))
			{
				LPDIRECTSOUNDBUFFER primaryBuffer;
				DSBUFFERDESC bufferDescription = {};
				bufferDescription.dwSize = sizeof(DSBUFFERDESC);
				bufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

				if (SUCCEEDED(directSound->CreateSoundBuffer(&bufferDescription, &primaryBuffer, 0)))
				{
					HRESULT error = primaryBuffer->SetFormat(&waveFormat);
					if (SUCCEEDED(error))
					{
						OutputDebugStringA("Primary Buffer Created\n");
					}

					else
					{
						//todo:logging
					}
				}
			}
			else
			{
				//todo:logging
			}

			LPDIRECTSOUNDBUFFER secondaryBuffer;
			DSBUFFERDESC bufferDescription = {};
			bufferDescription.dwSize = sizeof(DSBUFFERDESC);
			bufferDescription.dwBufferBytes = bufferSize;
			bufferDescription.lpwfxFormat = &waveFormat;

			HRESULT error = directSound->CreateSoundBuffer(&bufferDescription, &secondaryBuffer, 0);
			if (SUCCEEDED(error))
			{
				OutputDebugStringA("Secondary Buffer Created\n");
			}
			else
			{
				//todo:diagnostics/logs
			}
		}
		//Get a direct sound object

		//create a primary buffer
	}
}
//end dsound

internal Win32WindowDimensions GetWin32WindowDimensions(HWND window)
{
	RECT clientRect;
	Win32WindowDimensions windowDimensions;
	GetClientRect(window, &clientRect);
	windowDimensions.Width = clientRect.right - clientRect.left;
	windowDimensions.Height = clientRect.bottom - clientRect.top;
	return windowDimensions;
}

internal void RenderWeirdGradient(Win32OffscreenBuffer *Buffer, int xOffset, int yOffset)
{
	uint8_t *row = (uint8_t *)Buffer->Memory;
	for (int y = 0; y < Buffer->Height; ++y)
	{
		uint32_t *pixel = (uint32_t *)row;
		for (int x = 0; x < Buffer->Width; ++x)
		{
			*pixel++ = RGB_TO_UINT32(0, y + yOffset, x + xOffset);
		}
		row += Buffer->Pitch;
	}
}

internal void Win32ResizeDIBSection(Win32OffscreenBuffer *Buffer, int width, int height) //Device Independent Bitmap
{
	if (Buffer->Memory)
	{
		VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
	}

	Buffer->Width = width;
	Buffer->Height = height;
	Buffer->BytesPerPixel = 4;
	Buffer->Pitch = Buffer->Width * Buffer->BytesPerPixel;
	Buffer->Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;

	int bitmapMemorySize = Buffer->BytesPerPixel * Buffer->Width * Buffer->Height;
	Buffer->Memory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
	ZeroMemory(Buffer->Memory, bitmapMemorySize);
}

internal void Win32DisplayBufferInWindow(HDC deviceContext, int windowWidth, int windowHeight, Win32OffscreenBuffer *Buffer, int x, int y, int width, int height)
{
	//bufferRatio = buffer->Width / Buffer->Height;

	StretchDIBits(
			deviceContext,
			//x, y, width, height,
			//x, y, width, height,
			0, 0, windowWidth, windowHeight,
			0, 0, Buffer->Width, Buffer->Height,

			Buffer->Memory,
			&Buffer->Info,
			DIB_RGB_COLORS,
			SRCCOPY);
}

LRESULT CALLBACK Win32MainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		uint32_t VKCode = wParam;
		bool wasDown = ((lParam & (1 << 30)) != 0);
		bool isDown = ((lParam & (1 << 31)) == 0);
		if (isDown != wasDown)
		{

			if (VKCode == VK_UP)
			{
			}
			else if (VKCode == VK_DOWN)
			{
			}
			else if (VKCode == VK_LEFT)
			{
			}
			else if (VKCode == VK_RIGHT)
			{
			}
			else if (VKCode == VK_ESCAPE)
			{
				OutputDebugStringA("Escape: ");
				if (isDown)
				{
					OutputDebugStringA("IsDown ");
				}
				if (wasDown)
				{
					OutputDebugStringA("WasDown");
				}
				OutputDebugStringA("\n");
			}
			else if (VKCode == VK_SPACE)
			{
			}
			else if (VKCode == 'W')
			{
			}
			else if (VKCode == 'A')
			{
			}
			else if (VKCode == 'S')
			{
			}
			else if (VKCode == 'D')
			{
			}
			else if (VKCode == 'Q')
			{
				Running = false;
			}
			else if (VKCode == 'E')
			{
			}
			else if (VKCode == VK_F4 && ((lParam & (1 << 29)) != 0))
			{
				Running = false;
			}
		}
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT Paint;
		HDC deviceContext = BeginPaint(window, &Paint);

		int x = Paint.rcPaint.left;
		int y = Paint.rcPaint.top;
		int width = Paint.rcPaint.right - Paint.rcPaint.left;
		int height = Paint.rcPaint.bottom - Paint.rcPaint.top;

		Win32WindowDimensions windowDimensions = GetWin32WindowDimensions(window);
		Win32DisplayBufferInWindow(deviceContext, windowDimensions.Width, windowDimensions.Height, &OffscreenBuffer, x, y, width, height);
		EndPaint(window, &Paint);
	}
	break;

	case WM_SIZE:
	case WM_ACTIVATEAPP:
		break;

	case WM_CLOSE:	 //message
	case WM_DESTROY: //error recreate
		Running = false;
		break;

	default:
		return DefWindowProc(window, message, wParam, lParam);
	}

	return 0;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
	Win32LoadXInput();

	WNDCLASSEXA windowClass = {};

	Win32ResizeDIBSection(&OffscreenBuffer, 1280, 720);

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = Win32MainWindowCallback;
	windowClass.hInstance = instance;
	windowClass.hIcon = LoadIcon(instance, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszClassName = "emeakaWindowClass";
	windowClass.hIconSm = LoadIcon(windowClass.hInstance, IDI_APPLICATION);

	if (RegisterClassExA(&windowClass))
	{
		HWND window = CreateWindowExA(
				0,
				windowClass.lpszClassName,
				"Eme Aka",
				WS_OVERLAPPEDWINDOW | WS_VISIBLE,
				CW_USEDEFAULT, CW_USEDEFAULT,
				1280, 720,
				NULL,
				NULL,
				instance,
				NULL);

		if (window)
		{
			ShowWindow(window, showCode);
			UpdateWindow(window);

			Win32InitDSound(window, 48000, 48000 * sizeof(int16_t) * 2);

			MSG message;
			Running = true;
			int xOffset = 0;
			int yOffset = 0;

			while (Running)
			{
				while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
				{
					if (message.message == WM_QUIT)
					{
						Running = false;
					}
					TranslateMessage(&message);
					DispatchMessage(&message);
				}

				//game input, do we need more frequent polls

				for (int controllerIndex = 0; controllerIndex < XUSER_MAX_COUNT; ++controllerIndex)
				{
					XINPUT_STATE state;
					ZeroMemory(&state, sizeof(XINPUT_STATE));
					if (XInputGetState(controllerIndex, &state) == ERROR_SUCCESS)
					{
						XINPUT_GAMEPAD *pad = &state.Gamepad;
						bool dPadUp = (pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
						bool dPadDown = (pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
						bool dPadLeft = (pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
						bool dPadRight = (pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
						bool Start = (pad->wButtons & XINPUT_GAMEPAD_START);
						bool Back = (pad->wButtons & XINPUT_GAMEPAD_BACK);
						bool LeftShoulder = (pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
						bool RightShoulder = (pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
						bool AButton = (pad->wButtons & XINPUT_GAMEPAD_A);
						bool BButton = (pad->wButtons & XINPUT_GAMEPAD_B);
						bool XButton = (pad->wButtons & XINPUT_GAMEPAD_X);
						bool YButton = (pad->wButtons & XINPUT_GAMEPAD_Y);
						int16_t LeftStickX = pad->sThumbLX;
						int16_t LeftStickY = pad->sThumbLY;
						int16_t RightStickX = pad->sThumbRX;
						int16_t RightStickY = pad->sThumbRY;
						uint8_t RightTrigger = pad->bRightTrigger;
						uint8_t LeftTrigger = pad->bLeftTrigger;
						xOffset = LeftTrigger;
						yOffset = RightTrigger;
					}
				}

				RenderWeirdGradient(&OffscreenBuffer, xOffset, yOffset);
				HDC deviceContext = GetDC(window);

				Win32WindowDimensions windowDimensions = GetWin32WindowDimensions(window);
				Win32DisplayBufferInWindow(deviceContext, windowDimensions.Width, windowDimensions.Height, &OffscreenBuffer, 0, 0, windowDimensions.Width, windowDimensions.Height);

				ReleaseDC(window, deviceContext);
			}
		}
	}

	return 0; //useless
}