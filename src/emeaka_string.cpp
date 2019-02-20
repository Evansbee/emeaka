#include "emeaka.h"

//not including \0
size_t StringLen(const char* string, size_t max_search = (size_t)(-1))
{
    size_t idx = 0;
    while(string[idx] != '\0')
    {
        if(idx >= max_search)
        {
            return (size_t)-1;
        }
        idx++;
    }
    return idx;
}

//returns full length of copy with \0
size_t StringCopy(const char* src, char* dst, size_t max_length = (size_t)-1)
{
    size_t idx = 0;

    //odd case
    if(max_length == 0) return 0;
    
    do
    {
        dst[idx] = (max_length != idx)?src[idx]:'\0';
        
    } while(dst[idx++] != '\0');
    return idx;
}