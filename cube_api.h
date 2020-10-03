#include <stdio.h>
#include <typeinfo>

//must never be called with local pointers, use NativeInvoke
int native_invoke(const char* cmd, void* buff, size_t size);

template<class T>
int NativeInvoke(const T& val)
{
    static T global_value;
    global_value = val;
    return native_invoke(typeid(T).name(), &global_value, sizeof(T));
}

template<class T>
int NativeInvoke(T& val)
{
    static T global_value;
    global_value = val;
    int res = native_invoke(typeid(T).name(), &global_value, sizeof(T));
    val = global_value;
    return res;
}

#define assert(cond) //FIXME: define

uint16_t fColor(float r, float g, float b){
    assert(1.>= r && r>=0);
    assert(1.>= g && g>=0);
    assert(1.>= b && b>=0);
    return (int(r*0x1F) << 11)|(int(g*0x3F)<<5)|int(b*0x1F);
}

struct FB_drawPixelAlpha_1_0
{
    int displayNumber;
    uint32_t x, y;
    uint16_t color;
    uint8_t alpha;
};

struct FB_drawLine_1_0 {
    int displayNumber;
    uint32_t x1, y1, x2, y2;
    uint16_t color;
};

struct Event_1_0 {
    enum Event{
        eTick,
        eGEO,
        eMessage,
    } type;

    enum EGeoFlags{
        egfTRBL = 0x1,
        egfGyro = 0x2,
        egfAccel = 0x4,
    };

    union {
        uint32_t time;
        unsigned char geo_flags; // combination of EGeoFlags
        size_t msg_size;
    };
};

struct Get_Message_1_0
{
    void* data; // must be pre-allocated by receiver to msg_size
};

struct Get_TRBL_1_0
{

};