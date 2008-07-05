
#include <netdb.h>
#include <string.h>

#include "base/main.h"

char* getIPFormDNS(char *domain)
{
	struct hostent *host;

	host = gethostbyname(domain);

	if( host == NULL ||  host->h_addr_list[0] == NULL )
	{
		return NULL;
	}

	return strdup( inet_ntoa( * ( (struct in_addr *) host->h_addr_list[0] ) ) );
}
