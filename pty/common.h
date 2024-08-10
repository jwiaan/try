#pragma once

#include <assert.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>

void loop(int, int);
int start(int (*)(int, const struct sockaddr *, socklen_t), const char *,
	  uint16_t);
