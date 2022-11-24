#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <sys/timeb.h>
#include <time.h>
#include <termios.h>

#define RED 0x00FF0F3F
#define BLUE 0x003080FF
#define VIOLET 0x00FF30FF
#define BACKGROUND 0x00B1FEFF

#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'

int workFlag = 1;
int startFlag = 0;
int waitFlag = 0;
char numStep = 0;

struct args
{
  int sockfd;
  char* ptrDirect;
  char* ptrIsReadyPlayer;
  struct sockaddr_in* ptrAddr_p2;
  pthread_mutex_t* ptrMtx;
};
