#pragma once
#include "natives.h"
#include <typeinfo>
#include <stdarg.h>
#include <stdio.h>

template<class T>
int NativeInvoke(const T& val) // calls native by struct name
{
    static T global_value;
    global_value = val;
    return native_invoke(typeid(T).name(), &global_value, sizeof(T));
}

template<class T>
int NativeInvoke(T& val) // with return args
{
    static T global_value;
    global_value = val;
    int res = native_invoke(typeid(T).name(), &global_value, sizeof(T));
    val = global_value;
    return res;
}

int NativePrint(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    static char buff[1024] = {};
    vsprintf(buff, fmt, ap);
    int res = NativeInvoke(Print_1_0{(char*)buff});
    va_end(ap);
    return res;
}

class CDisplay
{

};

class CEventLoop
{
protected:
    virtual void OnTick(uint32_t time)
    {
        for (int display = 0; display < 3; ++display)
        {
            NativeInvoke(Fill_1_0{0x0});
        }
    }

    virtual void OnGeoChanged(unsigned char geo_flags)
    {
        if (geo_flags & Event_1_0::egfTRBL)
        {
            Get_TRBL_1_0 trbl = {};
            NativeInvoke(trbl);
        }
    }

    virtual void OnMessage(size_t size)
    {
        Get_Message_1_0 msg = {};
        msg.data = malloc(size);
        NativeInvoke(msg); //getting actual message
        OnMessage(msg);
        free(msg.data);
    }

    virtual void OnMessage(const Get_Message_1_0& msg){}

public:
    virtual int Main()
    {
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
        return 0;
    }
};