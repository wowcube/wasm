#pragma once

#include "stddef.h"
#include "stdint.h"

//must never be called with local pointers, use NativeInvoke function or similar technics
int native_invoke(const char* cmd, void* buff, size_t size);


#pragma pack(push, 1)

typedef struct {
    enum Event{
        eTick,
        eGEO,
        eMessage,
    } type;

    enum EGeoFlags{
        egfTRBL = 0x1,  // Get_TRBL_1_0
        egfGyro = 0x2,  // Get_Gyro_1_0
        egfAccel = 0x4, // Get_Accel_1_0
    };

    union {
        uint32_t time;      // when eTick
        uint8_t geo_flags;  // combination of EGeoFlags when eGEO
        size_t msg_size;    // when eMessage
    };
} Event_1_0;

typedef struct
{
    uint8_t from_cid;
    void* data; // must be pre-allocated by receiver to msg_size
} Get_Message_1_0;

typedef struct
{
    uint8_t to_cid;
    void* data;
    size_t size;
} Send_Message_1_0;

typedef struct
{

} Get_TRBL_1_0;

typedef struct
{
    const char* buff; //null terminated string
} Print_1_0;

uint16_t fColor(float r, float g, float b){ // 0.0f .. 1.0f
    return ((int)(r*0x1F) << 11)|((int)(g*0x3F)<<5)|(int)(b*0x1F);
}

typedef struct
{
    int displayNumber;
    uint32_t x, y;
    uint16_t color;
    uint8_t alpha;
} DrawPixelAlpha_1_0;

typedef struct {
    int displayNumber;
    uint32_t x1, y1, x2, y2;
    uint16_t color;
} DrawLine_1_0;

typedef struct {
    int displayNumber;
    uint16_t color;
} Fill_1_0;

typedef struct {
    int displayNumber;
    uint32_t x, y, w, h;
    uint16_t color;
} FillRect_1_0;

typedef struct {
    int displayNumber;
    uint32_t x, y, r;
    uint16_t color;
    uint8_t pen_width;
} FillCircle_1_0;

typedef struct {
    int displayNumber;
    const char* str;
    uint32_t x, y;
    uint16_t color;
    uint32_t scale;
    int32_t angle;
} DrawText_1_0;

typedef struct {
    int displayNumber;
} Flush_1_0;

typedef struct {
    int displayNumber;
    const char* addr;
    enum {edbRLE, edb565, edbJPG} format;
    uint32_t x, y;
    uint32_t scale;  // ignored so far
    int32_t angle;
    uint8_t mirror;
} DrawBitmap_1_0;

#pragma pack(pop)