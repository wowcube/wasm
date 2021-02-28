#pragma once
#include "natives.h"
#include <typeinfo>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <algorithm>
#include <memory>

#define WC_CHECKRET(cond, ret) if (!(cond)) {NativePrint("%s(%d):\t%s\t-\t%s", __FILE__, __LINE__, __FUNCTION__, #cond); return ret;}
#define WC_NORET ;

#define WCPL() NativePrint("%s(%d):\t%s", __FILE__, __LINE__, __FUNCTION__)

template<class T=char>
std::unique_ptr<T[]> GetSharableMem(uint32_t size)
{
    return std::unique_ptr<T[]>(new T[size]);
}

template<class T>
int32_t NativeInvokeDirect(T& val) // with return args
{
    return native_invoke(typeid(T).name(), &val, sizeof(T));
}

template<class T>
int32_t NativeInvoke(const T& val) // calls native by struct name
{
    static T global_value; //only static/global memory can pass the native boundary
    global_value = val;
    return NativeInvokeDirect(global_value);
}

template<class T>
int32_t NativeInvoke(T& val) // with return args
{
    static T global_value;
    global_value = val;
    int res = NativeInvokeDirect(global_value);
    val = global_value;
    return res;
}

int NativePrint(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    uint32_t len = vsnprintf(nullptr, 0, fmt, ap); //len is without trailing 0
    auto buff = GetSharableMem(len+1);
    vsprintf(buff.get(), fmt, ap);
    int32_t res = NativeInvoke(Print_1_0{(char*)buff.get(), len+1 });
    va_end(ap);
    return res;
}

int NativeSend(uint8_t to_cid, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    uint32_t len = vsnprintf(nullptr, 0, fmt, ap); //len is without trailing 0
    auto buff = GetSharableMem(len + 1);
    vsprintf(buff.get(), fmt, ap);
    int32_t res = NativeInvoke(Send_Message_1_0{to_cid, len + 1, buff.get()});
    va_end(ap);
    return res;
}

template<class T>
intptr_t unique_type_id()
{
    return reinterpret_cast<intptr_t>(&unique_type_id<T>);
};

template<class T>
int NativeSendStruct(uint8_t to_cid, const T& str)
{
    uint32_t len = sizeof(size_t) + sizeof(T); //len is without trailing 0
    auto buff = GetSharableMem(len);
    intptr_t* ptr = reinterpret_cast<intptr_t*>(buff.get());
    *ptr = unique_type_id<T>();
    memcpy(ptr + 1, &str, sizeof(T));
    int32_t res = NativeInvoke(Send_Message_1_0{ to_cid, len, buff.get() });
    return res;
}

class CBitmap
{
public:
    ~CBitmap() {
        Free();
    }

    bool Load(const void* ptr, uint32_t size, int fmt)
    {
        Free();
        switch (fmt) {
            case EPictureFormat::epfRGB565:
            case EPictureFormat::epfRLE:
                m_data = malloc(size);
                memcpy(m_data, ptr, size);
                m_size = size;
                m_format = (EPictureFormat)fmt;
                return true;

            default:
                NativePrint("Format not supported");
                return false;
        }

        return false;
    }

    const void* GetAddr() const { return m_data;     }
    uint32_t GetSize() const { return m_size;     }
    EPictureFormat GetFormat() const { return m_format;   }

protected:
    void Free()
    {
        free(m_data);
        m_data = nullptr;
        m_size = 0;
        m_format = epfNone;
    }

    void *m_data = nullptr;
    uint32_t m_size = 0;
    EPictureFormat m_format = epfNone;
};


class CSound
{
public:
    bool Load(ESoundFormat esf, const void* ptr, uint16_t size)
    {
        switch (esf) {
            case ESoundFormat::esfMidi:
                m_data = malloc(size);
                WC_CHECKRET(m_data, false);
                memcpy(m_data, ptr, size);
                m_size = size;
                m_format = esf;
                return true;

            default:
                NativePrint("Format not supported");
                return false;
        }

        return false;
    }

    int32_t Play()
    {
        return NativeInvoke(Sound_1_0{ static_cast<uint32_t>(m_format), m_data, m_size});
    }

    int32_t Stop()
    {
        return NativeInvoke(Sound_1_0{ static_cast<uint32_t>(ESoundFormat::esfStop)});
    }

protected:
    void* m_data = nullptr;
    ESoundFormat m_format = ESoundFormat::esfStop;
    uint16_t m_size = 0;
};

