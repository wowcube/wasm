#include "cube_api.h"
#include <cstdio>

class CEventLoopEx: public CEventLoop
{
protected:
    virtual void OnTick(uint32_t time)
    {
        CEventLoop::OnTick(time);
        for (int display = 0; display < 3; ++display)
        {
            CDisplay disp(display);
            disp.Fill();
            disp.DrawLine(0,0,240,240);
        }
    }

    virtual void OnMessage(const Get_Message_1_0& msg)
    {
        NativePrint("Msg from %d: %s", msg.from_cid, (const char*)msg.data);
        char answer[] = "I am fine too!";
        NativeInvoke(Send_Message_1_0{msg.from_cid, answer, sizeof(answer)});
    }

public:
    virtual int Main()
    {
        NativePrint("Hello WOWd");
        return CEventLoop::Main();
    }
};

extern "C" int run() // native cube code searches for this function and runs as a main()
{
    //whatever you return here will just be recorded into logs
    return CEventLoopEx().Main();
}