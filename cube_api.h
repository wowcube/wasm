#pragma once
#include "natives.h"
#include <typeinfo>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <algorithm>
#include <memory>
#include <cassert>
#include <tuple>

#define WC_CHECKRET(cond, ret) {if (!(cond)) {NativePrint("%s(%d):\t%s\t-\t%s", __FILE__, __LINE__, __FUNCTION__, #cond); assert(!#cond); return ret;}}
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
    uint32_t len = sizeof(intptr_t) + sizeof(T);
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


class CCubeGraph
{
public:
    enum EDir { ecgTop, ecgRight, ecgBottom, ecgLeft };

    struct pair_t {
        unsigned cid : 4;
        unsigned disp : 4;
    };

    void Init(const Get_TRBL_1_0& trbl)
    {
        pair_t faces[6][4] = {}; //0..3 is order on the face

        for (unsigned cid = 0; cid < 8; ++cid) // grouping by faces
            for (unsigned disp = 0; disp < 3; ++disp) {
                faces[trbl.CFID[cid][disp]][trbl.CFMID[cid][disp]] = { cid, disp };
                m_cg[cid][disp].dir[ecgRight] = pair_t{cid, (disp - 1 + 3) % 3 };
                m_cg[cid][disp].dir[ecgBottom] = pair_t{cid, (disp + 1) % 3};
            }

        // every face has 4 pairs in the order from 0 to 3
        for (int face = 0; face < 6; ++face)
        {
            //every next is the left neigbour
            pair_t last = faces[face][3];
            for (int ord = 0; ord < 4; ++ord)
            {
                const pair_t& pair = faces[face][ord];
                m_cg[pair.cid][pair.disp].dir[ecgLeft] = last;
                m_cg[last.cid][last.disp].dir[ecgTop] = pair;
                last = pair;
            }
        }
    }

    const pair_t& GetNeigbour(unsigned cid, unsigned disp, EDir dir)
    {
        return m_cg[cid][disp].dir[dir];
    }

protected:
    struct cube_graph_t {
        pair_t dir[4]; //EDir
    } m_cg[8][3];

};


template<int side> //width&height of the screen
class CScuboT {
    uint8_t m_ord = 0;
    int m_val[2] = {};
    uint8_t m_screens[3] = { 0, 2, 1 };
    uint8_t m_cid = 0;
    const Get_TRBL_1_0& m_trbl;
    CCubeGraph m_cg;

    template<class T>
    void moddec(T& var, int mod) //decrement by module
    {
        var = (var - 1 + mod) % mod;
    }

    auto Face2CID(uint8_t face, uint8_t fsi) //returns -> cid, disp
    {
        for (int cid = 0; cid < 8; ++cid)
            for (int disp = 0; disp < 4; ++disp)
                if (m_trbl.CFID[cid][disp] == face && m_trbl.CFMID[cid][disp] == fsi)
                    return std::make_pair(cid, disp);
        return std::make_pair(-1, -1);
    }

    bool Move(int delta, int val) //val is index of axis
    {
        if (delta >= side && delta < 2 * side) { //jumping to the other screen of the same module, where Y and X exchange
            m_val[val] = m_val[!val]; //flipping X,Y
            m_val[!val] = side - (delta - side) - 1; //X end of one screen touches Y end of other and vise versa
            if (val) //jumping to "previous" screen when 1(Y) is given as movement axis
                moddec(m_ord, 3);
            else //jumping to next screen, by their order
                ++m_ord %= 3; //advance by module
            return true;
        }
        else if (delta < 0) //alas, we go to other cube module
        {
            WC_CHECKRET(delta > -side, false);
            CCubeGraph::pair_t pair = m_cg.GetNeigbour(m_cid, disp(), val ? CCubeGraph::ecgTop : CCubeGraph::ecgLeft);
            m_cid = pair.cid;
            m_ord = disp2ord(pair.disp);
            m_val[val] = m_val[!val]; //flipping X,Y
            m_val[!val] = -delta; //X end of one screen touches Y end of other and vise versa
            return true;
        }
        else if (delta >= 2 * side) //alas, we go to other cube module
        {
            WC_CHECKRET(delta < 3 * side, false);
            return true;
        }
        else //everything happens inside current screen, no jumps required
        {
            m_val[val] = delta;
            return false;
        }
    }

    int disp2ord(int disp)
    {
        return (3 - disp) % 3;
    }

public:

    CScuboT(const Get_TRBL_1_0& trbl, int dx, int dy, int disp = 0, int cid = 0)
        : m_ord(disp2ord(disp)) //display to ord
        , m_cid(cid)
        , m_trbl(trbl)
    {
        m_cg.Init(m_trbl);
        if (Move(dx, 0))
            Move(dy, 0);
        else
            Move(dy, 1);
    }

    struct pack_t //4 bytes
    {
        unsigned char cid;
        unsigned char disp;
        unsigned char x, y;
    };

    CScuboT(const Get_TRBL_1_0& trbl, const pack_t& pack)
        : m_ord(disp2ord(pack.disp)) //display to ord
        , m_cid(pack.cid)
        , m_trbl(trbl)
    {
        m_val[0] = pack.x;
        m_val[1] = pack.y;
        m_cg.Init(m_trbl);
    }
    int x() { return m_val[0]; }
    int y() { return m_val[1]; }
    int disp() { return m_screens[m_ord]; }
    int cid() { return m_cid; }

    pack_t pack()
    {
        static_assert(side < 256);
        assert(m_val[0] >= 0 && m_val[1] >= 0);
        return pack_t{ m_cid, m_screens[m_ord], (uint8_t)m_val[0], (uint8_t)m_val[1] };
    }

#define WC_TEST() static void test()
#define WC_TEST_CHECK(cond) {if (!(cond)) {NativePrint("%s(%d): %s TEST FAIL:", __FILE__, __LINE__, __FUNCTION__, #cond); assert(!#cond);}}
    WC_TEST() {
        Get_TRBL_1_0 trbl = {};
        {
            CScuboT<240> scb(trbl, 240, 240);
            WC_TEST_CHECK(scb.disp() == 1);
            WC_TEST_CHECK(scb.x() == 239);
            WC_TEST_CHECK(scb.y() == 239);
        }
        {
            CScuboT<240> scb(trbl, 0, 240);
            WC_TEST_CHECK(scb.disp() == 1);
            WC_TEST_CHECK(scb.x() == 239);
            WC_TEST_CHECK(scb.y() == 0);
        }
    }
};

using CScubo = CScuboT<240>;

class CScuboSprite
{
    int m_x, m_y, m_w, m_h;
    const Get_TRBL_1_0& m_trbl;
public:
    CScuboSprite(const Get_TRBL_1_0& trbl, int x, int y, int w, int h)
        : m_x(x), m_y(y), m_w(w), m_h(h)
        , m_trbl(trbl)
    {
    }
    CScubo TopLeft() { return CScubo(m_trbl, m_x, m_y); }
    CScubo TopRight() { return CScubo(m_trbl, m_x + m_w, m_y); }
    CScubo BottomLeft() { return CScubo(m_trbl, m_x, m_y + m_h); }
    CScubo BottomRight() { return CScubo(m_trbl, m_x + m_w, m_y + m_h); }
    CScubo Center() { return CScubo(m_trbl, m_x + m_w / 2, m_y + m_h / 2); }

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

