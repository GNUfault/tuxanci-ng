#ifndef __WIN32__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
#include <windows.h>
#include <wininet.h>
#endif

#include <string.h>
#include "main.h"

char *gns_resolv(char *domain)
{
	struct hostent *host;

	host = gethostbyname(domain);

	if (host == NULL || host->h_addr_list[0] == NULL) {
		return NULL;
	}

	return strdup(inet_ntoa(*((struct in_addr *) host->h_addr_list[0])));
}
