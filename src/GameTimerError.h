#pragma once

#define ErrorWhen( a, code )            \
        if( a )                         \
        {                               \
            throw code;                 \
        }                               \

namespace SeaDrip
{
    namespace GameTimer
    {
        enum class ErrorCode : int
        {
            OK = 0,
            CANNOT_INIT_WNDCLS = 1,
            CANNOT_CREATE_WINDOW = 2,
        };
    };
};
