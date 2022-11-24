#include "libs.h"

static struct termios stored_settings;

//----------------------------------------------------------------
// in the order of the description at the end and files
void invertFourBytes(char *ptr);
void setKeypress(void);
void resetKeypress(void);
int getLocalIp(unsigned long opponentAddr);
int handler(int none);
// move.h
char setOppositeDirect(char direct, char directLast, char* ptrOppositeDirect);
void moveCar(uint32_t** ptrCar, char direct, int scrXres);
char wasCross(uint32_t * ptrCar_p1, uint32_t* ptrCar_p2, char direct_p1, char direct_p2, int scrXres);
// view.h
void drawArea(uint32_t *ptr, int xres, int yres, int scrXres);
int drawCar(uint32_t* ptr, char direction, uint32_t color, int scrXres);
void deleteCar(uint32_t* ptr, char direction, int scrXres);
// controlFuncs.h  
void threadControlNsync(struct args* args);
void threadInteractionNsync(struct args* args);
void threadControlSync(struct args* args);
void threadInteractionSync(struct args* args);
//----------------------------------------------------------------


int main(int argc, char* argv[])
{ 
  int modeSync = 0; // 0 - not sync, 1 - sync mode
  if(argc < 4)
  {
      printf("Use: ./miniTRON.exe xres yres ip <mode sync>\n");
      return -1;
  }
  else if(argc == 5)
      modeSync = atoi(argv[4]);

  signal(SIGINT, handler);
  //init screen
  printf("\033c"); //clear screen
  setKeypress();

  int fb;
  struct fb_var_screeninfo info;
  size_t fb_size, map_size, page_size;
  uint32_t *ptr;
  
  page_size = sysconf(_SC_PAGESIZE);
  
  if ( 0 > (fb = open("/dev/fb0", O_RDWR))) 
  {
    printf("open");
    resetKeypress();
    return __LINE__;
  }

  if ( (-1) == ioctl(fb, FBIOGET_VSCREENINFO, &info)) 
  {
    printf("ioctl");
    close(fb);
    resetKeypress();
    return __LINE__;
  }

  fb_size = sizeof(uint32_t) * info.xres_virtual * info.yres_virtual;
  map_size = (fb_size + (page_size - 1 )) & (~(page_size-1));

  ptr = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);
  if ( MAP_FAILED == ptr ) 
  {
    printf("mmap");
    close(fb);
    resetKeypress();
    return __LINE__;
  }
  
  int xresArea = atoi(argv[1]);
  int yresArea = atoi(argv[2]);

  if(xresArea + 2 > info.xres-32 || yresArea + 2 > info.yres
          || xresArea <= 10 || yresArea <= 10)
  {
    munmap(ptr, map_size);
    close(fb);
    resetKeypress();
    printf("Bad size of area");
    return __LINE__;
  }

  //init socket
  int sockfd;
  struct sockaddr_in playerAddr;
  struct sockaddr_in opponentAddr;
  
  if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    munmap(ptr, map_size);
    close(fb);
    resetKeypress();
    printf("Socket creation failed");
    return __LINE__;
  }
  
  opponentAddr.sin_family = AF_INET;
  opponentAddr.sin_port = htons(12345);
  opponentAddr.sin_addr.s_addr = inet_addr(argv[3]);
 
  playerAddr.sin_family = AF_INET;
  playerAddr.sin_port = htons(12345);
  playerAddr.sin_addr.s_addr = getLocalIp(opponentAddr.sin_addr.s_addr);

  if(playerAddr.sin_addr.s_addr == 0 || 
    playerAddr.sin_addr.s_addr == opponentAddr.sin_addr.s_addr)
  {
    close(sockfd);
    munmap(ptr, map_size);
    close(fb);
    resetKeypress();
    printf("Incorrect opponent's ip\n");
    printf("Your ip:%s", inet_ntoa(playerAddr.sin_addr));
    return __LINE__;
  }

  if(bind(sockfd, (struct sockaddr*)&playerAddr, sizeof(playerAddr)) < 0)
  {
    close(sockfd);
    munmap(ptr, map_size);
    close(fb);
    resetKeypress();
    printf("Bind error");
    return __LINE__;
  }

  pthread_t tidControl, tidSync;
  pthread_attr_t attr;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  
  if( pthread_attr_init(&attr) != 0 )
  {
    close(sockfd);
    munmap(ptr, map_size);
    close(fb);
    resetKeypress();
    fprintf(stderr, "Error of init attr\n");
    return 1;
  }

  uint32_t* Сar_p1 = ptr + info.xres/2 - xresArea/2 + 1 + info.xres_virtual*3;
  uint32_t* Сar_p2 = ptr + info.xres/2 - xresArea/2 + xresArea + info.xres_virtual*(yresArea-2);
  char direct_p1 = RIGHT;
  char direct_p2= LEFT;
  char directPrev_p1 = RIGHT;
  char directPrev_p2 = LEFT;
  char oppositeDirect_p1;
  char oppositeDirect_p2;
  char needAdditionalPixel_p1 = 0;
  char needAdditionalPixel_p2 = 0;
  char whoLose[] = {0,0};   
  char indexPlayer = 0;    
  char readyFlag_p1 = 0;    
  char readyFlag_p2 = 0;
  char tmp;

  struct args args1 = {sockfd, &direct_p1, &readyFlag_p1, &opponentAddr, &mutex};
  struct args args2 = {sockfd, &direct_p2, &readyFlag_p2, &opponentAddr, &mutex};  
  
  void (*controlThread) (struct args* args);
  void (*syncThread) (struct args* args);
  if (modeSync)
  {
      controlThread = threadControlSync;
      syncThread = threadInteractionSync;
  }
  else
  {
      controlThread = threadControlNsync;
      syncThread = threadInteractionNsync;
  }

  char opponent_ip[sizeof(unsigned long)];
  char player_ip[sizeof(unsigned long)];
  
  *((unsigned long*)opponent_ip) = opponentAddr.sin_addr.s_addr;
  *((unsigned long*)player_ip) = playerAddr.sin_addr.s_addr;
  
  invertFourBytes(opponent_ip);
  invertFourBytes(player_ip);
  
  if(*((int*)player_ip) < *((int*)opponent_ip))
  {
      args1.ptrDirect = &direct_p2; args1.ptrIsReadyPlayer = &readyFlag_p2;
      args2.ptrDirect = &direct_p1; args2.ptrIsReadyPlayer = &readyFlag_p1;
      indexPlayer = 1;
  }
  
  if( pthread_create(&tidControl, &attr,(void *)controlThread, &args1) != 0 )
  {
    close(sockfd);
    munmap(ptr, map_size);
    close(fb);
    resetKeypress();
    fprintf(stderr, "Error of create thread\n");
    return 2;
  }

  if( pthread_create(&tidSync, &attr,(void *)syncThread, &args2) != 0 )
  {
    close(sockfd);
    munmap(ptr, map_size);
    close(fb);
    resetKeypress();
    fprintf(stderr, "Error of create thread\n");
    return 2;
  }

  // wait
  struct timeb tb; 
  time_t start_t; 

  ftime(&tb);
  start_t = tb.time;
  char gameStartFlag = 1;
  while((readyFlag_p1 != 1 || readyFlag_p2 != 1)&&workFlag == 1)
  {
      if(tb.time - start_t >= 10)
      {
        workFlag = 0;
        gameStartFlag = 0;
        readyFlag_p1 = 1;
        readyFlag_p2 = 1;
      }
      else
      {
        usleep(1);
        ftime(&tb);
      }
  }

  if(connect(sockfd, (struct sockaddr*)&opponentAddr, sizeof(*(struct sockaddr*)&opponentAddr)))
  {
    close(sockfd);
    munmap(ptr, map_size);
    close(fb);
    resetKeypress();
    fprintf(stderr, "Error of connect\n");
    return 2;
  }

  startFlag = 1;

  // start game 
  ftime(&tb);
  unsigned start_m = tb.millitm;
  drawArea(ptr + info.xres/2 - xresArea/2  , xresArea,  yresArea, info.xres_virtual);
  drawCar(Сar_p1, direct_p1, RED, info.xres_virtual);
  drawCar(Сar_p2, direct_p2, BLUE, info.xres_virtual);
 
  while(workFlag)
  {
    if(modeSync) //with sync
    {
        if(indexPlayer == 0) //player is master
        {
            pthread_mutex_lock(&mutex);
            needAdditionalPixel_p1 = setOppositeDirect(direct_p1, directPrev_p1, &oppositeDirect_p1);
            waitFlag = 1;
            if(directPrev_p1 != oppositeDirect_p1)
                tmp = direct_p1 + numStep % 2;
            else //player is slave
                tmp = directPrev_p1 + numStep % 2;
            while(waitFlag)
            {
                sendto(sockfd, &tmp, 1, 0,(struct sockaddr*)(&opponentAddr), sizeof(opponentAddr));
                usleep(50);
            }
            needAdditionalPixel_p2 = setOppositeDirect(direct_p2, directPrev_p2, &oppositeDirect_p2);
        }
        else
        {
            waitFlag = 1;
            while(waitFlag)
                usleep(5);
            pthread_mutex_lock(&mutex);
            needAdditionalPixel_p2 = setOppositeDirect(direct_p2, directPrev_p2, &oppositeDirect_p2);
            if(directPrev_p1 != oppositeDirect_p1)
                tmp = direct_p2 + numStep % 2;
            else
                tmp = directPrev_p2 + numStep % 2;
            for(int i = 0; i<10; i++)
                sendto(sockfd, &tmp, 1, 0, (struct sockaddr*)(&opponentAddr), sizeof(opponentAddr));
            needAdditionalPixel_p1 = setOppositeDirect(direct_p1, directPrev_p1, &oppositeDirect_p1);
        }
    }
    else 
    {
        pthread_mutex_lock(&mutex);
        needAdditionalPixel_p1 = setOppositeDirect(direct_p1, directPrev_p1, &oppositeDirect_p1);
        needAdditionalPixel_p2 = setOppositeDirect(direct_p2, directPrev_p2, &oppositeDirect_p2);
    }
    if(directPrev_p1 != oppositeDirect_p1)
    {
      deleteCar(Сar_p1, directPrev_p1, info.xres_virtual);
      *Сar_p1 = RED;
      if(needAdditionalPixel_p1)
      {
        moveCar(&Сar_p1, direct_p1, info.xres_virtual);
        *Сar_p1 = RED;
        needAdditionalPixel_p1 = 0;
      }
      moveCar(&Сar_p1, direct_p1, info.xres_virtual);
      if(drawCar(Сar_p1, direct_p1, RED, info.xres_virtual))
      {
        workFlag= 0;
        whoLose[0] = 1;
      }
      directPrev_p1 = direct_p1;  
    }
    else
    {
      deleteCar(Сar_p1, directPrev_p1, info.xres_virtual);
      *Сar_p1 = RED;
      moveCar(&Сar_p1, directPrev_p1, info.xres_virtual);
      if(drawCar(Сar_p1, directPrev_p1, RED, info.xres_virtual))
      {
        workFlag = 0;
        whoLose[0] = 1;
      }
    }
    // move second player's car
    if(directPrev_p2 != oppositeDirect_p2)   
    {
      deleteCar(Сar_p2, directPrev_p2, info.xres_virtual);
      *Сar_p2= BLUE;     
      if(needAdditionalPixel_p2)
      {
        moveCar(&Сar_p2, direct_p2, info.xres_virtual);
        *Сar_p2 = BLUE;     
        needAdditionalPixel_p2 = 0;
      }
      moveCar(&Сar_p2, direct_p2, info.xres_virtual);
      if(drawCar(Сar_p2, direct_p2, BLUE, info.xres_virtual))
      {
        workFlag= 0;
        whoLose[1] = 1;
      }
      directPrev_p2 = direct_p2;
    }
    else    
    {
      deleteCar(Сar_p2, directPrev_p2, info.xres_virtual);
      *Сar_p2= BLUE; 
      moveCar(&Сar_p2, directPrev_p2, info.xres_virtual);
      if(drawCar(Сar_p2, directPrev_p2, BLUE, info.xres_virtual))
      {
        workFlag = 0;
        whoLose[1] = 1;
      }
    }
    pthread_mutex_unlock(&mutex);
    if(modeSync && indexPlayer == 0 || modeSync == 0)
    {
      if(modeSync) //player is master
      {
        ftime(&tb);
        usleep(62500 - (((unsigned)(tb.millitm  - start_m) < 10 ) ? (tb.millitm - start_m)*1000 : 5500)); 
        ftime(&tb);
        start_m = tb.millitm;
      }
      else // nsync mode
          usleep(62500);
    }
    else //player is slave
      usleep(20000);
    numStep++;
  }
  
  if(wasCross(Сar_p1,Сar_p2, directPrev_p1, directPrev_p2, info.xres_virtual))
  {
    workFlag = 0;
    whoLose[0] = 1;
    whoLose[1] = 1;
  }
  //close all
  pthread_kill(tidControl, 17); 
  pthread_kill(tidSync, 17);

  close(sockfd);
  munmap(ptr, map_size);
  close(fb);
  resetKeypress();
  //print result of game
  fprintf(stdout,"\033c");
  if(whoLose[indexPlayer] == 0 && whoLose[0] != whoLose[1])
    fprintf(stdout,"You win\n");
  else if(whoLose[indexPlayer] == 1 && whoLose[0] != whoLose[1])
     fprintf(stdout,"You lose\n");
  else if(whoLose[0] == whoLose[1])
    fprintf(stdout,"Drawn\n") 
   
  return 0;
}

