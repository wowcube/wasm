#define new signed
#define bool int
#define false 0
#define true 1

#include "header.h"

#define run pawn_run
#define pow pawn_pow


new abi_cubeN = 0;
new abi_TRBL[8][3];
new abi_TRBL_backup[3]; 
cell abi_MTD_Accel[6] = {0,0,0,0,0,0}; 
cell abi_MTD_Gyro[6] = {0,0,0,0,0,0}; 

cell abi_GetCubeN()
{
  return abi_cubeN;
}

cell abi_SetCubeN(cell cubeN)
{
  abi_cubeN = cubeN;
}


cell abi_ByteN(cell* arr, cell n)
{
  return ((arr[n/4] >> (8*(n%4))) & 0xFF);
}

cell abi_TRBL_Deserialize(cell* pkt)
{
  new nr = abi_ByteN(pkt, 1); 

  for(new r=0; r<nr; r++)
  {
    new rb = 2+r*6; 
    new cubeN=abi_ByteN(pkt, rb+0);
    new faceN=abi_ByteN(pkt, rb+1);
    abi_TRBL[cubeN][faceN]=(abi_ByteN(pkt, rb+2) << 24) | (abi_ByteN(pkt, rb+3) << 16) | (abi_ByteN(pkt, rb+4) << 8) | abi_ByteN(pkt, rb+5);
  }
}

cell abi_check_rotate()
{
  new faceN;
  new bool check=false;
  for(faceN=0;faceN<3;faceN++)
  {
    if (abi_topCubeN(abi_cubeN, faceN)<8)
    {

      if (abi_TRBL_backup[faceN] != abi_topCubeN(abi_cubeN, faceN))
      {
        if (abi_TRBL_backup[faceN] < 8)
          check=true;
        abi_TRBL_backup[faceN] = abi_topCubeN(abi_cubeN, faceN);
      }
    }
  }
  return(check);
}

cell abi_topCubeN(cell _cubeN, cell _faceN)
{
  return ((abi_TRBL[_cubeN][_faceN] >> 24) & 0xFF);
}

cell abi_topFaceN(cell _cubeN, cell _faceN)
{
  return ((abi_TRBL[_cubeN][_faceN] >> 16) & 0xFF);
}

cell abi_rightCubeN(cell _cubeN, cell _faceN)
{
  return _cubeN; 
}

cell abi_rightFaceN(cell _cubeN, cell _faceN)
{
  return ((_faceN == 0) ? 2 : ((_faceN == 1) ? 0 : 1));
}

cell abi_bottomCubeN(cell _cubeN, cell _faceN)
{
  return _cubeN; 
}

cell abi_bottomFaceN(cell _cubeN, cell _faceN)
{
  return ((_faceN == 0) ? 1 : ((_faceN == 1) ? 2 : 0));
}

cell abi_leftCubeN(cell _cubeN, cell _faceN)
{
  return ((abi_TRBL[_cubeN][_faceN] >> 8) & 0xFF);
}

cell abi_leftFaceN(cell _cubeN, cell _faceN)
{
  return (abi_TRBL[_cubeN][_faceN] & 0xFF);
}

cell abi_MTD_Deserialize(cell* pkt)
{
  for(new i=0; i<6; i++)
  {
    abi_MTD_Accel[i] = abi_ByteN(pkt, i+1);
    abi_MTD_Gyro[i] = abi_ByteN(pkt, 6+i+1);
  }
}

cell abi_MTD_GetFaceAccelX(cell faceN)
{
  new a=0;

  switch(faceN)
  {
    break;case 0:
    {
      if(abi_MTD_Accel[4] > 0) a = abi_MTD_Accel[4];
      else if(abi_MTD_Accel[5] > 0) a = -abi_MTD_Accel[5];
    }

    break;case 1:
    {
      if(abi_MTD_Accel[0] > 0) a = abi_MTD_Accel[0];
      else if(abi_MTD_Accel[1] > 0) a = -abi_MTD_Accel[1];
    }

    break;case 2:
    {
      if(abi_MTD_Accel[2] > 0) a = -abi_MTD_Accel[2];
      else if(abi_MTD_Accel[3] > 0) a = abi_MTD_Accel[3];
    }
  }

  return a;
}

cell abi_MTD_GetFaceAccelY(cell faceN)
{
  new a=0;

  switch(faceN)
  {
    break;case 0:
    {
      if(abi_MTD_Accel[2] > 0) a = -abi_MTD_Accel[2];
      else if(abi_MTD_Accel[3] > 0) a = abi_MTD_Accel[3];
    }

    break;case 1:
    {
      if(abi_MTD_Accel[4] > 0) a = abi_MTD_Accel[4];
      else if(abi_MTD_Accel[5] > 0) a = -abi_MTD_Accel[5];
    }

    break;case 2:
    {
      if(abi_MTD_Accel[0] > 0) a = abi_MTD_Accel[0];
      else if(abi_MTD_Accel[1] > 0) a = -abi_MTD_Accel[1];
    }
  }

  return a;
}

cell abi_MTD_GetFaceAccelZ(cell faceN)
{
  new a=0;

  switch(faceN)
  {
    break;case 0:
    {
      if(abi_MTD_Accel[0] > 0) a = abi_MTD_Accel[0];
      else if(abi_MTD_Accel[1] > 0) a = -abi_MTD_Accel[1];
    }

    break;case 1:
    {
      if(abi_MTD_Accel[2] > 0) a = -abi_MTD_Accel[2];
      else if(abi_MTD_Accel[3] > 0) a = abi_MTD_Accel[3];
    }

    break;case 2:
    {
      if(abi_MTD_Accel[4] > 0) a = abi_MTD_Accel[4];
      else if(abi_MTD_Accel[5] > 0) a = -abi_MTD_Accel[5];
    }
  }

  return a;
}

cell abi_MTD_GetFaceGyroX(cell faceN)
{
  new a=0;

  switch(faceN)
  {
    break;case 0:
    {
      if(abi_MTD_Gyro[4] > 0) a = abi_MTD_Gyro[4];
      else if(abi_MTD_Gyro[5] > 0) a = -abi_MTD_Gyro[5];
    }

    break;case 1:
    {
      if(abi_MTD_Gyro[0] > 0) a = abi_MTD_Gyro[0];
      else if(abi_MTD_Gyro[1] > 0) a = -abi_MTD_Gyro[1];
    }

    break;case 2:
    {
      if(abi_MTD_Gyro[2] > 0) a = -abi_MTD_Gyro[2];
      else if(abi_MTD_Gyro[3] > 0) a = abi_MTD_Gyro[3];
    }
  }

  return a;
}

cell abi_MTD_GetFaceGyroY(cell faceN)
{
  new a=0;

  switch(faceN)
  {
    break;case 0:
    {
      if(abi_MTD_Gyro[2] > 0) a = -abi_MTD_Gyro[2];
      else if(abi_MTD_Gyro[3] > 0) a = abi_MTD_Gyro[3];
    }

    break;case 1:
    {
      if(abi_MTD_Gyro[4] > 0) a = abi_MTD_Gyro[4];
      else if(abi_MTD_Gyro[5] > 0) a = -abi_MTD_Gyro[5];
    }

    break;case 2:
    {
      if(abi_MTD_Gyro[0] > 0) a = abi_MTD_Gyro[0];
      else if(abi_MTD_Gyro[1] > 0) a = -abi_MTD_Gyro[1];
    }
  }

  return a;
}

cell abi_MTD_GetFaceGyroZ(cell faceN)
{
  new a=0;

  switch(faceN)
  {
    break;case 0:
    {
      if(abi_MTD_Accel[0] > 0) a = abi_MTD_Accel[0];
      else if(abi_MTD_Accel[1] > 0) a = -abi_MTD_Accel[1];
    }

    break;case 1:
    {
      if(abi_MTD_Accel[2] > 0) a = -abi_MTD_Accel[2];
      else if(abi_MTD_Accel[3] > 0) a = abi_MTD_Accel[3];
    }

    break;case 2:
    {
      if(abi_MTD_Accel[4] > 0) a = abi_MTD_Accel[4];
      else if(abi_MTD_Accel[5] > 0) a = -abi_MTD_Accel[5];
    }
  }

  return a;
}




cell abi_CMD_REDRAW(cell faceN)
{
  cell pkt[1] = {0};
  pkt[0] = ((faceN & 0xFF) << 8) | (0+1 & 0xFF);


  sendpacket(pkt, 1);

}

cell abi_exit()
{
    cell pkt[1] = {0};
    pkt[0] = (0 + 20) & 0xFF;


    sendpacket(pkt, 1);

}

cell abi_trigger_debug_info()
{
  cell pkt[1] = {0};
  pkt[0] = 0+7 & 0xFF;


  sendpacket(pkt, 1);

}

cell abi_trigger_bluetooth()
{
  cell pkt[1] = {0};
  pkt[0] = 0+8 & 0xFF;

  sendpacket(pkt, 1);

}

cell abi_CMD_FILL(cell R, cell G, cell B)
{
  cell pkt[1] = {0};
  pkt[0] = ((B & 0x1F) << 24) | ((G & 0x3F) << 16) | ((R & 0x1F) << 8) | (0+2 & 0xFF); 

  sendpacket(pkt, 1);

}

cell abi_CMD_FILL_2(cell rgb)
{
  cell pkt[1] = {0};

  pkt[0] = ((rgb & 0x1F) << 24) | ((rgb & 0x3F00) << 8) | ((rgb & 0x1F0000) >> 8) | (0+2 & 0xFF);

  sendpacket(pkt, 1);

}

// FIXME: what the heck with size? Which should be default? Re-check source code and update + text
cell abi_CMD_TEXT(cell* text, cell fontResID, cell x, cell y, cell scale, cell angle, cell r, cell g, cell b, cell size)
{

  cell pkt[3+16] = {0}; 
  size %= 65;











  pkt[0] = ((0+13) & 0xff) | ((((fontResID) & 0xffff)) << (((  1) % 4) * 8)) | ((((x) & 0xff)) << ((( 3) % 4) * 8));
  pkt[1] = (((x) & 0xff00) >> 8) | ((((y) & 0xffff)) << (((  1) % 4) * 8)) | ((((scale) & 0xff)) << ((( 3) % 4) * 8));
  pkt[2] = (((scale) & 0xff00) >> 8) | ((((angle) & 0xffff)) << (((  1) % 4) * 8)) | ((((r & 0x1f) & 0xff)) << (((  3) % 4) * 8));
  pkt[3] = ((g & 0x3f) & 0xff) | ((((b & 0x1f) & 0xff)) << (((  1) % 4) * 8));

  new j = 3;
  for(new i = 0; i < size; i++) {
    if((i + 2) % 4 == 0) j++;
    pkt[j] |= ((((text[i]) & 0xff)) << (((  i + 2) % 4) * 8));
  }


    sendpacket(pkt, j + 1);

}

/*
cell abi_CMD_BITMAP(cell resID, cell x, cell y, cell angle, cell mirror, cell bool g2d)
{
  cell pkt[3] = {0};
  pkt[0] = ((x & 0xFF) << 24) | ((resID & 0xFFFF) << 8) | (0+3 & 0xFF);
  pkt[1] = ((angle & 0xFF) << 24) | ((y & 0xFFFF) << 8) | ((x & 0xFF00) >> 8);
  pkt[2] = ((g2d & 0x01) << 16) | ((mirror & 0xFF) << 8) | ((angle & 0xFF00) >> 8);

  sendpacket(pkt, 3);

}
*/

cell abi_CMD_PLAYSND(cell id, cell volume)
{
  cell pkt[1] = {0};
  pkt[0] = ((volume & 0xFF) << 16) |((id & 0xFF) << 8) | (0+15 & 0xFF);

  sendpacket(pkt, 1);

}

cell abi_CMD_LINE(cell x1, cell y1, cell x2, cell y2, cell R, cell G, cell B)
{
  cell pkt[3] = {0};
  pkt[0] = ((y1 & 0xFF) << 24) | ((x1 & 0xFFFF) << 8) | (0+4 & 0xFF);
  pkt[1] = ((y2 & 0xFF) << 24) | ((x2 & 0xFFFF) << 8) | ((y1 & 0xFF00) >> 8);
  pkt[2] = ((B & 0x1F) << 24) | ((G & 0x3F) << 16) | ((R & 0x1F) << 8) | ((y2 & 0xFF00) >> 8);

  sendpacket(pkt, 3);

}

cell abi_CMD_RECT(cell x1, cell y1, cell x2, cell y2, cell R, cell G, cell B)
{
  cell pkt[3] = {0};
  pkt[0] = ((y1 & 0xFF) << 24) | ((x1 & 0xFFFF) << 8) | (0+5 & 0xFF);
  pkt[1] = ((y2 & 0xFF) << 24) | ((x2 & 0xFFFF) << 8) | ((y1 & 0xFF00) >> 8);
  pkt[2] = ((B & 0x1F) << 24) | ((G & 0x3F) << 16) | ((R & 0x1F) << 8) | ((y2 & 0xFF00) >> 8);

  sendpacket(pkt, 3);

}

cell abi_CMD_NET_TX(cell line_tx, cell TTL, cell* data, cell with_pool)
{
  cell pkt[5] = {0};
  pkt[0] = (((with_pool & 0xFF) << 24) | ((TTL & 0xFF) << 16) | ((line_tx & 0xFF) << 8) | (20+1 & 0xFF)); 
  for(new i=1; i<5; i++) pkt[i] = data[i-1];

  sendpacket(pkt, 5);

}

cell abi_CMD_SAVE_STATE(cell* data, cell size)
{
  if (size > 8)
    return(false);

  cell pkt[8+1] = {0};

  pkt[0] = ((size << 2) << 8) | (0+18 & 0xFF);
  for(new i=0; i<size; i++) pkt[i+1] = data[i];

  sendpacket(pkt, size + 1);

  return(true);
}

cell abi_CMD_LOAD_STATE()
{
  cell pkt[1] = {0};
  pkt[0] = 100+5 & 0xFF;

  sendpacket(pkt, 1);

}

cell abi_CMD_CHANGE_SCRIPT(cell scriptID)
{
  cell pkt[1] = {0};
  pkt[0] = ((scriptID & 0xFF) << 8) | (30+1 & 0xFF);

  sendpacket(pkt, 1);

}

cell abi_CMD_SLEEP()
{
  cell pkt[1] = {0};
  pkt[0] = 0+6 & 0xFF;

  sendpacket(pkt, 1);

}

cell abi_CMD_G2D_BEGIN_BITMAP(cell resID, cell width, cell height, cell bool replace)
{
  cell pkt[2] = {0};
  pkt[0] = ((0+9) & 0xff) | ((((resID) & 0xffff)) << (((  1) % 4) * 8)) | ((((width) & 0xff)) << ((( 3) % 4) * 8));
  pkt[1] = (((width) & 0xff00) >> 8) | ((((height) & 0xffff)) << (((  1) % 4) * 8)) | ((((replace & 0x01) & 0xff)) << (((  3) % 4) * 8));

  sendpacket(pkt, 2);

}

cell abi_CMD_G2D_BEGIN_DISPLAY(cell display, cell bool replace)
{
  cell pkt[1] = {0};
  pkt[0] = ((0+14) & 0xff) | ((((display) & 0xff)) << (((  1) % 4) * 8)) | ((((replace) & 0xff)) << (((  2) % 4) * 8));

  sendpacket(pkt, 1);

}

