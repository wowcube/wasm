#include "header.h"

typedef unsigned short uint16_t;
#define run pawn_run
#define pow pawn_pow

#include "resources.h"


#define NET_SEND_WITH_TX_POOL 0
#define NET_SEND_WITHOUT_TX_POOL 1
#define REVERS 0 

#define MESSAGE_TYPES 6
#define RESOURCES_OFFSET 0

#define EMPTY  -999

#define BACKGROUND_SPRITE  RESOURCES_OFFSET + 12
#define ARROW  RESOURCES_OFFSET + 13
#define RESOURCES_NUMBERS  RESOURCES_OFFSET + 14
#define GAME_OVER_PIC  RESOURCES_OFFSET + 24
#define RED_LINE_PIC  RESOURCES_OFFSET + 25
#define GAME_OVER_LABEL  RESOURCES_OFFSET + 26
#define PLAY_NEXT_LEVEL_PIC  RESOURCES_OFFSET + 27
#define BEST_CHIP_RESULT_PIC  RESOURCES_OFFSET + 28
#define MOVES_RESULT_PIC  RESOURCES_OFFSET + 29
#define SCORES_RESULT_PIC  RESOURCES_OFFSET + 30



#define LEVEL_COMPLETE_PIC  RESOURCES_OFFSET + 31
#define STEP_COMPLETE_PIC  RESOURCES_OFFSET + 32
#define STEP_COMPLETE_LABEL  RESOURCES_OFFSET + 33
#define PLAY_NEXT_STEP_LABEL  RESOURCES_OFFSET + 34
#define RESOURCES_64x64        RESOURCES_OFFSET + 35


#define RESOURCES_32x32_white   RESOURCES_64x64 + 12
#define RESOURCES_32x32     RESOURCES_32x32_white + 12

#define BACKGROUND_BORDER    RESOURCES_32x32 + 12
#define BACKGROUND      BACKGROUND_BORDER + 1
#define BACKGROUND_SHADOW   BACKGROUND + 1


#define _2_  RESOURCES_OFFSET 0
#define _4_  RESOURCES_OFFSET 1
#define _8_  RESOURCES_OFFSET  2
#define _16_  RESOURCES_OFFSET + 3
#define _32_  RESOURCES_OFFSET + 4
#define _64_  RESOURCES_OFFSET + 5
#define _128_  RESOURCES_OFFSET + 6
#define _256_  RESOURCES_OFFSET + 7
#define _512_  RESOURCES_OFFSET + 8
#define _1024_  RESOURCES_OFFSET + 9
#define _2048_  RESOURCES_OFFSET + 10
#define _4096_  RESOURCES_OFFSET + 11


#define SOUND_VOLUME 95
#define GAME_START_SOUND 0
#define COMBINED_SOUND  1
#define NEW_SWEET_SOUND  2
#define WIN_SOUND  3


#define SCP  110
#define MAX_FRAME_MOVE_ANIM 9
#define MOVE_CHIP_STEP  (SCP + SCP) / MAX_FRAME_MOVE_ANIM
#define MOVE_FROM_END_POS  SCP - MOVE_CHIP_STEP * MAX_FRAME_MOVE_ANIM

#define RED_BACKGROUND 4
#define GREEN_BACKGROUND  5
#define BLUE_BACKGROUND 7
#define MAX_LAST_MESSAGES 4

#define MAX_PARTICLES 12

#define MUSOF 6


enum
{
    black,
    red,
    green,
    yellow,
    blue,
    magenta,
    cyan,
    white,
};

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

    break;

    case 1:
    {
      if(abi_MTD_Gyro[0] > 0) a = abi_MTD_Gyro[0];
      else if(abi_MTD_Gyro[1] > 0) a = -abi_MTD_Gyro[1];
    }

    break;

    case 2:
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
    break;
  
  case 0:
    {
      if(abi_MTD_Accel[0] > 0) a = abi_MTD_Accel[0];
      else if(abi_MTD_Accel[1] > 0) a = -abi_MTD_Accel[1];
    }

    break;
    
    case 1:
    {
      if(abi_MTD_Accel[2] > 0) a = -abi_MTD_Accel[2];
      else if(abi_MTD_Accel[3] > 0) a = abi_MTD_Accel[3];
    }

    break;
    
    case 2:
    {
      if(abi_MTD_Accel[4] > 0) a = abi_MTD_Accel[4];
      else if(abi_MTD_Accel[5] > 0) a = -abi_MTD_Accel[5];
    }
  }

  return a;
}

//cell abi_CMD_REDRAW(cell faceN)
//{
//  cell pkt[1] = {0};
//  pkt[0] = ((faceN & 0xFF) << 8) | (0+1 & 0xFF);
//
//
//  sendpacket(pkt, 1);
//
//}

//cell abi_exit()
//{
//    cell pkt[1] = {0};
//    pkt[0] = (0 + 20) & 0xFF;
//
//
//    sendpacket(pkt, 1);
//
//}

//cell abi_trigger_debug_info()
//{
//  cell pkt[1] = {0};
//  pkt[0] = 0+7 & 0xFF;
//
//
//  sendpacket(pkt, 1);
//
//}

//cell abi_trigger_bluetooth()
//{
//  cell pkt[1] = {0};
//  pkt[0] = 0+8 & 0xFF;
//
//  sendpacket(pkt, 1);
//
//}

//cell abi_CMD_FILL(cell R, cell G, cell B)
//{
//  cell pkt[1] = {0};
//  pkt[0] = ((B & 0x1F) << 24) | ((G & 0x3F) << 16) | ((R & 0x1F) << 8) | (0+2 & 0xFF); 
//
//  sendpacket(pkt, 1);
//
//}

//cell abi_CMD_FILL_2(cell rgb)
//{
//  cell pkt[1] = {0};
//
//  pkt[0] = ((rgb & 0x1F) << 24) | ((rgb & 0x3F00) << 8) | ((rgb & 0x1F0000) >> 8) | (0+2 & 0xFF);
//
//  sendpacket(pkt, 1);
//
//}

//cell abi_CMD_TEXT(cell* text, cell fontResID, cell x, cell y, cell scale, cell angle, cell r, cell g, cell b, cell size)
//{
//
//  cell pkt[3+16] = {0}; 
//  size %= 65;
//
//  pkt[0] = ((0+13) & 0xff) | ((((fontResID) & 0xffff)) << (((  1) % 4) * 8)) | ((((x) & 0xff)) << ((( 3) % 4) * 8));
//  pkt[1] = (((x) & 0xff00) >> 8) | ((((y) & 0xffff)) << (((  1) % 4) * 8)) | ((((scale) & 0xff)) << ((( 3) % 4) * 8));
//  pkt[2] = (((scale) & 0xff00) >> 8) | ((((angle) & 0xffff)) << (((  1) % 4) * 8)) | ((((r & 0x1f) & 0xff)) << (((  3) % 4) * 8));
//  pkt[3] = ((g & 0x3f) & 0xff) | ((((b & 0x1f) & 0xff)) << (((  1) % 4) * 8));
//
//  new j = 3;
//  for(new i = 0; i < size; i++) {
//    if((i + 2) % 4 == 0) j++;
//    pkt[j] |= ((((text[i]) & 0xff)) << (((  i + 2) % 4) * 8));
//  }
//
//    sendpacket(pkt, j + 1);
//}



