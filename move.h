char setOppositeDirect(char direct, char directLast, char* ptrOppositeDirect)
{
    switch(direct)
    {
      case UP:
      {
        *ptrOppositeDirect = DOWN;
        if( directLast == LEFT || directLast == RIGHT )
            return 1;
        else
            return 0;
      }
      case DOWN:
      {
        *ptrOppositeDirect = UP;  
        if( directLast == LEFT || directLast == RIGHT )
            return 1;
        else 
            return 0;
        break;
      }
      case LEFT:
      {
        *ptrOppositeDirect = RIGHT;  
        if( directLast == UP || directLast == DOWN )
            return 1;
        else 
            return 0;
      }
      case RIGHT:
      {
        *ptrOppositeDirect = LEFT;  
        if( directLast == UP || directLast == DOWN )
            return 1;
        else 
            return 0;
      }
    }
}

void moveCar(uint32_t** ptrCar, char direct, int scrXres)
{
  switch(direct)
  {
    case UP:
        *ptrCar -= scrXres;
        break;
    case DOWN:
        *ptrCar += scrXres;
        break;
    case LEFT:
        *ptrCar -= 1;
        break;
    case RIGHT:
        *ptrCar += 1;
        break;
  }
}



char wasCross(uint32_t * ptrCar_p1, uint32_t* ptrCar_p2, char direct_p1, char direct_p2, int scrXres)
{
    uint32_t* car_p1[40];
    int index = 0;
    switch (direct_p1)
    {
        case UP:
            for(int i = 0; i>-8; i--)
            {
                for(int j = -2; j<=2; j++)
                {
                   car_p1[index] = ptrCar_p1 + j + i*scrXres;
                   index ++;
                }
            }
            break;
        case DOWN:
            for(int i = 0; i<8; i++)
            {
                for(int j = -2; j<=2; j++)
                {
                   car_p1[index] = ptrCar_p1 + j + i*scrXres;
                   index ++;
                }
            }
            break;
        case LEFT:
            for(int i = 0; i>-8; i--)
            {
                for(int j = -2; j<=2; j++)
                {
                   car_p1[index] = ptrCar_p1 + i + j*scrXres;
                   index ++;
                }
            }
            break;
        case RIGHT:
            for(int i = 0; i<8; i++)
            {
                for(int j = -2; j<=2; j++)
                {
                   car_p1[index] = ptrCar_p1 + i + j*scrXres;
                   index ++;
                }
            }
            break;
    }  
    index = 0;
    switch (direct_p2)
    {
        case UP:
            for(int i = 0; i>-8; i--)
            {
                for(int j = -2; j<=2; j++)
                {
                  for(int indx = 0; indx < 40; indx++)
                  {
                        uint32_t* result = ptrCar_p2 + j + i*scrXres;
                        if(car_p1[indx] == result)
                        {
                            return 1;
                        }
                    }
                }
            }
            break;
        case DOWN:
            for(int i = 0; i<8; i++)
            {
                for(int j = -2; j<=2; j++)
                {
                    for(int indx = 0; indx < 40; indx++)
                    {
                        uint32_t* result = ptrCar_p2 + j + i*scrXres;
                        if(car_p1[indx] == result)
                        {
                            return 1;
                        }
                    }
                }
            }
            break;
        case LEFT:
            for(int i = 0; i>-8; i--)
            {
                for(int j = -2; j<=2; j++)
                {
                    for(int indx = 0; indx < 40; indx++)
                    {
                        uint32_t* result = ptrCar_p2 + i + j*scrXres;
                        if(car_p1[indx] == result)
                        {
                            return 1;
                        }
                    }
                }
            }
            break;
        case RIGHT:
            for(int i = 0; i<8; i++)
            {
                for(int j = -2; j<=2; j++)
                {
                    for(int indx = 0; indx < 40; indx++)
                    {
                        uint32_t* result = ptrCar_p2 + i + j*scrXres;
                        if(car_p1[indx] == result)
                        {
                            return 1;
                        }
                    }
                }
            }
            break;
    }
    return 0;
}