cell abi_CMD_G2D_ADD_SPRITE(cell resID, cell bool g2d, cell x, cell y, cell alpha, cell color, cell rotation, cell mirror)
{
    cell pkt[4] = {0};
    pkt[0] = ((0+10) & 0xff) | ((((resID) & 0xffff)) << (((  1) % 4) * 8)) | ((((g2d) & 0xff)) << (((  3) % 4) * 8));
    pkt[1] = ((x) & 0xffff) | ((((y) & 0xffff)) << (((  2) % 4) * 8));
    pkt[2] = color;
    pkt[3] = ((alpha) & 0xff) | ((((rotation) & 0xffff)) << (((  1) % 4) * 8)) | ((((mirror) & 0xff)) << (((  3) % 4) * 8));

  sendpacket(pkt, 4);

}

cell abi_CMD_G2D_ADD_RECTANGLE(cell x, cell y, cell width, cell height, cell color)
{
    cell pkt[4] = {0};
    pkt[0] = ((0+11) & 0xff) | ((((x) & 0xffff)) << (((  1) % 4) * 8)) | ((((((y) & 0xff)) & 0xff)) << (((  3) % 4) * 8));
    pkt[1] = (((y) & 0xff00) >> 8) | ((((width) & 0xffff)) << (((  1) % 4) * 8)) | ((((((height) & 0xff)) & 0xff)) << (((  3) % 4) * 8));
    pkt[2] = (((height) & 0xff00) >> 8) | ((color & 0x00FFFFFF) << 8);
    pkt[3] = ((color & 0xFF000000) >> 24);


  sendpacket(pkt, 4);

}

cell abi_CMD_G2D_END()
{
    cell pkt[1] = {0};
    pkt[0] = ((0+12) & 0xff);

  sendpacket(pkt, 1);

}

cell abi_trigger_nightlamp(cell c_mode)
{
  new pkt[1];
  pkt[0] = (((( 0+16) & 0xff)) << (((  0) % 4) * 8)) | ((((c_mode) & 0xff)) << (((  1) % 4) * 8));

  sendpacket(pkt, 1);

}

// FIXME: Not used in pacman
//cell abi_CMD_DYNAMIC_TEXTURE(cell effectId, cell time, cell* args, cell argsCount, cell bool g2d)
//{
//  cell pkt[argsCount+2] = {0};
//  pkt[0] = ((0 + 17) & 0xff) | ((((effectId) & 0xff)) << (((1) % 4) * 8)) | ((((g2d) & 0xff)) << (((2) % 4) * 8));
//  pkt[1] = time;
//  for(new i = 0; i < argsCount; i++)
//    pkt[i + 2] = args[i];
//
//  sendpacket(pkt, argsCount + 2);
//
//}

























new time=0;
new score=0;
new moves=0;
new is_completed;
new total_result;

new count_results=4;
new faceN_results=3;

new result_cubeN;
new result_faceN;


cell Draw_Results(cell _type)
{
  new base;
  new count;

  if (_type == 0)
  {
    abi_CMD_BITMAP(0+_type, 120, 120, 180, 0, 0);
  }
  else if (_type == 1)
  {
    base=10;
    for (count=0; count<10;count++)
    {
      if (moves/base == 0)
        break;
      else
        base*=10;
    }
    base=moves;
    for(new i=0;i<=count;i++)
    {
      abi_CMD_BITMAP(4+base%10,  120, 120  + (22+2)/2*count - (22+2)*i, 90, 0, 0);
      base/=10;
    }
    abi_CMD_BITMAP(0+_type, 70, 120, 90, 0, 0);
  }
  else if (_type == 2)
  {
    score=10000 - time/10 - moves * 10;
    base=10;
    for (count=0; count<10;count++)
    {
      if (score/base == 0)
        break;
      else
        base*=10;
    }
    base=score;
    for(new i=0;i<=count;i++)
    {
      abi_CMD_BITMAP(4+base%10, 120 + (22+2)/2*count - (22+2)*i, 120, 0, 0, 0);
      base/=10;
    }
    abi_CMD_BITMAP(0+_type, 120, 170, 0, 0, 0);
  }
  else if (_type == 3)
  {
    new timer=time/10;

    timer = timer/3600 * 10000 + (timer - timer / 3600 * 3600) / 60 * 100 + timer%60;

    base=10;
    for (count=0; count<10;count++)
    {
      if (timer/base == 0)
        break;
      else
        base*=10;
    }
    base=timer;
    count += count/2; 
    for(new i=0;i<=count;i++)
    {
      if (((i+1)-(i+1)/3*3 == 0) && (i!=0))
      {
        abi_CMD_BITMAP(4+10, 120, 120 - (22+2)/2*count + (22+2)*i, 270, 0, 0);
      }
      else
      {
        abi_CMD_BITMAP(4+base%10, 120, 120 - (22+2)/2*count + (22+2)*i, 270, 0, 0);
        base/=10;
      }
    }
    abi_CMD_BITMAP(0+_type, 170, 120, 270, 0, 0);
  }

}

cell abi_show_results(cell thiscommand, cell toCubeN, cell toFaceN, cell level)
{
  new data[4];
  new neighborCubeN = toCubeN;
  new neighborFaceN = toFaceN;
  new neighborleftCubeN = toCubeN;
  new neighborleftFaceN = toFaceN;
  new counter;
  for (counter=0;counter<4; counter++)
  {
    data[0]= (((thiscommand   & 0xFF) <<  0) |
              ((neighborCubeN & 0xFF) <<  8) |
              ((neighborFaceN & 0xFF) << 16) |
              ((level         & 0xFF) << 24));
    data[1]=time;
    data[2]=moves;
    data[3]=counter;
    if (neighborCubeN==abi_cubeN)
    {
      count_results=counter;
      faceN_results=neighborFaceN;
    }
    else
    {
      abi_CMD_NET_TX(0, 2, data, 0);
      abi_CMD_NET_TX(1, 2, data, 0);
      abi_CMD_NET_TX(2, 2, data, 0);
    }
    neighborleftCubeN = abi_leftCubeN(neighborCubeN, neighborFaceN);
    neighborleftFaceN = abi_leftFaceN(neighborCubeN, neighborFaceN);
    neighborCubeN = neighborleftCubeN;
    neighborFaceN = neighborleftFaceN;
  }
}
cell abi_send_level(cell thiscommand, cell level)
{
  new data[4];

  data[0]= ((thiscommand & 0xFF) <<  0);
  data[1]=level;
  data[2]= 0;
  data[3]= 0;
  abi_CMD_NET_TX(0, 2, data, 0);
  abi_CMD_NET_TX(1, 2, data, 0);
  abi_CMD_NET_TX(2, 2, data, 0);
}

cell abi_respose_to_request_results(cell thiscommand, cell fromCube, cell completed)
{
  new data[4];

  data[0]= (((thiscommand & 0xFF) <<  0) |
            ((fromCube    & 0xFF) <<  8) |
            ((completed   & 0xFF) << 16));

  abi_CMD_NET_TX(0, 2, data, 0); 
  abi_CMD_NET_TX(1, 2, data, 0); 
  abi_CMD_NET_TX(2, 2, data, 0); 
}

cell abi_set_result_from_RX(cell cubeN, cell value, cell total)
{
  total &= ~(1 << cubeN);
  total |=  ((value & 0x1) << cubeN);
}
cell abi_clearing_total(cell total)
{
  if (total==0xFF)
    return;
  new cubeN;
  for(cubeN=0; cubeN<8; cubeN++)
    if (!(abi_topCubeN(cubeN,0)<8))
      total = 0;
















}
cell getCubeFaceToResults(cell thisCubeN, cell thisFaceN, cell cubeN, cell faceN)
{
  new accelX = abi_MTD_GetFaceAccelX(thisFaceN);
  new accelY = abi_MTD_GetFaceAccelY(thisFaceN);
  new accelZ = abi_MTD_GetFaceAccelZ(thisFaceN);
  new neighborCubeN=8;
  new neighborFaceN=3;
  new cubeN_tmp = thisCubeN;
  new faceN_tmp = thisFaceN;

  if (( ABS(accelZ) > ABS(accelX) ) && ( ABS(accelZ) > ABS(accelY) ))
  {
    if (accelZ < 0)
    {
      if ( ABS(accelY) > ABS(accelX) )
      {
        if (accelY < 0)
        {


        }
        else
        {
          neighborCubeN = abi_topCubeN(cubeN_tmp, faceN_tmp);
          neighborFaceN = abi_topFaceN(cubeN_tmp, faceN_tmp);
          if (!((neighborCubeN < 8) && (neighborFaceN < 3)))
            return;
          cubeN_tmp = abi_topCubeN(neighborCubeN, neighborFaceN);
          faceN_tmp = abi_topFaceN(neighborCubeN, neighborFaceN);
          if (!((cubeN_tmp < 8) && (faceN_tmp < 3)))
            return;
        }
      }
      else
      {
        if (accelX < 0)
        {
          neighborCubeN = abi_topCubeN(cubeN_tmp, faceN_tmp);
          neighborFaceN = abi_topFaceN(cubeN_tmp, faceN_tmp);
          if (!((neighborCubeN < 8) && (neighborFaceN < 3)))
            return;
          cubeN_tmp = neighborCubeN;
          faceN_tmp = neighborFaceN;
        }
        else
        {
          neighborCubeN = abi_leftCubeN(cubeN_tmp, faceN_tmp);
          neighborFaceN = abi_leftFaceN(cubeN_tmp, faceN_tmp);
          if (!((neighborCubeN < 8) && (neighborFaceN < 3)))
            return;
          cubeN_tmp = neighborCubeN;
          faceN_tmp = neighborFaceN;
        }
      }
    }
    else
    {

      neighborCubeN = abi_leftCubeN(cubeN_tmp, faceN_tmp);
      neighborFaceN = abi_leftFaceN(cubeN_tmp, faceN_tmp);
      if (!((neighborCubeN < 8) && (neighborFaceN < 3)))
        return;
      cubeN_tmp = neighborCubeN;
      faceN_tmp = neighborFaceN;

      neighborCubeN = abi_leftCubeN(cubeN_tmp, faceN_tmp);
      neighborFaceN = abi_leftFaceN(cubeN_tmp, faceN_tmp);
      if (!((neighborCubeN < 8) && (neighborFaceN < 3)))
        return;
      neighborFaceN = abi_bottomFaceN(neighborCubeN, neighborFaceN);

      cubeN_tmp = neighborCubeN;
      faceN_tmp = neighborFaceN;

      neighborCubeN = abi_leftCubeN(cubeN_tmp, faceN_tmp);
      neighborFaceN = abi_leftFaceN(cubeN_tmp, faceN_tmp);
      if (!((neighborCubeN < 8) && (neighborFaceN < 3)))
        return;

      neighborFaceN = abi_bottomFaceN(neighborCubeN, neighborFaceN);

      cubeN_tmp = neighborCubeN;
      faceN_tmp = neighborFaceN;

      if (ABS(accelY) > ABS(accelX))
      {
        if (accelY > 0)
        {
          neighborCubeN = abi_topCubeN(cubeN_tmp, faceN_tmp);
          neighborFaceN = abi_topFaceN(cubeN_tmp, faceN_tmp);
          if (!((neighborCubeN < 8) && (neighborFaceN < 3)))
            return;
          cubeN_tmp = neighborCubeN;
          faceN_tmp = neighborFaceN;
        }
        else
        {
          neighborCubeN = abi_leftCubeN(cubeN_tmp, faceN_tmp);
          neighborFaceN = abi_leftFaceN(cubeN_tmp, faceN_tmp);
          if (!((neighborCubeN < 8) && (neighborFaceN < 3)))
            return;
          cubeN_tmp = neighborCubeN;
          faceN_tmp = neighborFaceN;
        }
      }
      else
      {
        if (accelX > 0)
        {


        }
        else
        {
          neighborCubeN = abi_leftCubeN(cubeN_tmp, faceN_tmp);
          neighborFaceN = abi_leftFaceN(cubeN_tmp, faceN_tmp);
          if (!((neighborCubeN < 8) && (neighborFaceN < 3)))
            return;
          cubeN_tmp = neighborCubeN;
          faceN_tmp = neighborFaceN;
          neighborCubeN = abi_leftCubeN(cubeN_tmp, faceN_tmp);
          neighborFaceN = abi_leftFaceN(cubeN_tmp, faceN_tmp);
          if (!((neighborCubeN < 8) && (neighborFaceN < 3)))
            return;
          cubeN_tmp = neighborCubeN;
          faceN_tmp = neighborFaceN;
        }
      }
    }
    cubeN = cubeN_tmp;
    faceN = faceN_tmp;
  }
}















new local_script=0;
new SelectedCube; 
new SelectedFace; 

new shaking = 0;
new count_shaking=0;
new packet_num;

new script_ticks;

cell getShake(cell num, cell shift)
{
  return((num >> shift) & 0x1);
}
cell send_info()
{
  packet_num++;
  if (packet_num > 0xFF)
    packet_num=0;

  new data[4];
  data[0] = ((0 + 4 & 0xFF) <<  0 |
             (SelectedCube & 0xFF)                  <<  8 |
             (SelectedFace & 0xFF)                  << 16 |
             (local_script & 0xFF)                  << 24
            );
  data[1] =  (packet_num & 0xFF) << 8;

  abi_CMD_NET_TX(0, 2, data, 0);
  abi_CMD_NET_TX(1, 2, data, 0);
  abi_CMD_NET_TX(2, 2, data, 0);
}

cell check_shake(cell script, cell face)
{
  if (script_ticks<1 * 20)
    return;
  new data[4];
  count_shaking = 0;
  shaking <<= 1;

  if ((ABS(abi_MTD_GetFaceAccelZ(face)) > 120) || 
      (ABS(abi_MTD_GetFaceAccelX(face)) > 120) || 
      (ABS(abi_MTD_GetFaceAccelY(face)) > 120))   
    shaking |= 0x1;

  for (new i=1;i<16;i++)
  {
    if (getShake(shaking,i) != getShake(shaking,i-1))
      count_shaking++;
  }
  if (count_shaking>=6)
  {
    shaking=0;
    if (script == 0) {
      abi_exit();
    } else if (script == 16) {
      abi_trigger_debug_info();
    } else if (script == 14) {
      abi_trigger_bluetooth();
    } else if (script == 15) {
      abi_CMD_SLEEP();
    } else if (script == 17) {
      abi_trigger_nightlamp(1);
    } else if (script == 18) {
      abi_trigger_nightlamp(2);
    } else {

      if (0==abi_cubeN) {
        SelectedCube = 8;
        SelectedFace = 3;
        local_script=script;
        abi_CMD_CHANGE_SCRIPT(local_script);
      } else {
        data[0]=((0 + 5 & 0xFF) <<  0 |
                (script & 0xFF)                      <<  8
                );
        abi_CMD_NET_TX(0, 2, data, 0);
        abi_CMD_NET_TX(1, 2, data, 0);
        abi_CMD_NET_TX(2, 2, data, 0);
      }
    }
  }
}












cell ticks_for_trbl_clear[8] = {0, 0, 0, 0, 0, 0, 0, 0};

new trbl_count_rotations = 0;

new TRBL_rotation_face = 3; 
new TRBL_rotation_side = 0;
cell TRBL_rotation_left_cube[3] = { 8, 8, 8};
cell TRBL_rotation_right_cube[3] = { 8, 8, 8};
cell TRBL_rotation_top_cube[3] = { 8, 8, 8};





