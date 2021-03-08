#include "../cube_api.h"
#include <cstdio>

char race[] = "race!";
char race_ok[] = "race_ok";

class CEventLoopEx: public CEventLoop
{
    uint32_t m_nPrevTime = 0;
    uint8_t m_myCID = 0;

protected:

    bool OnTick(uint32_t time) override
    {
        for (int display = 0; display < 3; ++display)
        {
            CDisplay disp(display);
            disp.Fill(fColor(1,1,1));
        }
        return CEventLoop::OnTick(time);
    }

    void OnMessage(uint32_t size, const Get_Message_1_0& msg) override
    {
        if (size == sizeof(race) && !strcmp(race, (const char*)msg.data)) {
            NativeSend(msg.from_cid, race_ok); //confirming we took it
        }    
    }

    void OwnCID(uint8_t cid) override
    {
        m_myCID = cid;
        if (cid == 0) {
            NativeSend(estAll, race);
        }
    }


public:
    int Main() override
    {
        NativePrint("Hello WOWd\n");
        return CEventLoop::Main();
    }
};

void MemTest()
{
    NativePrint("MemTest\n");
    size_t size = 6 * 1024 * 1024;

    size_t* buff = (size_t*)malloc(size);
    if (!buff)
    {
        NativePrint("MemTest FAIL: Can't alloc %d bytes\n", size);
        return;
    }

    NativePrint("MemTest Allocated: %d bytes\n", size);
    for (size_t i = 0; i < size / sizeof(size_t); i += 0xff)
        buff[i] = i;

    NativePrint("MemTest Checking...\n");
    bool passed = true;
    for (size_t i = 0; i < size / sizeof(size_t); i += 0xff)
        if (buff[i] != i)
        {
            passed = false;
            break;
        }

    NativePrint(passed ? "MemTest PASSED!!!\n" : "MemTest Fail :(\n");
    free(buff);
}

WASM_EXPORT int run() // native cube code searches for this function and runs as a main()
{
    MemTest();
    //whatever you return here will just be recorded into logs
    return CEventLoopEx().Main();
}
