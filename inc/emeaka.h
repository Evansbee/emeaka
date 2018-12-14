#pragma once
#include <cinttypes>
//helper defines

#define internal static
#define local_persist static
#define global_variable static

//Default values

#define PI32 3.14159265359f

//Macros
#define RGB_TO_UINT32(r, g, b) (((uint8_t)(r) << 16) | ((uint8_t)(g) << 8) | (uint8_t)(b))
#define Y_TO_UINT32(y) (((uint8_t)(y) << 16) | ((uint8_t)(y) << 8) | (uint8_t)(y))
#define ArrayCount(array) (sizeof(array)/sizeof((array)[0]))

#define KiloBytes(x) ((x)*(size_t)1024)
#define MegaBytes(x) (KiloBytes(x) * (size_t)1024)
#define GigaBytes(x) (MegaBytes(x) * (size_t)1024)
#define TeraBytes(x) (GigaBytes(x) * (size_t)1024)
#define FLOAT_EPSILON 1.19209290E-07F

#if EMEAKA_SLOW
#define Assert(expression, msg) if(!(expression)) {*(int *)0 = 0;}
#else
#define Assert(expression, msg)
#endif



//types
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
  GameButtonState ShiftButton;
  GameButtonState AltButton;
  GameButtonState EscapeButton;
  GameButtonState UpArrow;
  GameButtonState DownArrow;
  GameButtonState RightArrow;
  GameButtonState LeftArrow;
  GameButtonState CtrlButton;
  GameButtonState Key[255];
  GameButtonState FKey[12];
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

struct GameState
{
  float PlayerX;
  float PlayerY;
};




struct GameClocks
{
  float UpdateDT;
};

struct GameControl 
{
  bool Running;
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


struct GamePlatformFunctions 
{
  PlatformReadEntireFileType *PlatformReadEntireFile;
  PlatformFreeFileMemoryType *PlatformFreeFileMemory;
  PlatformWriteEntireFileType *PlatformWriteEntireFile;
};

struct GameMemory
{
  GamePlatformFunctions PlatformFunctions;
  bool IsInitialized;
  size_t PermanentStorageSize;
  void* PermanentStorage; //required to be zero at startup.
  size_t TransientStorageSize;
  void* TransientStorage;
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


