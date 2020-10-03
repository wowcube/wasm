#include "cube_api.h"

void OnTick(uint32_t time)
{
    for (int display = 0; display < 3; ++display)
        NativeInvoke(FB_drawLine_1_0{display, 0,0,240,240, fColor(1.,0,0)});
}

void OnGeoChanged(unsigned char geo_flags)
{
    if (geo_flags & Event_1_0::egfTRBL)
    {
        Get_TRBL_1_0 trbl = {};
        NativeInvoke(trbl);
    }
}

void OnMessage(size_t size)
{
    void* data = malloc(size);
    NativeInvoke(Get_Message_1_0{data});
    free(data);
}

extern "C" int run() // native cube code searches for this function and runs as a main()
{
    //message loop
    Event_1_0 event = {};
    while (NativeInvoke(event))
    {
        switch (event.type){
            case Event_1_0::eTick:
                OnTick(event.time);
                break;
            case Event_1_0::eGEO:
                OnGeoChanged(event.geo_flags);
                break;
            case Event_1_0::eMessage:
                OnMessage(event.msg_size);
                break;
        }
    }
    return 0; //whatever you return here will just be recorded into logs
}