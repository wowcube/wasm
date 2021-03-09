#include "../cube_api.h"
#include <cstdio>

struct sync_t {
    unsigned char finished;
} m_sync;

class CEventLoopEx: public CEventLoop
{
    uint8_t m_cid = 0;

protected:

    bool OnTick(uint32_t time) override
    {
        for (int display = 0; display < 3; ++display)
        {
            CDisplay disp(display);
            disp.Fill(fColor(1,1,1));
            for (int i = 0; i < 8; ++i)
            {
                if (m_sync.finished & (1 << i))
                {
                    disp.FillRect(i * 30, 120, 30, 30, fColor(0, 0, 0));
                }
            }
        }
        m_sync.finished |= 1 << m_cid;
        NativeSendStruct(estAll, m_sync);
        return CEventLoop::OnTick(time);
    }

    void OnMessage(uint32_t size, const Get_Message_1_0& msg) override
    {
        if (*(uintptr_t*)msg.data == unique_type_id<sync_t>())
        {
            const sync_t& sync = *reinterpret_cast<const sync_t*>((uintptr_t*)msg.data + 1);
            m_sync.finished |= sync.finished;
            if (sync.finished != m_sync.finished)
                NativeSendStruct(estAll, m_sync);
        }
    }

    void OwnCID(uint8_t cid) override
    {
        m_cid = cid;
        if (cid == 0) {
            NativeSendStruct(estAll, m_sync);
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
