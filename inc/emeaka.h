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
  void* PermanentStorage;
};
//Services platform layer provides to game
//who knows..

//Game provides to platform layer
//input, bitmap to output and sound output, timing
internal void
GameUpdateAndRender(GameMemory *gameMemory, GameOffscreenBuffer *offscreenBuffer, GameSoundBuffer *soundBuffer, GameInputBuffer &inputBuffer);
