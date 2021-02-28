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


const int g = 10;
const int k = 240 / g;
using TLife = char[k*k];
TLife life[3] = {};

template<int side> //width&height of the screen
class CScuboT {
    uint8_t m_ord = 0;
    int m_val[2] = {};
    uint8_t m_screens[3] = {0, 2, 1};
    uint8_t m_cid = 0;
    const Get_TRBL_1_0& m_trbl;

    template<class T>
    void moddec(T& var, int mod) //decrement by module
    {
        var = (var - 1 + mod) % mod;
    }

    auto Face2CID(uint8_t face, uint8_t fsi) //returns -> cid, disp
    {
        for (int cid = 0; cid < 8; ++cid)
            for (int disp = 0; disp < 4; ++disp)
                if (m_trbl.CFID[cid][disp] == face && m_trbl.CFMID[cid][disp] == fsi)
                    return std::make_pair(cid, disp);
        return std::make_pair(-1, -1);
    }

    bool Move(int delta, int val) //val is index of axis
    {
        if (delta >= side && delta < 2* side) { //jumping to the other screen of the same module, where Y and X exchange
            m_val[val] = m_val[!val]; //flipping X,Y
            m_val[!val] = side - (delta - side) - 1; //X end of one screen touches Y end of other and vise versa
            if (val) //jumping to "previous" screen when 1(Y) is given as movement axis
                moddec(m_ord, 3);
            else //jumping to next screen, by their order
                ++m_ord %= 3; //advance by module
            return true;
        }
        else if (delta < 0) //alas, we go to other cube module
        {
            WC_CHECKRET(delta > -side, false);
            uint8_t  face = m_trbl.CFID[m_cid][m_screens[m_ord]];
            uint8_t  fsi = m_trbl.CFMID[m_cid][m_screens[m_ord]];
            if (val) //jumping to "previous" screen when 1(Y) is given as movement axis
                moddec(fsi, 4); //minus by module
            else //jumping to next screen, by their order
                ++fsi %= 4; //advance by module
            int disp = -1;
            int new_cid = -1;
            std::tie(new_cid, disp) = Face2CID(face, fsi);
            //assert(m_cid != new_cid);
            m_cid = new_cid;
            m_ord = disp2ord(disp);
            m_val[val] = m_val[!val]; //flipping X,Y
            m_val[!val] = -delta; //X end of one screen touches Y end of other and vise versa
            return true;
        }
        else if (delta >= 2 * side) //alas, we go to other cube module
        {
            WC_CHECKRET(delta < 3*side, false);
            return true;
        }
        else //everything happens inside current screen, no jumps required
        {
            m_val[val] = delta;
            return false;
        }
    }

    int disp2ord(int disp)
    {
        return (3 - disp) % 3;
    }

public:

    CScuboT(const Get_TRBL_1_0& trbl, int dx, int dy, int disp = 0, int cid=0)
        : m_ord(disp2ord(disp)) //display to ord
        , m_cid(cid)
        , m_trbl(trbl)
    {
        if (Move(dx, 0))
            Move(dy, 0);
        else
            Move(dy, 1);
    }

    struct pack_t //4 bytes
    {
        unsigned char cid;
        unsigned char disp;
        unsigned char x, y;
    };

    CScuboT(const Get_TRBL_1_0& trbl, const pack_t& pack)
        : m_ord(disp2ord(pack.disp)) //display to ord
        , m_cid(pack.cid)
        , m_trbl(trbl)
    {
        m_val[0] = pack.x;
        m_val[1] = pack.y;
    }
    int x() { return m_val[0];}
    int y() { return m_val[1];}
    int disp() {return m_screens[m_ord];}
    int cid() {return m_cid;}

    pack_t pack()
    {
        static_assert(k < 256);
        assert(m_val[0] >= 0 && m_val[1] >= 0);
        return pack_t{m_cid, m_screens[m_ord], (uint8_t)m_val[0], (uint8_t)m_val[1]};
    }

#define WC_TEST() static void test()
#define WC_TEST_CHECK(cond) {if (!(cond)) {NativePrint("%s(%d): %s TEST FAIL:", __FILE__, __LINE__, __FUNCTION__, #cond); assert(!#cond);}}
    WC_TEST() {
        Get_TRBL_1_0 trbl;
        {
            CScuboT<240> scb(trbl, 240, 240);
            WC_TEST_CHECK(scb.disp() == 1);
            WC_TEST_CHECK(scb.x() == 239);
            WC_TEST_CHECK(scb.y() == 239);
        }
        {
            CScuboT<240> scb(trbl, 0, 240);
            WC_TEST_CHECK(scb.disp() == 1);
            WC_TEST_CHECK(scb.x() == 239);
            WC_TEST_CHECK(scb.y() == 0);
        }
    }
};

using CScubo = CScuboT<240>;

