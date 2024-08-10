#include <assert.h>
#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <fcntl.h>
#include <string.h>
#include <linux/if_tun.h>

struct sockaddr_in peer;

int tun(const char *name)
{
	int fd = open("/dev/net/tun", O_RDWR);
	if (fd < 0) {
		perror("open");
		return -1;
	}

	struct ifreq req = {.ifr_ifru.ifru_flags = IFF_TUN | IFF_NO_PI };
	strcpy(req.ifr_ifrn.ifrn_name, name);
	if (ioctl(fd, TUNSETIFF, &req) < 0) {
		perror("ioctl");
		return -1;
	}

	return fd;
}

int set(const char *name, const char *ip, const char *mask)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	struct ifreq req;
	strcpy(req.ifr_ifrn.ifrn_name, name);
	struct sockaddr_in addr = {.sin_family = AF_INET };

	inet_aton(ip, &addr.sin_addr);
	req.ifr_ifru.ifru_addr = *(struct sockaddr *)&addr;
	if (ioctl(fd, SIOCSIFADDR, &req) < 0) {
		perror("ioctl");
		return -1;
	}

	inet_aton(mask, &addr.sin_addr);
	req.ifr_ifru.ifru_addr = *(struct sockaddr *)&addr;
	if (ioctl(fd, SIOCSIFNETMASK, &req) < 0) {
		perror("ioctl");
		return -1;
	}

	if (ioctl(fd, SIOCGIFFLAGS, &req) < 0) {
		perror("ioctl");
		return -1;
	}

	req.ifr_ifru.ifru_flags |= IFF_UP;
	if (ioctl(fd, SIOCSIFFLAGS, &req) < 0) {
		perror("ioctl");
		return -1;
	}

	close(fd);
	return 0;
}

int Bind(int fd, uint16_t port)
{
	int so = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &so, sizeof(so));

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	if (bind(fd, (void *)&addr, sizeof(addr))) {
		perror("bind");
		return -1;
	}

	return 0;
}

int tun2net(int tfd, int sfd)
{
	char b[4096];
	ssize_t r = read(tfd, b, sizeof(b));
	if (r < 0) {
		perror("read");
		return -1;
	}

	printf("read %ld\n", r);
	assert(r);
	if (!peer.sin_family)
		return 0;

	ssize_t s = sendto(sfd, b, r, 0, (void *)&peer, sizeof(peer));
	if (s < 0) {
		perror("sendto");
		return -1;
	}

	printf("send %ld to %s:%hu\n", s, inet_ntoa(peer.sin_addr),
	       ntohs(peer.sin_port));
	assert(s == r);
	return 0;
}

int net2tun(int sfd, int tfd)
{
	char b[4096];
	socklen_t l = sizeof(peer);
	ssize_t r = recvfrom(sfd, b, sizeof(b), 0, (void *)&peer, &l);
	if (r < 0) {
		perror("recvfrom");
		return -1;
	}

	printf("recv %ld from %s:%hu\n", r, inet_ntoa(peer.sin_addr),
	       ntohs(peer.sin_port));
	assert(r);
	ssize_t w = write(tfd, b, r);
	if (w < 0) {
		perror("write");
		return -1;
	}

	printf("write %ld\n", w);
	assert(w == r);
	return 0;
}

int main(int c, char **v)
{
	const char *name = NULL, *ip = NULL, *mask = NULL, *server = NULL;
	for (int i; (i = getopt(c, v, "n:i:m:s:")) != -1;) {
		switch (i) {
		case 'n':
			name = optarg;
			break;
		case 'i':
			ip = optarg;
			break;
		case 'm':
			mask = optarg;
			break;
		case 's':
			server = optarg;
			break;
		default:
			return 1;
		}
	}

	if (!name || !ip || !mask)
		return 1;

	int tfd = tun(name);
	if (tfd < 0)
		return 1;

	if (set(name, ip, mask) < 0)
		return 1;

	int sfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (server) {
		peer.sin_family = AF_INET;
		peer.sin_port = htons(1116);
		inet_aton(server, &peer.sin_addr);
	} else {
		if (Bind(sfd, 1116) < 0)
			return 1;
	}

	struct pollfd fds[2] = { {sfd, POLLIN}, {tfd, POLLIN} };
	while (1) {
		int n = poll(fds, 2, -1);
		assert(n > 0);

		if (fds[0].revents & POLLIN) {
			if (net2tun(sfd, tfd) < 0)
				return 1;
		}

		if (fds[1].revents & POLLIN) {
			if (tun2net(tfd, sfd) < 0)
				return 1;
		}
	}
}
