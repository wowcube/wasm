#include "../cube_api.h"
#include "wasm_game.h"
#include <cmath>
#include <map>
#include <vector>


class CEventLoopEx: public CEventLoop
{
    uint8_t m_cid = 0;
    CBitmap m_resource[er_img_Size];
    CScubo::pack_t m_host = {0, 0};
    fpoint_t m_pos = {20, 20};
    Get_Accel_1_0 m_accel = {};
    Get_TRBL_1_0 m_trbl = {};

    struct block_t {
        uint8_t disp;
        uint8_t type;
        point_t pos;
    };

    block_t* m_pLastHit = nullptr;
    bool m_bExplosion = false;

    std::vector<block_t> m_blocks;

    CBitmap& GetBmp(uint8_t res)
    {
        WC_CHECKRET(res < er_img_Size, *(CBitmap*)nullptr);
        CBitmap& bmp = m_resource[res];
        if (!bmp.GetSize())
        {
            resource_t resource = get_resource(res);
            bmp.Load(resource.ptr, resource.size * 2, epfRLE);
        }
        return bmp;
    }

protected:

    void Hit(block_t& block)
    {
        if (m_pLastHit == &block)
            return;
        switch (block.type)
        {
        case er_img_brick_blue:
            return;
        case er_img_brick_solid:
            block.type = er_img_brick_half_broken;
            break;
        case er_img_brick_half_broken:
            block.type = er_img_brick_broken;
            break;
        case er_img_brick_broken:
            block.type = er_img_brick_hazard;
            break;
        case er_img_brick_hazard:
            m_bExplosion = true;
            NativeSendStruct(estAll, m_bExplosion);
            return;
        }
        m_pLastHit = &block;
    }

    bool OnTick(uint32_t time) override
    {
        if (0xFF == m_cid || m_trbl.CID[0][0] == m_trbl.CID[1][1]) //not set yet
            return true;

        CBitmap& ball = GetBmp(er_img_ball_metal);

        for (int display = 0; display < 3; ++display)
        {
            CDisplay disp(display);
            disp.DrawBitmap(GetBmp(er_img_field).Width() / 2, GetBmp(er_img_field).Height() / 2, GetBmp(er_img_field));

            if (m_bExplosion)
            {
                disp.DrawText(16, 120, "GAME OVER", rand(), 4);
                continue;
            }

            std::map<block_t*, rect_t> obstacles;
            for (block_t& block : m_blocks)
                if (block.disp == display)
                {
                    CBitmap & bmp = GetBmp(block.type);
                    point_t center = { block.pos.x + bmp.Width() / 2, block.pos.y + bmp.Height() / 2 };
                    disp.DrawBitmap(center.x, center.y, bmp);
                    obstacles[&block] = rect_t(center, bmp.Width(), bmp.Height());
                }

            if (m_cid == m_host.cid && disp.Index() == m_host.disp)
            {
                disp.DrawBitmap(std::lround(m_pos.x), std::lround(m_pos.y), ball);
                const fpoint_t& delta = AccelGyro(m_accel, disp.Index());
                fpoint_t np = {
                    bound<float>(m_pos.x + delta.x * 2000, -2, 240 - float(ball.Width()) / 2),
                    bound<float>(m_pos.y + delta.y * 2000, -2, 240 - float(ball.Height()) / 2)
                };

                for (auto& obstacle : obstacles)
                {
                    if (np.x != m_pos.x && IsRectOverlap(rect_t(point_t(np.x, m_pos.y), ball.Width(), ball.Height()), obstacle.second))
                    {
                        np.x = m_pos.x;
                        Hit(*obstacle.first);
                    }

                    if (np.y != m_pos.y && IsRectOverlap(rect_t(point_t(m_pos.x, np.y), ball.Width(), ball.Height()), obstacle.second))
                    {
                        np.y = m_pos.y;
                        Hit(*obstacle.first);
                    }
                }

                m_pos = np;

                if (m_pos.x < 0 || m_pos.y < 0)
                {
                    CScubo scubo(m_trbl, std::lround(m_pos.y), std::lround(m_pos.x), m_host.disp, m_host.cid);
                    CScubo::pack_t pack = scubo.pack();
                    if (m_host.cid != pack.cid) {
                        m_host = pack;
                        //NativeSendStruct(estAll, pack); //FIXME: fix broadcast and enable 
                        NativeSendStruct(pack.cid, pack);
                    }
                }
            }
          //  drawTRBLDebug(disp, m_cid, m_trbl);
        }
        return CEventLoop::OnTick(time);
    }

    void OnMessage(uint32_t size, const Get_Message_1_0& msg) override
    {
        uintptr_t type = *(uintptr_t*)msg.data;
        if (unique_type_id<CScubo::pack_t>() == type)
        {
            const CScubo::pack_t& pack = *reinterpret_cast<const CScubo::pack_t*>((uintptr_t*)msg.data + 1);
            m_host = pack;
            m_pos = { float(pack.y), float(pack.x) };
            m_pLastHit = nullptr;
        }
        else if (unique_type_id<bool>() == type)
        {
            const bool& val = *reinterpret_cast<const bool*>((uintptr_t*)msg.data + 1);
            m_bExplosion = val;
        }
    }

    void OwnCID(uint8_t cid) override
    {
        m_cid = cid;

        GenerateLevel();
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
        srand(m_cid);
        for (uint8_t disp = 0; disp < 3; ++disp)
            for (int x = 1; x < 6; ++x)
                for (int y = 1; y < 6; ++y)
                    if (rand() % 4 == 0)
                        m_blocks.push_back(block_t{ disp, uint8_t((rand() % 4 == 0) ? er_img_brick_solid : er_img_brick_blue), point_t{x * 40, y * 40} });
    }

public:
    int Main() override
    {
        return CEventLoop::Main();
    }
};


WASM_EXPORT int run() // native cube code searches for this function and runs as a main()
{
    return CEventLoopEx().Main();
}
