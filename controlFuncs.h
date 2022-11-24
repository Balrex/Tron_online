void threadControlNsync(struct args* args)
{
  int sockfd = args->sockfd;
  char* ptrDirect = args->ptrDirect;
  pthread_mutex_t* ptrMtx = args->ptrMtx;
  struct sockaddr* ptrAddr_p2 = (struct sockaddr*)args->ptrAddr_p2;
  int lenSockaddr = sizeof(*ptrAddr_p2);
  char character = 0;
  getchar();
  *(args->ptrIsReadyPlayer) = 1;
  sendto(sockfd, &character, 1, 0, ptrAddr_p2, lenSockaddr);

  character = 0;
  //wait start game
  while( startFlag != 1 )
  {
      usleep(1);
      sendto(sockfd, &character, 1, 0, ptrAddr_p2, lenSockaddr);
  }
  tcflush(0, TCIFLUSH); // 0 - stdin
  while( workFlag )
  {
    character = getchar();
    if(character == UP || character == DOWN || character == LEFT || character == RIGHT)
    {
      pthread_mutex_lock(ptrMtx);
      *ptrDirect = character; 
      sendto(sockfd, &character, 1, 0, ptrAddr_p2, lenSockaddr);
      pthread_mutex_unlock(ptrMtx);
    }
  }
}

void threadInteractionNsync(struct args* args)
{
  int sockfd = args->sockfd;
  char* ptrDirect = args->ptrDirect;
  pthread_mutex_t* ptrMtx = args->ptrMtx;
  struct sockaddr Addr_p2 = *((struct sockaddr*)args->ptrAddr_p2);
  int lenSockaddr = sizeof(Addr_p2);
  char character;
  do
  {
    recvfrom(sockfd, &character, 1, 0, &Addr_p2, &lenSockaddr);
  }while(((struct sockaddr_in*)&Addr_p2)->sin_addr.s_addr != (args->ptrAddr_p2)->sin_addr.s_addr);
  
  *(args->ptrIsReadyPlayer) = 1;
  
  //wait start game
  while(startFlag != 1)
  {
      usleep(1);
  }

  while(workFlag)
  {
    recvfrom(sockfd, &character, 1, 0, &Addr_p2, &lenSockaddr);
    if(character == UP || character == DOWN || character == LEFT || character == RIGHT
		    &&((struct sockaddr_in*)&Addr_p2)->sin_addr.s_addr == (args->ptrAddr_p2)->sin_addr.s_addr)
    {
      pthread_mutex_lock(ptrMtx);
      *ptrDirect = character; 
      pthread_mutex_unlock(ptrMtx);
    }
  }
}

void threadControlSync(struct args* args)
{
  int sockfd = args->sockfd;
  char* ptrDirect = args->ptrDirect;
  pthread_mutex_t* ptrMtx = args->ptrMtx;
  struct sockaddr* ptrAddr_p2 = (struct sockaddr*)args->ptrAddr_p2;
  int lenSockaddr = sizeof(*ptrAddr_p2);
  char character = 0;
  
  getchar();
  *(args->ptrIsReadyPlayer) = 1;
  sendto(sockfd, &character, 1, 0, ptrAddr_p2, lenSockaddr);

  //wait start game
  while( startFlag != 1 )
  {
      usleep(1);
      sendto(sockfd, &character, 1, 0, ptrAddr_p2, lenSockaddr);
  }

  tcflush(0, TCIFLUSH);
  while( workFlag )
  {
    character = getchar();
    if(character == UP || character == DOWN || character == LEFT || character == RIGHT)
    {
      pthread_mutex_lock(ptrMtx);
      *ptrDirect = character; 
      pthread_mutex_unlock(ptrMtx);
    }
  }
}

void threadInteractionSync(struct args* args)
{
  int sockfd = args->sockfd;
  char* ptrDirect = args->ptrDirect;
  pthread_mutex_t* ptrMtx = args->ptrMtx;
  struct sockaddr Addr_p2 = *((struct sockaddr*)args->ptrAddr_p2);
  int lenSockaddr = sizeof(Addr_p2);
  char character;
  do
  {
    recvfrom(sockfd, &character, 1, 0, &Addr_p2, &lenSockaddr);
  }while(((struct sockaddr_in*)&Addr_p2)->sin_addr.s_addr != (args->ptrAddr_p2)->sin_addr.s_addr);

  *(args->ptrIsReadyPlayer) = 1;
  
  //wait start game
  while(startFlag != 1)
  {
      usleep(1);
  }

  while(workFlag)
  {
    recvfrom(sockfd, &character, 1, 0, &Addr_p2, &lenSockaddr);
    character -= numStep % 2;
    if((character == UP || character == DOWN 
                || character == LEFT || character == RIGHT) && waitFlag
		    && ((struct sockaddr_in*)&Addr_p2)->sin_addr.s_addr == (args->ptrAddr_p2)->sin_addr.s_addr)
    {
      *ptrDirect = character;
      waitFlag = 0;
    }
  }
}
