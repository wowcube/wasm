#include "cube_api.h"
#include <cstdio>
#include <tuple>
#include <set>

#define _USE_MATH_DEFINES
#include <math.h>

typedef struct {
    const void* ptr;
    int size;
} resource_t; //FIXME: move to a common header

#include "resources.h"

class CEventLoopEx: public CEventLoop
{
    CFPS m_fps;
    int m_grad270 = 0;
    int m_grad360 = 0;
    Get_TRBL_1_0 m_trbl = {};
    CBitmap m_ball;
    CBitmap m_small_ball;
    CBitmap m_cake;
    uint8_t m_cid = 0xFF;
    std::unique_ptr<CScubo> m_spCrossGeo;
    fpoint_t m_accel_ball[3] = {};


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
    void OnTRBLChanged(const Get_TRBL_1_0& trbl) override
    {
        if (0 == memcmp(&m_trbl, &trbl, sizeof(Get_TRBL_1_0)))
            return;
#ifdef _DEBUG
        std::set<std::pair<int, int>> fo;
        for (int i = 0; i < 8; ++i)
            for (int j = 0; j < 3; ++j)
            {
                WC_CHECKRET(trbl.CID[i][j] != i, WC_NORET); //I am not my neigbour
                WC_CHECKRET(trbl.CFID[i][j] < 6, WC_NORET);
                WC_CHECKRET(trbl.CFMID[i][j] < 4, WC_NORET);
                WC_CHECKRET(fo.insert(std::make_pair(trbl.CFID[i][j], trbl.CFMID[i][j])).second, WC_NORET); //checks the combination is always unique
            }
#endif
        m_trbl = trbl;

        /*
        char buffer[256];
        print_transpon(m_trbl.CID, buffer);
        NativePrint("OnTRBLChanged CID %s", buffer);

        print_transpon(m_trbl.CFID, buffer);
        NativePrint("OnTRBLChanged CFID %s", buffer);

        print_transpon(m_trbl.CFMID, buffer);
        NativePrint("OnTRBLChanged CFMID %s", buffer);
        */
    }

    void OnGyroChanged(const Get_Gyro_1_0& gyro) override
    {
        m_gyro = gyro;
        //NativePrint("OnGyroChanged X:%f Y:%f Z:%f", gyro.axis_X, gyro.axis_Y, gyro.axis_Z);
    }

    void OnAccelChanged(const Get_Accel_1_0& accel) override
    {
        m_accel = accel;
        //NativePrint("OnAccelChanged X:%f Y:%f Z:%f", accel.axis_X, accel.axis_Y, accel.axis_Z);
    }

    auto Circle(int cx, int cy, int r, int grad)
    {
        int x = int(cx + r * cos((90 + grad) * M_PI / 180) + 0.5);
        int y = int(cy + r * sin((90 + grad) * M_PI / 180) + 0.5);
        return std::make_tuple(x, y);
    }

    void CrossGeo(CDisplay& disp)
    {
        if (0xFF == m_cid || m_trbl.CID[0][0] == m_trbl.CID[1][1]) //not set yet
            return;
        if (m_cid == 0 && disp.Index() == 0)
        {
            int x, y;
            std::tie(x, y) = Circle(0, 0, 150, m_grad360);
            CScubo pos(m_trbl, x, y, 0, m_cid);
            //NativePrint("CID:%d, Disp:%d, X:%d, Y:%d", pos.cid(), pos.disp(), pos.x(), pos.y());
            NativeSendStruct(estAll, pos.pack());
            if (pos.cid() == m_cid)
                m_spCrossGeo = std::make_unique<CScubo>(m_trbl, pos.pack());
            else
                return;
        }
        if (!m_spCrossGeo || m_spCrossGeo->cid() != m_cid || m_spCrossGeo->disp() != disp.Index())
            return;
        //disp.FillCircle(m_spCrossGeo->x(), m_spCrossGeo->y(), 50, fColor(1, 0, 1));
        disp.DrawBitmap(m_spCrossGeo->x(), m_spCrossGeo->y(), m_small_ball, 1);
    }