void invertFourBytes(char *ptr)
{
  char tmp=ptr[0];
  ptr[0]=ptr[3];
  ptr[3]=tmp;
  
  tmp=ptr[1];
  ptr[1]=ptr[2];
  ptr[2]=tmp;
}

void setKeypress(void)
{
  struct termios new_settings;

	tcgetattr(0,&stored_settings);

	new_settings = stored_settings;

	new_settings.c_lflag &= (~ICANON & ~ECHO);
	new_settings.c_cc[VTIME] = 0;
	new_settings.c_cc[VMIN] = 1;

	tcsetattr(0,TCSANOW,&new_settings);
	return;
}

void resetKeypress(void)
{
  tcsetattr(0,TCSANOW,&stored_settings);
	return;
}

int getLocalIp(unsigned long opponentAddr)
{
    struct ifaddrs *ifaddr;
    unsigned long addr;
    int mask;
    if (getifaddrs(&ifaddr) == -1) 
    {
        perror("getifaddrs");
        return 0;
    }

    for (struct ifaddrs *ifa = ifaddr; ifa != NULL;
                ifa = ifa->ifa_next) 
    {
        if (ifa->ifa_addr == NULL)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET) 
        {
            addr = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr.s_addr;
            if(addr != 0)
            {
                mask = ((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr.s_addr; 
                if((addr&mask) == (opponentAddr&mask))
                {
                    return addr;
                }
            }
        }
    }
    return 0;
}

int handler(int none)
{
  exit(0);
}

#include "move.h"
#include "view.h"
#include "controlFuncs.h"