cell trbl_clearing()
{
  new cubeN;
  new faceN;
  new thisCubeN;
  new thisFaceN;
  for(thisCubeN=0;thisCubeN<8;thisCubeN++)
  {
    if (thisCubeN==abi_cubeN)
    {
      ticks_for_trbl_clear[thisCubeN]= 0;
      continue;
    }
    if (ticks_for_trbl_clear[thisCubeN]>3)
    {
      abi_TRBL[thisCubeN][0] = 0xFFFFFFFF;
      abi_TRBL[thisCubeN][1] = 0xFFFFFFFF;
      abi_TRBL[thisCubeN][2] = 0xFFFFFFFF;
      ticks_for_trbl_clear[thisCubeN]= 0;
    }
    for(thisFaceN=0;thisFaceN<3;thisFaceN++)
    {
      if (!(abi_leftCubeN(thisCubeN,thisFaceN)<8))
      {
        for(cubeN=0;cubeN<8;cubeN++)
          for(faceN=0;faceN<3;faceN++)
          {

            if ((abi_topCubeN(cubeN,faceN)==thisCubeN) && (abi_topFaceN(cubeN,faceN)==thisFaceN))
            {
              abi_TRBL[cubeN][faceN] = abi_TRBL[cubeN][faceN] | 0xFFFF << 16;
              break;
            }
          }
      }
      if (!(abi_topCubeN(thisCubeN,thisFaceN)<8))
      {
        for(cubeN=0;cubeN<8;cubeN++)
          for(faceN=0;faceN<3;faceN++)
          {

            if ((abi_leftCubeN(cubeN,faceN)==thisCubeN) && (abi_leftFaceN(cubeN,faceN)==thisFaceN))
            {
              abi_TRBL[cubeN][faceN] = abi_TRBL[cubeN][faceN] | 0xFFFF <<  0;
              break;
            }
          }
      }
    }
  }
}
cell trbl_on_tick()
{
  for(new cubeN=0;cubeN<8;cubeN++)
    ticks_for_trbl_clear[cubeN]++;
  trbl_send(abi_cubeN);
  trbl_clearing();
}
cell trbl_send(cell ThisCubeN)
{
  new data[4];
  new thisFaceN;
  new thiscommand = 220+2;
  data[0]= (((thiscommand & 0xFF)           <<  0) |
            ((ThisCubeN    & 0xFF)          <<  8) |
            ((trbl_count_rotations & 0xFF)  << 16));
  for(thisFaceN=0;thisFaceN<3;thisFaceN++)
    data[thisFaceN+1]=abi_TRBL[ThisCubeN][thisFaceN];

  abi_CMD_NET_TX(0, 2, data, 0); 
  abi_CMD_NET_TX(1, 2, data, 0);
  abi_CMD_NET_TX(2, 2, data, 0);
}
cell trbl_init()
{
  for(new cubeN=0;cubeN<8;cubeN++)
    for(new faceN=0;faceN<3;faceN++)
      abi_TRBL[cubeN][faceN] = 0xFFFFFFFF;
}

cell get_rotation_side()
{
  new neighbor_top   = 8;
  for( new faceN=0; faceN<3; faceN++ )
  {
    neighbor_top = get_top_cube(faceN);
    if ((TRBL_rotation_top_cube[faceN] != neighbor_top) && (neighbor_top < 8))
    {

      if (neighbor_top == TRBL_rotation_left_cube[faceN])
      {
        TRBL_rotation_face = faceN; 
        TRBL_rotation_side = 1;
      }
      else if (neighbor_top == TRBL_rotation_right_cube[faceN]) 
      {
        TRBL_rotation_face = faceN; 
        TRBL_rotation_side = 2; 
      }
      else
      {
        TRBL_rotation_face = 3; 
        TRBL_rotation_side = 0; 
      }
    }
    TRBL_rotation_left_cube[faceN]  = 8;
    TRBL_rotation_right_cube[faceN] = 8;
    TRBL_rotation_top_cube[faceN]   = 8;
  }
}
cell get_top_cube(cell c_faceN)
{
  new neighbor_cube = abi_topCubeN(abi_cubeN, c_faceN);
  if (neighbor_cube < 8)
    return ( neighbor_cube);
  return( 8 );
}
cell get_left_cube(cell c_faceN)
{
  new neighbor_cube = abi_leftCubeN(abi_cubeN, c_faceN);
  new neighbor_face = abi_leftFaceN(abi_cubeN, c_faceN);
  if (neighbor_cube < 8)
  {
    neighbor_cube = abi_leftCubeN(neighbor_cube, neighbor_face);
    if (neighbor_cube < 8)
      return( neighbor_cube);
  }
  return( 8);
}
cell get_right_cube(cell c_faceN)
{
  new neighbor_cube = abi_cubeN;
  new neighbor_face = abi_rightFaceN(abi_cubeN, c_faceN);

  neighbor_cube = abi_topCubeN( abi_cubeN, neighbor_face);
  neighbor_face = abi_topFaceN( abi_cubeN, neighbor_face);

  if (neighbor_cube < 8)
  {
    neighbor_cube = abi_topCubeN(neighbor_cube, neighbor_face);
    if (neighbor_cube < 8)
      return( neighbor_cube);
  }
  return( 8);
}
cell update_trbl_record_for_rotation()
{
  new neighbor_top = 8;
  new neighbor_left = 8;
  new neighbor_right = 8;    
  for( new faceN=0; faceN<3; faceN++)
  {

    neighbor_top = get_top_cube(faceN);
    neighbor_left = get_left_cube(faceN);
    neighbor_right = get_right_cube(faceN);

    if ((neighbor_top < 8) && (neighbor_top != TRBL_rotation_top_cube[faceN]) && (TRBL_rotation_top_cube[faceN] >= 8))
      TRBL_rotation_top_cube[faceN] = neighbor_top;
    if ((neighbor_left < 8) && (neighbor_left != TRBL_rotation_left_cube[faceN]) && (TRBL_rotation_left_cube[faceN] >= 8))
      TRBL_rotation_left_cube[faceN] = neighbor_left;
    if ((neighbor_right < 8) && (neighbor_right != TRBL_rotation_right_cube[faceN]) && (TRBL_rotation_right_cube[faceN] >= 8))
      TRBL_rotation_right_cube[faceN] = neighbor_right;
  }
}

cell trbl_clear_after_rotation()
{
  for(new cubeN=0;cubeN<8;cubeN++)
    for(new faceN=0;faceN<3;faceN++)
      if (abi_cubeN != cubeN)
        abi_TRBL[cubeN][faceN] = 0xFFFFFFFF;
}


new _a = 1103515245;
new _c = 12345;
new _m = 2147483647;
new _seed=0;

cell Random(cell min, cell max)
{
    _seed = (_a * _seed + _c) % _m;
    return (min + (_seed % (max - min)));
}

cell Round(cell number, cell base)
{
  if (number*10/base%10 > 5)
  {
    number /= base;
    number+=GetSign(number);
  }
  else 
    number /= base;
}
GetSign (number) 
{
    return number < 0 ? -1 : 1;
}
cell pow(cell a, cell n)
{
    if (!n)
        return 1;

    new
        b = a;

    while (--n != 0)
    {
        b *= a;
    }

    return b;
}
cell int_sqrt(cell x)
{
    new a,b;
    b = x;
    a = x = 0x3f;
    x = b/x;
    a = x = (x+a)>>1;
    x = b/x;
    a = x = (x+a)>>1;
    x = b/x;
    x = (x+a)>>1;
    return(x); 
}

ABS (value) 
{
    return value < 0 ? value * -1 : value; 
}



new tick=0;
new bool initialization=false;
new bool is_rotation=false;

cell run(cell* pkt, cell size, cell* src)
{
  if (initialization==false)
  {
    tick++;
    if (abi_ByteN(pkt, 0) == 100+4)
    {
      abi_MTD_Deserialize(pkt);
      _seed += (ABS(abi_MTD_GetFaceGyroX(0)) + ABS(abi_MTD_GetFaceGyroY(0)) + ABS(abi_MTD_GetFaceGyroZ(0)));
    }

    if (tick<20)
      return;
    initialization=true;



    script_ticks=0;
    trbl_init();
    ON_INIT();
    for(new faceN=0;faceN<3;faceN++)
      abi_TRBL_backup[faceN] = abi_topCubeN(abi_cubeN, faceN);
  }
  switch(abi_ByteN(pkt, 0))
  {
    break;case 100+1:
    {
      trbl_on_tick();
      if ((is_rotation == true) || (abi_check_rotate()))
      {
        is_rotation=false;
        trbl_count_rotations = ((trbl_count_rotations == 0xFF) ? 0 : trbl_count_rotations + 1);
        get_rotation_side();
        ON_CHECK_ROTATE();
        script_ticks=0;
        trbl_clear_after_rotation();
      }
      update_trbl_record_for_rotation();
      ONTICK();
      if (script_ticks<1 * 20)
        script_ticks++; 

    }

    break;case 100+2:
    {
      is_rotation = abi_check_rotate();
      abi_TRBL_Deserialize(pkt);
    }

    break;case 100+3:
    {





      ON_CMD_NET_RX(pkt);
      switch (abi_ByteN(pkt, 4))
      {
        break;case 220+2:
        {

          if ((((abi_ByteN(pkt,6) == 0) && (abi_ByteN(pkt,6) > 200)) || (abi_ByteN(pkt,6) >= trbl_count_rotations)) && (abi_ByteN(pkt,5) != abi_cubeN))


          {

            trbl_count_rotations=abi_ByteN(pkt,6);
            abi_TRBL[abi_ByteN(pkt,5)][0]=pkt[2];
            abi_TRBL[abi_ByteN(pkt,5)][1]=pkt[3];
            abi_TRBL[abi_ByteN(pkt,5)][2]=pkt[4];
            ticks_for_trbl_clear[abi_ByteN(pkt,5)]= 0;
          }
        }

        break;case 0 + 4:
        {
          if (local_script != abi_ByteN(pkt, 7))
          {
            local_script = abi_ByteN(pkt, 7);

            abi_CMD_CHANGE_SCRIPT(local_script);
          }
        }
        break;case 0 + 5:
        {
          if (0 == abi_cubeN)
          {
            if ((local_script != abi_ByteN(pkt, 5)) && (abi_ByteN(pkt, 5) != 16))
            {
              local_script = abi_ByteN(pkt, 5);

              abi_CMD_CHANGE_SCRIPT(local_script);
            }
          }
        }
        break;case 0 + 6:
        {
          if (0 == abi_cubeN)
          {
              abi_CMD_SLEEP();
          }
        }
      }
    }

    break;case 100+4:
    {
      abi_MTD_Deserialize(pkt);
    }

    break;case 100+5:
    {

    }
  }
}


#define RESOURCES_NUMBERS 0
    //const cell RESOURCES_NUMBERS     = 0;
#define RESOURCES_OFFSET      (RESOURCES_NUMBERS + 14) 
#define PACMAN_START_FRAME RESOURCES_NUMBERS + 29
    //const PACMAN_START_FRAME    = RESOURCES_NUMBERS + 29;  
#define PACMAN_MIDDLE_FRAME (PACMAN_START_FRAME + 1)
   //const PACMAN_MIDDLE_FRAME   = PACMAN_START_FRAME + 1; 
#define PACMAN_END_FRAME (PACMAN_START_FRAME + 2)
   //const PACMAN_END_FRAME      = PACMAN_START_FRAME + 2; 
#define GHOST_PICS              (RESOURCES_NUMBERS + 32)  
#define SCARED_GHOSTS           (RESOURCES_NUMBERS + 40)  
#define GHOSTS_EYES             (RESOURCES_NUMBERS + 44)  
#define DEATH_ANIMATION_START   (RESOURCES_NUMBERS + 48)  
#define PACMAN_LIFES_ICON       (RESOURCES_NUMBERS + 55) 

#define FRUITS_ICONS            (RESOURCES_NUMBERS + 43)  
#define IN_GAME_SCORE_LABEL     (RESOURCES_NUMBERS + 58)
#define HIGH_SCORE_LABEL        (RESOURCES_NUMBERS + 59)
#define READY_LABEL             (RESOURCES_NUMBERS + 60)
#define PURPLE_PORTAL           (RESOURCES_NUMBERS + 68)
#define BLUE_PORTAL             (RESOURCES_NUMBERS + 69)
#define FONT                    (RESOURCES_NUMBERS + 70)



#define GAME_OVER_ICON    (RESOURCES_NUMBERS + 10)
#define RED_LINE          (RESOURCES_NUMBERS + 11)
#define GAME_OVER_LABEL   (RESOURCES_NUMBERS + 12)
#define TWIST_TO_PLAY     (RESOURCES_NUMBERS + 13)
#define LEVEL_FINISHED    (RESOURCES_NUMBERS + 61)
#define MOVES_LABEL       (RESOURCES_NUMBERS + 62)
#define SCORES_LABEL      (RESOURCES_NUMBERS + 63)
#define TIME_LABEL        (RESOURCES_NUMBERS + 64)
#define BONUSES_LABEL     (RESOURCES_NUMBERS + 65)
#define LIVES_LABEL       (RESOURCES_NUMBERS + 66)
#define PILLS_LABEL       (RESOURCES_NUMBERS + 67)




#define BLUE_WALL             (RESOURCES_NUMBERS + 14)
#define BLUE_CORNER           (RESOURCES_NUMBERS + 15)
#define BLUE_END              (RESOURCES_NUMBERS + 16)
#define BLUE_CROSS            (RESOURCES_NUMBERS + 17)
#define BLUE_T_SHAPE          (RESOURCES_NUMBERS + 18)
#define PURPLE_CORNER_CLOSE   (RESOURCES_NUMBERS + 19)
#define PURPLE_CORNER_OPEN    (RESOURCES_NUMBERS + 20)
#define PURPLE_SOLID_WALL     (RESOURCES_NUMBERS + 21)
#define PURPLE_WALL           (RESOURCES_NUMBERS + 22)
#define RED_END               (RESOURCES_NUMBERS + 23)
#define RED_WALL              (RESOURCES_NUMBERS + 24)
#define PILL                  (RESOURCES_NUMBERS + 25)
#define ENERGIZER             (RESOURCES_NUMBERS + 26)
#define CHERRY                (RESOURCES_NUMBERS + 27)
#define KEY                   (RESOURCES_NUMBERS + 28)

#define TURN_90_DEGREE      0x100
#define TURN_180_DEGREE     0x200
#define TURN_270_DEGREE     0x300



    const EAT_ENERGIZER_SOUND = 0;
    const EAT_GHOST_SOUND     = 1;
    const EAT_BONUS_SOUND     = 2;
    const PACMAN_DEATH_SOUND  = 3;
    const GAME_START_SOUND    = 4;
    const NEXT_LEVEL_SOUND    = 5;
    const PORT_IN_SOUND       = 6;
    const PORT_OUT_SOUND      = 7;
    const WA_SOUND            = 8;
    const KA_SOUND            = 9;
    const GHOST_PORTAL_SOUND  = 10;



    const CUBE_SHIFT = 29;
    const FACE_SHIFT = 27;
    const SPEED_SHIFT = 24;
    const TRANSFER_CUBE_SHIFT = 9;
    const TRANSFER_FACE_SHIFT = 13;
    const TELEPORT_SHIFT = 15;
    const PORT_OUT_SHIFT = 16;
    const PORT_IN_SHIFT  = 17;
    const SCARE_TIMER_SHIFT = 24;

    const IS_ACTIVE_MASK = 0x00010000;



    const ROOM_CENTER = 120;
    //const PACMAN_SIZE = 96;
#define PACMAN_SIZE 96
#define HALF_ACTOR_SIZE ( PACMAN_SIZE >> 1)
    //const TILE_SIZE = 80;
#define TILE_SIZE 80
#define HALF_TILE_SIZE 40
    //const HALF_TILE_SIZE = TILE_SIZE / 2;
    //const LEVEL_SIZE = 3;
#define LEVEL_SIZE 3
    //const MAX_ACTORS = 5;
#define MAX_ACTORS 5
#define MAX_LEVELS 3
#define PILLS_BEFORE_NEW_GHOST_RELEASE 6

#define MAXIMUM_PILLS 96
#define SECONDS_TO_START_GAME 8
#define SECONDS_TO_START_GAME_DEATH 3
    //const GHOST_SCARE_TIME = 15;
