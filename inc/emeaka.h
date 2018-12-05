#pragma once
#include <cinttypes>
//helper defines

#define internal static
#define local_persist static
#define global_variable static

//Default values

#define PI32 3.14159265359f

//Macros

#define ArrayCount(array) (sizeof(array)/sizeof((array)[0]))

#define KiloBytes(x) ((x)*(size_t)1024)
#define MegaBytes(x) (KiloBytes(x) * (size_t)1024)
#define GigaBytes(x) (MegaBytes(x) * (size_t)1024)
#define TeraBytes(x) (GigaBytes(x) * (size_t)1024)


#if EMEAKA_SLOW
#define Assert(expression) if(!(expression)) {*(int *)0 = 0 ;}
#else
#define Assert(expression)
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
  float StartX;
  float StartY;
  float MinX;
  float MinY;
  float MaxX;
  float MaxY;
  float EndX;
  float EndY;
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
};

struct GameInputBuffer
{
  //keyboard junk?
  GameControllerInput ControllerInput[4];
};

struct GameState
{
  int ToneHz;
  int GreenOffset;
  int BlueOffset;
};

struct GameMemory
{
  bool IsInitialized;
  size_t PermanentStorageSize;
  void* PermanentStorage; //required to be zero at startup.
  size_t TransientStorageSize;
  void* TransientStorage;
};



struct GameClocks
{
  float SecondsElapsed;
};
//Services platform layer provides to game
//who knows..
#if EMEAKA_INTERNAL
struct DebugFileResult
{
  size_t FileSize;
  void* Contents;
};
internal DebugFileResult PlatformReadEntireFile(char *filename);
internal void PlatformFreeFileMemory(void *memory);
internal bool PlatformWriteEntireFile(char *filename, size_t memorySize, void *memory);
#endif
//Game provides to platform layer
//input, bitmap to output and sound output, timing
internal void
GameUpdateAndRender(GameMemory *gameMemory, GameOffscreenBuffer *offscreenBuffer, GameSoundBuffer *soundBuffer, GameInputBuffer *inputBuffer, GameClocks *gameClocks);
