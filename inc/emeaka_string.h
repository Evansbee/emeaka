#pragma once
#include <cstdio>

size_t StringLen(const char* string, size_t max_search = (size_t)(-1));

//returns full length of copy with \0
size_t StringCopy(const char* src, char* dst, size_t max_length = (size_t)-1);