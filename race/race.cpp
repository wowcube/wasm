#include "../cube_api.h"
#include <cstdio>

char race[] = "race!";
char race_ok[] = "race_ok";

class CEventLoopEx: public CEventLoop
{
    uint32_t m_nPrevTime = 0;
    uint8_t m_myCID = 0;
    bool m_bRelayRace = false;

protected:

    bool OnTick(uint32_t time) override
    {
        for (int display = 0; display < 3; ++display)
        {
            CDisplay disp(display);
            disp.Fill(fColor(1,1,1));

            if (m_bRelayRace)
            {
                disp.FillCircle(120,120, 60, 100, 2);
            }
        }
        return CEventLoop::OnTick(time);
    }

    void OnMessage(uint32_t size, const Get_Message_1_0& msg) override
    {
        if (size == sizeof(race) && !strcmp(race, (const char*)msg.data)) {
            m_bRelayRace = true;
            NativeSend(msg.from_cid, race_ok); //confirming we took it
            NativeSend((m_myCID + 1) % 8, race); //send race to another
        } else {
            if (size == sizeof(race_ok) && !strcmp(race_ok, (const char*)msg.data))
            {
                m_bRelayRace = false;
            }
        }
    }

    void OwnCID(uint8_t cid) override
    {
        m_myCID = cid;
        if (!m_myCID) { //starting the relay race!
            m_bRelayRace = true;
            NativeSend(m_myCID + 1, race);
        }
    }


public:
    int Main() override
    {
        NativePrint("Hello WOWd\n");
        return CEventLoop::Main();
    }
};

WASM_EXPORT int run() // native cube code searches for this function and runs as a main()
{
    //whatever you return here will just be recorded into logs
    return CEventLoopEx().Main();
}
