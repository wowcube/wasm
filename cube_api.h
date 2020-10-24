#pragma once
#include "natives.h"
#include <typeinfo>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>

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
    int res = NativeInvoke(Print_1_0{(char*)buff, 1024});
    va_end(ap);
    return res;
}

class CBitmap
{
public:
    ~CBitmap() {
        free(_data);
    }
    bool Load(void* ptr, size_t size, int fmt) {
        switch (fmt) {
        case EBMPFormat::edb565:
            _data = malloc(size);
            memcpy(_data, ptr, size);
            break;
        default:
            NativePrint("Format not supported");
            return false;
        }

        if (_data != nullptr) {
            _size = size;
            _format = (EBMPFormat)fmt;
            return true;
        }
        return false;
    }

    const void* GetAddr()   const {   return _data;     }
    size_t getSize()        const {   return _size;     }
    EBMPFormat GetFormat()  const {   return _format;   }

private:
    void *_data = nullptr;
    size_t _size = 0;
    EBMPFormat _format = edbRLE;
};

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
        static char buff[1024] = {};
        strcpy(buff, text);
        return NativeInvoke(DrawText_1_0{(uint8_t)m_nDisplay, color, x, y, scale, angle, buff});
    }

    int DrawBitmap(uint32_t x, uint32_t y, const CBitmap& bmp, uint32_t scale = 1, int32_t angle = 0, uint8_t mirror = 0)
    {
        return NativeInvoke(DrawBitmap_1_0{(uint8_t)m_nDisplay, (uint8_t)bmp.GetFormat(), mirror, x, y, scale, angle, bmp.getSize(), bmp.GetAddr()});
    }

    int Flush()
    {
        return NativeInvoke(Flush_1_0{m_nDisplay});
    }

    ~CDisplay()
    {
        Flush();
    }

protected:
    int m_nDisplay = -1;
};

class CEventLoop
{
protected:
    virtual void OnTick(uint32_t time)
    {
    }

    virtual void OnGeoChanged(unsigned char geo_flags)
    {
        if (geo_flags & egfTRBL)
        {
            Get_TRBL_1_0 trbl = {};
            NativeInvoke(trbl);
            //TODO: OnTRBLChanged();
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