    bool OnTick(uint32_t time) override
    {
        m_fps.Tick(time);
        int x, y;
        std::tie(x, y) = Circle(239, 239, 150, m_grad360);
        //CScuboSprite cake(m_trbl, x - m_cake.Width() / 2, y - m_cake.Height() / 2, m_cake.Width(), m_cake.Height());
        CScuboSprite disco(m_trbl, x - m_ball.Width() / 2, y - m_ball.Height() / 2, m_ball.Width(), m_ball.Height());
        //NativePrint("%d:%d:%d->%d:%d:%d", 90 + m_grad, x, y, scb.disp(), scb.x(), scb.y());
        std::tie(x, y) = Circle(x, y, 75, m_grad360);
        CScubo small_circle(m_trbl, x, y);
        m_grad270 += 5; m_grad270 %= 270;
        m_grad360 += 5; m_grad360 %= 360;

        for (int display = 0; display < 3; ++display)
        {
            CDisplay disp(display);
            disp.Fill(fColor(0.5,0.5,0.5));

            fpoint_t& pt = m_accel_ball[disp.Index()];
            disp.DrawBitmap(uint32_t(pt.x), uint32_t(pt.y), m_small_ball, 1);
            const fpoint_t& delta = AccelGyro(m_accel, disp.Index());
            pt.x = bound<float>(pt.x + delta.x * 50, 0, 239);
            pt.y = bound<float>(pt.y + delta.y * 50, 0, 239);

            static char buf[64] = {};
            //snprintf(buf, sizeof(buf), "D:%d", display);

            static const char* faces[] = { "L", "D", "F", "U", "R", "B" };
            const unsigned char face = m_trbl.CFID[m_cid][disp.Index()];

            snprintf(buf, sizeof(buf), "%d:%d %s%d", m_cid, disp.Index(), ((face < 6) ? faces[face] : "?"), m_trbl.CFMID[m_cid][disp.Index()]);

            disp.DrawText(0, 100, buf, fColor(0, 0, 0), 6, 0);

            //cake.Draw(m_cid, disp, m_cake, 1, 0/*m_grad360*/);
            disco.Draw(m_cid, disp, m_ball, 1, 0/*m_grad360*/);

            disp.DrawText(0, 0, m_fps.c_str(), fColor(1, 1, 1), 3); 

            //NativePrint("Draw for display %d, time: %d\n", display, time);
            CrossGeo(disp);
        }
        return CEventLoop::OnTick(time);
    }

    void Draw(CDisplay& disp, int x, int y, int w, int h, bool fill)
    {
        disp.FillRect(x, y, w, h, fill ? fColor(1,0,0) : fColor(0,0,1));
    }


    void OnMessage(uint32_t size, const Get_Message_1_0& msg) override
    {
        unique_type_id_t fn = reinterpret_cast<unique_type_id_t>(*(uintptr_t*)msg.data);
        uintptr_t fnid = unique_type_id<CScubo::pack_t>();
        WC_CHECKRET(*(uintptr_t*)msg.data == unique_type_id<CScubo::pack_t>(), WC_NORET);
        const CScubo::pack_t& pack = *reinterpret_cast<const CScubo::pack_t*>((uintptr_t*)msg.data + 1);
        m_spCrossGeo = std::make_unique<CScubo>(m_trbl, pack);
    }

    void OwnCID(uint8_t cid) override
    {
        m_cid = cid;
        NativePrint("MY CID IS %d", cid);
    }

public:
    int Main() override
    {
        NativePrint("Hello WOWd\n");
        resource_t res = get_resource(er_img_disco_ball);
        m_ball.Load(res.ptr, res.size * 2, epfRLE);

        res = get_resource(er_img_small_ball);
        m_small_ball.Load(res.ptr, res.size * 2, epfRLE);

        res = get_resource(er_img_cake);
        m_cake.Load(res.ptr, res.size * 2, epfRLE);


        return CEventLoop::Main();
    }
};

WASM_EXPORT int run() // native cube code searches for this function and runs as a main()
{
   //whatever you return here will just be recorded into logs
    return CEventLoopEx().Main();
}

#ifdef _DEBUG
class CTest
{
public:
    CTest() {
        CScubo::test();
    }
} test;
#endif