#define GHOST_SCARE_TIME 15
#define GHOST_SCARE_BLINK_TIME ( GHOST_SCARE_TIME * 2/3)
#define MAX_DEATH_ANIM_FRAMES  7
#define ACCEL_MOVE_SENSITIVITY  10
#define OTHER_FACE_TRANSFER_BOUNDRY ( (LEVEL_SIZE - 1) * TILE_SIZE + HALF_TILE_SIZE)
#define TICKS_IN_SECOND 7
#define MOVE_DIR_SWAP_FLAG 3
#define MAX_TELE_IN_ANIM_FRAMES 12
#define MAX_TELE_OUT_ANIM_FRAMES 6
#define DEFAULT_GHOST_SPEED  5
    //const MODULE_GAP = 40;
#define MODULE_GAP 40
#define HALF_MODULE_GAP ( MODULE_GAP >> 1)
#define NUMBERS_ASCII_OFFSET  48



    new leaderBoardText[] = { 'L','E','A','D','E','R','B','O','A','R','D','\0' };
    new pacmanText[] = { 'P','A','C','M','A','N','\0' };
    new twistText[] = { 'T','W','I','S','T','\0' };
    new toText[] = { 'T','O','\0' };
    new startText[] = { 'S','T','A','R','T','\0' };
    new lastText[] = { 'L','A','S','T','\0' };
    new resultsText[] = { 'R','E','S','U','L','T','S','\0' };
    new scoresText[] = { 'S','C','O','R','E','S','\0' };
    new movesText[] = { 'M','O','V','E','S','\0' };
    new blocksText[] = { 'P','I','L','L','S','\0' };



    new drawLeaderboardFlag = 0;
    new lastResultMoves = 0;
    new lastResultScore = 0;
    new lastResultPills = 0;
    new personalRecord  = 0;

#define EMPTY_PART 0
    //const EMPTY_PART          = 0;
#define FIRST_LEADER_GROUP 1
    //const FIRST_LEADER_GROUP  = 1;
#define SECOND_LEADER_GROUP 2
    //const SECOND_LEADER_GROUP = 2;
#define YOUR_RECORD 3
    //const YOUR_RECORD         = 3;
#define LEADERBOARD_SPRITE 4
    //const LEADERBOARD_SPRITE  = 4;
#define GAME_TITLE 5
    //const GAME_TITLE          = 5;
#define LEVEL_PART 6
    //const LEVEL_PART          = 6;
#define LAST_RESULT 7
    //const LAST_RESULT         = 7;
#define LAST_SCORES 8
    //const LAST_SCORES         = 8;
#define  LAST_MOVES 9
    //const LAST_MOVES          = 9;
#define LAST_CHIPS 10
    //const LAST_CHIPS          = 10;

    new leaderBoardTable[] = {
                            GAME_TITLE, EMPTY_PART, LEVEL_PART, EMPTY_PART, 
                            GAME_TITLE, EMPTY_PART, LEVEL_PART, EMPTY_PART, 
                            SECOND_LEADER_GROUP, YOUR_RECORD, LEADERBOARD_SPRITE, FIRST_LEADER_GROUP, 
                            EMPTY_PART, EMPTY_PART, EMPTY_PART, GAME_TITLE, 
                            LAST_RESULT, LAST_SCORES, LAST_MOVES, LAST_CHIPS, 
                            EMPTY_PART, EMPTY_PART, GAME_TITLE, EMPTY_PART  
    };






#define RIGHT_DIR 0
    //const RIGHT_DIR = 0;
#define DOWN_DIR 1
    //const DOWN_DIR  = 1;
#define LEFT_DIR 2
    //const LEFT_DIR  = 2;
#define UP_DIR 3
    //const UP_DIR    = 3;
#define NONE_DIR 4
    //const NONE_DIR  = 4;

new oppositeDirections[] = {LEFT_DIR, UP_DIR, RIGHT_DIR, DOWN_DIR, NONE_DIR};
new munchAnim[] = {PACMAN_START_FRAME, PACMAN_MIDDLE_FRAME, PACMAN_END_FRAME, PACMAN_MIDDLE_FRAME};
new ghostAnim[] = {0, 0, 1, 1};
new faceCubeSide[3] = {0, 0, 0};


new neighbours[] = {0,0, 0,0, 0,0, 0,0};











cell actors[MAX_ACTORS] = {0, 0, 0, 0, 0};
cell lvlInitActors[MAX_ACTORS] = {0, 0, 0, 0, 0};











cell actorsSupportData[MAX_ACTORS] = {0, 0, 2, 4, 6};


new startCube = 8;
new startFace = 3;
new needToCheckRotation = 1;
cell newAngles[3] = {180, 180, 180};

new curFaceZoneOffeset = 0;
new packetNumber = 0;

cell pktActors[MAX_ACTORS] = {0};
new onOppositeCubeSide = 0;
new isTeleportOut[MAX_ACTORS] = {0, 0, 0, 0, 0};
new isLevelFinished = 0;
new isTimerPaused = 0;
new isGameStartSync = 0;
new alreadyTransferedToAnotherCube = 0;
new resaultsAldreadyDrawn = 0;
new levelInitFlag = 0;
new isCountdownLaunch = 0;
new curGameTime = 0;

new startTimer = 0;
new ticksTillStart = 0;


    new currentLevelNumber = 0;
    new ghostAreScared = 0;
    new ghostEaten = 0;
    new ghostReleased = 1;
    new pillsCollected = 0;
    new collectedBonuses = 0;
    new bonuses [20];
    new pacmanLives = 3;
    new pacmanDead = 0;
    new pacmanDeathAnimLaunch = 0;
    new pacmanDeathFrame = 0;
    new changeModule = 0;

    new changeFace = 0;
    new nextFace = 0;



    new actorPosY = TILE_SIZE + HALF_TILE_SIZE, actorPosX = TILE_SIZE + HALF_TILE_SIZE;
    new actorSpeed = 0;
    new actorView = 0;
    new actorAnimFrame = 0;
    new actorTileX = 0;
    new actorTileY = 0;
    new actorCube = 0;
    new actorFace = 0;
    new currentActor = 0;
    new actorSupData = 0;
    new activeActor = 0;
    new actorJustTeleported = 0;
    new actorPortOutAnim = 0;
    new actorPortInAnim  = 0;
    new actorPortAnimAngle = 0;
    new actorTransferCube = 0;
    new actorTransferFace = 0;
    new actorMoveDir = 4;
    new actorTurnDir = 4;

    new wasIEaten = 0;
    new ghostScareTimer = 0;

    new eatSound = 0;


new portalExitCube = 8;
new portalExitFace = 8;
new isPortalActive = 0;

const IS_MINE_MASK = 0x00020000;



cell portals[3] = {0, 0, 0};


cell zonesOnThisModule[][3] = {{0, 0, 0},
                              {0, 0, 0},
                              {0, 0, 0},

                              {0, 0, 0},
                              {0, 0, 0},
                              {0, 0, 0},

                              {0, 0, 0},
                              {0, 0, 0},
                              {0, 0, 0}};




cell zoneType[9*3][3] = { {BLUE_WALL | TURN_180_DEGREE, PILL, BLUE_CORNER | TURN_270_DEGREE},
                      {BLUE_WALL | TURN_180_DEGREE, PILL, PILL},
                      {BLUE_WALL | TURN_180_DEGREE, PILL, BLUE_CORNER},

                      {BLUE_CORNER | TURN_180_DEGREE, PILL,      BLUE_CORNER | TURN_270_DEGREE},
                      {PILL,                          ENERGIZER, PILL},
                      {BLUE_CORNER | TURN_90_DEGREE,  PILL,      BLUE_CORNER},

                      {BLUE_CORNER | TURN_180_DEGREE, PILL,   BLUE_CORNER | TURN_270_DEGREE},
                      {PILL,                          CHERRY, PILL},
                      {BLUE_CORNER | TURN_90_DEGREE,  PILL,   BLUE_CORNER},

                      {BLUE_CORNER | TURN_180_DEGREE, PILL, BLUE_CORNER | TURN_270_DEGREE},
                      {PILL,                          KEY,  PILL},
                      {BLUE_CORNER | TURN_90_DEGREE,  PILL, BLUE_CORNER},

                      {BLUE_CORNER,                   BLUE_WALL | TURN_90_DEGREE, BLUE_CORNER | TURN_90_DEGREE},
                      {BLUE_WALL | TURN_180_DEGREE,   0,                          RED_WALL},
                      {BLUE_CORNER | TURN_270_DEGREE, RED_WALL | TURN_90_DEGREE,  BLUE_CROSS},

                      {BLUE_CORNER | TURN_180_DEGREE, PILL, BLUE_CORNER | TURN_270_DEGREE},
                      {PILL,                          PILL, PILL},
                      {BLUE_CORNER | TURN_90_DEGREE,  PILL, BLUE_CORNER},

                      {PURPLE_WALL | TURN_180_DEGREE, PILL,      PURPLE_CORNER_OPEN | TURN_270_DEGREE},
                      {PURPLE_WALL | TURN_180_DEGREE, ENERGIZER, PILL},
                      {PURPLE_WALL | TURN_180_DEGREE, PILL,      PURPLE_CORNER_OPEN},

                      {PURPLE_CORNER_OPEN | TURN_180_DEGREE, PILL,   PURPLE_CORNER_OPEN | TURN_270_DEGREE},
                      {PILL,                                 CHERRY, PILL},
                      {PURPLE_CORNER_OPEN | TURN_90_DEGREE,  PILL,   PURPLE_CORNER_OPEN},

                      {PURPLE_CORNER_OPEN | TURN_180_DEGREE, PILL,             PURPLE_CORNER_OPEN | TURN_270_DEGREE},
                      {PILL,                                 PURPLE_PORTAL,    PILL},
                      {PURPLE_CORNER_OPEN | TURN_90_DEGREE,  PILL,             PURPLE_CORNER_OPEN}
                        };      



#define ROTATE_90_DEGREE  0x40000000
#define ROTATE_180_DEGREE 0x80000000
#define ROTATE_270_DEGREE 0xC0000000

#define EMPTY   0
#define ANGLE   ( BLUE_CORNER | (PURPLE_CORNER_OPEN << 8))
#define ANGLE_2 ( BLUE_CORNER | (PURPLE_CORNER_CLOSE << 8))
#define WALL    ( BLUE_WALL | (PURPLE_WALL << 8))
#define WALL_2  (BLUE_CROSS | (PURPLE_WALL << 8))
#define SPECIAL  4

#define CROSS_ROOM_PATTERN 0
    //const CROSS_ROOM_PATTERN   = 0;
#define T_SHAPE_ROOM_PATTERN 1
    //const T_SHAPE_ROOM_PATTERN = 1;
#define ANGLE_ROOM_PATTERN 2
    //const ANGLE_ROOM_PATTERN   = 2;
#define TUNNEL_ROOM_PATTERN 3
    //const TUNNEL_ROOM_PATTERN  = 3;
#define GHOST_ROOM_PATTERN   4
#define ROOM_PATTERN_COUNT   5


#define BLUE    0x00
#define PURPLE  0x10


#define  _0_    0x00
#define  _90_   0x40
#define  _180_  0x80
#define  _270_  0xC0

    cell roomPatterns[5*3][3] = {
                            {ANGLE | ROTATE_180_DEGREE, PILL,     ANGLE | ROTATE_270_DEGREE},
                            {PILL,                      SPECIAL,  PILL  },
                            {ANGLE | ROTATE_90_DEGREE, PILL,     ANGLE},


                            {ANGLE | ROTATE_180_DEGREE, PILL,    ANGLE | ROTATE_270_DEGREE},
                            {PILL,                      SPECIAL, PILL},
                            {WALL,                      WALL,    WALL},

                            {ANGLE_2,                  WALL | ROTATE_180_DEGREE, WALL| ROTATE_180_DEGREE},
                            {WALL | ROTATE_270_DEGREE, SPECIAL,                  PILL},
                            {WALL | ROTATE_270_DEGREE, PILL,                     ANGLE},

                            {WALL | ROTATE_180_DEGREE, WALL | ROTATE_180_DEGREE, WALL| ROTATE_180_DEGREE},
                            {PILL,                     SPECIAL,                  PILL},
                            {WALL,                     WALL,                     WALL},

                            {BLUE_CORNER,                     BLUE_WALL, BLUE_CORNER | ROTATE_90_DEGREE},
                            {BLUE_WALL | ROTATE_90_DEGREE,    EMPTY,     RED_WALL | ROTATE_90_DEGREE},
                            {BLUE_CORNER | ROTATE_270_DEGREE, RED_WALL,  BLUE_CROSS}
                        };
    cell patternLevelMap[3][8*3] = {

                    {1 | BLUE | _0_ | (PILL << 8),      1 | BLUE | _270_ | (ENERGIZER << 8),   0 | BLUE | _0_ | (CHERRY << 8),  
                     4 | BLUE | _0_,                    1 | BLUE | _90_ | (PILL << 8),         0 | BLUE | _0_ | (KEY << 8)      , 
                     1 | BLUE | _180_ | (KEY << 8),     0 | BLUE | _0_ | (PURPLE_PORTAL << 8), 1 | BLUE | _180_ | (ENERGIZER << 8), 
                     1 | BLUE | _180_ | (CHERRY << 8),  4 | BLUE | _0_,                        1 | BLUE | _90_ | (PILL << 8)     , 
                     2 | BLUE | _0_ | (CHERRY << 8),    2 | BLUE | _0_ | (PURPLE_PORTAL << 8), 2 | BLUE | _0_ | (KEY << 8)      , 
                     0 | BLUE | _0_ | (PILL << 8),      0 | BLUE | _0_ | (PILL << 8),          4 | BLUE | _0_                   , 
                     1 | BLUE | _180_ | (PILL << 8),    0 | BLUE | _0_ | (CHERRY << 8),        2 | BLUE | _0_ | (ENERGIZER << 8)   , 
                     4 | BLUE | _0_,                    0 | BLUE | _0_ | (KEY << 8),           0 | BLUE | _90_ | (PILL << 8)     },

                    {0 | BLUE | _0_ | (PILL << 8),      1 | BLUE | _90_ | (ENERGIZER << 8),    2 | BLUE | _0_ | (CHERRY << 8),  
                     4 | BLUE | _0_,                    1 | BLUE | _180_ | (PILL << 8),        1 | BLUE | _90_ | (KEY << 8)      , 
                     1 | BLUE | _180_ | (KEY << 8),     1 | BLUE | _180_ | (BLUE_PORTAL << 8), 1 | BLUE | _180_ | (ENERGIZER << 8), 
                     0 | BLUE | _0_ | (CHERRY << 8),    4 | BLUE | _0_,                        1 | BLUE | _90_ | (PILL << 8)     , 
                     2 | BLUE | _0_ | (ENERGIZER << 8), 2 | BLUE | _0_ | (BLUE_PORTAL << 8),   2 | BLUE | _0_ | (KEY << 8)      , 
                     0 | BLUE | _0_ | (PILL << 8),      1 | BLUE | _90_ | (PILL << 8),         4 | BLUE | _0_                   , 
                     1 | BLUE | _180_ | (PILL << 8),    1 | BLUE | _90_ | (CHERRY << 8),       2 | BLUE | _0_ | (ENERGIZER << 8)   , 
                     4 | BLUE | _0_,                    0 | BLUE | _0_ | (KEY << 8),           0 | BLUE | _0_ | (PILL << 8)     },

                    {1 | BLUE | _180_ | (PILL << 8),    2 | BLUE | _0_ | (ENERGIZER << 8),     1 | BLUE | _90_ | (PURPLE_PORTAL << 8), 
                     4 | BLUE | _0_,                    1 | BLUE | _180_ | (CHERRY << 8),      1 | BLUE | _90_ | (KEY << 8)      , 
                     1 | BLUE | _90_ | (CHERRY << 8),   1 | BLUE | _90_ | (KEY << 8),          1 | BLUE | _90_ | (ENERGIZER << 8), 
                     2 | BLUE | _0_ | (ENERGIZER << 8), 4 | BLUE | _0_,                        1 | BLUE | _90_ | (KEY << 8)     , 
                     3 | BLUE | _90_ | (ENERGIZER << 8),2 | BLUE | _0_ | (PURPLE_PORTAL << 8), 1 | BLUE | _0_ | (KEY << 8)      , 
                     1 | BLUE | _180_ | (PILL << 8),    0 | BLUE | _0_ | (PILL << 8),          4 | BLUE | _0_                   , 
                     1 | BLUE | _180_ | (PILL << 8),    1 | BLUE | _90_ | (BLUE_PORTAL << 8),  2 | BLUE | _0_ | (CHERRY << 8)   , 
                     4 | BLUE | _0_,                    2 | BLUE | _0_ | (KEY << 8),           0 | BLUE | _0_ | (BLUE_PORTAL << 8)} 
                };






