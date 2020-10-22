#include "cube_api.h"
#include <cstdio>
#include <cstdlib>
#define WASM_EXPORT __attribute__((used)) __attribute__((visibility ("default")))


const int g = 10;
const int k = 240 / g;
char b[k*k] = {};


class CEventLoopEx: public CEventLoop
{
    int m_nPos = 0;
protected:
    virtual bool OnTick(uint32_t time)
    {
        for (int display = 0; display < 1; ++display)
        {
            CDisplay disp(display);
            disp.Fill(fColor(1,1,1));
            Update(disp);
            /*
            disp.DrawLine(0,0,240,240, 100);
            disp.FillRect(m_nPos, m_nPos, 240, 240, fColor(0,1,0));
            disp.DrawPixelAlpha(66, 66, 255, 2);
            disp.FillCircle(120,120, 30, 100, 2);
            disp.Flush();
            */
            ++m_nPos %= 240;
            disp.DrawText(120, 120, "Life!", fColor(1,1,1), 240, m_nPos % 360);
            NativePrint("Draw for display %d, time: %d\n", display, time);
        }
        return CEventLoop::OnTick(time);
    }

    void Draw(CDisplay& disp, int x, int y, int w, int h, bool fill)
    {
        disp.FillRect(x, y, w, h, fill ? fColor(1,0,0) : fColor(0,0,1));
    }

    void Randomize()
    {
        for (int y = k * k; y--;)
            b[y] = rand() % 2;
    }

    void Update(CDisplay& disp) {
        char c[k*k] = {};
        for (int y = k * k; y--;) {
            int n = 0;
            for (int f = 9; f--;) //going around 
                n += b[(y / k + k + f % 3 - 1) % k * k + (y + k + f / 3 - 1) % k];
            c[y] = (n == 3) || (n - b[y] == 3);
            Draw(disp, y % k * g, (y / k) * g, g - 1, g - 1, c[y]);
        }

        if (0 != memcmp(b, c, sizeof(b)))
            memcpy(b, c, sizeof(b));
        else
            Randomize();
    }

    virtual void OnMessage(const Get_Message_1_0& msg)
    {
        NativePrint("Msg from %d: %s", msg.from_cid, (const char*)msg.data);
        char answer[] = "I am fine too!";
        NativeInvoke(Send_Message_1_0{msg.from_cid, answer, sizeof(answer)});
    }

public:
    virtual int Main()
    {
        NativePrint("Hello WOWd\n");
        Randomize();
        return CEventLoop::Main();
    }
};

extern "C" int WASM_EXPORT run() // native cube code searches for this function and runs as a main()
{
    //whatever you return here will just be recorded into logs
    return CEventLoopEx().Main();
}
