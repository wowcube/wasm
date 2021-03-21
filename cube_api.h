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


#define WC_TRACE 0
#if WC_TRACE
    #define WCPL() NativePrint("%s(%d):\t%s", __FILE__, __LINE__, __FUNCTION__)
    #define WCPLS(scope) NativePrint("%s(%d):\t%s \t%s", __FILE__, __LINE__, __FUNCTION__, scope)
#else
    #define WCPL() 0
    #define WCPLS(scope) 0
#endif

struct fpoint_t {
    float x, y;
};

struct point_t {
    int x, y;
    point_t(int _x, int _y) : x(_x), y(_y)
    {}
    point_t(double _x, double _y)
    {
        x = int(_x + 0.5);
        y = int(_y + 0.5);
    }
    point_t(): x(0), y(0) {}
};

struct rect_t {
    point_t lt;
    point_t rb;
    rect_t() {
        lt = {};
        rb = {};
    }
    rect_t(const point_t& center, int width, int height)
    {
        lt = { center.x - width / 2, center.y - height / 2 };
        rb = { center.x + width / 2, center.y + height / 2 };
    }
};

bool IsPointInCircle(const point_t& center, int r, const point_t& point)
{
    // Compare radius of circle with distance of its center from given point 
    return (point.x - center.x) * (point.x - center.x) + (point.y - center.y) * (point.y - center.y) <= r * r;
}

bool IsRectOverlap(const rect_t& one, const rect_t& two)
{
    // If one rectangle is on left side of other
    if (one.lt.x >= two.rb.x || two.lt.x >= one.rb.x)
        return false;

    // If one rectangle is above other
    if (one.lt.y >= two.rb.y || two.lt.y >= one.rb.y)
        return false;

    return true;
}

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

#define $(...) WCPLS(#__VA_ARGS__), NativeInvoke(__VA_ARGS__)

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
    int res = NativeInvoke(Send_Message_1_0{to_cid, len + 1, buff.get()});
    va_end(ap);
    return res;
}

#ifdef _WIN32
    #define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

typedef intptr_t(*unique_type_id_t)();

template<class T>
uintptr_t unique_type_id()
{
#if 0
    union {
        char word[4];
        intptr_t val;
    } transfrom = {};
    const char* str = __PRETTY_FUNCTION__;
    const char* type_name = typeid(T).name();
    memcpy(transfrom.word, str, 4); 
    return transfrom.val;
#else
    const char* str = __PRETTY_FUNCTION__;
    const char* type_name = typeid(T).name();
    uintptr_t type_id = reinterpret_cast<intptr_t>(&unique_type_id<T>) - reinterpret_cast<intptr_t>(&unique_type_id<void>);
    return type_id;
#endif
};

template<class T>
int NativeSendBlock(uint8_t to_cid, const T* first, uint16_t count)
{
    uint32_t len = sizeof(intptr_t) + sizeof(T) * count;
    auto buff = GetSharableMem(len);
    uintptr_t* ptr = reinterpret_cast<uintptr_t*>(buff.get());
    *ptr = unique_type_id<T>();
    memcpy(ptr + 1, first, sizeof(T) * count);
    int32_t res = NativeInvoke(Send_Message_1_0{ to_cid, len, buff.get() });
    return res;
}

template<class T>
int NativeSendStruct(uint8_t to_cid, const T& str)
{
    return NativeSendBlock(to_cid, &str, 1);
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
        switch (fmt) {//FIXME: check that epfRGB565 starts from uint16_t w,h;
            case EPictureFormat::epfRGB565:
                assert(!"Unsupported");
                return false;
            case EPictureFormat::epfRLE: {
                const uint16_t* pData = reinterpret_cast<const uint16_t*>(ptr);
                m_width = *pData++;
                m_height = *pData++;
                WC_CHECKRET(m_width <= 240 && m_height <= 240, false);
                m_data = malloc(size);
                memcpy(m_data, ptr, size);
                m_size = size;
                m_format = (EPictureFormat)fmt;
                return true;
            }

            default:
                NativePrint("Format not supported");
                return false;
        }

        return false;
    }

    const void* GetAddr() const { return m_data;     }
    uint32_t GetSize() const { return m_size;     }
    EPictureFormat GetFormat() const { return m_format;   }
    uint16_t Width() { return m_width; }
    uint16_t Height() { return m_height; }

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
    int m_width = 0;
    int m_height = 0;
};


class CSound
{
public:
    bool Load(ESoundFormat esf, const void* ptr, uint16_t size)
    {
        switch (esf) {
            case ESoundFormat::esfMidi:
            case ESoundFormat::esfWAV:
            case ESoundFormat::esfMP3:
                m_data = malloc(size);
                WC_CHECKRET(m_data, false);
                memcpy(m_data, ptr, size);
                m_size = size;
                m_format = esf;
                return true;

            default:
                NativePrint("Format esf:%d not supported", esf);
                return false;
        }

        return false;
    }

