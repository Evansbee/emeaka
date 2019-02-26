#pragma once
#include "emeaka.h"
#include "emeaka_vector.h"

void ClearBitmap(GameOffscreenBuffer *offscreenBuffer, float r, float g, float b);
void DrawPixel(GameOffscreenBuffer *offscreenBuffer, vec2i p, float r, float g, float b, float a = 1.0f);
void DrawHorizontalLine(GameOffscreenBuffer *osb, vec2i p0, vec2i p1, float r, float g, float b, float a = 1.0f);
extern "C" void DrawLine(GameOffscreenBuffer *offscreenBuffer, vec2i p0, vec2i p1, float r, float g, float b, float a = 1.0f);
void StrokeCircle(GameOffscreenBuffer *offscreenBuffer, vec2i p, int radius, float r, float g, float b, float a = 1.0f);
void DrawCircle(GameOffscreenBuffer *offscreenBuffer, vec2i p, int radius, float r, float g, float b, float a = 1.0f);
void DrawChar(GameOffscreenBuffer *offscreenBuffer, vec2i p, char c, float r, float g, float b, float a, bool shadow);
extern "C" void DrawText(GameOffscreenBuffer *offscreenBuffer, vec2i p, const char *text, float r, float g, float b, float a, bool shadow);
void DrawNewChar(GameOffscreenBuffer *osb, vec2i p, const FontInformation* font, char c, float r, float g, float b, float a);
void DrawNewText(GameOffscreenBuffer *osb, vec2i p, const FontInformation* font, const char *text, float r, float g, float b, float a);
void DrawTriangle(GameOffscreenBuffer *offscreenBuffer, vec2i p0, vec2i p1, vec2i p2, float r, float g, float b, float a = 1.0f);
void StrokeTriangle(GameOffscreenBuffer *offscreenBuffer, vec2i p0, vec2i p1, vec2i p2, float r, float g, float b, float a = 1.0f);
void DrawRect(GameOffscreenBuffer *offscreenBuffer, vec2i p0, vec2i p1, float r, float g, float b, float a = 1.0f);
void StrokeRect(GameOffscreenBuffer *offscreenBuffer, vec2i p0, vec2i p1, float r, float g, float b,float a= 1.0f);