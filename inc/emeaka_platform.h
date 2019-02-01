#pragma once
#include <cstdint>

//types
enum KeyCode : size_t 
{
  A = 0,
  B, C, D, E, F, G, H, I, J, K, L, M, N,
  O, P, Q, R, S, T, U, V, W, X, Y, Z,
  K1, K2, K3, K4, K5, K6, K7, K8, K9, K0,
  F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
  N1, N2, N3, N4, N5, N6, N7, N8, N9, N0,
  RETURN, ESC, BACKSPACE, TAB, SPACE,
  LEFTBRACKET, RIGHTBRACKET, SEMICOLON, APOSTROPHE, COMMA,
  PERIOD, SLASH, BACKSLASH, KMINUS, KEQUAL, GRAVE, 
  NPLUS, NMINUS, NENTER, NEQUAL, NDIVIDE, NMULTIPLY, NPERIOD,
  LCTRL, RCTRL, LALT, RALT, LSHIFT, RSHIFT, LGUI, RGUI, 
  PGDOWN, PGUP, HOME, END, DELETE,
  UP, DOWN, LEFT, RIGHT, PAUSE, INSERT, 
  KEY_TABLE_SIZE
};

struct GameOffscreenBuffer
{
  void *Memory;
  int Height;
  int Width;
  int BytesPerPixel;
  int Pitch;
};

struct GameSoundBuffer
{
  int16_t *SampleBuffer;
  size_t SampleCount;
  int32_t SamplesPerSecond;
};

struct GameButtonState
{
  uint32_t HalfTransitions;
  bool IsDown;
};

struct GameAnalogStickState
{
  float AverageX;
  float AverageY;
};

struct GameControllerInput
{
  bool Connected;

  union {
    GameButtonState Buttons[12];
    struct
    {
      GameButtonState UpButton;
      GameButtonState DownButton;
      GameButtonState LeftButton;
      GameButtonState RightButton;
      GameButtonState StartButton;
      GameButtonState BackButton;
      GameButtonState LeftThumbButton;
      GameButtonState RightThumbButton;
      GameButtonState LeftShoulder;
      GameButtonState RightShoulder;
      GameButtonState AButton;
      GameButtonState BButton;
      GameButtonState XButton;
      GameButtonState YButton;
    };
  };
  GameAnalogStickState LeftStick;
  GameAnalogStickState RightStick;

  float RightTrigger;
  float LeftTrigger;

  uint16_t LeftVibration;
  uint16_t RightVibration;
};

struct GameKeyboardInput
{
  GameButtonState Key[KeyCode::KEY_TABLE_SIZE];
  char TextInput[32];
  static const size_t TextInputSize = 32;
};

struct GameMouseInput
{
  bool MouseInWindow;

  float MouseLocationX;
  float MouseLocationY;
  float WheelDelta;

  union{
    GameButtonState MouseButton[5];
    struct
    {
      GameButtonState LeftButton;
      GameButtonState RightButton;
      GameButtonState MiddleButton;
    };
  };  
};

struct GameInputBuffer
{
  //keyboard junk?
  GameMouseInput MouseInput;
  GameKeyboardInput KeyboardInput;
  GameControllerInput ControllerInput[4];
};



//Services platform layer provides to game
//who knows..
#if EMEAKA_INTERNAL
struct DebugFileResult
{
  size_t FileSize;
  void* Contents;
};
#endif

struct ThreadContext
{
  size_t ThreadID;
};


#define PLATFORM_READ_ENTIRE_FILE(name) DebugFileResult name(ThreadContext *threadContext, char *filename)
typedef PLATFORM_READ_ENTIRE_FILE(PlatformReadEntireFileType);

#define PLATFORM_FREE_FILE_MEMORY(name) void name(ThreadContext *threadContext, void *memory)
typedef PLATFORM_FREE_FILE_MEMORY(PlatformFreeFileMemoryType);

#define PLATFORM_WRITE_ENTIRE_FILE(name) bool name(ThreadContext *threadContext, char *filename, size_t memorySize, void *memory)
typedef PLATFORM_WRITE_ENTIRE_FILE(PlatformWriteEntireFileType);


struct PlatformAPI 
{
  PlatformReadEntireFileType *PlatformReadEntireFile;
  PlatformFreeFileMemoryType *PlatformFreeFileMemory;
  PlatformWriteEntireFileType *PlatformWriteEntireFile;
};

struct GameMemory
{
  PlatformAPI PlatformFunctions;
  bool IsInitialized;
  size_t PermanentStorageSize;
  void* PermanentStorage; //required to be zero at startup.
  size_t TransientStorageSize;
  void* TransientStorage;
};


struct GameClocks
{
  float UpdateDT;
};

//Game provides to platform layer
//input, bitmap to output and sound output, timing

#define GAME_UPDATE_AND_RENDER(name) void name(ThreadContext *threadContext, GameMemory *gameMemory, GameOffscreenBuffer *offscreenBuffer, GameInputBuffer *inputBuffer, GameClocks *gameClocks)
typedef GAME_UPDATE_AND_RENDER(GameUpdateAndRenderType);
#define GAME_GET_SOUND_SAMPLES(name) void name(ThreadContext *threadContext, GameMemory *gameMemory,  GameSoundBuffer *soundBuffer)
typedef GAME_GET_SOUND_SAMPLES(GameGetSoundSamplesType);


#define GAME_CLEAR_BITMAP(name) void name(GameOffscreenBuffer *offscreenBuffer, float r, float g, float b)
typedef GAME_CLEAR_BITMAP(GameClearBitmapType);

#define GAME_DRAW_PIXEL(name) void name(GameOffscreenBuffer *offscreenBuffer, float x, float y, float r, float g, float b)
typedef GAME_DRAW_PIXEL(GameDrawPixelType);

#define GAME_DRAW_RECT(name) void name(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, float x1, float y1, float r, float g, float b)
typedef GAME_DRAW_RECT(GameDrawRectType);

#define GAME_DRAW_CIRCLE(name) void name(GameOffscreenBuffer *offscreenBuffer, float cx, float cy, float radius, float r, float g, float b)
typedef GAME_DRAW_CIRCLE(GameDrawCircleType);

#define GAME_DRAW_LINE(name) void name(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, float x1, float y1, float r, float g, float b)
typedef GAME_DRAW_LINE(GameDrawLineType);

#define GAME_DRAW_CHAR(name) void name(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, char c, float r, float g, float b, bool shadow)
typedef GAME_DRAW_CHAR(GameDrawCharType);

#define GAME_DRAW_TEXT(name) void name(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, char* text, float r, float g, float b, bool shadow)
typedef GAME_DRAW_TEXT(GameDrawTextType);

#define GAME_DRAW_TRIANGLE(name) void name(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, float x1, float y1, float x2, float y2, float r, float g, float b)
typedef GAME_DRAW_TRIANGLE(GameDrawTriangleType);

struct GameAPI {
  GameUpdateAndRenderType *UpdateAndRender;
  GameGetSoundSamplesType *GetSoundSamples;
  GameClearBitmapType *ClearBitmap;
  GameDrawPixelType *DrawPixel;
  GameDrawRectType *DrawRect;
  GameDrawCircleType *DrawCircle;
  GameDrawLineType *DrawLine;
  GameDrawCharType *DrawChar;
  GameDrawTextType *DrawText;
  GameDrawTriangleType *DrawTriangle;
};