class CScuboSprite
{
    int m_x, m_y, m_w, m_h;
    const Get_TRBL_1_0& m_trbl;
public:
    CScuboSprite(const Get_TRBL_1_0& trbl, int x, int y, int w, int h)
        : m_x(x), m_y(y), m_w(w), m_h(h)
        , m_trbl(trbl)
    {
    }
    CScubo TopLeft() { return CScubo(m_trbl,      m_x,        m_y); }
    CScubo TopRight() { return CScubo(m_trbl,     m_x + m_w,  m_y); }
    CScubo BottomLeft() { return CScubo(m_trbl,   m_x,        m_y + m_h); }
    CScubo BottomRight() { return CScubo(m_trbl,  m_x + m_w,  m_y + m_h); }
    CScubo Center() { return CScubo(m_trbl,       m_x + m_w/2, m_y + m_h/2); }

};

class CEventLoopEx: public CEventLoop
{
    int m_nPos = 0;
    uint32_t m_nPrevTime = 0;
    int m_grad = 0;
    Get_TRBL_1_0 m_trbl = {};
    CBitmap m_ball;
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
        if (m_cid == 0 && disp.Index() == 0)
        {
            int x, y;
            std::tie(x, y) = Circle(0, 0, 150, m_grad);
            CScubo pos(m_trbl, x, y, 0, m_cid);
            if (pos.cid() == m_cid)
                m_spCrossGeo = std::make_unique<CScubo>(m_trbl, pos.pack());
            else {
                NativeSendStruct(estAll, pos.pack());
                return;
            }
        }
        if (!m_spCrossGeo || m_spCrossGeo->cid() != m_cid || m_spCrossGeo->disp() != disp.Index())
            return;
        disp.FillCircle(m_spCrossGeo->x(), m_spCrossGeo->y(), 50, fColor(1, 1, 1));
    }

    bool OnTick(uint32_t time) override
    {
        if (0xFF == m_cid || m_trbl.CID[0][0] == m_trbl.CID[1][1]) //not set yet
            return true;
        int x, y;
        std::tie(x, y) = Circle(240, 240, 150, m_grad);
        CScuboSprite ball(m_trbl, x - 50, y - 50, 100, 100);
        //NativePrint("%d:%d:%d->%d:%d:%d", 90 + m_grad, x, y, scb.disp(), scb.x(), scb.y());
        std::tie(x, y) = Circle(x, y, 75, m_grad);
        CScubo small_circle(m_trbl, x, y);
        ++m_grad %= 270;

        for (int display = 0; display < 3; ++display)
        {
            CDisplay disp(display);
            disp.Fill(fColor(1,1,1));
#if 1

            UpdateLife(disp);

            if (display == 1)
            {
                ++m_nPos %= 240;
                disp.DrawText(120, 120, "Life!", fColor(1,1,1), 30, m_nPos % 360);
                static char buf[64] = {};
                snprintf(buf, sizeof(buf), "accel: %.2f %.2f %.2f", m_accel.axis_X, m_accel.axis_Y, m_accel.axis_Z);
                disp.DrawText(0, 20, buf, fColor(0,1,0), 2, 0);
            }

            if (display == 2)
            {
                disp.DrawLine(0,0,240,240, 100);
                disp.FillRect(m_nPos, m_nPos, 240, 10, fColor(0,1,0));
                disp.DrawPixelAlpha(66, 66, 255, 2);
            }

#endif
            static char buf[64] = {};
            //snprintf(buf, sizeof(buf), "D:%d", display);

            static const char* faces[] = { "L", "D", "F", "U", "R", "B" };
            const unsigned char face = m_trbl.CFID[m_cid][disp.Index()];

            snprintf(buf, sizeof(buf), "%d:%d %s%d", m_cid, disp.Index(), (face < 6) ? faces[face] : "?", m_trbl.CFMID[m_cid][disp.Index()]);

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
                disp.DrawBitmap(big_circle.x(), big_circle.y(), m_ball, 1, 360 - m_grad);
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

    void Randomize()
    {
        for (int i = 0; i < 3; ++i)
            for (int y = k * k; y--;)
                life[i][y] = rand() % 2;
    }

    void UpdateLife(CDisplay& disp) {
        static uint16_t colors[] = { fColor(1, 0, 0), fColor(0, 0, 1), fColor(0, 1, 0), fColor(0, 0, 0) };
        TLife& old = life[disp.Index()];
        char c[k*k] = {};
        for (int i = k * k; i--;) {
            int n = 0;
            int x = i % k;
            int y = i / k;
            //n += b[(i / k + k + f % 3 - 1) % k * k + (i + k + f / 3 - 1) % k];
            Draw(disp, x * g, y * g, g - 1, g - 1, old[i]);

            for (int f = 9; f--;)
            {
                int dx = f % 3 - 1;
                int dy = f / 3 - 1;
                int nx = x + dx;
                int ny = y + dy;

                if (nx < 0 || ny < 0)
                {
                    //other module
                }
                else
                {
                    CScuboT<k> scb(m_trbl, nx, ny, disp.Index());
                    n += life[scb.disp()][scb.y() * k + scb.x()];
                }
            }

            c[i] = (n == 3) || (n - old[i] == 3);
        }

        if (0 != memcmp(old, c, sizeof(old)))
            memcpy(old, c, sizeof(old));
        else
            Randomize();
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
        Randomize();
        NativeInvoke( Send_Message_1_0{ estSelf, 0, NULL} );
        resource_t res_ball = get_resource(0);
        m_ball.Load(res_ball.ptr, res_ball.size * 2, epfRLE);

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
