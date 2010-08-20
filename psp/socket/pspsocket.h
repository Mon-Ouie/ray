#ifndef PSPSOCKET_H
#define PSPSOCKET_H

#include <netdb.h>
#include <stdlib.h>

struct servent {
      char    *s_name;        /* official service name */
      char    **s_aliases;    /* alias list */
      int     s_port;         /* port number */
      char    *s_proto;       /* protocol to use */
};

struct protoent {
      char    *p_name;        /* official protocol name */
      char    **p_aliases;    /* alias list */
      int     p_proto;        /* protocol number */
};

int gethostname(char *name, size_t len);
int sethostname(const char *name, size_t len);

struct servent* getservbyname(const char *name, const char *proto);
struct servent* getservbyport(int port, const char *proto);

struct protoent* getprotobyname(const char *name);

#endif