// cell abi_CMD_BITMAP (cell resID, cell x, cell y, cell angle, cell mirror, cell bool g2d)
// {
//  cell pkt[3] = {0};
//  pkt[0] = ((x & 0xFF) << 24) | ((resID & 0xFFFF) << 8) | (0+3 & 0xFF);
//  pkt[1] = ((angle & 0xFF) << 24) | ((y & 0xFFFF) << 8) | ((x & 0xFF00) >> 8);
//  pkt[2] = ((g2d & 0x01) << 16) | ((mirror & 0xFF) << 8) | ((angle & 0xFF00) >> 8);

//  sendpacket(pkt, 3);
//  }

//cell abi_CMD_PLAYSND(cell id, cell volume)
//{
//  cell pkt[1] = {0};
//  pkt[0] = ((volume & 0xFF) << 16) |((id & 0xFF) << 8) | (0+15 & 0xFF);
//
//  sendpacket(pkt, 1);
//}

//cell abi_CMD_LINE(cell x1, cell y1, cell x2, cell y2, cell R, cell G, cell B)
//{
//  cell pkt[3] = {0};
//  pkt[0] = ((y1 & 0xFF) << 24) | ((x1 & 0xFFFF) << 8) | (0+4 & 0xFF);
//  pkt[1] = ((y2 & 0xFF) << 24) | ((x2 & 0xFFFF) << 8) | ((y1 & 0xFF00) >> 8);
//  pkt[2] = ((B & 0x1F) << 24) | ((G & 0x3F) << 16) | ((R & 0x1F) << 8) | ((y2 & 0xFF00) >> 8);
//
//  sendpacket(pkt, 3);
//}

//cell abi_CMD_RECT(cell x1, cell y1, cell x2, cell y2, cell R, cell G, cell B)
//{
//  cell pkt[3] = {0};
//  pkt[0] = ((y1 & 0xFF) << 24) | ((x1 & 0xFFFF) << 8) | (0+5 & 0xFF);
//  pkt[1] = ((y2 & 0xFF) << 24) | ((x2 & 0xFFFF) << 8) | ((y1 & 0xFF00) >> 8);
//  pkt[2] = ((B & 0x1F) << 24) | ((G & 0x3F) << 16) | ((R & 0x1F) << 8) | ((y2 & 0xFF00) >> 8);
//
//  sendpacket(pkt, 3);
//}

//cell abi_CMD_NET_TX(cell line_tx, cell TTL, cell* data, cell with_pool)
//{
//  cell pkt[5] = {0};
//  pkt[0] = (((with_pool & 0xFF) << 24) | ((TTL & 0xFF) << 16) | ((line_tx & 0xFF) << 8) | (20+1 & 0xFF)); 
//  for(new i=1; i<5; i++) pkt[i] = data[i-1];
//
//  sendpacket(pkt, 5);
//}

//cell abi_CMD_SAVE_STATE(cell* data, cell size)
//{
//  if (size > 8)
//    return(false);
//
//  cell pkt[8+1] = {0};
//
//  pkt[0] = ((size << 2) << 8) | (0+18 & 0xFF);
//  for(new i=0; i<size; i++) pkt[i+1] = data[i];
//
//  sendpacket(pkt, size + 1);
//
//  return(true);
//}

//cell abi_CMD_LOAD_STATE()
//{
//  cell pkt[1] = {0};
//  pkt[0] = 100+5 & 0xFF;
//
//  sendpacket(pkt, 1);
//}

//cell abi_CMD_CHANGE_SCRIPT(cell scriptID)
//{
//  cell pkt[1] = {0};
//  pkt[0] = ((scriptID & 0xFF) << 8) | (30+1 & 0xFF);
//
//  sendpacket(pkt, 1);
//}

//cell abi_CMD_SLEEP()
//{
//  cell pkt[1] = {0};
//  pkt[0] = 0+6 & 0xFF;
//
//  sendpacket(pkt, 1);
//}

//cell abi_CMD_G2D_BEGIN_BITMAP(cell resID, cell width, cell height, cell bool replace)
//{
//  cell pkt[2] = {0};
//  pkt[0] = ((0+9) & 0xff) | ((((resID) & 0xffff)) << (((  1) % 4) * 8)) | ((((width) & 0xff)) << ((( 3) % 4) * 8));
//  pkt[1] = (((width) & 0xff00) >> 8) | ((((height) & 0xffff)) << (((  1) % 4) * 8)) | ((((replace & 0x01) & 0xff)) << (((  3) % 4) * 8));
//
//  sendpacket(pkt, 2);
//}

//cell abi_CMD_G2D_BEGIN_DISPLAY(cell display, cell bool replace)
//{
//  cell pkt[1] = {0};
//  pkt[0] = ((0+14) & 0xff) | ((((display) & 0xff)) << (((  1) % 4) * 8)) | ((((replace) & 0xff)) << (((  2) % 4) * 8));
//
//  sendpacket(pkt, 1);
//}

//cell abi_CMD_G2D_ADD_SPRITE(cell resID, cell bool g2d, cell x, cell y, cell alpha, cell color, cell rotation, cell mirror)
//{
//    cell pkt[4] = {0};
//    pkt[0] = ((0+10) & 0xff) | ((((resID) & 0xffff)) << (((  1) % 4) * 8)) | ((((g2d) & 0xff)) << (((  3) % 4) * 8));
//    pkt[1] = ((x) & 0xffff) | ((((y) & 0xffff)) << (((  2) % 4) * 8));
//    pkt[2] = color;
//    pkt[3] = ((alpha) & 0xff) | ((((rotation) & 0xffff)) << (((  1) % 4) * 8)) | ((((mirror) & 0xff)) << (((  3) % 4) * 8));
//
//  sendpacket(pkt, 4);
//}

//cell abi_CMD_G2D_ADD_RECTANGLE(cell x, cell y, cell width, cell height, cell color)
//{
//    cell pkt[4] = {0};
//    pkt[0] = ((0+11) & 0xff) | ((((x) & 0xffff)) << (((  1) % 4) * 8)) | ((((((y) & 0xff)) & 0xff)) << (((  3) % 4) * 8));
//    pkt[1] = (((y) & 0xff00) >> 8) | ((((width) & 0xffff)) << (((  1) % 4) * 8)) | ((((((height) & 0xff)) & 0xff)) << (((  3) % 4) * 8));
//    pkt[2] = (((height) & 0xff00) >> 8) | ((color & 0x00FFFFFF) << 8);
//    pkt[3] = ((color & 0xFF000000) >> 24);
//
//
//  sendpacket(pkt, 4);
//}

//cell abi_CMD_G2D_END()
//{
//    cell pkt[1] = {0};
//    pkt[0] = ((0+12) & 0xff);
//
//  sendpacket(pkt, 1);
//}

//cell abi_trigger_nightlamp(cell c_mode)
//{
//  new pkt[1];
//  pkt[0] = (((( 0+16) & 0xff)) << (((  0) % 4) * 8)) | ((((c_mode) & 0xff)) << (((  1) % 4) * 8));
//
//  sendpacket(pkt, 1);
//}

