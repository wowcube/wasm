#include "cube_api.h"
#include <cstdio>
#include "rubic/header.h"
extern "C" int pawn_run(cell* pkt, int size, int* src);


class CEventLoopEx: public CEventLoop
{
    uint32_t m_nPrevTime = 0;
    uint8_t m_myCID = 0;
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

    virtual void OnMessage(uint32_t size, const Get_Message_1_0& msg)
    {

        if (!msg.data) {
            m_myCID = msg.from_cid;
        }

        NativePrint("Msg from %d: %s", msg.from_cid, (const char*)msg.data);
        char answer[] = "I am fine too!";
        NativeInvoke(Send_Message_1_0{msg.from_cid, answer, sizeof(answer)});
    }

public:
    virtual int Main()
    {
        NativePrint("Hello WOWd\n");
        NativeInvoke( Send_Message_1_0{ estSelf, NULL, 0 } );
        return CEventLoop::Main();
    }
};


WASM_EXPORT int run() // native cube code searches for this function and runs as a main()
{
    //whatever you return here will just be recorded into logs
    return CEventLoopEx().Main();
}


WC_EXTERN_C int sendpacket(int* packet, int size)
{
    //FIXME: fill up with CMD_ decoding into native_invoke commands
    return 0;
}