#undef DrawText
class CDisplay
{
public:
    CDisplay(int display)
        : m_nDisplay(display)
    {}

    int DrawPixelAlpha(uint32_t x, uint32_t y, uint16_t color = 0xFF, uint8_t alpha = 0)
    {
        return NativeInvoke(DrawPixelAlpha_1_0{m_nDisplay, x, y, color, alpha});
    }

    int DrawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint16_t color = 0xFF)
    {
        return NativeInvoke(DrawLine_1_0{m_nDisplay, x1, y1, x2, y2, color});
    }

    int Fill(uint16_t color = 0x0)
    {
        return NativeInvoke(Fill_1_0{m_nDisplay, color});
    }

    int FillRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint16_t color = 0xFF)
    {
        return NativeInvoke(FillRect_1_0{m_nDisplay, x, y, w, h, color});
    }

    int FillCircle(uint32_t x, uint32_t y, uint32_t r, uint16_t color = 0xFF, uint8_t pen_width = 1)
    {
        return NativeInvoke(FillCircle_1_0{m_nDisplay, x, y, r, color, pen_width});
    }

    int DrawText(uint32_t x, uint32_t y, const char* text, uint16_t color = 0xFFFF, uint32_t scale = 1, int32_t angle = 0)
    {
        static char buff[256] = {};
        snprintf(buff,sizeof(buff),"%s", text);
        return NativeInvoke(DrawText_1_0{(uint8_t)m_nDisplay, color, x, y, scale, angle, (uint16_t)(std::min)(strlen(text), (size_t)255), buff});
    }

    int DrawBitmap(uint32_t x, uint32_t y, const CBitmap& bmp, uint32_t scale = 1, int32_t angle = 0, uint8_t mirror = 0)
    {
        return NativeInvoke(DrawBitmap_1_0{(uint8_t)m_nDisplay, (uint8_t)bmp.GetFormat(), mirror, x, y, scale, angle, bmp.GetSize(), bmp.GetAddr()});
    }

    int Flush()
    {
        return NativeInvoke(Flush_1_0{m_nDisplay});
    }

    ~CDisplay()
    {
        Flush();
    }

    int Index() const
    {
        return m_nDisplay;
    }
protected:
    int m_nDisplay = -1;
};

class CEventLoop
{
protected:
    virtual bool OnTick(uint32_t time)
    {
        return true;
    }

    virtual void OnTRBLChanged(const Get_TRBL_1_0&) {}
    virtual void OnGyroChanged(const Get_Gyro_1_0&) {}
    virtual void OnAccelChanged(const Get_Accel_1_0&) {}

    virtual void OnGeoChanged(unsigned char geo_flags)
    {
        if (geo_flags & egfTRBL)
        {
            Get_TRBL_1_0 trbl = {};
            NativeInvoke(trbl);
            OnTRBLChanged(trbl);
        }

        if (geo_flags & egfGyro)
        {
            Get_Gyro_1_0 gyro = {};
            NativeInvoke(gyro);
            OnGyroChanged(gyro);
        }

        if (geo_flags & egfAccel)
        {
            Get_Accel_1_0 accel = {};
            NativeInvoke(accel);
            OnAccelChanged(accel);
        }
    }

    virtual void OnMessage(uint32_t size)
    {
        Get_Message_1_0 msg = {};
        auto buff = GetSharableMem(size);
        msg.data = buff.get();
        msg.size = size;
        NativeInvokeDirect(msg); //getting actual message, must be "Direct" as memory is managed manually here
        if (size)
            OnMessage(size, msg);
        else
            OwnCID(msg.from_cid);
    }

    virtual void OwnCID(uint8_t cid){}

    virtual void OnMessage(uint32_t size, const Get_Message_1_0& msg){};

    virtual void OnShutdown(){}

public:
    virtual int Main()
    {
        NativeInvoke( Send_Message_1_0{ estSelf, 0, NULL} ); //to get own CID, search for m_myCID to see how it is set

        Event_1_0 event = {};
        while (true)
        {
            NativeInvoke(event);
            switch (event.type) {
                case EventType::eTick:
                    if (!OnTick(event.time))
                        return 0;
                    break;

                case EventType::eGEO:
                    OnGeoChanged(event.geo_flags);
                    break;

                case EventType::eMessage:
                    OnMessage(event.msg_size);
                    break;

                case EventType::eShutdown:
                    OnShutdown();
                    return 0;
            }
        }
        return 0;
    }
};