new levelMap[3][24] = { {0,2,1, 4,5,3, 2,8,1, 2,4,5, 1,8,3, 0,5,4, 0,1,2, 4,3,5},
                         {5,7,6, 5,4,3, 7,3,6, 7,5,4, 6,5,3, 4,5,0, 5,6,7, 3,4,5},
                         {5,0,7, 0,4,5, 1,3,5, 4,2,3, 0,3,1, 7,4,0, 6,7,5, 0,2,4}
    };



new maximumPillsOnLevel = 0;




new ghostSpeedOnLevel[MAX_LEVELS] = {0, 5, 7};

new pillTillGhostRelease = 0;
new successfulReceive[MAX_ACTORS] = {0,0,0,0,0};





new oppTile[] = {1,0,1,2};

enum eFSM {
    eFSM_leaderBoard = 1,
    eFSM_countdown,
    eFSM_death,
    eFSM_playing,
    eFSM_results
};

new gameState = eFSM_playing;


Parse (number, xCenterPos, yCenterPos, angle, axis, reverse){
    new power = 1;
    new finder = ABS(number);
    for (;(finder /= 10) != 0;) {
        ++power;
    }
    new xDigitPos = xCenterPos, yDigitPos = yCenterPos;
    new i = 0, step = 1;
    if (!reverse) {
        i = power - 1;
        step = -step;
    }
    do {
        (!axis) ? (xDigitPos = xCenterPos + 10 * power - 24 * i) : (yDigitPos = yCenterPos + 10 * power - 24 * i);

        abi_CMD_BITMAP (RESOURCES_NUMBERS + number % 10, xDigitPos, yDigitPos, angle, 0, 0);

        number /= 10;
        i += step;
    } while ((!reverse) ? (i >= 0) : (i < power));
}

TextParse (number, posX, posY, fontSize, angle) {
    cell numberText[20] = {0};
    new finder = ABS(number);
    new power = 1;
    while ((finder /= 10)) {
        ++power;
    }
    for (new digit = power - 1; digit >= 0; --digit) {
        numberText[digit] = (number % 10) + NUMBERS_ASCII_OFFSET; 
        number /= 10;
    }
    abi_CMD_TEXT(numberText, FONT, posX, posY, fontSize, angle, 255, 255, 255, power + 1);
}

SaveGameState () {
    new saveData [8];

    saveData[0] = currentLevelNumber;


    saveData[1] = (score > personalRecord) ? (score) : (personalRecord);


    saveData[5] = score;
    saveData[6] = moves;
    saveData[7] = pillsCollected;

    abi_CMD_SAVE_STATE (saveData, 8);
}

LOAD_GAME_DATA (const pkt[]) {
    currentLevelNumber = pkt[1];

    personalRecord = pkt[2];

    lastResultScore = pkt[6];
    lastResultMoves = pkt[7];
    lastResultPills = pkt[8];
}

CheckTolerance (firstValue, secondValue, epsilon) {
    return (ABS(firstValue - secondValue) < epsilon) ? (1) : (0);
}

SetGameState (newState) {
    gameState = newState;
}

TileToCoords (tile) {
    return ((tile * TILE_SIZE) + HALF_TILE_SIZE);
}

//GetDirectionTile (cell &directionTileX,cell &directionTileY,cell direction) {
//    switch (direction) {
//        break;case UP_DIR    : { directionTileY -= 1; }
//        break;case DOWN_DIR  : { directionTileY += 1; }
//        break;case RIGHT_DIR : { directionTileX += 1; }
//        break;case LEFT_DIR  : { directionTileX -= 1; }
//        default : { return 0; }
//    }
//    return 1;
//}

GetDirectionTile(cell* directionTileX, cell* directionTileY, cell direction) {
    switch (direction) {
    break; case UP_DIR: { *directionTileY -= 1; }
    break; case DOWN_DIR: { *directionTileY += 1; }
    break; case RIGHT_DIR: { *directionTileX += 1; }
    break; case LEFT_DIR: { *directionTileX -= 1; }
    default: { return 0; }
    }
    return 1;
}

GetRoomAngle (cubeID, faceID) {
    return (patternLevelMap[currentLevelNumber][cubeID * 3 + faceID] >> 6) & 0x3;
}

GetPortalColor (portal) {
    switch(portal) {
        break;case PURPLE_PORTAL : { 

            return 0xE09F;

        }
        break;case BLUE_PORTAL   : { 

            return 0x03DF;

        }
        default : { 

            return 0; 

        }
    }
}

GetRGB (cell color, cell *r, cell *g, cell *b) {
    *r = color >> 11;
    *g = (color >> 5) & 0x3F;
    *b = color & 0x1F;
}

GetIndexBeforeRotation (cell angle, cell *x, cell *y) {
    new i = *x, j = *y;
    switch (angle) {
        break;case 3: {
            *x = j;
            *y = LEVEL_SIZE - 1 - i;
        }
        break;case 2: {
            *x = LEVEL_SIZE - 1 - i;
            *y = LEVEL_SIZE - 1 - j;
        }
        break;case 1: {
            *x = LEVEL_SIZE - 1 - j;
            *y = i;
        }
    }
}

GetPillsDependOnPattern (roomPattern) {
    switch (roomPattern) {
        break;case CROSS_ROOM_PATTERN: {
            return 4;
        }
        break;case T_SHAPE_ROOM_PATTERN: {
            return 3;
        }
        break;case ANGLE_ROOM_PATTERN: {
            return 2;
        }
        break;case TUNNEL_ROOM_PATTERN: {
            return 2;
        }
    }
    return 0;
}

Addscore (scoresToAdd) {
    score += scoresToAdd;
}

GetDiagonalNeighbour (cell *myFace,cell cube,cell face) {

    new topC = abi_topCubeN(cube, face);
    new topF = abi_topFaceN(cube, face);
    if (topC < 8) {
        new neighbour = abi_topCubeN(topC, topF);
        if (neighbour < 8){
            *myFace = abi_topFaceN(topC, topF);
            return neighbour;
        }
    }

    topC = abi_leftCubeN(cube, face);
    topF = abi_leftFaceN(cube, face);
    if (topC < 8) {
        new neighbour = abi_leftCubeN(topC, topF);
        if (neighbour < 8){
            *myFace = abi_leftFaceN(topC, topF);
            return neighbour;
        }
    }
    return 9;
}

cell GetOppositeScreen (cell *cube, cell *face) {
    new tmpCube = 0;
    new tmpFace = 0;

    for (new screenI = 0; screenI < 3; ++screenI) {
        if (screenI != 1) {
            tmpCube = abi_bottomCubeN(*cube, *face);
            tmpFace = abi_bottomFaceN(*cube, *face);
        } else {
            tmpCube = abi_leftCubeN(*cube, *face);
            if (tmpCube > 8)
                return 0;
            tmpFace = abi_leftFaceN(*cube, *face);
        }
        *cube = tmpCube;
        *face = tmpFace;
    }
    return 1;
}


DisassembleActor (who) {
    activeActor = actors[who];


    actorCube = (activeActor >> CUBE_SHIFT) & 0x07;





    actorSupData = actorsSupportData[who];


    actorFace = (activeActor >> FACE_SHIFT) & 0x03;

    currentActor = who;

    actorPosX = (activeActor >> 8) & 0xFF;
    actorPosY = activeActor & 0xFF;

    if (activeActor & 0x00400000) {
        actorPosY *= -1;
    }
    if (activeActor & 0x00800000) {
        actorPosX *= -1;
    }

    actorTileX = actorPosX / TILE_SIZE;
    actorTileY = actorPosY / TILE_SIZE;

    if (actorTileX >= LEVEL_SIZE) {
        actorTileX = LEVEL_SIZE - 1;
    }
    if (actorTileY >= LEVEL_SIZE) {
        actorTileY = LEVEL_SIZE - 1;
    }

    actorSpeed = ((activeActor >> SPEED_SHIFT) & 0x07) + ((currentLevelNumber >> 1) * (who != 0));

    actorMoveDir = (activeActor >> 16) & 0x07;
    actorTurnDir = (activeActor >> 19) & 0x07;


    actorView      = (actorSupData >> 6) & 0x03;
    actorAnimFrame = (actorSupData >> 4) & 0x03;
    actorJustTeleported = (actorSupData >> TELEPORT_SHIFT) & 0x1;
    actorPortOutAnim = (actorSupData >> PORT_OUT_SHIFT) & 0x1;
    actorPortInAnim  = (actorSupData >> PORT_IN_SHIFT) & 0x1;
    actorPortAnimAngle = (actorSupData >> 18) & 0xF;

    actorTransferCube = (actorSupData >> TRANSFER_CUBE_SHIFT) & 0x0F;
    actorTransferFace = (actorSupData >> TRANSFER_FACE_SHIFT) & 0x03;
    wasIEaten = (actorSupData >> 8) & 0x01;
    ghostScareTimer = (actorSupData >> SCARE_TIMER_SHIFT) & 0xFF;

    eatSound = (actorSupData >> 22) & 0x01;


}

CollectItem (itemID) {
    if ((itemID < PILL) || (itemID > KEY)) {
        return;
    }
    if (itemID > ENERGIZER) {

        abi_CMD_PLAYSND(EAT_BONUS_SOUND, 95);

        new scoresToAdd = 200 + (collectedBonuses * 200);
        bonuses[collectedBonuses++] = itemID;
        Addscore(scoresToAdd);
    } else if (itemID == ENERGIZER) {

        abi_CMD_PLAYSND(EAT_ENERGIZER_SOUND, 95);

        ScareGhosts(ghostAreScared + 1);
        Addscore(50);
    } else if (itemID == PILL) {

        abi_CMD_PLAYSND(WA_SOUND + eatSound, 95);
        eatSound = (++eatSound) % 2;

        Addscore(10);
    }
    ++pillsCollected;


    zonesOnThisModule[actorTileY + curFaceZoneOffeset][actorTileX] = 0;


}

cell PaintOverRect(cell x, cell y)
{

    abi_CMD_G2D_END();

    abi_CMD_G2D_BEGIN_BITMAP(actorFace, 240, 240, false);

    abi_CMD_G2D_ADD_RECTANGLE(x, y, TILE_SIZE, TILE_SIZE, 0xFF000000);

    abi_CMD_G2D_END();

    abi_CMD_G2D_BEGIN_DISPLAY(actorFace, true);

    abi_CMD_G2D_ADD_SPRITE(actorFace,   
                            true,
                            120,       
                            120,       
                            0xFF,    
                            0, 
                            0,               
                            0);
}

IsTileSafe (nextTile) {
    return (!nextTile || (nextTile == PURPLE_PORTAL) || (nextTile == BLUE_PORTAL) || ((nextTile >= PILL) && (nextTile <= KEY)) ||
            (currentActor && wasIEaten && (nextTile == RED_WALL))) ? (1) : (0);
}

Swap (cell *value1, cell *value2) {
    new temp = *value1;
    *value1 = *value2;
    *value2 = temp;
}

ReversePosDependOnSide () {
    Swap ( &actorPosX,  &actorPosY);
    Swap ( &actorTileX,  &actorTileY);

    actorMoveDir ^= 3;

    if (actorCube == neighbours[2]) {
        actorPosX = -actorPosX - MODULE_GAP;

    } else if (actorCube == neighbours[4]) {
        actorPosY = -actorPosY - MODULE_GAP;
    }

    if (actorTileX < 0) {
        actorTileX = 0;
    }
    if (actorTileY < 0) {
        actorTileY = 0;
    }
}

CheckGameOver () {
    if (pacmanDead) {
        DisassembleActor(0);
        ScareGhosts(0);
        pacmanDeathAnimLaunch = 1;
        pacmanDead = 0;

        if (abi_cubeN == actorCube) {
            abi_CMD_PLAYSND(PACMAN_DEATH_SOUND, 95);
        }

        SetGameState (eFSM_death);
    }

    if (pillsCollected == maximumPillsOnLevel) {
        isLevelFinished = 2;
        SetGameState (eFSM_results);
    }
}



CanTurnThisDirection (directionTileX, directionTileY, direction) {
    if (!GetDirectionTile (&directionTileX, &directionTileY, direction)) {
        return 0;
    }
    if ((directionTileX < 0) || (directionTileY < 0) || (directionTileX >= LEVEL_SIZE) || (directionTileY >= LEVEL_SIZE)) {
        return 0;
    }

    new nextTile = zonesOnThisModule[directionTileY + curFaceZoneOffeset][directionTileX] & 0xFF;





    if ((currentActor) && (nextTile == RED_WALL)
    && ((direction == RIGHT_DIR) || (direction == DOWN_DIR))
    && ((ghostReleased >= currentActor) || wasIEaten)) {
        return 1;
    }



    return IsTileSafe (nextTile);
}

CheckIfPossibleToMove (turnDir) {



    if (!CanTurnThisDirection(actorTileX, actorTileY, turnDir)) {
        return 0;
    }


    if (actorMoveDir == oppositeDirections[turnDir]) {
        actorMoveDir = turnDir;
    }
    actorTurnDir = turnDir;
    return 1;
}

Turn () {
    new turningPointX = TileToCoords(actorPosX / TILE_SIZE);
    new turningPointY = TileToCoords(actorPosY / TILE_SIZE);

    if (!(CheckTolerance(actorPosX, turningPointX, actorSpeed))
     || !(CheckTolerance(actorPosY, turningPointY, actorSpeed))
     || !(CanTurnThisDirection(actorTileX, actorTileY, actorTurnDir))) {


        return;
    }



    actorPosX = turningPointX;
    actorPosY = turningPointY;

    actorMoveDir = actorTurnDir;

    actorTurnDir = NONE_DIR;
}

cell TransferToOtherCube (cell fromPos) 
{
    if ((actorPosX <= fromPos) || (actorPosY <= fromPos)) {

        new actorGoLeft = actorPosX < TILE_SIZE;
        new actorGoTop  = actorPosY < TILE_SIZE;





        new cube = neighbours[2] * actorGoLeft + neighbours[4] * actorGoTop;

        if (cube < 8) {
            new face = neighbours[3] * actorGoLeft + neighbours[5] * actorGoTop;
            new oppositeRoomAngle = GetRoomAngle(cube, face);








            new oppositeTileX = (actorTileX >= 0) * actorTileX;
            new oppositeTileY = (actorTileY >= 0) * actorTileY;
            GetIndexBeforeRotation (oppositeRoomAngle,  &oppositeTileX,  &oppositeTileY);


            new neighZone = patternLevelMap[currentLevelNumber][cube * 3 + face] & 0xF;





            if (IsTileSafe(roomPatterns[oppositeTileX + LEVEL_SIZE * neighZone][oppositeTileY] & 0xFF)) {

                if (((actorPosX - actorSpeed) < -HALF_MODULE_GAP) || ((actorPosY - actorSpeed) < -HALF_MODULE_GAP)) {
                    changeModule = (cube + 1) * 3 + face;





                }
                return 1;
            }
        }




    }
    return 0;
}

