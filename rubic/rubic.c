

#define new signed
#define bool int
#define false 0
#define true 1

#include "header.h"
#include "pawn2c.h"

#define run pawn_run
#define pow pawn_pow
























new abi_cubeN = 0;
new abi_TRBL[8][3];
new abi_TRBL_backup[3]; 
cell abi_MTD_Accel[6] = {0,0,0,0,0,0}; 
cell abi_MTD_Gyro[6] = {0,0,0,0,0,0}; 





abi_GetCubeN();
abi_SetCubeN(const cubeN);

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


bool abi_check_rotate()
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

/*
cell abi_CMD_BITMAP(cell resID, cell x, cell y, cell angle, cell mirror)
{
  cell pkt[3] = {0};
  pkt[0] = ((x & 0xFF) << 24) | ((resID & 0xFFFF) << 8) | (0+3 & 0xFF);
  pkt[1] = ((angle & 0xFF) << 24) | ((y & 0xFFFF) << 8) | ((x & 0xFF00) >> 8);
  pkt[2] = (((mirror & 0xFF) << 8) |(angle & 0xFF00) >> 8);

  sendpacket(pkt, 3);

}
*/

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
    if (script == 12) {
      abi_trigger_debug_info();
    } else if (script == 10) {
      abi_trigger_bluetooth();
    } else if (script == 11) {
      abi_CMD_SLEEP();
    } else {

      if (0==abi_cubeN) {
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
    abi_CMD_BITMAP(0+_type, 120, 120, 180, 0);
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
      abi_CMD_BITMAP(4+base%10,  120, 120  + (22+2)/2*count - (22+2)*i, 90, 0);
      base/=10;
    }
    abi_CMD_BITMAP(0+_type, 70, 120, 90, 0);
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
      abi_CMD_BITMAP(4+base%10, 120 + (22+2)/2*count - (22+2)*i, 120, 0, 0);
      base/=10;
    }
    abi_CMD_BITMAP(0+_type, 120, 170, 0, 0);
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
        abi_CMD_BITMAP(4+10, 120, 120 - (22+2)/2*count + (22+2)*i, 270, 0);
      }
      else
      {
        abi_CMD_BITMAP(4+base%10, 120, 120 - (22+2)/2*count + (22+2)*i, 270, 0);
        base/=10;
      }
    }
    abi_CMD_BITMAP(0+_type, 170, 120, 270, 0);
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
  data[2]=0;
  data[3]=0;
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
          if (!((cubeN_tmp < 8) && (faceN_tmp < 3)))
            return;
          cubeN_tmp = neighborCubeN;
          faceN_tmp = neighborFaceN;
        }
        else
        {
          neighborCubeN = abi_leftCubeN(cubeN_tmp, faceN_tmp);
          neighborFaceN = abi_leftFaceN(cubeN_tmp, faceN_tmp);
          if (!((cubeN_tmp < 8) && (faceN_tmp < 3)))
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





cell ticks_for_trbl_clear[8] = {0, 0, 0, 0, 0, 0, 0, 0};

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
      ticks_for_trbl_clear[thisCubeN]=0;
      continue;
    }
    if (ticks_for_trbl_clear[thisCubeN]>3)
    {
      abi_TRBL[thisCubeN][0] = 0xFFFFFFFF;
      abi_TRBL[thisCubeN][1] = 0xFFFFFFFF;
      abi_TRBL[thisCubeN][2] = 0xFFFFFFFF;
      ticks_for_trbl_clear[thisCubeN]=0;
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
  data[0]= (((thiscommand & 0xFF) <<  0) |
            ((ThisCubeN    & 0xFF) <<  8));
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
      abi_TRBL[cubeN][faceN]=0xFFFFFFFF;
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
stock pow(a, n)
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
stock int_sqrt(x)
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
    time=0;
    moves=0;
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
      if (abi_check_rotate()==true)
      {
        ON_CHECK_ROTATE();
        script_ticks=0;
      }
      ONTICK();
      if (script_ticks<1 * 20)
        script_ticks++; 

    }

    break;case 100+2:
    {
      abi_TRBL_Deserialize(pkt);
    }

    break;case 100+3:
    {





      ON_CMD_NET_RX(pkt);
      switch (abi_ByteN(pkt, 4))
      {
        break;case 220+2:
        {

          new cubeN=abi_ByteN(pkt,5);
          if (cubeN != abi_cubeN)
          {
            abi_TRBL[cubeN][0]=pkt[2];
            abi_TRBL[cubeN][1]=pkt[3];
            abi_TRBL[cubeN][2]=pkt[4];
            ticks_for_trbl_clear[cubeN]=0;
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
            if ((local_script != abi_ByteN(pkt, 5)) && (abi_ByteN(pkt, 5) != 12))
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
  }
}











new platformCubeId = 0;


new rubikCubeId;


cell faceMap[3] = { 0                 , 1                  , 2                   };


cell lineMap[3] = {0                    , 1                   , 2                  };


cell c_faceColorArray[9][3] = { {  0xFFFFFF , 0x00FF00 , 0xFF0000    }, 
                                       {  0xFFFFFF , 0xFFA500, 0x00FF00  },
                                       {  0xFFFF00, 0x00FF00 , 0xFFA500 },
                                       {  0xFFFF00, 0xFF0000   , 0x00FF00  },
                                       {  0xFFFFFF , 0xFF0000   , 0x0000FF   },
                                       {  0xFFFFFF , 0x0000FF  , 0xFFA500 },
                                       {  0xFFFF00, 0xFFA500, 0x0000FF   },
                                       {  0xFFFF00, 0x0000FF  , 0xFF0000    },
                                       {  0x808080  , 0x808080  , 0x808080}  };







new assignCfmMap;
new assignReqSendAttempts;

cell platform_TwoWayLineToFaceOpp(cell face_or_line)
{
  return (4 - face_or_line) % 3;
}

cell platform_TwoWayLineToFaceLeft(cell face_or_line)
{
  return (3 - face_or_line) % 3;
}

cell rubik_initCfmMap()
{
  switch(rubikCubeId)
  {
    break;case 0:
    {
      assignCfmMap = (1 << lineMap[2]) | (1 << lineMap[1]);                          
    }
    break;case 1: case 5:
    {
      assignCfmMap = 1 << lineMap[0];                  
    }
    break;case 2:
    {
      assignCfmMap = 1 << lineMap[2];                
    }
    break;case 4: case 6:
    {
      assignCfmMap = 1 << lineMap[1];                 
    }
    default:
    {
      assignCfmMap = 0;
    }
  }

  if(assignCfmMap != 0)
  {
    assignReqSendAttempts = 5;
  }
}

cell rubik_mapLinesAndFaces(cell receivedFromLine)
{

  switch(rubikCubeId)
  {
    break;case 0:
    {

      for(new currentFace = 0; currentFace < 3; currentFace++)
      {
        lineMap[currentFace] = platform_TwoWayLineToFaceOpp(faceMap[currentFace]);
      }
    }

    break;case 1: case 3:
    {

      lineMap[1] = receivedFromLine;
      faceMap[1] = platform_TwoWayLineToFaceOpp(lineMap[1]);


      faceMap[0] = platform_TwoWayLineToFaceLeft(lineMap[1]);
      lineMap[0] = platform_TwoWayLineToFaceOpp(faceMap[0]);


      faceMap[2] = platform_TwoWayLineToFaceLeft(lineMap[0]);
      lineMap[2] = platform_TwoWayLineToFaceOpp(faceMap[2]);
    }

    break;case 2: case 6:
    {

      lineMap[0] = receivedFromLine;
      faceMap[0] = platform_TwoWayLineToFaceOpp(lineMap[0]);


      faceMap[2] = platform_TwoWayLineToFaceLeft(lineMap[0]);
      lineMap[2] = platform_TwoWayLineToFaceOpp(faceMap[2]);


      faceMap[1] = platform_TwoWayLineToFaceLeft(lineMap[2]);
      lineMap[1] = platform_TwoWayLineToFaceOpp(faceMap[1]);
    }

    break;case 4: case 5: case 7:
    {

      lineMap[2] = receivedFromLine;
      faceMap[2] = platform_TwoWayLineToFaceOpp(lineMap[2]);


      faceMap[1] = platform_TwoWayLineToFaceLeft(lineMap[2]);
      lineMap[1] = platform_TwoWayLineToFaceOpp(faceMap[1]);


      faceMap[0] = platform_TwoWayLineToFaceLeft(lineMap[1]);
      lineMap[0] = platform_TwoWayLineToFaceOpp(faceMap[0]);
    }
  }
}

cell rubik_assignNumberToNeihbours()
{
  cell pktToSend[4] = {0, 0, 0, 0};

  if((assignCfmMap & 0x7 ) == 0)
  {
    return;
  }

  if(assignReqSendAttempts == 0)
  {
    return;
  }
  assignReqSendAttempts--;


  switch(rubikCubeId)
  {
    break;case 0:
    {

      if((assignCfmMap & (1 << lineMap[2])) != 0)
      {
        pktToSend[0] = (1 << 8) | 10+1;
        abi_CMD_NET_TX(lineMap[2], 0, pktToSend, 0);
      }


      if((assignCfmMap & (1 << lineMap[1])) != 0)
      {
        pktToSend[0] = (4 << 8) | 10+1;
        abi_CMD_NET_TX(lineMap[1], 0, pktToSend, 0);
      }
    }

    break;case 1:
    {

      if((assignCfmMap & (1 << lineMap[0])) != 0)
      {
        pktToSend[0] = (2 << 8) | 10+1;
        abi_CMD_NET_TX(lineMap[0], 0, pktToSend, 0);
      }
    }

    break;case 2:
    {

      if((assignCfmMap & (1 << lineMap[2])) != 0)
      {
        pktToSend[0] = (3 << 8) | 10+1;
        abi_CMD_NET_TX(lineMap[2], 0, pktToSend, 0);
      }
    }

    break;case 4:
    {

      if((assignCfmMap & (1 << lineMap[1])) != 0)
      {
        pktToSend[0] = (5 << 8) | 10+1;
        abi_CMD_NET_TX(lineMap[1], 0, pktToSend, 0);
      }
    }

    break;case 5:
    {

      if((assignCfmMap & (1 << lineMap[0])) != 0)
      {
        pktToSend[0] = (6 << 8) | 10+1;
        abi_CMD_NET_TX(lineMap[0], 0, pktToSend, 0);
      }
    }

    break;case 6:
    {

      if((assignCfmMap & (1 << lineMap[1])) != 0)
      {
        pktToSend[0] = (7 << 8) | 10+1;
        abi_CMD_NET_TX(lineMap[1], 0, pktToSend, 0);
      }
    }
  }
}

cell rubik_paintFaces()
{
  new currentFace = 0;
  new lineToCheck = 0;


  for(currentFace = 0; currentFace < 3; currentFace++)
  {
    lineToCheck = platform_TwoWayLineToFaceOpp(faceMap[currentFace]);
    abi_CMD_FILL_2(c_faceColorArray[rubikCubeId][currentFace]);
    abi_CMD_REDRAW(faceMap[currentFace]);
  }
}

cell ONTICK()
{
  if(platformCubeId == 0)
  {
    send_info(false);
    check_shake(0, 0);
  }

  rubik_paintFaces();
  if(rubikCubeId < 8)
  {
    rubik_assignNumberToNeihbours();
  }
}

cell ON_INIT()
{
  local_script = 7;
  platformCubeId = abi_cubeN;
  assignReqSendAttempts = 0;
  if( platformCubeId == 0 )
  {
    rubikCubeId = 0;
    rubik_mapLinesAndFaces(0);
  }
  else
  {
    rubikCubeId = 8;
  }
  rubik_initCfmMap();
}

cell ON_CMD_NET_RX(cell* pkt)
{
  new receivedFromLine = abi_ByteN(pkt, 1);
  cell pktToSend[4] = {0, 0, 0, 0};

  switch(abi_ByteN(pkt,4))
  {
    break;case 10+1:
    {
      if(rubikCubeId == 8)
      {
        rubikCubeId = abi_ByteN(pkt,5);
        rubik_mapLinesAndFaces(receivedFromLine);
        rubik_initCfmMap();
      }

      pktToSend[0] = 10+2;
      abi_CMD_NET_TX(receivedFromLine, 0, pktToSend, 0);
    }

    break;case 10+2:
    {
      if(assignCfmMap != 0)
      {
        assignCfmMap &= ~(1 << receivedFromLine);
        assignCfmMap |= (1 << (3 + receivedFromLine));
      }
    }
  }
}

cell ON_CHECK_ROTATE()
{


}

resource_t get_resource(int idx) {
    static resource_t dummy = {0,0};
    return dummy;
};

