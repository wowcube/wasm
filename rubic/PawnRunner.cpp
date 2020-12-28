#include "cube_api.h"
#include <cstdio>
#include "rubic/header.h"
extern "C" int pawn_run(cell* pkt, int size, int* src);


class CEventLoopEx: public CEventLoop
{
    uint32_t m_nPrevTime = 0;
    uint8_t m_myCID = 0xFF;
    Get_TRBL_1_0 m_trbl = {};

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
    virtual void OnTRBLChanged(const Get_TRBL_1_0& trbl)
    {
        m_trbl = trbl;
        char buffer[256];
        print_transpon(m_trbl.CID, buffer);
        NativePrint("OnTRBLChanged CID %s", buffer);

        print_transpon(m_trbl.CFID, buffer);
        NativePrint("OnTRBLChanged CFID %s", buffer);

        print_transpon(m_trbl.CFMID, buffer);
        NativePrint("OnTRBLChanged CFMID %s", buffer);

        cell amx_pkt_geo[20] = {0};
        pawn_run(amx_pkt_geo, sizeof(amx_pkt_geo)/sizeof(cell), 0);
    }
    virtual void OnGyroChanged(const Get_Gyro_1_0& gyro)
    {
        m_gyro = gyro;
        NativePrint("OnGyroChanged X:%x Y:%x Z:%x", gyro.axis_X, gyro.axis_Y, gyro.axis_Z);
        cell amx_pkt_mtd[16] = {0};
        pawn_run((cell*)amx_pkt_mtd, sizeof(amx_pkt_mtd)/sizeof(cell), 0);
    }
    virtual void OnAccelChanged(const Get_Accel_1_0& accel)
    {
        m_accel = accel;
        NativePrint("OnAccelChanged X:%x Y:%x Z:%x", accel.axis_X, accel.axis_Y, accel.axis_Z);
        cell amx_pkt_mtd[16] = {0};
        pawn_run((cell*)amx_pkt_mtd, sizeof(amx_pkt_mtd)/sizeof(cell), 0);
    }

    virtual bool OnTick(uint32_t time)
    {
        cell amx_pkt_tick[] = {CMD_TICK, 0, 0, 0};

        pawn_run((cell*)amx_pkt_tick, sizeof(amx_pkt_tick)/sizeof(cell), 0);

        for (int display = 0; display < 3; ++display)
        {
            CDisplay disp(display);
            disp.Fill(fColor(1,1,1));

            if (display == 0)
            {

            }

            if (display == 1)
            {

            }

            if (display == 2)
            {
            }
            
            disp.DrawLine(0,0,240,240, 100);
            disp.DrawPixelAlpha(66, 66, 255, 2);
            //disp.FillCircle(120,120, 30, 100, 2);

            if (m_nPrevTime)
            {
                uint32_t diff = time - m_nPrevTime;
                static char buff[20] = {};
                sprintf(buff, "fps: %f", 1000. / diff);
                disp.DrawText(0, 0, buff, fColor(1,1,1), 30);
            }
            m_nPrevTime = time;

            //NativePrint("Draw for display %d, time: %d\n", display, time);
        }
        return CEventLoop::OnTick(time);
    }

    void Draw(CDisplay& disp, int x, int y, int w, int h, bool fill)
    {
        disp.FillRect(x, y, w, h, fill ? fColor(1,0,0) : fColor(0,0,1));
    }

    void OnMessage(uint32_t size, const Get_Message_1_0& msg) override
    {
        NativePrint("Msg from %d: %s", msg.from_cid, (const char*)msg.data);
        char answer[] = "I am fine too!";
        NativeInvoke(Send_Message_1_0{msg.from_cid, sizeof(answer), answer});
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
        NativeInvoke( Send_Message_1_0{estSelf, 0, NULL} );
        return CEventLoop::Main();
    }

    void Send(uint8_t line, uint32_t size, void* data)
    {
        if (0xFF == m_myCID)
            return;
        NativeInvoke(Send_Message_1_0{m_trbl.CID[m_myCID][line], size, data});
    }
};

CEventLoopEx g_runner;

WASM_EXPORT int run() // native cube code searches for this function and runs as a main()
{
    //whatever you return here will just be recorded into logs
    return g_runner.Main();
}


