#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <netdb.h>
#include <stdio.h>

const char *family[] = {
	[AF_INET] = "AF_INET",
	[AF_INET6] = "AF_INET6"
};

const char *socktype[] = {
	[SOCK_STREAM] = "SOCK_STREAM",
	[SOCK_DGRAM] = "SOCK_DGRAM",
	[SOCK_RAW] = "SOCK_RAW",
};

const char *protocol[] = {
	[IPPROTO_IP] = "IPPROTO_IP",
	[IPPROTO_TCP] = "IPPROTO_TCP",
	[IPPROTO_UDP] = "IPPROTO_UDP",
	[IPPROTO_ICMP] = "IPPROTO_ICMP",
	[IPPROTO_ICMPV6] = "IPPROTO_ICMPV6"
};

void ntop(const struct sockaddr *s, char *buf, socklen_t len, in_port_t * port)
{
	const void *addr;
	if (s->sa_family == AF_INET) {
		addr = &((struct sockaddr_in *)s)->sin_addr;
		*port = ((struct sockaddr_in *)s)->sin_port;
	} else if (s->sa_family == AF_INET6) {
		addr = &((struct sockaddr_in6 *)s)->sin6_addr;
		*port = ((struct sockaddr_in6 *)s)->sin6_port;
	} else {
		assert(0);
	}

	if (!inet_ntop(s->sa_family, addr, buf, len)) {
		perror("inet_ntop");
		assert(0);
	}

	*port = ntohs(*port);
}

int main(int c, char **v)
{
	const char *host = NULL, *port = NULL;
	struct addrinfo hints = { };
	for (int i; (i = getopt(c, v, "h:p:P")) != -1;) {
		switch (i) {
		case 'h':
			host = optarg;
			break;
		case 'p':
			port = optarg;
			break;
		case 'P':
			hints.ai_flags = AI_PASSIVE;
			break;
		default:
			return 1;
		}
	}

	struct addrinfo *l;
	int e = getaddrinfo(host, port, &hints, &l);
	if (e) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(e));
		return 1;
	}

	for (struct addrinfo * a = l; a; a = a->ai_next) {
		printf("%s\n", family[a->ai_family]);
		printf("%s\n", socktype[a->ai_socktype]);
		printf("%s\n", protocol[a->ai_protocol]);

		char addr[256];
		in_port_t port;
		ntop(a->ai_addr, addr, sizeof(addr), &port);
		printf("%s:%d\n\n", addr, port);
	}
}
