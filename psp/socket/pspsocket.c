#include "pspsocket.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

/*
  These fake implementations are taken from Python's port.
*/

int gethostname(char *name, size_t len)
{
    FILE *fp;
    size_t a;

    if (len < 0)
    {
       errno = EINVAL;
       return -1;
    }

    fp = fopen("./hostname", "r");
    if (!fp)
    {
       strncpy(name, "psp.net", len);
       return 0;
    }

    a = fread(name, 1, len - 1, fp);
    name[a] = 0;
    fclose(fp);

    return 0;
}

int sethostname(const char *name, size_t len)
{
    FILE *fp = fopen("./hostname", "w");

    if (len < 0)
    {
       errno = EINVAL;
       return -1;
    }

    if (!fp)
    {
       errno = EPERM;
       return -1;
    }

    fwrite(name, 1, len, fp);
    fclose(fp);

    return 0;
}

struct servent* getservbyname(const char *name, const char *proto)
{
    return NULL;
}

struct servent* getservbyport(int port, const char *proto)
{
    return NULL;
}

struct protoent* getprotobyname(const char *name)
{
    return NULL;
}
