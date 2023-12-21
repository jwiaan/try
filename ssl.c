#include <openssl/ssl.h>
#include <unistd.h>
#include <netdb.h>

int start(const char *host, const char *port, const struct addrinfo *hint,
	  int (*action)(int, const struct sockaddr *, socklen_t))
{
	struct addrinfo *a, *l;
	getaddrinfo(host, port, hint, &l);
	for (a = l; a; a = a->ai_next) {
		int fd = socket(a->ai_family, a->ai_socktype, a->ai_protocol);
		if (fd < 0) {
			perror("socket");
			continue;
		}

		if (!action(fd, a->ai_addr, a->ai_addrlen))
			return fd;

		close(fd);
	}
}

int Listen(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
	int opt = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (bind(fd, addr, addrlen) < 0) {
		perror("bind");
		return -1;
	}

	return listen(fd, 10);
}

void *server(void *port)
{
	struct addrinfo hint = {
		.ai_flags = AI_PASSIVE,
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM
	};

	int fd = start(NULL, port, &hint, Listen);
	fd = accept(fd, NULL, NULL);

	SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
	SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM);
	SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM);

	SSL *ssl = SSL_new(ctx);
	SSL_set_fd(ssl, fd);
	SSL_accept(ssl);
	SSL_write(ssl, "hello", 5);
}

void client(const char *port)
{
	struct addrinfo hint = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM
	};

	int fd = start(NULL, port, &hint, connect);
	SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
	SSL_CTX_load_verify_file(ctx, "ca.crt");
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

	SSL *ssl = SSL_new(ctx);
	SSL_set_fd(ssl, fd);
	SSL_connect(ssl);

	char buf[10] = { };
	SSL_read(ssl, buf, sizeof(buf));
	printf("%s\n", buf);
}

int main(void)
{
	char *port = "1116";
	pthread_t t;
	pthread_create(&t, NULL, server, port);
	sleep(1);
	client(port);
}