TransferToOtherFace () {

    new transferBoundry = OTHER_FACE_TRANSFER_BOUNDRY - actorSpeed;
    if ((actorPosX >= transferBoundry) || (actorPosY >= transferBoundry)) {
        if ((actorPosX >= (255 - (actorSpeed << 1))) || (actorPosY >= (255 - (actorSpeed << 1)))) {
            changeFace = 1;
        }

        new actorGoRight = actorPosX >= transferBoundry;
        nextFace = abi_rightFaceN(abi_cubeN, actorFace) * actorGoRight + abi_bottomFaceN(abi_cubeN, actorFace) * (!actorGoRight);

        new nextTile = zonesOnThisModule[actorTileX + LEVEL_SIZE * nextFace][actorTileY] & 0xFF;

        if (currentActor && wasIEaten && (nextTile == RED_WALL)) {
            return 1;
        }
        return IsTileSafe(nextTile);
    }
    return 0;
}

cell PacmanMovement()
{
    new accelX = abi_MTD_GetFaceAccelX(actorFace);
    new accelY = abi_MTD_GetFaceAccelY(actorFace);

    new turnDir = NONE_DIR;












    if (       (accelY <= -ACCEL_MOVE_SENSITIVITY)) {
        turnDir = DOWN_DIR;

    } else if ((accelY >=  ACCEL_MOVE_SENSITIVITY)) {
        turnDir = UP_DIR;

    } else if ((accelX >=  ACCEL_MOVE_SENSITIVITY)) {
        turnDir = LEFT_DIR;

    } else if ((accelX <= -ACCEL_MOVE_SENSITIVITY)) {
        turnDir = RIGHT_DIR;

    }

    CheckIfPossibleToMove(turnDir);


    if (!CanTurnThisDirection(actorTileX, actorTileY, actorMoveDir)) {




        if (!TransferToOtherFace() && !TransferToOtherCube(HALF_TILE_SIZE)) {
            if ((CheckTolerance(actorPosX, TileToCoords(actorTileX), actorSpeed)
              && CheckTolerance(actorPosY, TileToCoords(actorTileY), actorSpeed))
             || ((actorPosX < HALF_TILE_SIZE) || (actorPosY < HALF_TILE_SIZE))) {




                    actorView = actorMoveDir;
                    actorMoveDir = NONE_DIR;

            }
        }
    }
    if ((actorTileX >= 0) && (actorTileY >= 0)) {


        CollectItem(zonesOnThisModule[actorTileY + curFaceZoneOffeset][actorTileX] & 0xFF);
    }
}

cell RunActorsLogic (who) 
{
    if (actorCube != abi_cubeN) {
        ReversePosDependOnSide();

        if (IsTileSafe(zonesOnThisModule[actorTileX + curFaceZoneOffeset][actorTileY] & 0xFF)) {
            DrawActor (who, isTeleportOut[who] * 60);
        }
    } else {
        if (!actorJustTeleported) {
            if (currentActor == 0) {
                PacmanMovement();
                SendGeneralInfo();
            } else {
                RunGhostBehaviour ();
            }

            if (actorTurnDir != NONE_DIR) {
                Turn();
            }

            switch (actorMoveDir) {
                break;case UP_DIR    : { actorPosY -= actorSpeed; }
                break;case DOWN_DIR  : { actorPosY += actorSpeed; }
                break;case RIGHT_DIR : { actorPosX += actorSpeed; }
                break;case LEFT_DIR  : { actorPosX -= actorSpeed; }
                default : { 
                            if (!currentActor) {
                                actorAnimFrame = 1;
                            }

                            actorMoveDir = actorView;
                            actorPosX = TileToCoords(actorTileX) * (actorPosX >= HALF_TILE_SIZE) + actorPosX * (actorPosX < HALF_TILE_SIZE);
                            actorPosY = TileToCoords(actorTileY) * (actorPosY >= HALF_TILE_SIZE) + actorPosY * (actorPosY < HALF_TILE_SIZE);
                            }
            }

            if (actorTileX == actorTileY) {
                new portal = portals[actorFace];
                if (portal && (portal & IS_ACTIVE_MASK) && (!actorPortInAnim)) {
                    if (CheckTolerance(actorPosX, TileToCoords(actorTileX), actorSpeed)
                     && CheckTolerance(actorPosY, TileToCoords(actorTileY), actorSpeed))
                    {
                        actorJustTeleported = actorPortOutAnim = actorPortInAnim = 1;
                        isTeleportOut[who] += 1;
                        changeModule = (((portal >> 8) & 0xF) + 1) * 3 + ((portal >> 12) & 0xF);

                        abi_CMD_PLAYSND ((!currentActor) ? (PORT_IN_SOUND) : (GHOST_PORTAL_SOUND), 95);

                    }
                }
            } else {
                actorPortInAnim = 0;
            }

            DrawActor (who, 0);

        } else {
            if (actorPortAnimAngle == MAX_TELE_IN_ANIM_FRAMES) {
                actorJustTeleported = actorPortAnimAngle = actorTransferCube = 0;

                if (!currentActor) {
                    abi_CMD_PLAYSND (PORT_OUT_SOUND, 95);
                }

            } else {
                ++actorPortAnimAngle;
                if (actorPortAnimAngle > MAX_TELE_OUT_ANIM_FRAMES) {
                    DrawActor (who, actorPortAnimAngle * 60);
                }
            }
        }        

        if (changeFace || changeModule) {
            Swap( &actorPosX,  &actorPosY);
            actorMoveDir ^= MOVE_DIR_SWAP_FLAG;
            if (changeFace) {
                actorFace = nextFace;
            } else {




                actorCube = changeModule / 3 - 1;
                actorFace = changeModule % 3;
            }
            changeFace = changeModule = 0;
        }


        actorAnimFrame = ++actorAnimFrame % 4;

        if (actorPosX < 0) {
            actorPosX *= -1;
            activeActor |= 0x00800000;
        } else {
            activeActor &= 0xFF7FFFFF;
        }

        if (actorPosY < 0) {
            actorPosY *= -1;
            activeActor |= 0x00400000;
        } else {
            activeActor &= 0xFFBFFFFF;
        }


        actorsSupportData[currentActor] = (actorSupData & 0x0000000F) |
                                        (ghostScareTimer << SCARE_TIMER_SHIFT) |
                                        (eatSound << 22) |
                                        (actorPortAnimAngle << 18) |
                                        (actorPortInAnim << PORT_IN_SHIFT) |
                                        (actorPortOutAnim << PORT_OUT_SHIFT) |
                                        (actorJustTeleported << TELEPORT_SHIFT) |
                                        (actorTransferFace << TRANSFER_FACE_SHIFT) | 
                                        (abi_cubeN << TRANSFER_CUBE_SHIFT) | 
                                        (wasIEaten << 8) | (actorView << 6) | (actorAnimFrame << 4);

        actors[currentActor] = (activeActor & 0x07C00000) | 
                            (actorCube << CUBE_SHIFT) | (actorFace << FACE_SHIFT) | 
                            (actorTurnDir << 19) | (actorMoveDir << 16) | (actorPosX << 8) | actorPosY;


        SendActor((pktActors[currentActor] += 1) % 0x7FFFFFFF);
    }
}



cell RunGhostBehaviour () 
{
    CheckIfGhostScare();

    actorSpeed <<= wasIEaten;


    actorSpeed >>= (actorView & 0x01);

    new targetPosX = ROOM_CENTER;
    new targetPosY = ROOM_CENTER;



    if (CheckTolerance(TileToCoords(actorTileX), actorPosX, actorSpeed) && 
        CheckTolerance(TileToCoords(actorTileY), actorPosY, actorSpeed)) 
    {
        new targetCube = 8;
        new targetFace = 3;
        GetTargetTile( &targetPosX,  &targetPosY,  &targetCube,  &targetFace);
        if ((actorCube == targetCube) && (actorFace == targetFace)) {
            wasIEaten = 0;
        }
    }

    if ((actors[0] & 0xF8000000) == (activeActor & 0xF8000000)) {
        if (CheckTolerance(actorPosX, (actors[0] >> 8) & 0xFF, actorSpeed << 1) &&
            CheckTolerance(actorPosY, actors[0] & 0xFF, actorSpeed << 1)) 
        {
            if (!wasIEaten) {
                if (!actorJustTeleported) {
                    if (!(actorView & 0x01)) {
                        pacmanDead = 1;
                        --pacmanLives;
                    } else {
                        wasIEaten = 1;
                        actorView = 0;
                        Addscore(300 + (300 * ghostEaten++));

                        abi_CMD_PLAYSND(EAT_GHOST_SOUND, 95);

                    }
                }
            }
        }
    }

    new directions [4];
    new numberOfDirections = -1;

    for (new direction = 0; direction < 4; ++direction) {
        if ((direction != oppositeDirections[actorMoveDir]) && CanTurnThisDirection(actorTileX, actorTileY, direction)) {
            directions[++numberOfDirections] = direction;
        }
    }


    ++numberOfDirections;
    if (numberOfDirections == 0) {




























        if (!TransferToOtherFace() && !TransferToOtherCube(TILE_SIZE)) {
            if (CanTurnThisDirection(actorTileX, actorTileY, oppositeDirections[actorMoveDir])) {

                actorTurnDir = oppositeDirections[actorMoveDir];
            } else {

                actorMoveDir = 4;
            }
        } else {

            actorTurnDir = actorMoveDir;
        }

    } else if (numberOfDirections == 1) {
        actorTurnDir = directions[0];
    } else {

        new idx = 0;
        new targetTileX = targetPosX / TILE_SIZE;
        new targetTileY = targetPosY / TILE_SIZE;
        for (new i = 0; i < numberOfDirections; ++i) {
            new dirTileX = actorTileX;
            new dirTileY = actorTileY;
            GetDirectionTile ( &dirTileX,  &dirTileY, directions[i]);
            if ((dirTileX == targetTileX) && (dirTileY == targetTileY)) {
                idx = i;
            }
        }

        actorTurnDir = directions[idx];
    }
}

cell GetTargetTile (cell *targetPosX, cell *targetPosY, cell *targetCube, cell *targetFace) {
    FindTargetLocation ( targetPosX,  targetPosY,  targetCube,  targetFace);

    new xIndex = 0;
    new yIndex = 0;
    new cubesToCheck = 1;
    new neighbourCube = 0;
    new neighbourFace = 0;

    if (*targetCube == actorCube) {
        if (*targetFace == actorFace) {
            return;
        } else {
            if (abi_rightFaceN(abi_cubeN, actorFace) == *targetFace) {
                xIndex = 1;
                yIndex = 2;
                neighbourFace = (actorFace - 1) % 3;
            } else {
                xIndex = 2;
                yIndex = 1;
                neighbourFace = (actorFace + 1) % 3;
            }
            GetExitTileSameCube ( targetPosX,  targetPosY, xIndex, yIndex, neighbourFace);
        }
    } else {

        if (neighbours[2] == *targetCube) {
            xIndex = 1;
            neighbourCube = neighbours[2];
            neighbourFace = neighbours[3];
        } else if (neighbours[4]  == *targetCube) {
            yIndex = 1;
            neighbourCube = neighbours[4];
            neighbourFace = neighbours[5];
        } else if (neighbours[6] == *targetCube) {
            xIndex = 1;
            cubesToCheck = 2;
            neighbourCube = neighbours[2];
            neighbourFace = neighbours[3];
        } else {

            xIndex = 1;
            yIndex = 2;
            neighbourFace = (actorFace + 1) % 3;
            GetExitTileSameCube ( targetPosX,  targetPosY, xIndex, yIndex, neighbourFace);
            return;
        }
        GetExitTileOtherCube( targetPosX,  targetPosY, xIndex, yIndex, cubesToCheck, neighbourCube, neighbourFace);
    }
}

cell FindGhostRoom (cell *targetCube, cell *targetFace) {
    for (new neighbour = 0; neighbour < 8; neighbour += 2) {
        new cube = neighbours[neighbour];
        for (new faceI = 0; faceI < 3; ++faceI){


            if ((cube < 8) && (patternLevelMap[currentLevelNumber][cube * 3 + faceI] == GHOST_ROOM_PATTERN)) {
                *targetCube = cube;
                *targetFace = faceI;
                return;
            }
        }
    }
    *targetCube = actorCube;
    *targetFace = abi_bottomFaceN(actorCube, actorFace);
}

FindTargetLocation (cell *targetPosX, cell *targetPosY, cell *targetCube, cell *targetFace) {
    *targetPosX = ROOM_CENTER;
    *targetPosY = ROOM_CENTER;

    if (!wasIEaten) {
        new pacman = actors[0];
        new pacmanCube = (pacman >> CUBE_SHIFT) & 0x07;
        new pacmanFace = (pacman >> FACE_SHIFT) & 0x03;
        new pacmanPosX = (pacman >> 8) & 0xFF;
        new pacmanPosY = (pacman & 0xFF);

        if (pacman & 0x00400000) {
            pacmanPosY *= -1;
        }
        if (pacman & 0x00800000) {
            pacmanPosX *= -1;
        }

        *targetCube = pacmanCube;
        *targetFace = pacmanFace;
        switch (currentActor) {
            break;case 1: {
                    *targetPosX = pacmanPosX;
                    *targetPosY = pacmanPosY;
                }
            break;case 2: { 
                    *targetPosX = ROOM_CENTER;
                    *targetPosY = ROOM_CENTER - TILE_SIZE;
                }
            break;case 3: {
                    *targetPosX = ROOM_CENTER - TILE_SIZE;
                    *targetPosY = ROOM_CENTER;
                }
            break;case 4: { 
                    *targetCube = abi_rightCubeN(pacmanCube, pacmanFace);
                    *targetFace = abi_rightFaceN(pacmanCube, pacmanFace);
                }
        }
    } else {

        for (new neighbour = 0; neighbour < 8; neighbour += 2) {
            new cube = neighbours[neighbour];
            for (new faceI = 0; faceI < 3; ++faceI){
                if ((cube < 8) && (patternLevelMap[currentLevelNumber][cube * 3 + faceI] == GHOST_ROOM_PATTERN)) {
                    *targetCube = cube;
                    *targetFace = faceI;
                    return;
                }
            }
        }
        *targetCube = actorCube;
        *targetFace = abi_bottomFaceN(actorCube, actorFace);
    }
}

GetExitTileSameCube (cell *targetPosX, cell *targetPosY,cell xIndex,cell yIndex,cell neighbourFace) 
{

    if (IsTileSafe(zonesOnThisModule[xIndex + LEVEL_SIZE * actorFace][yIndex] & 0xFF)) {

        if (IsTileSafe(zonesOnThisModule[xIndex + LEVEL_SIZE * neighbourFace][yIndex] & 0xFF)) {
            *targetPosX = TileToCoords(xIndex);
            *targetPosY = TileToCoords(yIndex);
            return;
        }
    }
    *targetPosX = TileToCoords(yIndex);
    *targetPosY = TileToCoords(xIndex);
}

