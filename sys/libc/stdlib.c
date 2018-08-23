#include <string.h>
#include <ctype.h>

#include "fmt.h"

long int strtol(const char *nptr, char **endptr, int base)
{
    (void)base;

    const size_t len = strlen(nptr);

    if(endptr) {
        *endptr = (char*)nptr + len;
    }

    return scn_u32_dec(nptr, len);
}

int atoi( const char *p)
{
    int sign = 0;
    int res = 0;

    while(   *p==' '
             || *p=='\t'
             || *p=='\n'
             || *p=='\f'
             || *p=='\r'
             || *p=='\v' ) p++;

    if(*p == '-' ) 
    {
      sign = 1; 
      p++;
    }
    else if(*p == '+') p++;

    if(!isdigit(*p)) return 0;

    while(1)
    {
        res += *p - '0';
        p++;
        if(!isdigit(*p)) break;
        res = res*10;
    }

    if(sign) res = -res;

    return res;
}
