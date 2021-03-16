#include "../cube_api.h"
#include "wasm_game.h"
#include <cmath>

class CEventLoopEx: public CEventLoop
{
    uint8_t m_cid = 0;
    CBitmap m_field;
    CBitmap m_ball;
    CScubo::pack_t m_host = {0, 0};
    fpoint_t m_pos = {120, 120};
    Get_Accel_1_0 m_accel = {};
    Get_TRBL_1_0 m_trbl = {};

protected:

    bool OnTick(uint32_t time) override
    {
        if (0xFF == m_cid || m_trbl.CID[0][0] == m_trbl.CID[1][1]) //not set yet
            return true;
        for (int display = 0; display < 3; ++display)
        {
            CDisplay disp(display);
            disp.DrawBitmap(m_field.Width() / 2, m_field.Height() / 2, m_field);
            if (m_cid == m_host.cid && disp.Index() == m_host.disp)
            {
                disp.DrawBitmap(std::lround(m_pos.x), std::lround(m_pos.y), m_ball);
                const fpoint_t& delta = AccelGyro(m_accel, disp.Index());
                m_pos.x = bound<float>(m_pos.x + delta.x * 50, -10, 239);
                m_pos.y = bound<float>(m_pos.y + delta.y * 50, -10, 239);

                if (m_pos.x < 0 || m_pos.y < 0)
                {
                    CScubo scubo(m_trbl, std::lround(m_pos.x), std::lround(m_pos.y), m_host.disp, m_host.cid);
                    CScubo::pack_t pack = scubo.pack();
                    if (m_host.cid != pack.cid) {
                        m_host.disp = pack.disp;
                        m_host.cid = pack.cid;
                        for (int i = 0; i < 3; ++i)
                            NativeSendStruct(estAll, pack);
                    }
                }
            }
        }
        return CEventLoop::OnTick(time);
    }

    void OnMessage(uint32_t size, const Get_Message_1_0& msg) override
    {
        WC_CHECKRET(*(uintptr_t*)msg.data == unique_type_id<CScubo::pack_t>(), WC_NORET);
        const CScubo::pack_t& pack = *reinterpret_cast<const CScubo::pack_t*>((uintptr_t*)msg.data + 1);
        m_host.disp = pack.disp;
        m_host.cid = pack.cid;
        m_pos.x = pack.x;
        m_pos.y = pack.y;
    }

    void OwnCID(uint8_t cid) override
    {
        m_cid = cid;
        if (m_cid == 0)
        {
            GenerateLevel();
        }
    }

    void OnAccelChanged(const Get_Accel_1_0& accel) override
    {
        m_accel = accel;
    }

    void OnTRBLChanged(const Get_TRBL_1_0& trbl) override
    {
        if (0 == memcmp(&m_trbl, &trbl, sizeof(Get_TRBL_1_0)))
            return;
        m_trbl = trbl;
    }

    void GenerateLevel()
    {
    }

public:
    int Main() override
    {
        uintptr_t ti = unique_type_id<CScubo::pack_t>();
        NativePrint("TI: %d\n", ti);

        resource_t res = get_resource(er_img_field);
        m_field.Load(res.ptr, res.size * 2, epfRLE);

        res = get_resource(er_img_ball_metal);
        m_ball.Load(res.ptr, res.size * 2, epfRLE);

        return CEventLoop::Main();
    }
};


WASM_EXPORT int run() // native cube code searches for this function and runs as a main()
{
    //whatever you return here will just be recorded into logs
    return CEventLoopEx().Main();
}