//cell abi_CMD_DYNAMIC_TEXTURE(cell effectId, cell time, cell* args,  cell argsCount, cell bool g2d)
//{
//    
//   //  cell pkt[argsCount + 2] = { 0 }; 
//  cell pkt[24] = { 0 }; // Vad : work around
//  pkt[0] = ((0 + 17) & 0xff) | ((((effectId) & 0xff)) << (((1) % 4) * 8)) | ((((g2d) & 0xff)) << (((2) % 4) * 8));
//  pkt[1] = time;
//  for(new i = 0; i < argsCount; i++)
//    pkt[i + 2] = args[i];
//
//  sendpacket(pkt, argsCount + 2);
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
      abi_CMD_BITMAP(0, 120, 120, 180, 0, 0);
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
    abi_CMD_BITMAP(0+_type, 70, 120, 90, 0,0);
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
      abi_CMD_BITMAP(4+base%10, 120 + (22+2)/2*count - (22+2)*i, 120, 0, 0,0);
      base/=10;
    }
    abi_CMD_BITMAP(0+_type, 120, 170, 0, 0,0);
  }
  else if (_type == 3)
  {
    new timer=time/10;

    timer = timer/3600 * 10000 + (timer - timer / 3600 * 3600) / 60 * 100 + timer%60;

    base = 10;
    for (count=0; count<10;count++)
    {
      if (timer/base == 0)
        break;
      else
        base*=10;
    }
    base = timer;
    count += count/2; 
    for(new i=0;i<=count;i++)
    {
      if (((i+1)-(i+1)/3*3 == 0) && (i!=0))
      {
        abi_CMD_BITMAP (4+10, 120, 120 - (22+2)/2*count + (22+2)*i, 270, 0,0);
      }
      else
      {
        abi_CMD_BITMAP (4 + base%10, 120, 120 - (22+2)/2*count + (22+2)*i, 270,0,0);
        base/=10;
      }
    }
    abi_CMD_BITMAP(0+_type, 170, 120, 270, 0,0);
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
      abi_CMD_NET_TX(0, 2, data, NET_SEND_WITH_TX_POOL);
      abi_CMD_NET_TX(1, 2, data, NET_SEND_WITH_TX_POOL);
      abi_CMD_NET_TX(2, 2, data, NET_SEND_WITH_TX_POOL);
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
  data[1] = level;
  data[2] = 0;
  data[3] = 0;
  abi_CMD_NET_TX(0, 2, data, NET_SEND_WITH_TX_POOL);
  abi_CMD_NET_TX(1, 2, data, NET_SEND_WITH_TX_POOL);
  abi_CMD_NET_TX(2, 2, data, NET_SEND_WITH_TX_POOL);
}

