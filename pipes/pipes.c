#include "header.h"
typedef unsigned short uint16_t;
#define run pawn_run
#define pow pawn_pow
#include "resources.h"

















































new abi_cubeN = 0;
new abi_TRBL[8][3];
new abi_TRBL_backup[3]; 
new abi_MTD_Accel[6] = {0,0,0,0,0,0}; 
new abi_MTD_Gyro[6] = {0,0,0,0,0,0}; 
new abi_MTD_TapFace = 0;
new abi_MTD_TapOpposite = 0;
new abi_MTD_TapsCount = 0;








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
  abi_MTD_TapFace = abi_ByteN(pkt, 13) & 0x03;
  abi_MTD_TapOpposite = abi_ByteN(pkt, 13) >> 2;
  abi_MTD_TapsCount = abi_ByteN(pkt, 14);
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

cell abi_MTD_GetTapFace()
{
  return abi_MTD_TapFace;
}

cell abi_MTD_IsTapOpposite()
{
  return abi_MTD_TapOpposite;
}

cell abi_MTD_GetTapsCount()
{
  return abi_MTD_TapsCount;
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
    } else if (script == 17) {
      abi_trigger_debug_info();
    } else if (script == 15) {
      abi_trigger_bluetooth();
    } else if (script == 16) {
      abi_CMD_SLEEP();
    } else if (script == 18) {
      abi_trigger_nightlamp(1);
    } else if (script == 19) {
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












new ticks_for_trbl_clear[8] = {0, 0, 0, 0, 0, 0, 0, 0};

new trbl_count_rotations = 0;

new TRBL_rotation_face = 3; 
new TRBL_rotation_side = 0;
new TRBL_rotation_left_cube[3] = { 8, 8, 8};
new TRBL_rotation_right_cube[3] = { 8, 8, 8};
new TRBL_rotation_top_cube[3] = { 8, 8, 8};





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
            ticks_for_trbl_clear[abi_ByteN(pkt,5)]=0;
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
            if ((local_script != abi_ByteN(pkt, 5)) && (abi_ByteN(pkt, 5) != 17))
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

      ON_LOAD_GAME_DATA (pkt);
    }
  }
}



















new neighborCubeN = 8;
new neighborFaceN = 3;
new newAngles[3] = {180, 180, 180};

new num_level = 0;

new steam_frame[3];
new tick_to_birst;

new figures[5][4][4] = {
    {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {1, 0, 1, 0},
        {0, 1, 0, 1},
        {1, 0, 1, 0},
        {0, 1, 0, 1}
    },
    {
        {0, 1, 1, 0},
        {0, 0, 1, 1},
        {1, 0, 0, 1},
        {1, 1, 0, 0}
    },
    {
        {1, 1, 0, 1},
        {1, 1, 1, 0},
        {0, 1, 1, 1},
        {1, 0, 1, 1}
    },
    {
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1}
    }
};
new curr_level[8][3][2]; 



new level1[8] = {000000, 002200, 000000, 204020, 002200, 220000, 000000, 000000}; 
new level2[8] = {002310, 000000, 002200, 002200, 210011, 000000, 000000, 000000}; 
new level3[8] = {000000, 112100, 000000, 233121, 000000, 000000, 220000, 002333}; 
new level4[8] = {212320, 323120, 321011, 102332, 000000, 000000, 000000, 000000}; 
new level5[8] = {000000, 000000, 234021, 220000, 003021, 000000, 000000, 330023}; 
new level6[8] = {003021, 000000, 233300, 003021, 000000, 000000, 233300, 000000}; 
new level7[8] = {001133, 234010, 234021, 000000, 003021, 000022, 000000, 220000}; 
new level8[8] = {212331, 212332, 212340, 212340, 000000, 000000, 000000, 000000}; 
new level9[8] = {003021, 234021, 203240, 234010, 000000, 220000, 220000, 000000}; 
new level10[8] = {003021, 234021, 204040, 234033, 000000, 220000, 220000, 000000}; 