//TODO: check all (usure) outputs with real data (just search it)
WC_EXTERN_C int sendpacket(int* packet, int size)
{ 
    int result = 0;
    //native_invoke();
   
    /*
    // receives packet from Pawn Script
    cell result = 0;
    uint8_t* pkt = (uint8_t*)amx_Address(amx, params[1]);
    //uint16_t size = params[2] * sizeof(cell); // it passed but not used now, commented to avoid compilation warnings
    uint8_t cmd = pkt[0];
    */
    uint8_t* pkt = (uint8_t*)packet;
    uint16_t sizeUnpacked = size * sizeof(int);
    uint8_t cmd = pkt[0];
    
    switch (cmd)
    {
    case CMD_REDRAW: {
        uint8_t screen = pkt[1];

//        FB_FlushTmpFrameBuffer(screen, pawn_tmp_framebuffer);
//        if (CUBIOS_isDebugInfoEnabled()) {
//            FB_drawDebugInfo(screen);
//        }
//        
//#ifdef DEBUG_DRAW_FPS
//        FB_drawFPS(screen);
//#endif
//        FB_drawBatteryInfo(screen, CUBIOS_getBatteryCharge(), CUBIOS_isCharging());
//        if (CUBIOS_isBluetoothEnabled()) {
//            FB_drawBluetoothOn(screen);
//        }
//        DISPLAY_flushFramebufferAsync(screen);
        printf("CMD_REDRAW which %d screen: %d\n",cmd, screen);
    }
                   break;

    case CMD_FILL: {
        uint16_t r = (pkt[1] & 0x1F) << 11; // Rmax=31
        uint16_t g = (pkt[2] & 0x3F) << 5; // Gmax=63
        uint16_t b = (pkt[3] & 0x1F) << 0; // Bmax=31
        uint32_t packed = r | g | b;
        //// Old:
        //FB_fillBuffer(pawn_tmp_framebuffer, r | g | b /*packed*/); // RGB565
        printf("CMD_FILL which %d r,g,b: %d %d %d packed: %08X\n", cmd, pkt[1], pkt[2], pkt[3], packed);
    }
                 break;

    case CMD_TEXT: {
        int16_t fontResID = pkt[1] | pkt[2] << 8;
        uint16_t x = pkt[3] | pkt[4] << 8;
        uint16_t y = pkt[5] | pkt[6] << 8;
        uint16_t scale = pkt[7] | pkt[8] << 8;
        uint16_t angle = pkt[9] | pkt[10] << 8;
        uint16_t r = (pkt[11] & 0x1f) << 11; // Rmax=31
        uint16_t g = (pkt[12] & 0x3f) << 5;  // Gmax=63
        uint16_t b = pkt[13] & 0x1f;         // Bmax=31



        //uint8_t scriptID = CUBIOS_getActiveAMXScript();
        //const char* packName = (scriptID == UNDEFINED_SCRIPT) ? CUBIOS_getActiveAMXPackName() : getPackNameByScriptID(scriptID);

        const char* text = (const char*)(pkt + 14);

        printf("CMD_TEXT which %d fontResID: %d x,y: %d %d scale: %d angle: %d r, g, b: %d %d %d\n", cmd, fontResID, x, y, scale, angle, r, g, b);
        //if (fontResID < 0)
        //    FB_drawTextBuffer(pawn_tmp_framebuffer, text, x, y, r | g | b, scale, angle); // RGB565
        //else if (isResourceAvailable(packName, fontResID)) {
        //    FB_drawTextWithExternalRLEFontInBuffer(pawn_tmp_framebuffer, text, packName, fontResID, x, y, r | g | b, scale, angle);
        //}
    }
                 break;

    case CMD_BITMAP: {
        uint16_t resID = pkt[1] | pkt[2] << 8;
        uint16_t x = pkt[3] | pkt[4] << 8;
        uint16_t y = pkt[5] | pkt[6] << 8;
        uint16_t angle = pkt[7] | pkt[8] << 8;
        uint8_t mirror = pkt[9];
        bool g2d = pkt[10];
        printf("CMD_RECT (unsure) which %d resID %d x, y: %d %d angle %d mirror %d g2d %d\n", cmd, resID, x, y, angle, mirror, g2d);
        /*if (!g2d) {
            uint8_t scriptID = CUBIOS_getActiveAMXScript();
            const char* packName = (scriptID == UNDEFINED_SCRIPT) ? CUBIOS_getActiveAMXPackName() : getPackNameByScriptID(scriptID);

            if (isResourceAvailable(packName, resID)) {
                FB_drawExternalRLEBitmapInBuffer2(pawn_tmp_framebuffer, packName, resID, x, y, angle, mirror);
            }
        }
        else {
            FB_drawG2DBitmapInBuffer(
                pawn_tmp_framebuffer,
                DISPLAY_WIDTH,
                DISPLAY_HEIGHT,
                resID,
                x, y,
                angle, mirror);
        }*/
    }
                   break;
                   
    case CMD_LINE: {
        uint16_t x1 = pkt[1] | pkt[2] << 8;
        uint16_t y1 = pkt[3] | pkt[4] << 8;
        uint16_t x2 = pkt[5] | pkt[6] << 8;
        uint16_t y2 = pkt[7] | pkt[8] << 8;
        uint16_t r = (pkt[9] & 0x1F) << 11; // Rmax=31
        uint16_t g = (pkt[10] & 0x3F) << 5; // Gmax=63
        uint16_t b = (pkt[11] & 0x1F) << 0; // Bmax=31
        //FB_drawLineBuffer(pawn_tmp_framebuffer, x1, y1, x2, y2, (r | g | b));
        printf("CMD_LINE (unsure) which %d x1, y1: %d %d x2, y2: %d %d r, g, b: %d %d %d packed: %08X\n", cmd, x1, y1, x2, y2, pkt[9] & 0x1F, pkt[10] & 0x3F, pkt[11] & 0x1F, (r | g | b));
    } break;
    
    case CMD_RECT: {
        uint16_t x1 = pkt[1] | pkt[2] << 8;
        uint16_t y1 = pkt[3] | pkt[4] << 8;
        uint16_t x2 = pkt[5] | pkt[6] << 8;
        uint16_t y2 = pkt[7] | pkt[8] << 8;
        uint16_t r = (pkt[9] & 0x1F) << 11; // Rmax=31
        uint16_t g = (pkt[10] & 0x3F) << 5; // Gmax=63
        uint16_t b = (pkt[11] & 0x1F) << 0; // Bmax=31
        printf("CMD_RECT (unsure) which %d x1, y1, x2, y2: %d %d %d %d r, g, b: %d %d %d\n", cmd, x1, y1, x2, y2, pkt[9] & 0x1F, pkt[10] & 0x3F, pkt[11] & 0x1F);
        /*FB_fillRectBuffer(
            pawn_tmp_framebuffer,
            ((x1 < x2) ? x1 : x2),
            ((y1 < y2) ? y1 : y2),
            ((x1 < x2) ? (x2 - x1) : (x1 - x2)),
            ((y1 < y2) ? (y2 - y1) : (y1 - y2)),
            (r | g | b));*/
    }
  
  break;

    case CMD_NET_TX: {
        uint8_t line_tx = pkt[1]; // UART 0-2 through which to send this packet
        uint32_t* payload = (uint32_t*)&pkt[4];

        uint8_t ttl = (pkt[2] > /*NET_BROADCAST_TTL_MAX*/ 2) ? /*NET_BROADCAST_TTL_MAX*/ 2 : pkt[2]; // expected 0 .. NET_BROADCAST_TTL_MAX
        if (size == 1 + 1)
            printf("CMD_NET_TX which %d payload %08X\n", cmd, payload[0]);
        else if (size == 1 + 2)
            printf("CMD_NET_TX which %d payload %08X %08X\n", cmd, payload[0], payload[1]);
        else if (size == 1 + 3)
            printf("CMD_NET_TX which %d payload %08X %08X %08X\n", cmd, payload[0], payload[1], payload[2]);
        else if (size == 1 + 4)
            printf("CMD_NET_TX which %d payload %08X %08X %08X %08X\n", cmd, payload[0], payload[1], payload[2], payload[3]);
        else
            printf("CMD_NET_TX which %d payload too large\n", cmd);
        //NET_sendAMXPacket(line_tx, ttl, payload);
        
        g_runner.Send(line_tx, size, payload);
    }
                   break;

    case CMD_CHANGE_SCRIPT: {
        uint8_t scriptID = pkt[1];
        printf("CMD_CHANGE_SCRIPT (unsure) which %d scriptID %d\n", cmd, scriptID);
        ///*CUBIOS_setActiveAMXScript(scriptID);
        //G2D_clearResources();
        //if ((scriptID > 0) && (GEO_getModuleCID() == 0)) {
        //    AUDIO_playPlatformSound(SOUND_GAME_START, DEFAULT_VOLUME);
        //}*/
    }
                          break;

    case CMD_DRAW_OVERLAY: {
        printf("CMD_DRAW_OVERLAY which %d\n", cmd);
        //if (GEO_getModuleCID() == 0) {
        //    // Master cube - trigger debug info
        //    CUBIOS_setDebugInfoEnabled(!CUBIOS_isDebugInfoEnabled(), 0);
        //}
        //else if (GEO_getModuleCID() != CID_UNKNOWN) {
        //    // Not master cube - broadcast message
        //    NET_broadcastActionPacket(ACTION_TRIGGER_DEBUG_INFO);
        //}
    }
                         break;

    case CMD_SLEEP: {
        printf("CMD_SLEEP which %d\n", cmd);
        //if (GEO_getModuleCID() == 0) {
        //    // Master cube - go to sleep
        //    if (CUBIOS_getState() != STATE_SLEEP_READY) {
        //        CUBIOS_switchState(STATE_SLEEP_READY);
        //        CUBIOS_setActiveAMXScript(AMX_GAME_MENU);
        //    }
        //}
        //else if (GEO_getModuleCID() != CID_UNKNOWN) {
        //    // Not master cube - broadcast message
        //    NET_broadcastActionPacket(ACTION_SLEEP);
        //}
    }
                  break;

    case CMD_TRIGGER_BLUETOOTH: {
        printf("CMD_TRIGGER_BLUETOOTH which %d\n", cmd);
        //if (GEO_getModuleCID() == MASTER_MODULE_CID) {
        //    // Master cube - toggle bluetooth state
        //    CUBIOS_setBluetoothEnabled(!CUBIOS_isBluetoothEnabled(), 0);
        //}
        //else {
        //    NET_broadcastActionPacket(ACTION_TRIGGER_BLUETOOTH);
        //}
    }
                              break;

    case CMD_G2D_BEGIN_BITMAP: {
        uint16_t resID = pkt[1] | pkt[2] << 8;
        uint16_t width = pkt[3] | pkt[4] << 8;
        uint16_t height = pkt[5] | pkt[6] << 8;
        bool replace = pkt[7];
        printf("CMD_G2D_BEGIN_BITMAP (unsure) which %d resID: %d width, height: %d %d replace: %d\n", cmd, resID, width, height, replace);
        //G2D_requestBeginBitmap(resID, width, height, replace);
    }
                             break;

    case CMD_G2D_BEGIN_DISPLAY: {
        uint8_t display = pkt[1];
        bool replace = pkt[2];
        printf("CMD_G2D_BEGIN_DISPLAY which %d display: %d replace %d", cmd, display, replace);
        //G2D_requestBeginDisplay(display, replace);
    }
                              break;

    case CMD_G2D_ADD_SPRITE: {
        uint16_t resID = pkt[1] | pkt[2] << 8;
        uint16_t scriptID = 0;//TODO pkt[3] ? G2D_SCRIPT_ID : CUBIOS_getActiveAMXScript();
        uint16_t x = pkt[4] | pkt[5] << 8;
        uint16_t y = pkt[6] | pkt[7] << 8;
        uint32_t color = pkt[8] | pkt[9] << 8 | pkt[10] << 16 | pkt[11] << 24;
        uint8_t alpha = pkt[12];
        uint16_t angle = pkt[13] | pkt[14] << 8;
        uint8_t mirror = pkt[15];
        printf("CMD_G2D_ADD_SPRITE (unsure) which %d resID: %d scriptID (to do): %d x, y: %d %d color: %08X alpha: %d angle: %d mirror: %d\n", cmd, resID, scriptID, x, y, color, alpha, angle, mirror);
        /*const char* packName = (scriptID == UNDEFINED_SCRIPT) ? CUBIOS_getActiveAMXPackName() : getPackNameByScriptID(scriptID);
        G2D_requestAddSprite(packName, resID, x, y, alpha, color, angle, mirror);*/
    }
                           break;

    case CMD_G2D_ADD_RECTANGLE: {
        uint16_t x = pkt[1] | pkt[2] << 8;
        uint16_t y = pkt[3] | pkt[4] << 8;
        uint16_t width = pkt[5] | pkt[6] << 8;
        uint16_t height = pkt[7] | pkt[8] << 8;
        uint32_t color = pkt[9] | pkt[10] << 8 | pkt[11] << 16 | pkt[12] << 24;
        printf("CMD_G2D_ADD_RECTANGLE (unsure) which %d x, y: %d %d width, height: %d %d color: %d\n", cmd, x, y, width, height, color);
        //G2D_requestAddRectangle(x, y, width, height, color);
    }
                              break;

    case CMD_G2D_END: {
        printf("CMD_G2D_END which %d\n", cmd);
       // G2D_requestEnd();
    }
                    break;

    case CMD_TRIGGER_NIGHTLAMP: {
        printf("CMD_TRIGGER_NIGHTLAMP which %d\n", cmd);
        //action_args_t args;
        //args.arg0 = 0;
        //args.arg1 = pkt[1];

        //if (GEO_getModuleCID() == MASTER_MODULE_CID) {
        //    // Master cube - toggle nightlamp state
        //    CUBIOS_switchState(STATE_NIGHTLAMP);
        //    CUBIOS_changeNightLampEffect(NIGHTLAMP_ACTION_FLAG_START_NEW_EFFECT, args);
        //}
        //else {
        //    NET_broadcastActionPacketArgs(ACTION_TRIGGER_NIGHTLAMP, NIGHTLAMP_ACTION_FLAG_START_NEW_EFFECT, args);
        //}
    }
                              break;

    case CMD_G2D_DYNAMIC_TEXTURE: {
        //dynamic_texture_effect_t effectType = (dynamic_texture_effect_t)pkt[1];             // 2,3 - not used yet. planned 2 - g2d mode  
        uint32_t time = *((uint32_t*)&pkt[4]); // pkt 4,5,6,7
        //G2D_renderDynamicTexture(g2d_getPawnOutputBuffer(), effectType, time, &pkt[8]);
        printf("CMD_G2D_DYNAMIC_TEXTURE which %d time: %d\n pkt[8]: (reference to some chunk)", cmd, time);
    }
                                break;

    //case CMD_EXIT:
    //    //NET_broadcastActionPacket(ACTION_MENU_EXIT);
    //    //CUBIOS_MenuExit();
    //    printf("CMD_EXIT which %d\n", cmd);
    //    break;

    case CMD_PLAYSND: {
        printf("CMD_PLAYSND which %d args: %d %d\n", cmd, pkt[1], pkt[2]);
        /*if (CUBIOS_getActiveAMXScript() == AMX_GAME_MENU) {
            AUDIO_playPlatformSound((platformSound_t)pkt[1], pkt[2]);
        }
        else {
            sound_t gameSound;
            if (RESOURCES_getGameSoundByID(pkt[1], &gameSound) == 0) {
                AUDIO_playSound(&gameSound, pkt[2]);
            }
        }*/
    }
                    break;

    case CMD_STATE_SAVE: {
        uint16_t saveSize = pkt[1] | pkt[2] << 8;
        uint8_t* saveData = pkt + 4;
        printf("CMD_STATE_SAVE (unsure) which %d saveSize, saveData: %d (chunk of data)\n", cmd, saveSize);
        //CUBIOS_savePawnGameState(saveData, saveSize);
        break;
    }

    case CMD_STATE_LOAD: {
        printf("CMD_STATE_LOAD which %d\n", cmd);
        /*async_amx_request_t pkt;
        if (PKT_createAsyncAmxRequest(&pkt, CMD_STATE_LOAD, NULL, 0)) {
            if (!ASYNC_AMXQ_push(&pkt)) {
                printf("Send async amx request failed\r\n");
            }
        }*/
        break;
    }

    case CMD_STATE_SYNC: {
        printf("CMD_STATE_SYNC which %d\n", cmd);
        //TODO implement file share
        break;
    }
    default:
        printf("Unknown amx request: %d\r\n", cmd);
    }
        
    return result;
}
