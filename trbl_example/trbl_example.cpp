#include "cube_api.h"
#include <cstdio>

class CEventLoopEx: public CEventLoop
{
    uint8_t m_cid = 0;

protected:

    bool OnTick(uint32_t time) override
    {
        for (int display = 0; display < 3; ++display) {
            CDisplay disp(display);
            disp.Fill(fColor(1,1,1));
            
            static char buf[64] = {};
            snprintf(buf, sizeof(buf), "DISPLAY: %d", display);
            disp.DrawText(0, 0, buf, fColor(0, 0, 0), 1, 0);

            snprintf(buf, sizeof(buf), "MODULE: %d", m_cid);
            disp.DrawText(0, 60, buf, fColor(0, 0, 0), 1, 0);

            disp.DrawLine(120, 0, 120, 120, fColor(1, 0, 0));
            disp.DrawLine(0, 120, 120, 120, fColor(0, 1, 0));

        }
        return CEventLoop::OnTick(time);
    }


    void OwnCID(uint8_t cid) override
    {
        NativePrint("MY CID IS %d", cid);
        m_cid = cid;
    }

public:
    int Main() override
    {
        NativeInvoke( Send_Message_1_0{ estSelf, 0, NULL} );
        return CEventLoop::Main();
    }
};

WASM_EXPORT int run() // native cube code searches for this function and runs as a main()
{
    //whatever you return here will just be recorded into logs
    return CEventLoopEx().Main();
}