cell angle2pipeangle(cell angle)
{
    if(angle == 90) return 1;
    else if(angle == 180) return 2;
    else if(angle == 270) return 3;
    else return 0;
}
cell set_steam(cell steam, cell base)
{
    if((steam / base) % 10 == 0)
        steam += base;
    if((steam / base) % 10 == 5)
        steam -= 3 * base;
}
cell draw_level(cell thisFaceN)
{
    if((abi_MTD_GetFaceAccelZ(thisFaceN) < 0) && (abi_MTD_GetFaceAccelY(thisFaceN) < 0) && (ABS(abi_MTD_GetFaceAccelY(thisFaceN)) > ABS(abi_MTD_GetFaceAccelX(thisFaceN))) && (ABS(abi_MTD_GetFaceAccelZ(thisFaceN)) > ABS(abi_MTD_GetFaceAccelX(thisFaceN))) && (ABS(abi_MTD_GetFaceAccelZ(thisFaceN)) > ABS(abi_MTD_GetFaceAccelY(thisFaceN)))) {
        abi_CMD_BITMAP(54, 29, 240 - 7, 180, 0, 0);
        abi_CMD_BITMAP(4 + num_level % 10, 11, 240 - 14 - 17, 180, 0, 0);
        if(num_level > 9)
            abi_CMD_BITMAP(4 + num_level / 10 % 10, 34, 240 - 14 - 17, 180, 0, 0);
    }
}
cell ONTICK()
{
    new thisCubeN = abi_cubeN;
    new thisFaceN = 0;
    new thisFigure = 0;
    new thisFigureAngle = 0;
    new neighborCubeN = 0xFF;
    new neighborFaceN = 0xFF;
    new neighborFigure = 0;
    new neighborFigureAngle = 0;

    if(0 == abi_cubeN) {
        send_info();
        check_shake(0, 0);
    }

    if(abi_cubeN == 0)
        if(total_result != 0xFF) {
            time++;
            abi_send_level(230 + 3, num_level);
        }
    CheckAngles();
    for (thisFaceN = 0; thisFaceN < 3; thisFaceN++) {
        thisFigure = curr_level[thisCubeN][thisFaceN][0];
        thisFigureAngle = curr_level[thisCubeN][thisFaceN][1];

        abi_CMD_FILL(0, 0, 0);

        abi_CMD_BITMAP(29, 120, 120, 0, 0, 0);


        if(thisFigure != 0)
            abi_CMD_BITMAP(15 + thisFigure, 120, 120, thisFigureAngle, 0, 0); 





        neighborCubeN = abi_topCubeN(thisCubeN, thisFaceN);
        neighborFaceN = abi_topFaceN(thisCubeN, thisFaceN);

        if((neighborCubeN < 8) && (neighborFaceN < 3)) {
            neighborFigure = curr_level[neighborCubeN][neighborFaceN][0];
            neighborFigureAngle = curr_level[neighborCubeN][neighborFaceN][1];


            if((figures[neighborFigure][angle2pipeangle(neighborFigureAngle)][3] == 1) && (figures[thisFigure][angle2pipeangle(thisFigureAngle)][0] == 0))
                set_steam(steam_frame[thisFaceN], 1);

            else if((figures[neighborFigure][angle2pipeangle(neighborFigureAngle)][3] == 1) && (figures[thisFigure][angle2pipeangle(thisFigureAngle)][0] == 1) && (thisCubeN == abi_cubeN))
                abi_CMD_BITMAP(15, 120, 16, 180, 0, 0); 
        }


        neighborCubeN = thisCubeN; 
        neighborFaceN = abi_rightFaceN(thisCubeN, thisFaceN);

        if((neighborCubeN < 8) && (neighborFaceN < 3)) {
            neighborFigure = curr_level[neighborCubeN][neighborFaceN][0];
            neighborFigureAngle = curr_level[neighborCubeN][neighborFaceN][1];


            if((figures[neighborFigure][angle2pipeangle(neighborFigureAngle)][2] == 1) && (figures[thisFigure][angle2pipeangle(thisFigureAngle)][1] == 0))
                set_steam(steam_frame[thisFaceN], 10);

            else if((figures[neighborFigure][angle2pipeangle(neighborFigureAngle)][2] == 1) && (figures[thisFigure][angle2pipeangle(thisFigureAngle)][1] == 1) && (thisCubeN == abi_cubeN))
                abi_CMD_BITMAP(15, 240 - 16, 120, 270, 0, 0); 
        }


        neighborCubeN = thisCubeN; 
        neighborFaceN = abi_bottomFaceN(thisCubeN, thisFaceN);

        if((neighborCubeN < 8) && (neighborFaceN < 3)) {
            neighborFigure = curr_level[neighborCubeN][neighborFaceN][0];
            neighborFigureAngle = curr_level[neighborCubeN][neighborFaceN][1];


            if((figures[neighborFigure][angle2pipeangle(neighborFigureAngle)][1] == 1) && (figures[thisFigure][angle2pipeangle(thisFigureAngle)][2] == 0))
                set_steam(steam_frame[thisFaceN], 100);

            else if((figures[neighborFigure][angle2pipeangle(neighborFigureAngle)][1] == 1) && (figures[thisFigure][angle2pipeangle(thisFigureAngle)][2] == 1) && (thisCubeN == abi_cubeN))
                abi_CMD_BITMAP(15, 120, 240 - 16, 0, 0, 0); 
        }


        neighborCubeN = abi_leftCubeN(thisCubeN, thisFaceN);
        neighborFaceN = abi_leftFaceN(thisCubeN, thisFaceN);

        if((neighborCubeN < 8) && (neighborFaceN < 3)) {
            neighborFigure = curr_level[neighborCubeN][neighborFaceN][0];
            neighborFigureAngle = curr_level[neighborCubeN][neighborFaceN][1];


            if((figures[neighborFigure][angle2pipeangle(neighborFigureAngle)][0] == 1) && (figures[thisFigure][angle2pipeangle(thisFigureAngle)][3] == 0))
                set_steam(steam_frame[thisFaceN], 1000);

            else if((figures[neighborFigure][angle2pipeangle(neighborFigureAngle)][0] == 1) && (figures[thisFigure][angle2pipeangle(thisFigureAngle)][3] == 1) && (thisCubeN == abi_cubeN))
                abi_CMD_BITMAP(15, 16, 120, 90, 0, 0); 

        }

        if((steam_frame[thisFaceN] / 1) % 10 > 0) {
            steam_frame[thisFaceN] += 1;
            if((steam_frame[thisFaceN] / 1) % 10 == 9)
                steam_frame[thisFaceN] -= 9 * 1;
            else
                abi_CMD_BITMAP(15+4 + (steam_frame[thisFaceN] / 1) % 10, 120, 120 / 2, 0, 0, 0); 
        }
        if((steam_frame[thisFaceN] / 10) % 10 > 0) {
            steam_frame[thisFaceN] += 10;
            if((steam_frame[thisFaceN] / 10) % 10 == 9)
                steam_frame[thisFaceN] -= 9 * 10;
            else
                abi_CMD_BITMAP(15+4 + (steam_frame[thisFaceN] / 10) % 10, 240 - 120 / 2, 120, 90, 0, 0); 
        }
        if((steam_frame[thisFaceN] / 100) % 10 > 0) {
            steam_frame[thisFaceN] += 100;
            if((steam_frame[thisFaceN] / 100) % 10 == 9)
                steam_frame[thisFaceN] -= 9 * 100;
            else
                abi_CMD_BITMAP(15+4 + (steam_frame[thisFaceN] / 100) % 10, 120, 240 - 120 / 2, 180, 0, 0); 
        }
        if((steam_frame[thisFaceN] / 1000) % 10 > 0) {
            steam_frame[thisFaceN] += 1000;
            if((steam_frame[thisFaceN] / 1000) % 10 == 9)
                steam_frame[thisFaceN] -= 9 * 1000;
            else
                abi_CMD_BITMAP(15+4 + (steam_frame[thisFaceN] / 1000) % 10, 120 / 2, 120, 270, 0, 0); 
        }

        if((count_results < 4) && (thisFaceN == faceN_results))
            Draw_Results(count_results);








        abi_CMD_REDRAW(thisFaceN); 
    }

    if((steam_frame[0] == 0) && (steam_frame[1] == 0) && (steam_frame[2] == 0)) {
        is_completed = 1;
    }
    else{
        is_completed = 0;

        if (tick_to_birst == 0) abi_CMD_PLAYSND(2 + Random(0,3), 95);

    }


    if((abi_cubeN == 0) && (total_result == 0xFF)) {
        if((result_cubeN == 8) && (result_faceN == 3)) {


            getCubeFaceToResults(abi_cubeN, 0, result_cubeN, result_faceN);
            getCubeFaceToResults(abi_cubeN, 1, result_cubeN, result_faceN);
            getCubeFaceToResults(abi_cubeN, 2, result_cubeN, result_faceN);
        }
        if((result_cubeN < 8) && (result_faceN < 3))
            abi_show_results(230 + 4, result_cubeN, result_faceN, num_level); 
    }
    if(total_result != 0xFF)
        abi_respose_to_request_results(230 + 2, abi_cubeN, is_completed);

    tick_to_birst = ((tick_to_birst == 0) ? Random(25, 150) * 10 : tick_to_birst - 1);
}
cell setcurr_level(cell* level)
{
    for (new cubeN = 0; cubeN < 8; cubeN++) {
        curr_level[cubeN][0][0] = (level[cubeN] / 10) % 10;
        curr_level[cubeN][0][1] = (level[cubeN] / 1) % 10 * 90;
        curr_level[cubeN][1][0] = (level[cubeN] / 1000) % 10;
        curr_level[cubeN][1][1] = (level[cubeN] / 100) % 10 * 90;
        curr_level[cubeN][2][0] = (level[cubeN] / 100000) % 10;
        curr_level[cubeN][2][1] = (level[cubeN] / 10000) % 10 * 90;
    }
    result_cubeN = 8;
    result_faceN = 3;
    time = 0;
    score = 0;
    moves = 0;
}
cell CheckAngles()
{
    new far_cubeN = 0;
    new tmp_cubeN = 0;
    new tmp_faceN = 0;

    FindNewAngles(far_cubeN);

    neighborCubeN = 0;
    neighborFaceN = 0;


    tmp_cubeN = abi_leftCubeN(neighborCubeN, neighborFaceN);
    tmp_faceN = abi_leftFaceN(neighborCubeN, neighborFaceN);
    if(!(tmp_cubeN < 8))
        return;
    neighborCubeN = tmp_cubeN;
    neighborFaceN = tmp_faceN;
    tmp_cubeN = abi_leftCubeN(neighborCubeN, neighborFaceN);
    tmp_faceN = abi_leftFaceN(neighborCubeN, neighborFaceN);
    if(!(tmp_cubeN < 8))
        return;
    neighborCubeN = tmp_cubeN;
    neighborFaceN = tmp_faceN;
    tmp_cubeN = abi_bottomCubeN(neighborCubeN, neighborFaceN);
    tmp_faceN = abi_bottomFaceN(neighborCubeN, neighborFaceN);
    neighborCubeN = tmp_cubeN;
    neighborFaceN = tmp_faceN;
    tmp_cubeN = abi_leftCubeN(neighborCubeN, neighborFaceN);
    tmp_faceN = abi_leftFaceN(neighborCubeN, neighborFaceN);
    if(!(tmp_cubeN < 8))
        return;
    neighborCubeN = tmp_cubeN;
    neighborFaceN = tmp_faceN;

    far_cubeN = neighborCubeN;

    FindNewAngles(far_cubeN);
}
cell FindNewAngles(cell farCube)
{
    new angle;
    new tmp_cubeN = 0;
    new tmp_faceN = 0;
    for (new faceN = 0; faceN < 3; faceN++) {
        neighborCubeN = farCube;
        neighborFaceN = faceN;
        if(farCube == abi_cubeN) {
            newAngles[faceN] = 180;
        }
        angle = 180;
        for (new count = 0; count < 3; count++) {
            tmp_cubeN = abi_topCubeN(neighborCubeN, neighborFaceN);
            tmp_faceN = abi_topFaceN(neighborCubeN, neighborFaceN);
            angle += 90;
            if(angle == 360) {
                angle = 0;
            }
            if(!(tmp_cubeN < 8)) {
                break;
            }
            if(tmp_cubeN == abi_cubeN) {
                newAngles[tmp_faceN] = angle;
            }
            neighborCubeN = tmp_cubeN;
            neighborFaceN = tmp_faceN;
        }
    }
}
cell ON_CMD_NET_RX(cell* pkt)
{
    switch (abi_ByteN(pkt, 4)) {
        break;case 230 + 2:{

            abi_set_result_from_RX(abi_ByteN(pkt, 5), abi_ByteN(pkt, 6), total_result);

            abi_set_result_from_RX(abi_cubeN, is_completed, total_result);


        }
        break;case 230 + 4:{
            if((abi_ByteN(pkt, 5) == abi_cubeN) && (abi_ByteN(pkt, 7) == num_level)) {
                time = pkt[2];
                moves = pkt[3];
                count_results = pkt[4];
                faceN_results = abi_ByteN(pkt, 6);
            }
            if(total_result != 0xFF) {
                count_results = 4;
                faceN_results = 3;
            }
        }
        break;case 230 + 3:{
            if(num_level != pkt[2]) {
                num_level = pkt[2];
                count_results = 4;
                faceN_results = 3;
                switch (num_level) {
                    break;case 1:{ setcurr_level(level1); }
                    break;case 2:{ setcurr_level(level2); }
                    break;case 3:{ setcurr_level(level3); }
                    break;case 4:{ setcurr_level(level4); }
                    break;case 5:{ setcurr_level(level5); }
                    break;case 6:{ setcurr_level(level6); }
                    break;case 7:{ setcurr_level(level7); }
                    break;case 8:{ setcurr_level(level8); }
                    break;case 9:{ setcurr_level(level9); }
                    break;case 10:{ setcurr_level(level10); }
                }
            }
        }
    }
}
cell ON_LOAD_GAME_DATA()
{
}
cell ON_INIT()
{
    setcurr_level(level1);
    time = 0;
    moves = 0;
    num_level = 1;
    is_completed = 0;
    total_result = 0;
    local_script = 1;


    abi_CMD_PLAYSND(0, 95);

}
cell ON_CHECK_ROTATE()
{

    if(abi_cubeN == 0) {
        moves++;
        if(total_result == 0xFF) {
            moves = 0;
            time = 0;
            total_result = 0;
            count_results = 4;
            faceN_results = 3;
            if(num_level == 10)
                num_level = 1;
            else
                num_level++;
            switch (num_level) {
                break;case 1:{ setcurr_level(level1); }
                break;case 2:{ setcurr_level(level2); }
                break;case 3:{ setcurr_level(level3); }
                break;case 4:{ setcurr_level(level4); }
                break;case 5:{ setcurr_level(level5); }
                break;case 6:{ setcurr_level(level6); }
                break;case 7:{ setcurr_level(level7); }
                break;case 8:{ setcurr_level(level8); }
                break;case 9:{ setcurr_level(level9); }
                break;case 10:{ setcurr_level(level10); }
            }

            abi_CMD_PLAYSND(1, 95);

        }
    }
    tick_to_birst = Random(25, 150) * 10;
}

