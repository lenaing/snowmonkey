#include "string_utils.h"

char *
concat(char *src1, char *src2)
{
    char *tmp;

    tmp = calloc(strlen(src1) + strlen(src2) + 1, sizeof(char));
    strcat(tmp, src1);
    strcat(tmp, src2);
    return tmp;
}

int
is_slashed(char *str)
{
    char *lastChar;
    unsigned int pos;

    lastChar = strrchr(str, '/');
    pos = (unsigned int)(lastChar - str + 1);
    if ((NULL == lastChar) || (pos != strlen(str))) {
        return 0;
    }
    return 1;
}
