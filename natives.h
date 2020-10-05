#include "stddef.h"
#include "stdint.h"

//must never be called with local pointers, use NativeInvoke
int native_invoke(const char* cmd, void* buff, size_t size);

uint16_t fColor(float r, float g, float b){ // 0.0f .. 1.0f
    return ((int)(r*0x1F) << 11)|((int)(g*0x3F)<<5)|(int)(b*0x1F);
}

typedef struct
{
    int displayNumber;
    uint32_t x, y;
    uint16_t color;
    uint8_t alpha;
} FB_drawPixelAlpha_1_0;

typedef struct {
    int displayNumber;
    uint32_t x1, y1, x2, y2;
    uint16_t color;
} FB_drawLine_1_0;

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