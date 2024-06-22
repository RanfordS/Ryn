#include "stringlib.h"

bool nstringsMatch (Size aLen, const char* aStr, Size bLen, const char* bStr)
{
    if (aLen != bLen) { return false; }
    {
        for (Size i = 0; i < aLen; ++i)
        {
            if (aStr[i] != bStr[i])
            {
                return false;
            }
        }
    }
    return true;
}

bool nstringMatchCstring (Size nLen, const char* const nStr, const char* const cStr)
{
    for (Size i = 0; i < nLen; ++i)
    {
        if ((cStr[i] != nStr[i])
        ||  (cStr[i] == 0))
        {
            return false;
        }
    }
    return cStr[nLen] == 0;
}

char* readFile (const char* path, Size* dataLength)
{
    FILE* file = fopen (path, "rb");

    if (!file)
    {
        printf ("Failed to open file!\n");
        return NULL;
    }

    fseek (file, 0, SEEK_END);
    *dataLength = ftell (file);
    rewind (file);

    char* data = malloc (*dataLength*sizeof (char));
    if (!data)
    {
        printf ("Cannot allocate the memory required to load in the file\n");
    }
    fread (data, sizeof (char), *dataLength, file);
    fclose (file);

    return data;
}
