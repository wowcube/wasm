#include "cube_api.h"
#include <cstdio>
#include <tuple>
#include <set>

#define _USE_MATH_DEFINES
#include <math.h>

const int g = 10;
const int k = 240 / g;
using TLife = char[k*k];
TLife life[8][3] = {};


class CEventLoopEx: public CEventLoop
{

    uint32_t m_nPrevTime = 0;
    Get_TRBL_1_0 m_trbl = {};
    CCubeGraph m_cg;
    uint8_t m_cid = 0xFF;
    struct sync_t {
        unsigned char finished;
    } m_sync;

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
        if (m_cid == 0)
            m_cg.Init(m_trbl);
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


    bool OnTick(uint32_t time) override
    {
        if (0xFF == m_cid || m_trbl.CID[0][0] == m_trbl.CID[1][1] || m_sync.finished != 0xFF) //not set yet
            return true;
        m_sync.finished = 0;

        UpdateLife();

        for (int display = 0; display < 3; ++display)
        {
            CDisplay disp(display);
            disp.Fill(fColor(1,1,1));

            DrawLife(disp);

            static char buf[64] = {};
            //snprintf(buf, sizeof(buf), "D:%d", display);

            static const char* faces[] = { "L", "D", "F", "U", "R", "B" };
            const unsigned char face = m_trbl.CFID[m_cid][disp.Index()];

            snprintf(buf, sizeof(buf), "%d:%d %s%d", m_cid, disp.Index(), ((face < 6) ? faces[face] : "?"), m_trbl.CFMID[m_cid][disp.Index()]);

            disp.DrawText(0, 100, buf, fColor(0, 0, 0), 6, 0);

            static char fps[32] = {};
            if (m_nPrevTime && display == 0)
            {
                uint32_t diff = time - m_nPrevTime;
                snprintf(fps, sizeof(fps), "fps: %.2f", 1000. / diff);
            }
            disp.DrawText(0, 0, fps, fColor(1, 1, 1), 3);
            m_nPrevTime = time;

        }
        m_sync.finished |= 1 << m_cid;
        NativeSendStruct(estAll, m_sync);
        return CEventLoop::OnTick(time);
    }

    void Draw(CDisplay& disp, int x, int y, int w, int h, bool fill)
    {
        disp.FillRect(x, y, w, h, fill ? fColor(1,0,0) : fColor(0,0,1));
    }

    void Randomize()
    {
        for (int cid = 0; cid < 8; ++cid)
            for (int disp = 0; disp < 3; ++disp)
                for (int y = k * k; y--;)
                    life[cid][disp][y] = rand() % 2;

        uint32_t len = sizeof(life) + sizeof(intptr_t);
        auto buff = GetSharableMem(len);
        intptr_t* ptr = reinterpret_cast<intptr_t*>(buff.get());
        *ptr = unique_type_id<TLife>();
        memcpy(ptr + 1, life, sizeof(life));
        NativeInvoke(Send_Message_1_0{ estAll, len, buff.get() });
        NativePrint("Sent random");
        m_sync.finished = 0xFF;
    }

    void DrawLife(CDisplay& disp) {
        TLife& old = life[m_cid][disp.Index()];

        for (int i = k * k; i--;) {
            int x = i % k;
            int y = i / k;
            Draw(disp, x * g, y * g, g - 1, g - 1, old[i]);
        }
    }

    void UpdateLife() {
        //static uint16_t colors[] = { fColor(1, 0, 0), fColor(0, 0, 1), fColor(0, 1, 0), fColor(0, 0, 0) };
        for (int cid = 0; cid < 8; ++cid)
            for (int disp = 0; disp < 3; ++disp)
            {
                TLife& old = life[cid][disp];
                char c[k * k] = {};
                for (int i = k * k; i--;) {
                    int n = 0;
                    int x = i % k;
                    int y = i / k;

                    for (int f = 9; f--;)
                    {
                        int dx = f % 3 - 1;
                        int dy = f / 3 - 1;
                        int nx = x + dx;
                        int ny = y + dy;

                        CScuboT<k> scb(m_trbl, nx, ny, disp, cid);
                        n += life[scb.cid()][scb.disp()][scb.y() * k + scb.x()];
                    }

                    c[i] = (n == 3) || (n - old[i] == 3);
                }
                if (0 != memcmp(old, c, sizeof(old)))
                    memcpy(old, c, sizeof(old));
                else if (m_cid == 0)
                    Randomize();
            }
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
        else if (*(uintptr_t*)msg.data == unique_type_id<TLife>())
        {
            memcpy(life, (uintptr_t*)msg.data + 1, sizeof(life));
            m_sync.finished = 0xFF;
            NativePrint("Got Random: %d", m_cid);
        }
        else
        {
            WC_CHECKRET(!"Unexpected", WC_NORET);
        }
    }

    void OwnCID(uint8_t cid) override
    {
        m_cid = cid;
        NativePrint("MY CID IS %d", cid);
        if (m_cid == 0)
            Randomize();
    }

public:
    int Main() override
    {
        NativePrint("Hello WOWd\n");
        NativeInvoke( Send_Message_1_0{ estSelf, 0, NULL} );

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