GetExitTileOtherCube (cell *targetPosX, cell *targetPosY, cell xIndex, cell yIndex, cell cubesToCheck, cell neighbourCube, cell neighbourFace)
{
    new thisCube = actorCube;
    new thisFace = actorFace;
    new thisRoomAngle = GetRoomAngle(thisCube, thisFace);
    new oppositeRoomAngle = GetRoomAngle(neighbourCube, neighbourFace);

    for (new cubeI = 0; cubeI < cubesToCheck; ++cubeI) {
        if (neighbourCube < 8) {
            new x = xIndex;
            new y = yIndex;
            GetIndexBeforeRotation (thisRoomAngle,  &x,  &y);

            new myTile = roomPatterns[x + LEVEL_SIZE * (patternLevelMap[currentLevelNumber][thisCube * 3 + thisFace] & 0xF)] [y];
            if (IsTileSafe(myTile)) {
                x = yIndex;
                y = xIndex;
                GetIndexBeforeRotation (oppositeRoomAngle,  &x,  &y);

                new neighbourTile = roomPatterns[x + LEVEL_SIZE * (patternLevelMap[currentLevelNumber][neighbourCube * 3 + neighbourFace] & 0xF)] [y];
                if (IsTileSafe(neighbourTile)) {
                    thisCube = neighbourCube;
                    thisFace = neighbourFace;
                    if ((cubesToCheck > 1) && (!cubeI)) {
                            neighbourCube = abi_leftCubeN(thisCube, thisFace);
                            neighbourFace = abi_leftFaceN(thisCube, thisFace);
                            thisRoomAngle = oppositeRoomAngle;
                            oppositeRoomAngle = GetRoomAngle(neighbourCube, neighbourFace);
                    }
                    continue;
                }
            }
        }
        break;
    }

    if ((thisCube == neighbourCube) && (thisFace == neighbourFace)) {
        *targetPosX = TileToCoords(yIndex);
        *targetPosY = TileToCoords(xIndex);
    } else {
        *targetPosX = TileToCoords(xIndex);
        *targetPosY = TileToCoords(yIndex);
    }
}

ScareGhosts (turnOn) 
{
    for (new ghost = 1; ghost < MAX_ACTORS; ++ghost) {
        if (turnOn) {
            new ghostSupData = actorsSupportData[ghost];

            if (!(ghostSupData & 0x00000100)) {

                actorsSupportData[ghost] = (ghostSupData | 0xC0) & 0x00FFFFFF;

                if (!(ghostSupData & 0x00001E00)) {
                    actors[ghost] ^= 0x00020000;
                }
            }
        } else {

            actorsSupportData[ghost] &= 0x00FFFF3F;

            ghostEaten = 0;
        }
    }
    ghostAreScared = turnOn;
}

CheckIfGhostScare () 
{
    if (actorView) {
        new curSecond = ++ghostScareTimer / TICKS_IN_SECOND;
        if (curSecond < GHOST_SCARE_TIME) {
            if (curSecond >= GHOST_SCARE_BLINK_TIME) {
                new second = ghostScareTimer % (TICKS_IN_SECOND >> 1);
                if (second == 0) {
                    actorView ^= 0x02;
                }
            }
        } else {

            ghostScareTimer = actorView = 0;
        }
    }
}



SendGeneralInfo () 
{

    packetNumber = (++packetNumber) % 0x7FFFFFFF;

    new data[4];






    data[0] = 10 + 2 | (ghostAreScared << 8) | (pacmanLives << 16) | (pillsCollected << 24);
    data[1] = ticksTillStart | (collectedBonuses << 16);
    data[2] = score;
    data[3] = packetNumber;

    abi_CMD_NET_TX(0, 2, data, 0);
    abi_CMD_NET_TX(1, 2, data, 0);
    abi_CMD_NET_TX(2, 2, data, 0);
}

cell SendActor (cell actorPktNumber) 
{
    new data[4];

    data[0] = 10 + 1 | (abi_cubeN << 8) | (currentActor << 16);
    data[1] = actors[currentActor];
    data[2] = actorsSupportData[currentActor];

    data[3] = pktActors[currentActor] = actorPktNumber;

    abi_CMD_NET_TX(0, 2, data, 0);
    abi_CMD_NET_TX(1, 2, data, 0);
    abi_CMD_NET_TX(2, 2, data, 0);
}



cell FindNewAngles (cell farCube) 
{
    new angle;
    new neighborCubeN;
    new neighborFaceN;
    new tmp_cubeN = 0;
    new tmp_faceN = 0;
    for (new faceN = 0; faceN < 3; ++faceN) {
        neighborCubeN = farCube;
        neighborFaceN = faceN;
        angle = 180;
        if (farCube == abi_cubeN) {
            newAngles[faceN] = angle;
            faceCubeSide[faceN] = faceN + ((farCube != 0) * 3);
        }
        for (new count = 0; count < 3; ++count) {
            tmp_cubeN = abi_topCubeN(neighborCubeN, neighborFaceN);
            tmp_faceN = abi_topFaceN(neighborCubeN, neighborFaceN);
            angle = (angle += 90) % 360;
            if (tmp_cubeN > 8) {
                return 0; 
            }
            if (tmp_cubeN == abi_cubeN) {
                newAngles[tmp_faceN] = angle;
                faceCubeSide[tmp_faceN] = faceN + ((farCube != 0) * 3);
            }
            neighborCubeN = tmp_cubeN;
            neighborFaceN = tmp_faceN;
        }
    }
    return 1;
}

cell CheckAngles()
{
    new far_cubeN = 0;
    new tmp_cubeN = 0;
    new tmp_faceN = 0;
    new neighborCubeN;
    new neighborFaceN;

    if (!FindNewAngles(far_cubeN)) {
        return;
    }

    neighborCubeN = 0;
    neighborFaceN = 0;

    for (new cube = 0; cube < 4; cube++) {
        if (cube != 2) {
            tmp_cubeN = abi_leftCubeN(neighborCubeN,neighborFaceN);
            tmp_faceN = abi_leftFaceN(neighborCubeN,neighborFaceN);
            if (tmp_cubeN > 8) {
                return;
            }
            neighborCubeN = tmp_cubeN;
            neighborFaceN = tmp_faceN;
        } else {
            tmp_faceN = abi_bottomFaceN(neighborCubeN,neighborFaceN);
            neighborFaceN = tmp_faceN;
        }
    }

    far_cubeN = neighborCubeN;

    if (!FindNewAngles(far_cubeN)) {
        return;
    }

    needToCheckRotation = 0;
}



cell DrawActor(cell who,cell angle)
{
    new sprite = 0;
    if (who == 0) {
        if (!angle) {
            angle = actorMoveDir * 90;
        }
        sprite = munchAnim[actorAnimFrame];
    } else {
        if (!angle) {
            angle = newAngles[actorFace];
        }
        if (actorView & 0x02) {
            sprite = SCARED_GHOSTS + ghostAnim[actorAnimFrame];
        } else {
            if (!wasIEaten) {

                abi_CMD_BITMAP (GHOST_PICS + (actorsSupportData[who] & 0x0F) + ghostAnim[actorAnimFrame], actorPosX, actorPosY, angle, 0, 0);

            }


            sprite = GHOSTS_EYES + ((actorMoveDir - angle / 90) % 4);
        }


    }

    abi_CMD_BITMAP (sprite, actorPosX, actorPosY, angle, 0, 0);

}

DrawLevel (faceI) {

    abi_CMD_FILL (0, 0, 0);
    for (new x = 0; x < LEVEL_SIZE; ++x) {
        new zoneOnFace = LEVEL_SIZE * faceI;
        for (new y = 0; y < LEVEL_SIZE; ++y) {

            new zoneTile = zonesOnThisModule[x + zoneOnFace][y];
            if (zoneTile) {
                abi_CMD_BITMAP (zoneTile & 0xFF,   
                                TileToCoords(y),                        
                                TileToCoords(x),                        
                                ((zoneTile >> 8) & 0x0F) * 90,          
                                0,
                                false);
            }
        }
    }



    new portal = portals[faceI];
    if (portal) {
        new angle = (portal >> 24) & 0xFF;
        portal &= ~IS_ACTIVE_MASK;

        abi_CMD_BITMAP (portal & 0xFF, ROOM_CENTER, ROOM_CENTER, angle << 1, 0, 0);

        new exitCube = (portal >> 8) & 0xF;

        new face = 0;
        new bWasDisconnect = 0;
        new tempCube = abi_cubeN;
        new tempFace = face;
        new topC = tempCube;
        new topF = tempFace;

        do {
            tempCube = abi_cubeN;
            tempFace = face;
            topC = tempCube;
            topF = tempFace;
            for (new cube = 0; cube < 3; ++cube) {
                if (topC >= 8) {
                    ++bWasDisconnect;
                    break;
                }
                if (topC == exitCube) {
                    portal |= IS_ACTIVE_MASK;
                    break;
                }
                tempCube = topC;
                tempFace = topF;
                topC = abi_topCubeN (tempCube, tempFace);
                topF = abi_topFaceN (tempCube, tempFace);
            }
            ++face;
        } while ((face < 3) && !(portal & IS_ACTIVE_MASK));

        if (!bWasDisconnect && (!(portal & IS_ACTIVE_MASK))) {
            portal |= IS_ACTIVE_MASK;
        }

        if (portal & IS_ACTIVE_MASK) {
            portal = (portal & 0x00FFFFFF) | (((++angle) % 180) << 24);
        }
        portals[faceI] = portal;
    }
}


DrawCountdown () {
    new curSecond = ticksTillStart / TICKS_IN_SECOND;
    if (curSecond <= startTimer) {

        abi_CMD_BITMAP ((RESOURCES_NUMBERS + startTimer - curSecond) * (curSecond < startTimer) + READY_LABEL * (curSecond == startTimer),
                            120, 120, 0, 0, 0);

    } else {
        ticksTillStart = isCountdownLaunch = 0;
        SetGameState (eFSM_playing);
    }
}

DrawResults (face) {
    switch (newAngles[face]) {
        break;case 0: { 

                    abi_CMD_BITMAP(MOVES_LABEL, 120, 80,   0, 0, 0); 

                    Parse (moves, 120, 120, 0, 0, 1);
        }
        break;case 90: { 

                    abi_CMD_BITMAP(SCORES_LABEL, 130, 120,  90, 0, 0); 

                    Parse (score, 100, 120,  90, 1, 1); 
        }
        break;case 180: { 
                    if (isLevelFinished == 1) {

                        abi_CMD_BITMAP(GAME_OVER_ICON,  120, 170, 180, 0, 0); 
                        abi_CMD_BITMAP(RED_LINE,        120,  75, 180, 0, 0); 
                        abi_CMD_BITMAP(GAME_OVER_LABEL, 120, 100, 180, 0, 0); 
                        abi_CMD_BITMAP(TWIST_TO_PLAY,   120,  40, 180, 0, 0); 

                    } else {

                        abi_CMD_BITMAP(LEVEL_FINISHED, 120, 120, 180, 0, 0);

                    } 
        }
        break;case 270: { 

                    abi_CMD_BITMAP(PILLS_LABEL, 100, 120, 270, 0, 0); 

                    Parse (pillsCollected, 130, 110, 270, 1, 0); 
        }
    }
    ++resaultsAldreadyDrawn;
}

DrawDeathAnimation (faceI) {
    pacmanDeathFrame = ticksTillStart / (TICKS_IN_SECOND);

    if (pacmanDeathFrame <= MAX_DEATH_ANIM_FRAMES) {
        if ((abi_cubeN == actorCube) && (faceI == actorFace)) {

            abi_CMD_BITMAP (DEATH_ANIMATION_START + pacmanDeathFrame - (pacmanDeathFrame == MAX_DEATH_ANIM_FRAMES), actorPosX, actorPosY, 0, 0, 0);

            SendGeneralInfo();
        } else {
            new angle = newAngles[faceI];




            new x = 0, y = 0;
            new is_y_const = ((angle % 180) == 0);
            for (new live = 0; live < pacmanLives + 1; ++live) {
                if ((pacmanDeathFrame % 2) && (live == pacmanLives)) {
                    continue;
                }
                new temp = ABS(65 + (-240 * (angle > 90)) + live * 55);
                x = temp * is_y_const + 120 * !is_y_const; 
                y = 120 * is_y_const  + temp * !is_y_const;

                abi_CMD_BITMAP (PACMAN_LIFES_ICON, x, y, angle, 0, 0);

            }
        }
    }

    if (pacmanDeathFrame == MAX_DEATH_ANIM_FRAMES) {
        if ((neighbours[2] >= 8) 
         || (neighbours[4] >= 8) 
         || (abi_topCubeN(abi_cubeN, abi_rightFaceN(abi_cubeN, faceI)) >= 8)) {
            isTimerPaused = 1;
            return;
        } else {
            isTimerPaused = 0;
        }
    }

    if (pacmanDeathFrame > MAX_DEATH_ANIM_FRAMES) {
        pacmanDeathAnimLaunch = pacmanDeathFrame = ticksTillStart = 0;
        needToCheckRotation = 1;
        ResetActors ();
        if (pacmanLives <= 0) {
            isLevelFinished = 1;
            SetGameState (eFSM_results);
        } else {
            isCountdownLaunch = startTimer = SECONDS_TO_START_GAME_DEATH;
            SetGameState (eFSM_countdown);
        }
    }
}

