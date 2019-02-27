#pragma once

#include "emeaka.h"
#include <cstdint>

enum class LogLevel
{
    INFO,
    NOTICE,
    WARNING,
    ERROR,
    FATAL
};

struct LogEntry
{
    uint64_t EntryID;
    double EntryTime;
    bool ShouldWriteToFile;
    bool ShouldDisplay;
    LogLevel Level;
    char *Text;
    float DisplayTimeLeft;
    float FadeTimeLeft;
    LogEntry* Next;
}

struct Logger
{
    LogEntry* Head;
}