#include "cube_api.h"
#include <cstdio>
#include <tuple>

#define _USE_MATH_DEFINES
#include <math.h>

typedef struct {
    const void* ptr;
    int size;
} resource_t; //FIXME: move to a common header

#include "resources.h"

template<uint16_t _size, class T>
class CCubeNet
{
public:
    static const uint16_t width = _size * 4;
    static const uint16_t height = _size * 3;
    static const uint16_t size = _size;

    T& At(int16_t x, int16_t y)
    {
        x %= width;
        y %= height;
        int16_t nx = 0;
        int16_t ny = 0;
        if ((size < y && y < 2*size) || (size < x && x < 2*size)) // direct mapping on the cross
        {
            nx = x;
            ny = y;
        } else if (x < size && y < size) {// leftmost upper quadrant
            nx = size*2 - y;
            ny = x;
        }
        return m_values[ny][nx];
    }

protected:
    T m_values[height][width];
};

const int g = 10;
const int k = 240 / g;
using TLife = char[k*k];
TLife life[3] = {};

template<int side>
class CScuboT {
    int m_ord = 0;
    int m_val[2] = {};
    int m_screens[3] = {0, 2, 1};

    bool Move(int delta, int val)
    {
        if (delta >= side) {
            m_val[val] = m_val[!val];
            m_val[!val] = side - (delta - side) - 1;
            if (val)
                m_ord = (m_ord -1 + 3) % 3;
            else
                ++m_ord %= 3;
            return true;
        }
        else {
            m_val[val] = delta;
            return false;
        }
    }

public:

    CScuboT(int dx, int dy, int ord = 0)
        : m_ord(ord)
    {
        if (Move(dx, 0))
            Move(dy, 0);
        else
            Move(dy, 1);
    }

    int x() { return m_val[0];}
    int y() { return m_val[1];}
    int disp() {return m_screens[m_ord];}

#define WC_TEST() static void test()
#define WC_TEST_CHECK(cond) {if (!(cond)) NativePrint("%s(%d): %s TEST FAIL:", __FILE__, __LINE__, __FUNCTION__, #cond);}
    WC_TEST() {
        {
            CScuboT<240> scb(241, 241);
            WC_TEST_CHECK(scb.disp() == 1);
            WC_TEST_CHECK(scb.x() == 239);
            WC_TEST_CHECK(scb.y() == 239);
        }
        {
            CScuboT<240> scb(0, 241);
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
public:
    CScuboSprite(int x, int y, int w, int h)
        : m_x(x), m_y(y), m_w(w), m_h(h)
    {
    }
    CScubo TopLeft() { return CScubo(       m_x,        m_y); }
    CScubo TopRight() { return CScubo(      m_x + m_w,  m_y); }
    CScubo BottomLeft() { return CScubo(    m_x,        m_y + m_h); }
    CScubo BottomRight() { return CScubo(   m_x + m_w,  m_y + m_h); }
    CScubo Center() { return CScubo(m_x + m_w/2, m_y + m_h/2); }

};

class CEventLoopEx: public CEventLoop
{
    int m_nPos = 0;
    uint32_t m_nPrevTime = 0;
    int m_grad = 0;
    Get_TRBL_1_0 m_trbl = {};
    CBitmap m_ball;


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
        m_trbl = trbl;
        char buffer[256];
        print_transpon(m_trbl.CID, buffer);
        NativePrint("OnTRBLChanged CID %s", buffer);

        print_transpon(m_trbl.CFID, buffer);
        NativePrint("OnTRBLChanged CFID %s", buffer);

        print_transpon(m_trbl.CFMID, buffer);
        NativePrint("OnTRBLChanged CFMID %s", buffer);

    }

    void OnGyroChanged(const Get_Gyro_1_0& gyro) override
    {
        m_gyro = gyro;
        NativePrint("OnGyroChanged X:%f Y:%f Z:%f", gyro.axis_X, gyro.axis_Y, gyro.axis_Z);
    }

    void OnAccelChanged(const Get_Accel_1_0& accel) override
    {
        m_accel = accel;
        NativePrint("OnAccelChanged X:%f Y:%f Z:%f", accel.axis_X, accel.axis_Y, accel.axis_Z);
    }

    auto Circle(int cx, int cy, int r, int grad)
    {
        int x = int(cx + r * cos((90 + grad) * M_PI / 180) + 0.5);
        int y = int(cy + r * sin((90 + grad) * M_PI / 180) + 0.5);
        return std::make_tuple(x, y);
    }

    bool OnTick(uint32_t time) override
    {
        int x, y;
        std::tie(x, y) = Circle(240, 240, 150, m_grad);
        CScuboSprite ball(x - 50, y - 50, 100, 100);
        //NativePrint("%d:%d:%d->%d:%d:%d", 90 + m_grad, x, y, scb.disp(), scb.x(), scb.y());
        std::tie(x, y) = Circle(x, y, 75, m_grad);
        CScubo small_circle(x, y);
        ++m_grad %= 270;

        for (int display = 0; display < 3; ++display)
        {
            CDisplay disp(display);
            disp.Fill(fColor(1,1,1));
#if 1

            UpdateLife(disp, display);

            if (display == 1)
            {
                ++m_nPos %= 240;
                disp.DrawText(120, 120, "Life!", fColor(1,1,1), 30, m_nPos % 360);
                static char buf[64] = {};
                snprintf(buf, sizeof(buf), "X:%f Y:%f Z:%f", m_accel.axis_X, m_accel.axis_Y, m_accel.axis_Z);
                disp.DrawText(5, 100, buf, fColor(0,1,0), 3, 0);
            }

            if (display == 2)
            {
                disp.DrawLine(0,0,240,240, 100);
                disp.FillRect(m_nPos, m_nPos, 240, 240, fColor(0,1,0));
                disp.DrawPixelAlpha(66, 66, 255, 2);
            }
#endif
            static char buf[64] = {};
            snprintf(buf, sizeof(buf), "D:%d", display);
            disp.DrawText(0, 100, buf, fColor(0, 0, 0), 10, 0);

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

            if (m_nPrevTime)
            {
                uint32_t diff = time - m_nPrevTime;
                static char buff[32] = {};
                snprintf(buff, sizeof(buff), "fps: %f", 1000. / diff);
                disp.DrawText(0, 0, buff, fColor(1,1,1), 3);
            }
            m_nPrevTime = time;

            //NativePrint("Draw for display %d, time: %d\n", display, time);
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

    void UpdateLife(CDisplay& disp, int ind) {
        TLife& b = life[ind];
        char c[k*k] = {};
        for (int i = k * k; i--;) {
            int n = 0;
            int x = i % k;
            int y = i / k;
            if (x > 0 && x < k && y > 0 && y < k)
                for (int f = 9; f--;) //going around
                    n += b[(i / k + k + f % 3 - 1) % k * k + (i + k + f / 3 - 1) % k];
            else
            {
                for (int f = 9; f--;)
                {
                    int dx = f % 3 - 1;
                    int dy = f / 3 - 1;
                    CScuboT<k> scb(x + dx, y + dy, ind);
                    n += life[scb.disp()][scb.y() * k + scb.x()];
                }
            }

            c[i] = (n == 3) || (n - b[i] == 3);
            Draw(disp, x * g, y * g, g - 1, g - 1, c[i]);
        }

        if (0 != memcmp(b, c, sizeof(b)))
            memcpy(b, c, sizeof(b));
        else
            Randomize();
    }

    void OnMessage(uint32_t size, const Get_Message_1_0& msg) override
    {
    }

    void OwnCID(uint8_t cid) override
    {
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
