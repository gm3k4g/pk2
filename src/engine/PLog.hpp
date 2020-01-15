#pragma once

#include "engine/types.hpp"

namespace PLog {

enum {

    DEBUG = 1 << 0,
    INFO  = 1 << 1,
    WARN  = 1 << 2,
    ERR   = 1 << 3,
    FATAL = 1 << 4,
    ALL   = 255,
    OFF   = 0
    
};

void Init(u8 level, const char* file);
void Write(u8 level, const char* origin, const char* format, ...);
void Exit();

}