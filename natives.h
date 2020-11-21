#pragma once

#include "stddef.h"
#include "stdint.h"

#ifdef __cplusplus
    #define WASM_EXPORT extern "C" __attribute__((used)) __attribute__((visibility ("default")))
#else
    #define WASM_EXPORT extern __attribute__((used)) __attribute__((visibility ("default")))
#endif

#ifdef __EMSCRIPTEN__
    #define WASM_MAGIC_RESOLVE
#else
    #define WASM_MAGIC_RESOLVE  __attribute__((weak))
#endif

//IMPORTANT: must never be called with local pointers, use NativeInvoke function or similar techniques
//this is the only function for communication with native

WASM_EXPORT int32_t WASM_MAGIC_RESOLVE native_invoke(const char* cmd, void* buff, size_t size); // return value meaning might differer depending on the cmd
//for simple cases it might be used as cmd="GetTemperatureOnMars", buff = nullptr, size = 0, and the temperature will be returned as the native_invoke return value
//the complex cases should pass the input arguments and receive the output arguments, the return value of the function should be treated as the error code
//by default, 0 means "no problem"


#pragma pack(push, 1)

typedef enum {
    egfTRBL = 0x1,  // Get_TRBL_1_0
    egfGyro = 0x2,  // Get_Gyro_1_0
    egfAccel = 0x4, // Get_Accel_1_0
} EGeoFlags;


typedef struct {
    enum Event{
        eTick,
        eGEO,
        eMessage,
        eShutdown,
    } type;

    union {
        uint32_t time;      // when eTick, RTOS_getTimeMs
        EGeoFlags geo_flags;  // combination of EGeoFlags when eGEO
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
    const char* buff; //null terminated string, must be globally/statically allocated to work
    size_t size;
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
    uint8_t displayNumber;
    uint16_t color;
    uint32_t x, y;
    uint32_t scale;
    int32_t angle;
    const char* str;  // MUST be global or static!
} DrawText_1_0;

typedef struct {
    int displayNumber;
} Flush_1_0;

typedef enum {epfNone, epfRLE, epfRGB565, epfJPG} EPictureFormat;

typedef struct {
    uint8_t displayNumber;
    uint8_t format;
    uint8_t mirror; // 0 bit - flip by X, 1 bit - flip by Y, can be both
    uint32_t x, y;
    uint32_t scale;  // ignored so far
    int32_t angle;
    uint64_t size;
    const void* addr;  // MUST be global or static!
} DrawBitmap_1_0;


typedef struct {
    const void* source;  // MUST be global or static!
    uint16_t source_size;
    EPictureFormat source_format;
    void* target;  // MUST be global or static! If null is given, target_size will be calculated
    uint16_t target_size;
    EPictureFormat target_format;
} Transform_1_0;

typedef enum {esfStop, esfMidi, esfMP3, esfWAV} ESoundFormat;

typedef struct {
    ESoundFormat fmt;
    const void* ptr;  // MUST be global or static!
    uint16_t size;
} Sound_1_0;

#pragma pack(pop)
