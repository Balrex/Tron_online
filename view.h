void drawArea(uint32_t *ptr, int xres, int yres, int scrXres)
{
  for(int i = 0; i<xres+2; i++)
  {
      ptr[i] = VIOLET;
      ptr[i+(yres+1)*scrXres] = VIOLET;
  }
  for(int i = 1; i<=yres; i++)
  {
      ptr[i*scrXres] = VIOLET;
      ptr[i*scrXres+(xres+1)] = VIOLET;
  }
  for(int i = 1; i<=xres; i++)
  {
      for(int j = 1; j<=yres; j++)
        ptr[i+j*scrXres] = BACKGROUND;
  }
}

int drawCar(uint32_t* ptr, char direction, uint32_t color, int scrXres)
{
    int wasOverlap = 0;
    switch (direction)
    {
        case UP:
            for(int i = 0; i>-8; i--)
            {
                for(int j = -2; j<=2; j++)
                {
                    if( ptr[j+i*scrXres] == VIOLET || 
                        ptr[j+i*scrXres] == RED || 
                        ptr[j+i*scrXres] == BLUE)
                        wasOverlap = 1;
                    ptr[j+i*scrXres] = color;
                }
            }
            break;
        case DOWN:
            for(int i = 0; i<8; i++)
            {
                for(int j = -2; j<=2; j++)
                {
                    if( ptr[j+i*scrXres] == VIOLET || 
                        ptr[j+i*scrXres] == RED || 
                        ptr[j+i*scrXres] == BLUE)
                        wasOverlap = 1;
                    ptr[j+i*scrXres] = color;
                }
            }
            break;
        case LEFT:
            for(int i = 0; i>-8; i--)
            {
                for(int j = -2; j<=2; j++)
                {
                    if( ptr[i+j*scrXres] == VIOLET || 
                        ptr[i+j*scrXres] == RED || 
                        ptr[i+j*scrXres] == BLUE)
                        wasOverlap = 1;
                    ptr[i+j*scrXres] = color;
                }
            }
            break;
        case RIGHT:
            for(int i = 0; i<8; i++)
            {
                for(int j = -2; j<=2; j++)
                {
                    if( ptr[i+j*scrXres] == VIOLET || 
                        ptr[i+j*scrXres] == RED || 
                        ptr[i+j*scrXres] == BLUE)
                        wasOverlap = 1;
                    ptr[i+j*scrXres] = color;
                }
            }
            break;
    }
    return wasOverlap;
}

void deleteCar(uint32_t* ptr, char direction, int scrXres)
{
    switch (direction)
    {
        case UP:
        {
            for(int i = 0; i>-8; i--)
            {
                ptr[-2+i*scrXres] = BACKGROUND;
                ptr[-1+i*scrXres] = BACKGROUND;
                ptr[i*scrXres] = BACKGROUND;
                ptr[1+i*scrXres] = BACKGROUND;
                ptr[2+i*scrXres] = BACKGROUND;
            }
            break;
        }
        case DOWN:
        {
            for(int i = 0; i<8; i++)
            {
                ptr[-2+i*scrXres] = BACKGROUND;
                ptr[-1+i*scrXres] = BACKGROUND;
                ptr[i*scrXres] = BACKGROUND;
                ptr[1+i*scrXres] = BACKGROUND;
                ptr[2+i*scrXres] = BACKGROUND;
            }
            break;
        }
        case LEFT:
        {
            for(int i = 0; i>-8; i--)
            {
                ptr[-2*scrXres+i] = BACKGROUND;
                ptr[-1*scrXres+i] = BACKGROUND;
                ptr[i] = BACKGROUND;
                ptr[scrXres+i] = BACKGROUND;
                ptr[2*scrXres+i] = BACKGROUND;
            }
            break;
        }
        case RIGHT:
        {
            for(int i = 0; i<8; i++)
            {
                ptr[-2*scrXres+i] = BACKGROUND;
                ptr[-1*scrXres+i] = BACKGROUND;
                ptr[i] = BACKGROUND;
                ptr[scrXres+i] = BACKGROUND;
                ptr[2*scrXres+i] = BACKGROUND;         
            }
            break;
        }
    }
}
