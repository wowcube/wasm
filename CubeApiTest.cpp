#include "cube_api.h"
#include <cstdio>
#define WASM_EXPORT __attribute__((used)) __attribute__((visibility ("default")))

class CEventLoopEx: public CEventLoop
{
protected:
    virtual void OnTick(uint32_t time)
    {
        CEventLoop::OnTick(time);
        for (int display = 0; display < 3; ++display)
        {
            CDisplay disp(display);
            disp.Fill(fColor(1,1,1));
            disp.DrawLine(0,0,240,240, 100);
            disp.FillRect(10, 40, 100, 100, 250);

            NativePrint("Draw for display %d\n", display);

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

extern "C" int WASM_EXPORT run() // native cube code searches for this function and runs as a main()
{
    //whatever you return here will just be recorded into logs
    return CEventLoopEx().Main();
}