    int32_t Play()
    {
        return $(Sound_1_0{ static_cast<uint32_t>(m_format), m_data, m_size});
    }

    int32_t Stop()
    {
        return $(Sound_1_0{ static_cast<uint32_t>(ESoundFormat::esfStop)});
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
    {
        $(Select_1_0{ m_nDisplay});
    }

    int DrawPixelAlpha(uint32_t x, uint32_t y, uint16_t color = 0xFF, uint8_t alpha = 0)
    {
        return $(DrawPixelAlpha_1_0{m_nDisplay, x, y, color, alpha});
    }

    int DrawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint16_t color = 0xFF)
    {
        return $(DrawLine_1_0{m_nDisplay, x1, y1, x2, y2, color});
    }

    int Fill(uint16_t color = 0x0)
    {
        return $(Fill_1_0{m_nDisplay, color});
    }

    int FillRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint16_t color = 0xFF)
    {
        return $(FillRect_1_0{m_nDisplay, x, y, w, h, color});
    }

    int FillCircle(uint32_t x, uint32_t y, uint32_t r, uint16_t color = 0xFF, uint8_t pen_width = 1)
    {
        return $(FillCircle_1_0{m_nDisplay, x, y, r, color, pen_width});
    }

    int DrawText(uint32_t x, uint32_t y, const char* text, uint16_t color = 0xFFFF, uint32_t scale = 1, int32_t angle = 0)
    {
        static char buff[256] = {};
        snprintf(buff,sizeof(buff),"%s", text);
        return $(DrawText_1_0{(uint8_t)m_nDisplay, color, x, y, scale, angle, (uint16_t)(std::min)(strlen(text), (size_t)255), buff});
    }

    int DrawBitmap(uint32_t x, uint32_t y, const CBitmap& bmp, uint32_t scale = 1, int32_t angle = 0, uint8_t mirror = 0)
    {
        return $(DrawBitmap_1_0{(uint8_t)m_nDisplay, (uint8_t)bmp.GetFormat(), mirror, x, y, scale, angle, bmp.GetSize(), bmp.GetAddr()});
    }

    int Flush()
    {
        return $(Flush_1_0{m_nDisplay});
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

    bool OnDisplay(int cid, int disp) {
        return this->cid() == cid && this->disp() == disp;
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
    CScuboSprite(const Get_TRBL_1_0& trbl, int x, int y, int w, int h) //left, top, width, height
        : m_x(x), m_y(y), m_w(w), m_h(h)
        , m_trbl(trbl)
    {
    }
    CScubo TopLeft() { return CScubo(m_trbl, m_x, m_y); }
    CScubo TopRight() { return CScubo(m_trbl, m_x + m_w, m_y); }
    CScubo BottomLeft() { return CScubo(m_trbl, m_x, m_y + m_h); }
    CScubo BottomRight() { return CScubo(m_trbl, m_x + m_w, m_y + m_h); }
    CScubo Center() { return CScubo(m_trbl, m_x + m_w / 2, m_y + m_h / 2); }

    std::unique_ptr<point_t> OnDisplay(int cid, int disp)
    {
        {
            CScubo pt = TopLeft();
            if (pt.OnDisplay(cid, disp))
                return std::make_unique<point_t>(point_t{ pt.x(), pt.y() });
        }

        {
            CScubo pt = TopRight();
            if (pt.OnDisplay(cid, disp))
                return std::make_unique<point_t>(point_t{ pt.x(), pt.y() });
                //return std::make_unique<point_t>(point_t{ pt.x(), pt.y() + m_w });
        }

        {
            CScubo pt = BottomLeft();
            if (pt.OnDisplay(cid, disp))
                return std::make_unique<point_t>(point_t{ pt.x(), pt.y() });
                //return std::make_unique<point_t>(point_t{ pt.x() + m_h, pt.y() });
        }

        if (0){
            CScubo pt = BottomRight();
            if (pt.OnDisplay(cid, disp))
                return std::make_unique<point_t>(point_t{ pt.x() + m_h, pt.y() + m_w });
        }


        return nullptr;
        //return TopRight().OnDisplay(cid, disp) || BottomLeft().OnDisplay(cid, disp) || BottomRight().OnDisplay(cid, disp);
    }

    void Draw(int cid, CDisplay& disp, const CBitmap& bmp, int scale = 1, int angle = 0)
    {
        {
            CScubo pt = TopLeft();
            if (pt.OnDisplay(cid, disp.Index())) {
                disp.DrawBitmap(pt.x() + Width() / 2, pt.y() + Height() / 2, bmp, 1, angle);
                disp.DrawText(0, 40, "TL", fColor(0, 0, 0), 5);
                disp.FillRect(pt.x(), pt.y(), 20, 20, fColor(1, 0, 0));
                return;
            }
        }

        {
            CScubo pt = TopRight();
            if (pt.OnDisplay(cid, disp.Index())) {
                disp.DrawBitmap(pt.x() + Width() / 2, pt.y() + Height() / 2, bmp, 1, angle + 270);
                disp.DrawText(0, 40, "TR", fColor(0, 0, 0), 5);
                disp.FillRect(pt.x(), pt.y(), 20, 20, fColor(0, 1, 0));
                return;
            }
        }

        {
            CScubo pt = BottomLeft();
            if (pt.OnDisplay(cid, disp.Index())) {
                disp.DrawBitmap(pt.x() + Width() / 2, pt.y() + Height() / 2, bmp, 1, angle + 90);
                disp.DrawText(0, 40, "BL", fColor(0, 0, 0), 5);
                disp.FillRect(pt.x(), pt.y(), 20, 20, fColor(0, 0, 1));
                return;
            }
        }



        {
            CScubo pt = BottomRight();
            if (pt.OnDisplay(cid, disp.Index())) {
                disp.DrawBitmap(pt.x() + Width() / 2, pt.y() + Height() / 2, bmp, 1, angle);
                disp.DrawText(0, 40, "BR", fColor(0, 0, 0), 5);
                disp.FillRect(pt.x(), pt.y(), 20, 20, fColor(1, 0, 1));
                return;
            }
        }

    }

    int Width() { return m_w; }
    int Height() { return m_h; }
};


template<class T>
T bound(T val, T left, T right)
{
    if (val > right) val = right;
    if (val < left) val = left;
    return val;
}

template<class T>
fpoint_t AccelGyro(const T& point, int disp) {
    //z01 x12 y02
    //z: -x for D0, -y for D1
    //x: -x for D1, -y for D2
    //y: -x for D2, -y for D0

    const float _2G = -2 * 9.81f;
    fpoint_t val[3] = {
        {-point.axis_Z / _2G, point.axis_Y / _2G}, //Disp 0
        {-point.axis_X / _2G, -point.axis_Z / _2G}, //Disp 1
        {point.axis_Y / _2G, -point.axis_X / _2G} //Disp 2
    };
    return val[disp];
};

class CFPS
{
    uint32_t m_nPrevTime = 0;
    float m_fps = 0.;
    char m_str[32] = {};
public:
    void Tick(uint32_t time)
    {
        if (m_nPrevTime) {
            uint32_t diff = time - m_nPrevTime;
            m_fps = 1000.f / float(diff);
        }
        m_nPrevTime = time;
    }

    const char* c_str()
    {
        snprintf(m_str, sizeof(m_str), "fps: %.2f", m_fps);
        return m_str;
    }

    operator float() {
        return m_fps;
    }
};

class CEventLoop
{
protected:

    virtual bool OnTick(uint32_t time)
    {
        return true;
    }

    float GetFPS()
    {
    }

    virtual void OnTRBLChanged(const Get_TRBL_1_0&) {}
    virtual void OnGyroChanged(const Get_Gyro_1_0&) {}
    virtual void OnAccelChanged(const Get_Accel_1_0&) {}

    virtual void OnGeoChanged(unsigned char geo_flags)
    {
        if (geo_flags & egfTRBL)
        {
            Get_TRBL_1_0 trbl = {};
            $(trbl);
            OnTRBLChanged(trbl);
        }

        if (geo_flags & egfGyro)
        {
            Get_Gyro_1_0 gyro = {};
            $(gyro);
            OnGyroChanged(gyro);
        }

        if (geo_flags & egfAccel)
        {
            Get_Accel_1_0 accel = {};
            $(accel);
            OnAccelChanged(accel);
        }
    }

    void drawTRBLDebug(CDisplay &disp, uint8_t cid, const Get_TRBL_1_0 &trbl) {
        static char buf[64] = {};

        snprintf(buf, sizeof(buf), "DISPN: %d", disp.Index());
        disp.DrawText(0, 0, buf, fColor(0, 0, 0), 2, 0);

        snprintf(buf, sizeof(buf), "CID: %d", cid);
        disp.DrawText(0, 20, buf, fColor(0, 0, 0), 2, 0);

        snprintf(buf, sizeof(buf), "CFID: %d", trbl.CFID[cid][disp.Index()]);
        disp.DrawText(0, 40, buf, fColor(0, 0, 0), 2, 0);

        snprintf(buf, sizeof(buf), "CFMID: %d", trbl.CFMID[cid][disp.Index()]);
        disp.DrawText(0, 60, buf, fColor(0, 0, 0), 2, 0);

        snprintf(buf, sizeof(buf), "%d %d %d", trbl.CID[cid][0], trbl.CID[cid][1], trbl.CID[cid][2]);
        disp.DrawText(0, 80, buf, fColor(0, 0, 0), 2, 0);
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
        $(Send_Message_1_0{ estSelf, 0, NULL}); //to get own CID

        Event_1_0 event = {};
        while (true)
        {
            $(event);
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

