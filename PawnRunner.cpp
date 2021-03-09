#include "cube_api.h"
#include <cstdio>
#include <map>
#include <list>
#include <functional>
#include "pawn2c.h"


extern "C" int pawn_run(cell* pkt, int size, int* src);


inline uint16_t ToRGB565(uint32_t r, uint32_t g, uint32_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b >> 3) & 0x1F);
}

inline uint16_t ToRGB565(cell color) {
    return ToRGB565((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}


class CEventLoopEx: public CEventLoop
{
    uint32_t m_nPrevTime = 0;
    uint8_t m_myCID = 0xFF;
    Get_TRBL_1_0 m_trbl = {};
    struct SBuff
    {
        uint16_t color;
        using TStep = std::function<void(CDisplay&)>;
        std::list<TStep> steps;
    };
    std::map<int, SBuff> m_buff;
    std::map<int, CBitmap> m_bitmaps;

    size_t print_transpon(const uint8_t(&matrix)[8][3], char* buffer) {
        uint8_t result[3][8];

        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 3; j++)
                result[j][i] = matrix[i][j];

        int l = 0;
        for (int j = 0; j < 3; j++) {
            if (j)
                buffer[l++] = '\n';
            for (int i = 0; i < 8; i++) {
                if (i)
                    buffer[l++] = ' ';
                l += sprintf(&buffer[l], "%x", result[j][i]);
            }
        }
        buffer[l++] = 0;
        return l;
    }

    Get_Gyro_1_0 m_gyro = {};
    Get_Accel_1_0 m_accel = {};
protected:
    void OnTRBLChanged(const Get_TRBL_1_0& trbl) override
    {
        m_trbl = trbl;
        char buffer[256];
        print_transpon(m_trbl.CID, buffer);
        NativePrint("OnTRBLChanged CID %s", buffer);

        print_transpon(m_trbl.CFID, buffer);
        NativePrint("OnTRBLChanged CFID %s", buffer);

        print_transpon(m_trbl.CFMID, buffer);
        NativePrint("OnTRBLChanged CFMID %s", buffer);

        auto pkt = GetSharableMem(20);// CMD_GEO{8},n_records{8},CID[0]{8},screen[0]{8},...,CID[N]{8},screen[N]{8}

        Get_Legacy_1_0 glTRBL= {Get_Legacy_1_0::eglTRBL, pkt.get(), 20};
        $(glTRBL);

        pawn_run((int*)pkt.get(), 20/sizeof(cell), 0);
    }
    void OnGyroChanged(const Get_Gyro_1_0& gyro) override
    {
        m_gyro = gyro;
        //NativePrint("OnGyroChanged X:%x Y:%x Z:%x", gyro.axis_X, gyro.axis_Y, gyro.axis_Z);
        UpdateMTD();
    }
    void OnAccelChanged(const Get_Accel_1_0& accel) override
    {
        m_accel = accel;
        //NativePrint("OnAccelChanged X:%x Y:%x Z:%x", accel.axis_X, accel.axis_Y, accel.axis_Z);
        UpdateMTD();
    }

    void UpdateMTD()
    {
#define MTD_POS_NEG(val) (val > 0 ? cell(val) : 0), (val < 0 ? cell(-val) : 0)
        /*CMD_MTD{8},
        accel_X_pos{ 8 }, accel_X_neg{ 8 }, accel_Y_pos{ 8 }, accel_Y_neg{ 8 }, accel_Z_pos{ 8 }, accel_Z_neg{ 8 },
            gyro_X_pos{ 8 }, gyro_X_neg{ 8 }, gyro_Y_pos{ 8 }, gyro_Y_neg{ 8 }, gyro_Z_pos{ 8 }, gyro_Z_neg{ 8 }*/
        cell amx_pkt_mtd[16] = {
            CMD_MTD,
            MTD_POS_NEG(m_accel.axis_X),
            MTD_POS_NEG(m_accel.axis_Y),
            MTD_POS_NEG(m_accel.axis_Z),
            MTD_POS_NEG(m_gyro.axis_X),
            MTD_POS_NEG(m_gyro.axis_Y),
            MTD_POS_NEG(m_gyro.axis_Z),
        };
        pawn_run((cell*)amx_pkt_mtd, sizeof(amx_pkt_mtd) / sizeof(cell), 0);
    }

    bool OnTick(uint32_t time) override
    {
        cell amx_pkt_tick[] = {CMD_TICK, 0, 0, 0};

        pawn_run((cell*)amx_pkt_tick, sizeof(amx_pkt_tick)/sizeof(cell), 0);

        return CEventLoop::OnTick(time);
    }

    void Draw(CDisplay& disp, int x, int y, int w, int h, bool fill)
    {
        disp.FillRect(x, y, w, h, fill ? fColor(1,0,0) : fColor(0,0,1));
    }

    void OnMessage(uint32_t size, const Get_Message_1_0& msg) override
    {
        NativePrint("Msg from %d: %s", msg.from_cid, (const char*)msg.data);
        cell amx_pkt_rx[5] = {CMD_NET_RX}; //CMD_NET_RX{8},line_rx{8},neighbor_line_tx{8},TTL{8},<4 CELLs of arbitrary data here>{128}
        /*
        amx_pkt_rx[0] = ((((CMD_NET_RX) & 0xFF) << 0) | // translate (replace) NET_CMD_AMX to CMD_NET_RX for Pawn
            ((line_id & 0xFF) << 8) | // UART 0-2 through which packet was received
            ((pkt.header.neighbor_line_tx & 0xFF) << 16) | // UART 0-2 through which packet was sent by neighbor
            ((pkt.header.ttl & 0xFF) << 24)); // TTL "as is" as it was sent by neighbor
        */
        WC_CHECKRET(4 * sizeof(cell) == size, WC_NORET);
        memcpy(&amx_pkt_rx[1], msg.data, size);
        pawn_run((cell*)amx_pkt_rx, sizeof(amx_pkt_rx) / sizeof(cell), 0);
    }

    void OwnCID(uint8_t cid) override
    {
        m_myCID = cid;
        NativePrint("MY CID IS %d", cid);
    }


public:
    int Main() override
    {
        NativePrint("Hello WOWd\n");
        $( Send_Message_1_0{estSelf, 0, NULL} );
        return CEventLoop::Main();
    }

    void Send(uint8_t line, uint32_t size, void* data)
    {
        if (0xFF == m_myCID)
            return;
        auto copy = GetSharableMem(size);
        memcpy(copy.get(), data, size);
        $(Send_Message_1_0{m_trbl.CID[m_myCID][line], size, copy.get()});
    }

    void FB_FlushTmpFrameBuffer(int screen, int buff)
    {
        CDisplay disp(screen);

        for (auto& step: m_buff[buff].steps)
        {
            step(disp);
        }
        
        m_buff[buff].steps.clear();
        
    }
    void DISPLAY_flushFramebufferAsync(int screen)
    {
    }
    void FB_fillBuffer(int buff, uint16_t color)
    {
        /*
        m_buff[buff].steps.push_back(
            [color](CDisplay& disp) {
                disp.Fill(color);
            }
        );
        */
    }

    void DrawBitmap(int buff, uint16_t resID, uint16_t x, uint16_t y, uint16_t angle, uint8_t mirror, bool g2d)
    {
        resource_t res = get_resource(resID);
        if (!m_bitmaps.count(resID)) {
            bool ret = m_bitmaps[resID].Load(res.ptr, res.size*sizeof(uint16_t), EPictureFormat::epfRLE);
            WC_CHECKRET(ret, WC_NORET);
        }

        m_buff[buff].steps.push_back(
            [resID, x, y, angle, mirror, this](CDisplay& disp) {
                disp.DrawBitmap(x,y, m_bitmaps[resID], 1, angle, mirror);
            }
        );
    }
};

CEventLoopEx g_runner;

WASM_EXPORT int run() // native cube code searches for this function and runs as a main()
{
    //whatever you return here will just be recorded into logs
    return g_runner.Main();
}

int pawn_tmp_framebuffer = 3; //offscreen display

extern "C"
{
    cell abi_CMD_BITMAP(cell resID, cell x, cell y, cell angle, cell mirror, int g2d)
    {
        //printf("CMD_BITMAP resID %d x, y: %d %d angle %d mirror %d g2d %d\n", resID, x, y, angle, mirror, g2d);

        g_runner.DrawBitmap(pawn_tmp_framebuffer, resID, x, y, angle, mirror, g2d);
        return 0;
    }
    // Remove TODO on realization
    cell abi_CMD_REDRAW(cell faceN)
    {
        //printf("TODO: abi_CMD_REDRAW %d\n", faceN);
        g_runner.FB_FlushTmpFrameBuffer(faceN, pawn_tmp_framebuffer);
        g_runner.DISPLAY_flushFramebufferAsync(faceN);
        return 0;
    }

    cell abi_exit()
    {
        printf("TODO: abi_exit\n");
        return 0;
    }

    cell abi_trigger_debug_info()
    {
        printf("TODO: abi_trigger_debug_info\n");
        return 0;
    }

    cell abi_trigger_bluetooth()
    {
        printf("TODO: abi_trigger_bluetooth\n");
        return 0;
    }

    cell abi_CMD_FILL(cell R, cell G, cell B)
    {
        //printf("TODO: abi_CMD_FILL %d %d %d\n", R, G, B);

        g_runner.FB_fillBuffer(pawn_tmp_framebuffer, ToRGB565(R, G, B)); // RGB565
        return 0;
    }

    cell abi_CMD_FILL_2(cell rgb)
    {
        printf("TODO: abi_CMD_FILL_2 %d\n", rgb);
        g_runner.FB_fillBuffer(pawn_tmp_framebuffer, ToRGB565(rgb));
        return 0;
    }

    cell abi_CMD_TEXT(cell* text, cell fontResID, cell x, cell y, cell scale, cell angle, cell r, cell g, cell b, cell size)
    {
        printf("TODO: abi_CMD_TEXT  * text (not supported),  fontResID %d,  x %d,  y %d,  scale %d,  angle %d,  r %d,  g %d,  b %d,  size %d", fontResID, x, y, scale, angle, r, g, b, size);
        return 0;
    }

    cell abi_CMD_PLAYSND(cell id, cell volume)
    {
        printf("TODO: abi_CMD_PLAYSND id %d, volume %d\n", id, volume);
        return 0;
    }

    cell abi_CMD_LINE(cell x1, cell y1, cell x2, cell y2, cell R, cell G, cell B)
    {
        printf("TODO: abi_CMD_LINE x1 %d,  y1 %d,  x2 %d,  y2 %d,  R %d,  G %d,  B %d\n", x1, y1, x2, y2, R, G, B);
        return 0;
    }

    cell abi_CMD_RECT(cell x1, cell y1, cell x2, cell y2, cell R, cell G, cell B)
    {
        printf("TODO: abi_CMD_RECT x1 %d,  y1 %d,  x2 %d,  y2 %d,  R %d,  G %d,  B %d\n", x1, y1, x2, y2, R, G, B);
        return 0;
    }

    cell abi_CMD_NET_TX(cell line_tx, cell TTL, cell* data, cell with_pool)
    {
        //printf("TODO: abi_CMD_NET_TX line_tx %d,  TTL %d,  data (not supported),  with_pool %d\n", line_tx, TTL, with_pool);
        g_runner.Send(line_tx, 4 * sizeof(int), data);
        return 0;
    }

    cell abi_CMD_SAVE_STATE(cell* data, cell size)
    {
        printf("TODO: abi_CMD_SAVE_STATE data (not supported), size %d\n", size);
        return 0;
    }

    cell abi_CMD_LOAD_STATE()
    {
        printf("TODO: abi_CMD_LOAD_STATE\n");
        return 0;
    }

    cell abi_CMD_CHANGE_SCRIPT(cell scriptID)
    {
        printf("TODO: abi_CMD_CHANGE_SCRIPT scriptID %d\n", scriptID);
        return 0;
    }

    cell abi_CMD_SLEEP()
    {
        printf("TODO: abi_CMD_SLEEP\n");
        return 0;
    }

    cell abi_CMD_G2D_BEGIN_BITMAP(cell resID, cell width, cell height, int replace)
    {
        printf("TODO: abi_CMD_G2D_BEGIN_BITMAP resID %d, width %d, height %d, replace %d\n", resID, width, height, replace);
        return 0;
    }

    cell abi_CMD_G2D_BEGIN_DISPLAY(cell display, int replace)
    {
        //printf("TODO: abi_CMD_G2D_BEGIN_DISPLAY display %d, replace %d\n", display, replace);
        return 0;
    }

    cell abi_CMD_G2D_ADD_SPRITE(cell resID, int g2d, cell x, cell y, cell alpha, cell color, cell rotation, cell mirror)
    {
        //printf("TODO: abi_CMD_G2D_ADD_SPRITE  resID %d, g2d %d,  x %d,  y %d,  alpha %d,  color %d,  rotation %d,  mirror%d\n", resID, g2d, x, y, alpha, color, rotation, mirror);
        g_runner.DrawBitmap(pawn_tmp_framebuffer, resID, x, y, rotation, mirror, g2d);
        return 0;
    }

    cell abi_CMD_G2D_ADD_RECTANGLE(cell x, cell y, cell width, cell height, cell color)
    {
        printf("TODO: abi_CMD_G2D_ADD_RECTANGLE  x %d,  y %d,  width %d,  height %d,  color %d\n", x, y, width, height, color);
        return 0;
    }

    cell abi_CMD_G2D_END()
    {
        //printf("TODO: abi_CMD_G2D_END\n");
        return 0;
    }

    cell abi_trigger_nightlamp(cell c_mode)
    {
        printf("TODO: abi_trigger_nightlamp c_mode %d\n", c_mode);
        return 0;
    }
}
