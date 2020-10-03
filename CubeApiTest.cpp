#include "cube_api.h"

extern "C" int run() // native cube code searches for this function and runs as a main()
{
    NativeInvoke(FB_drawLine_1_0{0, 0,0,240,240, fColor(1.,0,0)});
    return 0; //whatever you return here will just be recorded into logs
}