cell abi_respose_to_request_results(cell thiscommand, cell fromCube, cell completed)
{
  new data[4];

  data[0]= (((thiscommand & 0xFF) <<  0) |
            ((fromCube    & 0xFF) <<  8) |
            ((completed   & 0xFF) << 16));

  abi_CMD_NET_TX(0, 2, data, NET_SEND_WITH_TX_POOL);
  abi_CMD_NET_TX(1, 2, data, NET_SEND_WITH_TX_POOL);
  abi_CMD_NET_TX(2, 2, data, NET_SEND_WITH_TX_POOL);
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

  abi_CMD_NET_TX(0, 2, data, NET_SEND_WITH_TX_POOL);
  abi_CMD_NET_TX(1, 2, data, NET_SEND_WITH_TX_POOL);
  abi_CMD_NET_TX(2, 2, data, NET_SEND_WITH_TX_POOL);
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
        abi_CMD_NET_TX(0, 2, data, NET_SEND_WITH_TX_POOL);
        abi_CMD_NET_TX(1, 2, data, NET_SEND_WITH_TX_POOL);
        abi_CMD_NET_TX(2, 2, data, NET_SEND_WITH_TX_POOL);
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
      ticks_for_trbl_clear[thisCubeN] = 0;
      continue;
    }
    if (ticks_for_trbl_clear[thisCubeN] > 3)
    {
      abi_TRBL[thisCubeN][0] = 0xFFFFFFFF;
      abi_TRBL[thisCubeN][1] = 0xFFFFFFFF;
      abi_TRBL[thisCubeN][2] = 0xFFFFFFFF;
      ticks_for_trbl_clear[thisCubeN] = 0;
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

  abi_CMD_NET_TX(0, 2, data, NET_SEND_WITH_TX_POOL);
  abi_CMD_NET_TX(1, 2, data, NET_SEND_WITH_TX_POOL);
  abi_CMD_NET_TX(2, 2, data, NET_SEND_WITH_TX_POOL);
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

ON_CMD_NET_RX(const pkt[]);

new tick=0;
new bool initialization = false;
new bool is_rotation = false;

        cell run(cell* pkt, cell size, cell* src)
        {
          if (initialization == false)
          {
            tick++;
            if (abi_ByteN(pkt, 0) == 100 + 4)
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
            break;
            case 100+2:
            {
              is_rotation = abi_check_rotate();
              abi_TRBL_Deserialize(pkt);
            }

            break;
            case 100+3:
            {
              ON_CMD_NET_RX(pkt);
              switch (abi_ByteN(pkt, 4))
              {
                break;
              case 220+2:
                {

                  if ((((abi_ByteN(pkt,6) == 0) && (abi_ByteN(pkt,6) > 200)) ||
                                       (abi_ByteN(pkt,6) >= trbl_count_rotations)) && 
                                                                (abi_ByteN(pkt,5) != abi_cubeN))

                  {

                    trbl_count_rotations=abi_ByteN(pkt,6);
                   abi_TRBL[abi_ByteN(pkt,5)][0]=pkt[2];
                    abi_TRBL[abi_ByteN(pkt,5)][1]=pkt[3];
                    abi_TRBL[abi_ByteN(pkt,5)][2]=pkt[4];
                    ticks_for_trbl_clear[abi_ByteN(pkt, 5)] =  0 ;
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
                break;
                case 0 + 5:
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
                break;
                case 0 + 6:
                {
                  if (0 == abi_cubeN)
                  {
                      abi_CMD_SLEEP();
                 }
               }
              }
            }

            break;
            case 100+4:
            {
             abi_MTD_Deserialize(pkt);
           }

           break;
           case 100+5:
            {

              ON_LOAD_GAME_DATA (pkt);
           }
         }
        }

/*
    const  int EMPTY                 = -999;
// const RESOURCES_OFFSET      = 0;
    const BACKGROUND_SPRITE     = RESOURCES_OFFSET + 12;
    const ARROW                 = RESOURCES_OFFSET + 13;
    const RESOURCES_NUMBERS     = RESOURCES_OFFSET + 14;
    const GAME_OVER_PIC         = RESOURCES_OFFSET + 24;
    const RED_LINE_PIC          = RESOURCES_OFFSET + 25;
    const GAME_OVER_LABEL       = RESOURCES_OFFSET + 26;
    const PLAY_NEXT_LEVEL_PIC   = RESOURCES_OFFSET + 27;
    const BEST_CHIP_RESULT_PIC  = RESOURCES_OFFSET + 28;
    const MOVES_RESULT_PIC      = RESOURCES_OFFSET + 29;
    const SCORES_RESULT_PIC     = RESOURCES_OFFSET + 30;



    const LEVEL_COMPLETE_PIC    = RESOURCES_OFFSET + 31;
    const STEP_COMPLETE_PIC     = RESOURCES_OFFSET + 32;
    const STEP_COMPLETE_LABEL   = RESOURCES_OFFSET + 33;
    const PLAY_NEXT_STEP_LABEL  = RESOURCES_OFFSET + 34;
   // const RESOURCES_64x64       = RESOURCES_OFFSET + 35; 



    const RESOURCES_32x32_white = RESOURCES_64x64 + 12;
    const RESOURCES_32x32 = RESOURCES_32x32_white + 12;

    const BACKGROUND_BORDER = RESOURCES_32x32 + 12;
    const BACKGROUND = BACKGROUND_BORDER + 1;
    const BACKGROUND_SHADOW = BACKGROUND + 1;


    const _2_    = RESOURCES_OFFSET + 0;
    const _4_    = RESOURCES_OFFSET + 1;
    const _8_    = RESOURCES_OFFSET + 2;
    const _16_   = RESOURCES_OFFSET + 3;
    const _32_   = RESOURCES_OFFSET + 4;
    const _64_   = RESOURCES_OFFSET + 5;
    const _128_  = RESOURCES_OFFSET + 6;
    const _256_  = RESOURCES_OFFSET + 7;
    const _512_  = RESOURCES_OFFSET + 8;
    const _1024_ = RESOURCES_OFFSET + 9;
    const _2048_ = RESOURCES_OFFSET + 10;
    const _4096_ = RESOURCES_OFFSET + 11;


    const SOUND_VOLUME     = 95;
    const GAME_START_SOUND = 0;
    const COMBINED_SOUND   = 1;
    const NEW_SWEET_SOUND  = 2;
    const WIN_SOUND        = 3;


const SCP = 110;
const MAX_FRAME_MOVE_ANIM = 9;
const MOVE_CHIP_STEP = (SCP + SCP) / MAX_FRAME_MOVE_ANIM;
const MOVE_FROM_END_POS = SCP - MOVE_CHIP_STEP * MAX_FRAME_MOVE_ANIM;

const RED_BACKGROUND   = 4;
const GREEN_BACKGROUND = 5;
const BLUE_BACKGROUND  = 7;
const MAX_LAST_MESSAGES = 4;

const MAX_PARTICLES = 12;

const MUSOF = 6;

*/

new IS_LEVEL_FINISHED = 0;

cell currentAngles[3] = { 180, 180, 180};
cell newAngles[3] = { 180, 180, 180};



    new bestChip = 0;
    new currentLevel = 0;
    new localScores  = 0;
    new globalScores = 0;
    new isLevelFinished = 0;
    new _90_DegreeRotationCount = 0;
    new _180_DegreeRotationCount = 0;



    new flagResendMoveMsg= 0;
    new resendPck        = 0;
    new resendCube       = 8;
    new resendFace       = 3;
    new resendValue      = 0;
    new resendSenderCube = 8;
    new resendSenderFace = 3;
    new resendDirection  = 0;


new gameStagesComplete = 0;
new isRotateSuccessful = 0;
new soundPlayed = 0;
new curFaceAngle = 0;
new wasRotation = 0;
new pckAppear = 0;
new pckAppearO = 0;
new pckScoreI  [] = {0, 0, 0, 0, 0, 0, 0, 0};
new pckMoveO   = 0;
new pckMoveI [] = {0, 0, 0, 0, 0, 0, 0, 0};
new currLevel[8][3];
cell eachCubeScore[8] = {0, 0, 0, 0, 0, 0, 0, 0};


new diagNeig [] = {9, 9, 9};
new prevTop  [] = {9, 9, 9};
new prevLeft [] = {9, 9, 9};

new timer = 0;
new timeBeforeConfirm = 0;
new freeMessageSlot = -1;
new lastMessages [MAX_LAST_MESSAGES][4];

cell level2048[8][3] = { {EMPTY,   EMPTY,   EMPTY},
                       {EMPTY,   EMPTY,   EMPTY},
                       {EMPTY,   EMPTY,   EMPTY},
                       {EMPTY,   EMPTY,   EMPTY},
                       {EMPTY,   EMPTY,   EMPTY},
                       {EMPTY,   EMPTY,   EMPTY},
                       {EMPTY,   EMPTY,   EMPTY},
                       {EMPTY,   EMPTY,   EMPTY}
};

RandomRnd () {
    return(_seed = (_a * _seed + _c) % _m);
}

cell RandomRange(cell min, cell max)
{	
    return (min + (RandomRnd() % (max - min + 1))); 
}

new stepY = 0;
new stepX = 0;

new particles [MAX_PARTICLES];
new appearRed   = 0;
new appearGreen = 0;
new appearBlue  = 0;



    new animTFA [] = {0,0,0,  0,0,0,   0,0,0,  0,0,0};
    const MOVE_FROM_OFFSET = 3;
    const APPEAR_OFFSET = 6;
    const ARROW_OFFSET = 9;
    const MOVE_TO_TOP = 1;
    const MOVE_TO_LEFT = 2;
    const MOVE_FROM_TOP = 1;
    const MOVE_FROM_LEFT = 2;
    const APPEAR_MERGE = 1;
    const APPEAR_MESSAGE = 2;
    const NO_ANIMATION = 0;


new animationFrame = 0;
new appearFrame[3] = { 0, 0, 0 };
cell previousValue[3] = { 0,0,0 }; // {EMPTY, EMPTY, EMPTY};
new outValue = 0;

cell Parse(cell number, cell xCenterPos, cell yCenterPos, cell angle, cell axis, cell reverse)
{
    new power = 1;
    new finder = ABS(number);
    for (;(finder /= 10) != 0;) {
        ++power;
    }
    new xDigitPos = xCenterPos, yDigitPos = yCenterPos;
    new i = 0, step = 1;
    if (reverse) {
        i = power - 1;
        step = -step;
    }
    do {
        (!axis) ? (xDigitPos = xCenterPos + 10 * power - 24 * i) : (yDigitPos = yCenterPos + 10 * power - 24 * i);
        
        abi_CMD_G2D_ADD_SPRITE (RESOURCES_NUMBERS + number % 10, false, xDigitPos, yDigitPos, 0xFF, 0, angle, 0);

        abi_CMD_BITMAP (RESOURCES_NUMBERS + number % 10, xDigitPos, yDigitPos, angle, 0,0);

        number /= 10;
        i += step;
    } while ((reverse) ? (i >= 0) : (i < power));
}

MoveTo (chipOffset, face, moveTo) {
    new posArrowX = SCP, posArrowY = SCP;
    new posChipX = SCP, posChipY = SCP;
    new arrowAngle = 90, finalPosition;

    if (moveTo == MOVE_TO_TOP) {
        posArrowY = 140 - animationFrame * 3;
        finalPosition = posChipY = SCP - stepY;
        arrowAngle = 180;

    } else if (moveTo == MOVE_TO_LEFT) {
        posArrowX = 140 - animationFrame * 3;
        finalPosition = posChipX = SCP - stepX;
    }
    
    abi_CMD_G2D_ADD_SPRITE (ARROW, false, posArrowX, posArrowY, 0xFF, 0, arrowAngle, 0);

    abi_CMD_BITMAP (ARROW, posArrowX, posArrowY, arrowAngle, 0,0);

    if (finalPosition >= -SCP) {
        
        abi_CMD_G2D_ADD_SPRITE (outValue, false, posChipX, posChipY, 0xFF, 0, curFaceAngle, 0);

        abi_CMD_BITMAP (outValue,  posChipX,  posChipY, curFaceAngle, 0,0);

    }
    (moveTo == MOVE_TO_TOP) ? (stepY += MOVE_CHIP_STEP) : (stepX += MOVE_CHIP_STEP);



    if ((animationFrame >= 20) || 

       (wasRotation > 1)) {


        (moveTo == MOVE_TO_TOP) ? (stepY = 0) : (stepX = 0);
        animTFA [face] = NO_ANIMATION;
    }
}

MoveFrom (chipOffset, face, moveFrom) {
    new prevValue = previousValue[face];
    new posArrowX = SCP, posArrowY = SCP;
    new posChipX = SCP, posChipY = SCP;
    new arrowAngle = 0, finalPosition;

    if (moveFrom == MOVE_FROM_TOP) {
        posArrowX = 80 + animationFrame  * 3;

        finalPosition = posChipX = MOVE_FROM_END_POS + stepX;
        arrowAngle = 270;

    } else if (moveFrom == MOVE_FROM_LEFT) {
        posArrowY = 80 + animationFrame  * 3;

        finalPosition = posChipY = MOVE_FROM_END_POS + stepY;
    }
    if (prevValue != EMPTY) {

        
        abi_CMD_G2D_ADD_SPRITE (chipOffset - 1, false, SCP,      SCP,      0xFF, 0, curFaceAngle, 0);
        abi_CMD_G2D_ADD_SPRITE (chipOffset - 1, false, posChipX, posChipY, 0xFF, 0, curFaceAngle, 0);

        abi_CMD_BITMAP (chipOffset - 1, SCP,      SCP,      curFaceAngle, 0,0);
        abi_CMD_BITMAP (chipOffset - 1, posChipX, posChipY, curFaceAngle, 0,0);

    } else {
        
        abi_CMD_G2D_ADD_SPRITE (ARROW,      false, posArrowX, posArrowY, 0xFF, 0, arrowAngle, 0);
        abi_CMD_G2D_ADD_SPRITE (chipOffset, false, posChipX,  posChipY,  0xFF, 0, curFaceAngle, 0);

        abi_CMD_BITMAP (ARROW,      posArrowX, posArrowY, arrowAngle,   0,0);
        abi_CMD_BITMAP (chipOffset, posChipX,  posChipY,  curFaceAngle, 0,0);

    }
    (moveFrom == MOVE_FROM_TOP) ? (stepX += MOVE_CHIP_STEP) : (stepY += MOVE_CHIP_STEP);

    if ((finalPosition >= SCP) || (wasRotation > 1)) {
        animTFA [face + APPEAR_OFFSET] = APPEAR_MERGE * (prevValue != EMPTY);
        appearFrame[face] = 0;
        animTFA [face + MOVE_FROM_OFFSET] = animTFA [face + ARROW_OFFSET] = NO_ANIMATION;
        (moveFrom == MOVE_FROM_TOP) ? (stepX = 0) : (stepY = 0);
    }
}

DrawResults (face) {
    switch (newAngles[face]) {
        break;case 0: { 
                    
                    abi_CMD_G2D_ADD_SPRITE (MOVES_RESULT_PIC, false, 120, 90, 0xFF, 0, 0, 0);

                    abi_CMD_BITMAP(MOVES_RESULT_PIC, 120, 90,   0, 0,0); 

                    Parse(moves, 110, 130, 0, 0,REVERS); // Vad: it was a 4 arguments only before
        }
        break;case 90: {
                    
                    abi_CMD_G2D_ADD_SPRITE (SCORES_RESULT_PIC, false, 140, 120, 0xFF, 0, 90, 0);

                    abi_CMD_BITMAP(SCORES_RESULT_PIC, 140, 120,  90, 0,0); 

                    Parse(globalScores, 100, 120,  90, 1, REVERS);
        }

        break;case 180: {                               

                    if (isLevelFinished == 2) {
                        
                        abi_CMD_G2D_ADD_SPRITE (STEP_COMPLETE_PIC,    false, 120, 170, 0xFF, 0, 180, 0);
                        abi_CMD_G2D_ADD_SPRITE (RED_LINE_PIC,         false, 120,  75, 0xFF, 0, 180, 0);
                        abi_CMD_G2D_ADD_SPRITE (STEP_COMPLETE_LABEL,  false, 120, 100, 0xFF, 0, 180, 0);
                        abi_CMD_G2D_ADD_SPRITE (PLAY_NEXT_STEP_LABEL, false, 120,  40, 0xFF, 0, 180, 0);

                        abi_CMD_BITMAP(STEP_COMPLETE_PIC,    120, 170, 180, 0,0);
                        abi_CMD_BITMAP(RED_LINE_PIC,         120,  75, 180, 0,0);
                        abi_CMD_BITMAP(STEP_COMPLETE_LABEL,  120, 100, 180, 0,0);
                        abi_CMD_BITMAP(PLAY_NEXT_STEP_LABEL, 120,  40, 180, 0,0);


                    } else {
                        
                        abi_CMD_G2D_ADD_SPRITE (LEVEL_COMPLETE_PIC, false, 120,  120, 0xFF, 0, 180, 0);

                        abi_CMD_BITMAP(LEVEL_COMPLETE_PIC, 120, 120, 180, 0,0);

                    } 
        }
        break;case 270: {
                    
                    abi_CMD_G2D_ADD_SPRITE (BEST_CHIP_RESULT_PIC, false, 90,  120, 0xFF, 0, 270, 0);
                    abi_CMD_G2D_ADD_SPRITE (RESOURCES_64x64 + bestChip, false, 140,  120, 0xFF, 0, 270, 0);

                    abi_CMD_BITMAP(BEST_CHIP_RESULT_PIC, 90, 120, 270, 0,0); 
                    abi_CMD_BITMAP(RESOURCES_64x64 + bestChip, 140, 120, 270, 0,0);

        }
    }
}

DrawUnlockedSweets (start, end, angle, isAngle_270, isAngle_0) {
    for (new sweet = start; sweet < end; ++sweet) {

        abi_CMD_G2D_ADD_SPRITE (RESOURCES_32x32_white + (12 * (sweet <= bestChip)) + sweet, false,
            220 * isAngle_270                  + (24 + ((sweet - MUSOF) * 38)) * isAngle_0,
            (216 - (sweet * 38)) * isAngle_270 + 220 * isAngle_0, 
            0xFF, 0, angle, 0);

        abi_CMD_BITMAP(RESOURCES_32x32_white + (12 * (sweet <= bestChip)) + sweet, 
            220 * isAngle_270                  + (24 + ((sweet - MUSOF) * 38)) * isAngle_0,
            (216 - (sweet * 38)) * isAngle_270 + 220 * isAngle_0, 
            angle, 0,0);

    }
}

cell DrawHUD(cell angle)
{
    if (angle == 270)  {
        DrawUnlockedSweets(0, MUSOF, angle, 1, 0);

    } else if (angle == 0) {
        DrawUnlockedSweets(MUSOF, MUSOF << 1, angle, 0, 1);

    }
}

DrawParticles(cell, cell);
DoRotation(cell);

new devisor = 2;
DrawFace (chipCurrentValue, face) {
    new wasAnimation = 0;

    curFaceAngle = currentAngles[face];

    abi_CMD_G2D_ADD_SPRITE (BACKGROUND_SPRITE, false, 120, 120, 0xFF, 0, 0, 0);

    abi_CMD_BITMAP (BACKGROUND_SPRITE, 120, 120, 0, 0,0);

    if ((animTFA [face + ARROW_OFFSET])) {
        if ((animationFrame >= 20) || (chipCurrentValue != EMPTY) || (wasRotation > 1)) {
            animTFA [face + ARROW_OFFSET] = NO_ANIMATION;
        } else {
            switch (animTFA [face + ARROW_OFFSET]) {

                break;case 1: {
                    
                    abi_CMD_G2D_ADD_SPRITE (ARROW, false, SCP, 140 - animationFrame * 3, 0xFF, 0, 180, 0);

                    abi_CMD_BITMAP (ARROW, SCP, 140 - animationFrame * 3, 180, 0,0);

                }

                break;case 2: {
                    
                    abi_CMD_G2D_ADD_SPRITE (ARROW, false, 140 - animationFrame * 3, SCP, 0xFF, 0, 90, 0);

                    abi_CMD_BITMAP (ARROW, 140 - animationFrame * 3, SCP,  90, 0,0);

                }

                break;case 3: {
                    
                    abi_CMD_G2D_ADD_SPRITE (ARROW, false, SCP, 80 + animationFrame * 3, 0xFF, 0, 0, 0);

                    abi_CMD_BITMAP (ARROW, SCP, 80 + animationFrame * 3,    0, 0,0);

                }

                break;case 4: {
                    
                    abi_CMD_G2D_ADD_SPRITE (ARROW, false,  80 + animationFrame * 3, SCP, 0xFF, 0, 270, 0);

                    abi_CMD_BITMAP (ARROW, 80 + animationFrame * 3, SCP,  270, 0,0);

                }
            }
            wasAnimation = 1;
        }
    }

    if (animTFA [face]){

        MoveTo (chipCurrentValue, face, animTFA [face]);
        wasAnimation = 1;
    }

    if (animTFA [face + MOVE_FROM_OFFSET ]) {

        MoveFrom (chipCurrentValue, face, animTFA [face + MOVE_FROM_OFFSET]);
        wasAnimation = 1;
    }


    if ((animTFA [face + APPEAR_OFFSET]) && (animTFA [face] == animTFA [face + MOVE_FROM_OFFSET])) {
        new localFrame = appearFrame [face];

        if (previousValue[face] != EMPTY) {
            
            abi_CMD_G2D_ADD_SPRITE (previousValue[face], false, SCP, SCP, 0xFF, 0, curFaceAngle, 0);

            abi_CMD_BITMAP (previousValue[face], SCP, SCP, curFaceAngle, 0,0);

        }
        DrawParticles  (localFrame++, chipCurrentValue);
        if ((localFrame == 2) || (wasRotation > 1)) {
            previousValue[face] = chipCurrentValue;
        }
        if ((localFrame >= 7) || (wasRotation > 1)) {
            animTFA [face + APPEAR_OFFSET] = animTFA [face + ARROW_OFFSET] = localFrame = 0;
            previousValue[face] = EMPTY;
        }
        appearFrame [face] = localFrame;
        wasAnimation = 1;
    }

    DoRotation(face);

    if ((!wasAnimation) && (chipCurrentValue != EMPTY)) {
        
        abi_CMD_G2D_ADD_SPRITE (chipCurrentValue, false, SCP, SCP, 0xFF, 0, curFaceAngle, 0);

        abi_CMD_BITMAP (chipCurrentValue, SCP, SCP, curFaceAngle, 0,0);

    }

    DrawHUD(newAngles[face]);

    if (isLevelFinished) {
        DrawResults(face);
    }
}

int CheckAnimationInprogress () {
    for (new i = 0; i < 12; ++i) {
        
        if (animTFA[i] != 0) { return 1;}
    }
    return 0;
}

cell GeneratePartiles()
{
    for (new i = 0; i < MAX_PARTICLES; ++i) {

        particles[i] = 0  |  RandomRange( 0, bestChip) |     
                            (RandomRange( 4,  14) <<  8) |  
                            (RandomRange( 4,  14) << 12) | 
                            ((i % 4) << 16);
    }
}

DrawParticles (frame, chip) {
    new ySpeed = 1;
    new xSpeed = 1;

    for (new particleI = 0; particleI < MAX_PARTICLES; ++ particleI) {
        new particle = particles[particleI];
        ySpeed = ((particle >>  8) & 0xF) * ((particle & 0x10000) ? (-1) : (1));
        xSpeed = ((particle >> 12) & 0xF) * ((particle & 0x20000) ? (-1) : (1));
        
        abi_CMD_G2D_ADD_SPRITE (RESOURCES_64x64 + (particle & 0xF), false, 
                                SCP  + (xSpeed * frame), 
                                SCP  + (ySpeed * frame), 0xFF, 0, 0, 0);

        abi_CMD_BITMAP(RESOURCES_64x64 + (particle & 0xF),
                       SCP + (xSpeed * frame),
                       SCP + (ySpeed * frame), 0, 0,0);

    }
}


GetRandom2or4 () {

    return RandomRange(1, 100) >= 90 ? 1 : 0;
}


GenerateRandomLevel () {
    for (new cubeI = 0; cubeI < 8; ++cubeI) {
        for (new faceI = 0; faceI < 3; ++faceI) {

            currLevel[cubeI][faceI] = EMPTY;
            if (abi_cubeN == cubeI) {
                if (RandomRange(1, 100) <= 70) {
                    currLevel[cubeI][faceI] = GetRandom2or4();
                }
            }
        }
    }
}

FindBestChip () {
    new value = 0;
    for (new cubeI = 0; cubeI < 8; ++cubeI) {
        for (new faceI = 0; faceI < 3; ++faceI) {
            value = currLevel[cubeI][faceI];

            if (value > bestChip) {
                bestChip = value;
                GeneratePartiles();
            }
        }
    }
}

CheckGameOver () {

    for (new cubeI = 0; cubeI < 8; ++cubeI) {
        for (new faceI = 0; faceI < 3; ++faceI) {
            new chip = currLevel[cubeI][faceI];

            if (((chip == _256_) && !gameStagesComplete) || 
                ((chip == _2048_)&& (gameStagesComplete == 1))) {
                ++gameStagesComplete;
                isLevelFinished = 2;
                return;
            }

            if (chip == _4096_){
                isLevelFinished = 3;
                
                if ((abi_cubeN == 0) && (!soundPlayed)) {
                    abi_CMD_PLAYSND(WIN_SOUND, SOUND_VOLUME);
                    soundPlayed = 1;
                }
                return;
            }
        }
    }
}

GetDiagonalNeighbour (cube, face) {

    new topC = abi_topCubeN(cube, face);
    new topF = abi_topFaceN(cube, face);
    if ((topC < 8) && (topF < 3)) {
        new neighbour = abi_topCubeN(topC, topF);
        if (neighbour < 8){
            return neighbour;
        }
    }

    topC = abi_leftCubeN(cube, face);
    topF = abi_leftFaceN(cube, face);
    if ((topC < 8) && (topF < 3)) {
        new neighbour = abi_leftCubeN(topC, topF);
        if (neighbour < 8){
            return neighbour;
        }
    }
    return 9;
}

RememberNeighbours () {
    if (!isRotateSuccessful) {
        if ((TRBL_rotation_side == 0) && (TRBL_rotation_face == 3)) {
            isRotateSuccessful = 1;
            ++_180_DegreeRotationCount;
        } else {
            for (new faceI = 0; faceI < 3; ++faceI) {
                if ((TRBL_rotation_side == 1) && (TRBL_rotation_face == faceI)) {
                    animTFA[((faceI - 1) % 3) + ARROW_OFFSET] = 3;
                    TransferChip(MOVE_TO_LEFT, faceI);
                } else if ((TRBL_rotation_side == 2) && (TRBL_rotation_face == faceI)) {
                    animTFA [faceI + ARROW_OFFSET] = 4;
                    TransferChip(MOVE_TO_TOP, (faceI - 1) % 3);
                }
            }
        }
    }
}

SendAppear(cell, cell, cell, cell);

GenerateNewRandomValue (visitedModules) {

    new emptyFaces [23]; // vad Q:why 23 ?
    new emptyCubes [23];
    new emptyArrayIterator = 0;

    for (new cube = 0; cube < 8; ++cube) {
        if (!((visitedModules >> abi_cubeN) & 1)) {
            for (new face = 0; face < 3; ++face) {
                if (currLevel[cube][face] == EMPTY) {
                    emptyCubes [emptyArrayIterator] = cube;
                    emptyFaces [emptyArrayIterator++] = face;
                }
            }
        }
    }

    if (emptyArrayIterator > 0) {

        new NEO = RandomRange (0, emptyArrayIterator - 1);
        new cube = emptyCubes [NEO];
        new face = emptyFaces [NEO];
        new number = GetRandom2or4();

        currLevel[cube][face] = number;

        SendAppear(cube, face, number, abi_cubeN);
    }
}

SendAppear (cubeWhoGetNewValue, faceWhoGetNewValue, value, cube) {

    cube = 0;
    new data[4];

    data[0] = 10 + 5 | (abi_cubeN << 8);

    data[1] = cubeWhoGetNewValue | (faceWhoGetNewValue << 8) | ((1 << cube) << 16);
    data[2] = value;
    data[3] = ++pckAppear % 0xFF;

    abi_CMD_NET_TX(0, 2, data, NET_SEND_WITH_TX_POOL);
    abi_CMD_NET_TX(1, 2, data, NET_SEND_WITH_TX_POOL);
    abi_CMD_NET_TX(2, 2, data, NET_SEND_WITH_TX_POOL);
}


SendMovement (pckNumber, cubeWhoGetNewValue, faceWhoGetNewValue, value, senderCube, senderFace, direction) {
    
    direction = 0;
    new data[4];

    data[0] = 10 + 1 | (senderCube << 8) | (senderFace << 16) | (pckNumber << 24);
    data[1] = cubeWhoGetNewValue | (faceWhoGetNewValue << 8);
    data[2] = value;
    data[3] = direction;

    abi_CMD_NET_TX(0, 2, data, NET_SEND_WITH_TX_POOL);
    abi_CMD_NET_TX(1, 2, data, NET_SEND_WITH_TX_POOL);
    abi_CMD_NET_TX(2, 2, data, NET_SEND_WITH_TX_POOL);

    resendPck = pckNumber;
    resendCube = cubeWhoGetNewValue;
    resendFace = faceWhoGetNewValue;
    resendValue = value;
    resendSenderCube = senderCube;
    resendSenderFace = senderFace;
    resendDirection = direction;
}


cell ResendLastMessage(cell messageNumber)
{
    abi_CMD_NET_TX(0, 2, lastMessages[messageNumber], NET_SEND_WITH_TX_POOL);
    abi_CMD_NET_TX(1, 2, lastMessages[messageNumber], NET_SEND_WITH_TX_POOL);
    abi_CMD_NET_TX(2, 2, lastMessages[messageNumber], NET_SEND_WITH_TX_POOL);
}

SendMyValue () {
    new data[4];

    data[0] = 10 + 2 | (abi_cubeN << 8);
    for (new faceI = 0; faceI < 3; ++faceI) {
        data[faceI + 1] = currLevel[abi_cubeN][faceI];
    }
    abi_CMD_NET_TX(0, 2, data, NET_SEND_WITH_TX_POOL);
    abi_CMD_NET_TX(1, 2, data, NET_SEND_WITH_TX_POOL);
    abi_CMD_NET_TX(2, 2, data, NET_SEND_WITH_TX_POOL);
}

SendMyScore () {
    new data[3];

    data[0] = 10 + 3;
    data[1] = abi_cubeN;
    data[2] = localScores;
    abi_CMD_NET_TX(0, 2, data, NET_SEND_WITH_TX_POOL);
    abi_CMD_NET_TX(1, 2, data, NET_SEND_WITH_TX_POOL);
    abi_CMD_NET_TX(2, 2, data, NET_SEND_WITH_TX_POOL);
}

SendConfirm (confirmTo, messageRecived) {
    new data[3];

    data[0] = 10 + 4;
    data[1] = confirmTo;
    data[2] = messageRecived;
    abi_CMD_NET_TX(0, 2, data, NET_SEND_WITH_TX_POOL);
    abi_CMD_NET_TX(1, 2, data, NET_SEND_WITH_TX_POOL);
    abi_CMD_NET_TX(2, 2, data, NET_SEND_WITH_TX_POOL);
}


FindNewAngles (farCube) {
    new angle;
    new neighborCubeN;
    new neighborFaceN;
    new tmp_cubeN = 0;
    new tmp_faceN = 0;
    for (new faceN = 0; faceN < 3; ++faceN) {
        neighborCubeN = farCube;
        neighborFaceN = faceN;
        if (farCube == abi_cubeN) {
            newAngles[faceN] = 180;
        }
        angle = 180;
        for (new count = 0; count < 3; ++count) {
            tmp_cubeN = abi_topCubeN(neighborCubeN, neighborFaceN);
            tmp_faceN = abi_topFaceN(neighborCubeN, neighborFaceN);
            angle += 90;
            if (angle == 360) {
                angle = 0;
            }
            if (!(tmp_cubeN  < 8)) {
                break;
            }
            if (tmp_cubeN == abi_cubeN) {
                newAngles[tmp_faceN] = angle;
            }
            neighborCubeN = tmp_cubeN;
            neighborFaceN = tmp_faceN;
        }
    }
}

cell CheckAngles()
{
    new far_cubeN = 0;
    new tmp_cubeN = 0;
    new tmp_faceN = 0;
    new neighborCubeN;
    new neighborFaceN;

    FindNewAngles(far_cubeN);

    neighborCubeN = 0;
    neighborFaceN = 0;

    for (new cube = 0; cube < 4; ++cube) {
        if (cube != 2) {
            tmp_cubeN = abi_leftCubeN(neighborCubeN,neighborFaceN);
            tmp_faceN = abi_leftFaceN(neighborCubeN,neighborFaceN);
            if (!(tmp_cubeN  < 8))
                break;
            neighborCubeN = tmp_cubeN;
            neighborFaceN = tmp_faceN;
        } else {
            tmp_faceN = abi_bottomFaceN(neighborCubeN,neighborFaceN);
            neighborFaceN = tmp_faceN;
        }
    }

    far_cubeN = neighborCubeN;

    FindNewAngles(far_cubeN);
}

DoRotation (face) {
    new newAng = newAngles[face];
    new curAng = currentAngles[face];

    new rotationStep = 30;
    if (newAng != curAng) {
        if (newAng == 270) {
            if ((curAng < 90) || (curAng > 270)) {
                rotationStep = -rotationStep;
            }
        } else if (newAng == 180) {
            if (curAng > 180) {
                rotationStep = -rotationStep;
            }
        } else if (newAng == 90) {
            if ((curAng > 90) && (curAng <= 270)) {
                rotationStep = -rotationStep;
            }
        } else if (newAng == 0) {
            if (curAng < 180) {
                rotationStep = -rotationStep;
            }
        }
        curAng = currentAngles[face] += rotationStep;
        if (curAng < 0) {
            currentAngles[face] = 360 - ABS(rotationStep);
        } else if (curAng >= 360) {
            currentAngles[face] = 0;
        }
    }
}


cell TransferChip(cell direction, cell face)
{

    if (animTFA [face + APPEAR_OFFSET] == APPEAR_MESSAGE) {
        return 1;
    }
    new targetCube = 0;
    new targetFace = 0;

    if (direction == MOVE_TO_LEFT) {
        targetCube = abi_leftCubeN(abi_cubeN, face);
        targetFace = abi_leftFaceN(abi_cubeN, face);
    }

    else {
        targetCube = abi_topCubeN(abi_cubeN, face);
        targetFace = abi_topFaceN(abi_cubeN, face);
    }
    new wasChanges = 0;
    if ((targetCube < 8) && (targetFace < 3)) {
        new currentChip = currLevel[abi_cubeN][face];
        if (currentChip != EMPTY) {
            if (currLevel[targetCube][targetFace] == currentChip) {
                wasChanges = ++currLevel[targetCube][targetFace];
                eachCubeScore[abi_cubeN] = localScores += pow(2, currLevel[targetCube][targetFace] + 1);
                
                if (wasChanges <= bestChip) {
                    abi_CMD_PLAYSND(COMBINED_SOUND, SOUND_VOLUME);
                } else if (wasChanges != _4096_) {
                    abi_CMD_PLAYSND(NEW_SWEET_SOUND, SOUND_VOLUME);
                }

            } else if (currLevel[targetCube][targetFace] == EMPTY) {
                currLevel[targetCube][targetFace] = currentChip;
                wasChanges = 1;
            }
            if (wasChanges) {
                outValue = currentChip;
                currLevel[abi_cubeN][face] = EMPTY;
                animTFA[face] = direction;
                pckMoveO = ++pckMoveO % 0xFF;
                SendMovement(pckMoveO, targetCube, targetFace, currLevel[targetCube][targetFace], abi_cubeN, face, animTFA[face]);
                flagResendMoveMsg = 1;
            }
        } else {
            animTFA [face + ARROW_OFFSET] = direction;
        }
        ++_90_DegreeRotationCount;
        isRotateSuccessful = 1;
        return 1;
    }
    return 0;
}


cell ONTICK()
{
    check_shake(0, 0);

    if (flagResendMoveMsg) {
        SendMovement(resendPck, resendCube, resendFace, resendValue, resendSenderCube, resendSenderFace, resendDirection);
    }

    CheckAngles();

    for (new face = 0; face < 3; ++face) {
        
        abi_CMD_G2D_BEGIN_DISPLAY(face, true);

        DrawFace(currLevel[abi_cubeN][face], face);
        
        abi_CMD_G2D_END();

        abi_CMD_REDRAW (face);

    }

    if (wasRotation > 1) { 
        --wasRotation; 
        animationFrame = 0; 
    }

    RememberNeighbours();

    FindBestChip();

    (CheckAnimationInprogress() == 1) ? (++animationFrame) : (wasRotation = animationFrame = 0);

    CheckGameOver();

    SendMyValue();
    SendMyScore();
}

cell ON_LOAD_GAME_DATA()
{}

cell ON_INIT()
{
    
    if (abi_cubeN == 0) {
        abi_CMD_PLAYSND(GAME_START_SOUND, SOUND_VOLUME);
    }

    currentLevel = 1;
    local_script = 11;
    GeneratePartiles();
    GenerateRandomLevel();
}

cell ON_CHECK_ROTATE()
{
    for (new faceI = 0; faceI < 3; ++faceI) {
        previousValue[faceI] = EMPTY;
    }
    isRotateSuccessful = flagResendMoveMsg = 0;
    ++wasRotation;
    ++moves;

    if (isLevelFinished) {
        if (isLevelFinished != 2) {
            pckAppear = gameStagesComplete = bestChip = localScores = moves = 0;
            pckAppearO = pckMoveO = 0;
            for (new cubeI = 0; cubeI < 8; ++cubeI) {
                pckMoveI[cubeI] = 0;
            }
            GenerateRandomLevel();
        }
        isLevelFinished = 0;
        soundPlayed = 0;
    }
    if (abi_cubeN == 0) {
        GenerateNewRandomValue(abi_cubeN);
    }
}

//const MESSAGE_TYPES = 6;
new messageStack [MESSAGE_TYPES];
new messageCounter = MESSAGE_TYPES;

ON_CMD_NET_RX (const pkt[]) {
    switch (abi_ByteN(pkt, 4)) {
       
         case 10 + 5: {
            new packetNumberReceived = pkt[4];
            if ((pckAppear < packetNumberReceived) || (ABS(pckAppear - packetNumberReceived) > (0xFF >> 1))) {
                pckAppear = packetNumberReceived;
                new newCube = abi_ByteN(pkt, 8);
                if (abi_cubeN == newCube) {
                    new newFace = abi_ByteN(pkt, 9);
                    new newValue = pkt[3];
                    new searchNewCandidate = 0;

                    if (!animTFA[newFace + MOVE_FROM_OFFSET] && (currLevel[abi_cubeN][newFace] == EMPTY)) {
                        animTFA[newFace + APPEAR_OFFSET] = APPEAR_MESSAGE;

                        previousValue[newFace] = EMPTY;

                        currLevel[newCube][newFace] = newValue;
                    } else {
                        new face = 0;
                        for (face = 0; face < 3; ++face) {
                            if ((currLevel[abi_cubeN][face] == EMPTY) && (!animTFA [face + MOVE_FROM_OFFSET])) {
                                animTFA[face + APPEAR_OFFSET] = APPEAR_MESSAGE;
                                previousValue[face] = EMPTY;
                                currLevel[abi_cubeN][face] = newValue;
                                break;
                            }
                        }
                        if (face >= 3) {
                            GenerateNewRandomValue(abi_ByteN(pkt, 10));
                        }
                    }
                }
            }

        }
        break;
        
        case 10 + 1: {
            new packetNumberReceived = abi_ByteN(pkt, 7);
            new senderCube = abi_ByteN(pkt, 5);
            if ((pckMoveI [senderCube] < packetNumberReceived) || (ABS(pckMoveI[senderCube] - packetNumberReceived) > (0xFF >> 1))) {
                pckMoveI [senderCube] = packetNumberReceived;

                if (senderCube < 8) {
                    currLevel[senderCube][abi_ByteN(pkt, 6)] = EMPTY;
                }

                new newCube = abi_ByteN(pkt, 8);
                if (abi_cubeN == newCube) {
                    new newFace = abi_ByteN(pkt, 9);
                    new newValue = pkt[3];
                    new searchNewCandidate = 0;
                    if (!animTFA [newFace + MOVE_FROM_OFFSET]) {
                        animTFA [newFace + MOVE_FROM_OFFSET] = pkt[4];
                        previousValue[newFace] = currLevel[newCube][newFace];
                        currLevel[newCube][newFace] = newValue;
                        if (animTFA[newFace + APPEAR_OFFSET]) {
                            animTFA[newFace + APPEAR_OFFSET] = NO_ANIMATION;
                            appearFrame[newFace] = 0;
                            new face = 0;
                            for (face = 0; face < 3; ++face) {
                                if ((currLevel[abi_cubeN][face] == EMPTY) && (animTFA[face + MOVE_FROM_OFFSET] == NO_ANIMATION)) {
                                    animTFA [face + APPEAR_OFFSET] = APPEAR_MESSAGE;
                                    previousValue[face] = EMPTY;
                                    currLevel[abi_cubeN][face] = previousValue[newFace];
                                    previousValue[newFace] = EMPTY;
                                    break;
                                }
                            }
                            if (face >= 3) {
                                GenerateNewRandomValue(1 | (1 << abi_cubeN));
                            }
                            previousValue[newFace] = EMPTY;
                        }
                    }
                }
            }
        }
        break;
        
        case 10 + 2: {
            new cube = abi_ByteN(pkt, 5);
            for (new faceI = 0; faceI < 3; ++faceI) {
                currLevel[cube][faceI] = pkt[faceI + 2];
            }
        }
        break;
        
        case 10 + 3: {
            new cube = pkt[2];
            new cubeScore = pkt[3];
            if (cubeScore > eachCubeScore[cube]) {
                eachCubeScore[cube] = cubeScore;
            }
        }
        break;
        
        case 10 + 4: {
            if (pkt[2] == abi_cubeN) {
                lastMessages[pkt[3]][0]=0;
            }
        }
    }
}

/*
cell main()
{
  new opt{100}
  argindex(0, opt);
  abi_cubeN = strval(opt);
  printf("Cube %d logic. Listening on port: %d\n", abi_cubeN, (PAWN_PORT_BASE+abi_cubeN));
  listenport(PAWN_PORT_BASE+abi_cubeN);
}
*/
