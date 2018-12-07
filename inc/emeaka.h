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
#define ArrayCount(array) (sizeof(array)/sizeof((array)[0]))

#define KiloBytes(x) ((x)*(size_t)1024)
#define MegaBytes(x) (KiloBytes(x) * (size_t)1024)
#define GigaBytes(x) (MegaBytes(x) * (size_t)1024)
#define TeraBytes(x) (GigaBytes(x) * (size_t)1024)


#if EMEAKA_SLOW
#define Assert(expression, msg) if(!(expression)) {PlatformAssertFail(msg);}
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

  GameButtonState LeftButton;
  GameButtonState RightButton;
  GameButtonState MiddleButton;
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
  float ToneHz;
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
internal void PlatformAssertFail(char *failureMsg);
internal DebugFileResult PlatformReadEntireFile(char *filename);
internal void PlatformFreeFileMemory(void *memory);
internal bool PlatformWriteEntireFile(char *filename, size_t memorySize, void *memory);
#endif
//Game provides to platform layer
//input, bitmap to output and sound output, timing
internal void
GameUpdateAndRender(GameMemory *gameMemory, GameOffscreenBuffer *offscreenBuffer, GameInputBuffer *inputBuffer, GameClocks *gameClocks);

internal void
GameGetSoundSamples(GameMemory *gameMemory,  GameSoundBuffer *soundBuffer);
