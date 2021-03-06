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
    int m_nPos = 0;
    uint32_t m_nPrevTime = 0;
    int m_grad270 = 0;
    int m_grad360 = 0;
    Get_TRBL_1_0 m_trbl = {};
    CBitmap m_ball, m_small_ball;
    uint8_t m_cid = 0xFF;
    std::unique_ptr<CScubo> m_spCrossGeo;


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
            NativePrint("CID:%d, Disp:%d, X:%d, Y:%d", pos.cid(), pos.disp(), pos.x(), pos.y());
            if (pos.cid() == m_cid)
                m_spCrossGeo = std::make_unique<CScubo>(m_trbl, pos.pack());
            else {
                NativeSendStruct(pos.cid(), pos.pack());
                return;
            }
        }
        if (!m_spCrossGeo || m_spCrossGeo->cid() != m_cid || m_spCrossGeo->disp() != disp.Index())
            return;
        //disp.FillCircle(m_spCrossGeo->x(), m_spCrossGeo->y(), 50, fColor(1, 0, 1));
        disp.DrawBitmap(m_spCrossGeo->x(), m_spCrossGeo->y(), m_small_ball, 1);
    }

    bool OnTick(uint32_t time) override
    {
        int x, y;
        std::tie(x, y) = Circle(240, 240, 150, m_grad270);
        CScuboSprite ball(m_trbl, x - 50, y - 50, 100, 100);
        //NativePrint("%d:%d:%d->%d:%d:%d", 90 + m_grad, x, y, scb.disp(), scb.x(), scb.y());
        std::tie(x, y) = Circle(x, y, 75, m_grad360);
        CScubo small_circle(m_trbl, x, y);
        m_grad270 += 5; m_grad270 %= 270;
        m_grad360 += 5; m_grad360 %= 360;

        for (int display = 0; display < 3; ++display)
        {
            CDisplay disp(display);
            disp.Fill(fColor(1,1,1));
#if 1

            if (display == 1)
            {
                static char buf[64] = {};
                snprintf(buf, sizeof(buf), "accel: %.2f %.2f %.2f", m_accel.axis_X, m_accel.axis_Y, m_accel.axis_Z);
                disp.DrawText(0, 20, buf, fColor(0,1,0), 2, 0);
            }

            if (display == 2)
            {
                ++m_nPos %= 240;
                disp.DrawLine(0,0,240,240, 100);
                disp.FillRect(m_nPos, m_nPos, 240, 10, fColor(0,1,0));
                disp.DrawPixelAlpha(66, 66, 255, 2);
            }

#endif
            static char buf[64] = {};
            //snprintf(buf, sizeof(buf), "D:%d", display);

            static const char* faces[] = { "L", "D", "F", "U", "R", "B" };
            const unsigned char face = m_trbl.CFID[m_cid][disp.Index()];

            snprintf(buf, sizeof(buf), "%d:%d %s%d", m_cid, disp.Index(), ((face < 6) ? faces[face] : "?"), m_trbl.CFMID[m_cid][disp.Index()]);

            disp.DrawText(0, 100, buf, fColor(0, 0, 0), 6, 0);

#if 0
            CScubo tl = ball.TopLeft();
            CScubo br = ball.BottomRight();
            if (tl.disp() == display)
                disp.DrawBitmap(tl.x(), tl.y(), m_ball, 1, 360 - m_grad);
            else if (br.disp() == display)
                disp.FillCircle(br.x(), br.y(), 10, fColor(1, 0, 1));
                //disp.DrawBitmap(br.x() - 100, br.y() - 100, m_ball, 1, 360 - m_grad);
#else
            CScubo big_circle = ball.Center();
            if (big_circle.disp() == display)
                disp.DrawBitmap(big_circle.x(), big_circle.y(), m_ball, 1, 360 - m_grad270);
            if (small_circle.disp() == display)
                disp.FillCircle(small_circle.x(), small_circle.y(), 20, fColor(1, 0, 1));
#endif

            static char fps[32] = {};
            if (m_nPrevTime && display == 0)
            {
                uint32_t diff = time - m_nPrevTime;
                snprintf(fps, sizeof(fps), "fps: %.2f", 1000. / diff);
            }
            disp.DrawText(0, 0, fps, fColor(1, 1, 1), 3);
            m_nPrevTime = time;

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
        NativeInvoke( Send_Message_1_0{ estSelf, 0, NULL} );
        resource_t res_ball = get_resource(0);
        m_ball.Load(res_ball.ptr, res_ball.size * 2, epfRLE);
        res_ball = get_resource(1);
        m_small_ball.Load(res_ball.ptr, res_ball.size * 2, epfRLE);

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