DrawLeaderBoard (faceI) {
    abi_CMD_FILL (0, 0, 0);
    new leaderTableFont = 7;
    switch (leaderBoardTable[faceCubeSide[faceI] * 4 + newAngles[faceI] / 90]) {
        break;case FIRST_LEADER_GROUP: {
            //TODO: FIX
                /*abi_CMD_TEXT(['1','\0'],                 FONT, 216, 180, leaderTableFont, 180, 16, 31, 16);
                abi_CMD_TEXT(['A','L','E','X','\0'],     FONT, 190, 180, leaderTableFont, 180, 24, 48, 24);
                abi_CMD_TEXT(['3','4','5','3','4','0','3','\0'], FONT, 100, 180, leaderTableFont, 180, 255, 255, 255);

                abi_CMD_TEXT(['2','\0'],                 FONT, 216, 140, leaderTableFont, 180, 16, 31, 16);
                abi_CMD_TEXT(['Y','O','U','\0'],         FONT, 190, 140, leaderTableFont, 180, 31, 15, 14);
                abi_CMD_TEXT(['2','9','4','3','0','2','3','\0'], FONT, 100, 140, leaderTableFont, 180, 255, 255, 255);

                abi_CMD_TEXT(['3','\0'],                 FONT, 216, 100, leaderTableFont, 180, 16, 31, 16);
                abi_CMD_TEXT(['M','A','X','\0'],         FONT, 190, 100, leaderTableFont, 180, 24, 48, 24);
                abi_CMD_TEXT(['1','2','3','0','4','5','3','\0'], FONT, 100, 100, leaderTableFont, 180, 255, 255, 255);

                abi_CMD_TEXT(['4','\0'],                 FONT, 216,  60, leaderTableFont, 180, 16, 31, 16);
                abi_CMD_TEXT(['I','L','Y','A','\0'],     FONT, 190,  60, leaderTableFont, 180, 24, 48, 24);
                abi_CMD_TEXT(['4','3','4','3','2','9','\0'], FONT, 100, 60, leaderTableFont, 180, 255, 255, 255);

                abi_CMD_TEXT(['5','\0'],                 FONT, 216,  20, leaderTableFont, 180, 16, 31, 16);
                abi_CMD_TEXT(['P','A','V','E','L','\0'], FONT, 190,  20, leaderTableFont, 180, 24, 48, 24);
                abi_CMD_TEXT(['3','3','4','3','2','9','\0'], FONT, 100, 20, leaderTableFont, 180, 255, 255, 255);*/

        }
        break;case SECOND_LEADER_GROUP: {
            //TODO: FIX
                /*abi_CMD_TEXT(['6','\0'],                 FONT,  20, 216, leaderTableFont, 270, 16, 31, 16);
                abi_CMD_TEXT(['A','N','D','R','\0'],     FONT,  20, 190, leaderTableFont, 270, 24, 48, 24);
                abi_CMD_TEXT(['2','9','0','6','6','4','\0'],     FONT,  20, 100, leaderTableFont, 270, 255, 255, 255);

                abi_CMD_TEXT(['7','\0'],                 FONT,  60, 216, leaderTableFont, 270, 16, 31, 16);
                abi_CMD_TEXT(['I','V','A','N','\0'],     FONT,  60, 190, leaderTableFont, 270, 24, 48, 24);
                abi_CMD_TEXT(['2','4','5','3','4','5','\0'],     FONT,  60, 100, leaderTableFont, 270, 255, 255, 255);

                abi_CMD_TEXT(['8','\0'],                 FONT, 100, 216, leaderTableFont, 270, 16, 31, 16);
                abi_CMD_TEXT(['O','L','E','G','\0'],     FONT, 100, 190, leaderTableFont, 270, 24, 48, 24);
                abi_CMD_TEXT(['1','5','8','6','4','3','\0'],     FONT,  100, 100, leaderTableFont, 270, 255, 255, 255);

                abi_CMD_TEXT(['9','\0'],                 FONT, 140, 216, leaderTableFont, 270, 16, 31, 16);
                abi_CMD_TEXT(['D','E','N','\0'],         FONT, 140, 190, leaderTableFont, 270, 24, 48, 24);
                abi_CMD_TEXT(['1','2','5','9','8','6','\0'],     FONT,  140, 100, leaderTableFont, 270, 255, 255, 255);

                abi_CMD_TEXT(['1','0','\0'],             FONT, 180, 232, leaderTableFont, 270, 16, 31, 16);
                abi_CMD_TEXT(['E','V','G','E','N','\0'], FONT, 180, 190, leaderTableFont, 270, 24, 48, 24);
                abi_CMD_TEXT(['7','8','6','7','8','\0'],     FONT,  180, 100, leaderTableFont, 270, 255, 255, 255);*/

        }
        break;case YOUR_RECORD: {


            TextParse (personalRecord, 22, 114, 14, 0);        
            abi_CMD_TEXT("YOU", FONT, 84, 154, 12, 0, 31, 15, 14, 0);
        }
        break;case LEADERBOARD_SPRITE: {
            abi_CMD_BITMAP(GAME_OVER_ICON, 120, 120, 90, 0, 0);
            abi_CMD_TEXT(leaderBoardText, FONT, 40, 10, 10, 90, 255, 255, 255, 0);
            abi_CMD_BITMAP(RED_LINE, 26, 120, 90, 0, 0);
        }
        break;case GAME_TITLE: {
            abi_CMD_TEXT(pacmanText, FONT, 24, 160, 12, 0, 255, 255, 255, 0);
            abi_CMD_TEXT(twistText, FONT, 12,  200, 8, 0, 26, 14, 14, 0);
            abi_CMD_TEXT(toText,    FONT, 102, 200, 8, 0, 26, 14, 14, 0);
            abi_CMD_TEXT(startText, FONT, 144, 200, 8, 0, 26, 14, 14, 0);
        }
        break;case LAST_RESULT: {
            abi_CMD_BITMAP(GAME_OVER_ICON, 120, 120, 180, 0, 0);
            abi_CMD_TEXT(lastText,    FONT, 205, 40, 9, 180, 255, 255, 255, 0);
            abi_CMD_TEXT(resultsText, FONT, 120, 40, 9, 180, 255, 255, 255, 0);
            abi_CMD_BITMAP(RED_LINE, 120, 26, 180, 0, 0);
        }
        break;case LAST_SCORES: {


            TextParse (lastResultScore, 110, 180, 15, 270);
            abi_CMD_TEXT(scoresText, FONT, 150, 180, 10, 270, 31, 15, 14, 0);
        }
        break;case LAST_MOVES: {


            TextParse (lastResultMoves, 90, 110, 15, 0);
            abi_CMD_TEXT(movesText, FONT, 70, 152, 10, 0, 31, 15, 14, 0);
        }
        break;case LAST_CHIPS: {


            TextParse (lastResultPills, 121, 90, 15, 90);
            abi_CMD_TEXT(blocksText, FONT, 88, 60, 10, 90, 31, 15, 14, 0);
        }
    }
    ++drawLeaderboardFlag;
}

ResetActors () 
{
    for (new i = 0; i < MAX_ACTORS; ++i) {
        actors[i] = lvlInitActors[i];
    }
}

InitGhosts (ghost, cube, face) {
    new initGhost = 2127992;

    switch (ghost) {
        break;case 1: {



            initGhost |= (cube << CUBE_SHIFT) | (face << FACE_SHIFT) | ((DEFAULT_GHOST_SPEED + 1) << SPEED_SHIFT);
        }
        break;case 2: {
            initGhost |= (cube << CUBE_SHIFT) | (face << FACE_SHIFT) | ((DEFAULT_GHOST_SPEED) << SPEED_SHIFT);
        }
        break;case 3: {
            initGhost |= (cube << CUBE_SHIFT) | (face << FACE_SHIFT) | (DEFAULT_GHOST_SPEED << SPEED_SHIFT);
        }
        break;case 4: {
            initGhost |= (cube << CUBE_SHIFT) | (face << FACE_SHIFT) | (DEFAULT_GHOST_SPEED << SPEED_SHIFT);
        }
    }
    return initGhost;
}

RememberBackground (faceI) {
    abi_CMD_G2D_BEGIN_BITMAP(faceI, 240, 240, true);
    for (new x = 0; x < LEVEL_SIZE; ++x) {
        new roomColumnOffset = LEVEL_SIZE * faceI;
        for (new y = 0; y < LEVEL_SIZE; ++y) {

            new tile = zonesOnThisModule[x + roomColumnOffset][y];
            if (tile) {
                abi_CMD_G2D_ADD_SPRITE(tile & 0xFF,   
                                       false,
                                       TileToCoords(y),       
                                       TileToCoords(x),       
                                       0xFF,    
                                       0, 
                                       ((tile >> 8) & 0x0F) * 90,               
                                       0);
            }
        }
    }
    abi_CMD_G2D_END();
}

InitLevel () {
    for (new faceI = 0; faceI < 3; ++faceI) {
        new roomOnThisFace = patternLevelMap [currentLevelNumber][abi_cubeN * 3 + faceI];
        new roomPattern    = roomOnThisFace & 0xF;
        new color          = ((roomOnThisFace >> 4) & 0x3) * 8;
        new roomAngle      = (roomOnThisFace >> 6) & 0x3;
        new roomAngleTemp  = (roomAngle % 2) ? (roomAngle ^ 2) : (roomAngle);



        for (new x = 0; x < LEVEL_SIZE; ++x) {
            new roomPatternOffset = LEVEL_SIZE * roomPattern;
            new roomColumnOffset  = LEVEL_SIZE * faceI;
            for (new y = 0; y < LEVEL_SIZE; ++y) {


                new angle = ((roomPatterns[x + roomPatternOffset][y] >> 30) + roomAngle) % 4;
                new roomElement = roomPatterns[x + roomPatternOffset][y] & 0x3FFFFFFF;
                if (roomElement == SPECIAL) {
                    roomElement = (roomOnThisFace >> 8) & 0xFF;
                    if ((roomElement >= PURPLE_PORTAL) && (roomElement <= BLUE_PORTAL)) {
                        portals[faceI] = roomElement;
                    }
                } else {
                    new element = (roomElement >> color) & 0xFF;
                    if (element) {
                        roomElement = element;
                    } else {
                        roomElement = roomElement & 0xFF;
                    }
                }

                new xTemp = x;
                new yTemp = y;
                GetIndexBeforeRotation(roomAngleTemp,  &xTemp,  &yTemp);


                zonesOnThisModule[xTemp + roomColumnOffset][yTemp] = roomElement | (angle << 8);





            }
        }






    }


    new pillsOnThisLevel = 0;
    new meetPortals = 0;
    for (new cubeI = 0; cubeI < 8; ++cubeI) {
        for (new faceI = 0; faceI < 3; ++faceI) {
            new roomPattern = patternLevelMap[currentLevelNumber][cubeI * 3 + faceI];
            new roomSpecial = (roomPattern >> 8) & 0xFF;
            if ((roomSpecial >= PILL) && (roomSpecial <= KEY)) {
                ++pillsOnThisLevel;
            }
            if ((roomSpecial >= PURPLE_PORTAL) && (roomSpecial <= BLUE_PORTAL)) {
                if (abi_cubeN != cubeI) {
                    for (new portalI = 0; portalI < 3; ++portalI) {
                        if (portals[portalI] == roomSpecial) {
                            portals[portalI] |= (cubeI << 8) | (faceI << 12);
                            break;
                        }
                    }
                }
            }
            pillsOnThisLevel += GetPillsDependOnPattern(roomPattern & 0xF);
        }
    }




    maximumPillsOnLevel  = pillsOnThisLevel;
    pillTillGhostRelease = pillsOnThisLevel / 10;

    new actor = 0;

    lvlInitActors[actor++] = 103053432;
    for (new cube = 0; (cube < 8) && (actor < MAX_ACTORS); ++cube) {
        for (new face = 0; face < 3; ++face) {
            if ((patternLevelMap[currentLevelNumber][cube * 3 + face] & 0xF) == GHOST_ROOM_PATTERN) {
                lvlInitActors[actor] = InitGhosts(actor, cube, face);
                ++actor;
                break;
            }
        }
    }

    ResetActors();

    SetGameState (eFSM_countdown);
}

RotateRoom (faceI) {
    new roomColumnOffset  = LEVEL_SIZE * faceI;
    for (new i = 0; i < (LEVEL_SIZE >> 1); ++i) {
        new size = LEVEL_SIZE - 1 - i;
        for (new j = i; j < (size - i); ++j) {

            new temp = zonesOnThisModule[j + roomColumnOffset][i];
            zonesOnThisModule[j + roomColumnOffset][i] = zonesOnThisModule[size + roomColumnOffset][j];
            zonesOnThisModule[size + roomColumnOffset][j] = zonesOnThisModule[size - j + roomColumnOffset][size - i];
            zonesOnThisModule[size - j + roomColumnOffset][size - i] = zonesOnThisModule[i + roomColumnOffset][size - j];
            zonesOnThisModule[i + roomColumnOffset][size - j] = temp;







        }
    }
}

CheckNearestActors (actor) {
    DisassembleActor(actor);
    for (new i = 0; i < 8 - 2; i += 2) {
        if (neighbours[i] == actorCube) {
            if (neighbours[i + 1] == actorFace) {
                return 1;
            }
        }
    }
    if ((isTeleportOut[actor]) && (neighbours[1] == actorFace)) {
        return (isTeleportOut[actor] = (isTeleportOut[actor] += 1) % MAX_TELE_OUT_ANIM_FRAMES);
    }
    return 0;
}

ONTICK () {

    check_shake (0, 0);

    if (needToCheckRotation) {
        CheckAngles ();
    }

    if (resaultsAldreadyDrawn == 3) {
        return;
    }

    CheckGameOver ();

    new intPills = pillsCollected / pillTillGhostRelease;
    if ((intPills > ghostReleased) && (intPills < MAX_ACTORS)) {

        ++ghostReleased;
    }

    if (!isTimerPaused && ((gameState == eFSM_countdown) || (gameState == eFSM_death))) {
        ++ticksTillStart;
    }

    for (new faceI = 0; faceI < 3; ++faceI) {
        neighbours [0] = abi_cubeN;
        neighbours [1] = faceI;
        neighbours [2] = abi_leftCubeN(abi_cubeN, faceI);
        neighbours [3] = abi_leftFaceN(abi_cubeN, faceI);
        neighbours [4] = abi_topCubeN(abi_cubeN, faceI);
        neighbours [5] = abi_topFaceN(abi_cubeN, faceI);
        new diagFace = 0;
        neighbours [6] = GetDiagonalNeighbour(&diagFace, abi_cubeN, faceI);
        neighbours [7] = diagFace;
        DrawLevel (faceI);
        switch (gameState) {
            break;case eFSM_leaderBoard: {
                DrawLeaderBoard (faceI);
            }
            break;case eFSM_death: {
                DrawDeathAnimation (faceI);
            }
            break;case eFSM_results: {
                DrawResults (faceI);
            }
        }
        for (new actor = 0; actor < MAX_ACTORS; ++actor) {
            switch (gameState) {
                break;case eFSM_countdown: {
                    if (CheckNearestActors(actor)){
                        if (actorCube != abi_cubeN) {
                            ReversePosDependOnSide ();
                        }
                        DrawActor (actor, 0);
                    }
                    DrawCountdown ();
                }
                break;case eFSM_playing: {
                    if (CheckNearestActors(actor)) {
                        curFaceZoneOffeset = LEVEL_SIZE * faceI;
                        RunActorsLogic (actor);
                    } else {
                        if ((actorTransferCube == abi_cubeN) && (actorFace == faceI)) {


                            SendActor (pktActors[currentActor]);
                        }
                    }
                }
            }
        }


        abi_CMD_REDRAW (faceI);

    }
}

ResetAllFlags () {
    ghostEaten = collectedBonuses = isGameStartSync = pillsCollected = moves = isLevelFinished = levelInitFlag = 0;
    ticksTillStart = resaultsAldreadyDrawn = packetNumber = curGameTime = 0;
    isCountdownLaunch = ghostReleased = 1;
    startTimer = SECONDS_TO_START_GAME;
    pacmanLives = 3;
}

ON_INIT () {
    abi_CMD_LOAD_STATE ();

    ResetAllFlags ();
    local_script = 6;
    InitLevel ();

    SetGameState (eFSM_leaderBoard);
}

ON_CHECK_ROTATE () {
    ++moves;
    needToCheckRotation = 1;
    drawLeaderboardFlag = 0;

    if (isLevelFinished) {

        if (isLevelFinished == 2) {
            currentLevelNumber = ++currentLevelNumber % MAX_LEVELS;

            if (abi_cubeN == 0) {
                abi_CMD_PLAYSND(NEXT_LEVEL_SOUND, 95);
            }

        }
        SaveGameState ();

        if (isLevelFinished == 1) {
            score = 0;
        }
        ResetAllFlags ();
        ScareGhosts (0);
        InitLevel ();        
    }

    if (gameState == eFSM_leaderBoard) {

        if (abi_cubeN == 0) {
            abi_CMD_PLAYSND(GAME_START_SOUND, 95);
        }

        SetGameState (eFSM_countdown);
    }
}

ON_CMD_NET_RX (cell* pkt) {
    switch (abi_ByteN(pkt, 4)) {
        break;case 10 + 2: {
            new packetNumberReceived = pkt[4];
            if ((packetNumber < packetNumberReceived) || (ABS(packetNumber - packetNumberReceived) > (0x7FFFFFFF >> 1))) {

                packetNumber = packetNumberReceived;

                new data = abi_ByteN(pkt, 6);

                if (pacmanLives > data) {
                    pacmanDead = 1;
                }
                pacmanLives = data;

                data = abi_ByteN(pkt, 5);
                if ((ghostAreScared < data) && !pacmanDead) {
                    ScareGhosts (data);
                }

                data = abi_ByteN(pkt, 7);
                if (pillsCollected < data) {
                    pillsCollected = data;
                }

                data = pkt[2] & 0xFFFF;
                if (ticksTillStart < data) {
                    ticksTillStart = data;
                }

                data = abi_ByteN(pkt, 10);
                if (collectedBonuses < data) {

                    collectedBonuses = data;
                }

                data = pkt[3];
                if (score < data) {
                    score = data;
                }
            }
        }
        break;case 10 + 1: {
            new packetNumberReceived = pkt[4];
            new actor = abi_ByteN(pkt, 6);

            if ((pktActors[actor] < packetNumberReceived) || (ABS(pktActors[actor] - packetNumberReceived) > (0x7FFFFFFF >> 1))) {


                pktActors[actor] = packetNumberReceived;

                actors[actor] = pkt[2];
                actorsSupportData[actor] = pkt[3];
            }
        }
    }
}


