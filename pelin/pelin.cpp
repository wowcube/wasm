#include "cube_api.h"
#include <cstdio>
#include <string>
#include <cmath>
#include "PerlinNoise.h"


class CEventLoopEx: public CEventLoop
{
    uint8_t m_cid = 0;
    Get_TRBL_1_0 m_trbl;

protected:

    bool OnTick(uint32_t time) override
    {
        for (int display = 0; display < 3; ++display) {
            CDisplay disp(display);
            disp.Fill(fColor(1,1,1));
            int height = 240, width = 240;


            // Create a PerlinNoise object with a random permutation vector generated with seed
            unsigned int seed = 237;
            PerlinNoise pn(seed);

            // Visit every pixel of the image and assign a color generated with Perlin noise
            for (unsigned int i = 0; i < height; ++i) {     // y
                for (unsigned int j = 0; j < width; ++j) {  // x
                    double x = (double)j / ((double)width);
                    double y = (double)i / ((double)height);

                    // Typical Perlin noise
                    double n = ((time / 2000) % 10 + 10) * pn.noise(x, y, 0.8);
                    n = n - floor(n);


                    // Map the values to the [0, 255] interval, for simplicity we use 
                    // tones of grey
                    disp.DrawPixelAlpha(i, j, fColor(n, n, n), 255);

                }
            }

        }
        return CEventLoop::OnTick(time);
    }

    
    void OnGyroChanged(const Get_Gyro_1_0& _gyro) override {
    }
    void OnAccelChanged(const Get_Accel_1_0& _accel) override {

    }

    void OwnCID(uint8_t cid) override
    {
        NativePrint("MY CID IS %d", cid);
        m_cid = cid;
    }

    void OnMessage(uint32_t size, const Get_Message_1_0& msg) override{
        NativePrint("WASM OnMessage from: %d, size: %d, message: %s", msg.from_cid, msg.size, (char*)msg.data);
    };
   
    void OnTRBLChanged(const Get_TRBL_1_0& trbl) override {

    }

public:
    int Main() override
    {
        NativeInvoke( Send_Message_1_0{ estSelf, 0, NULL} );
        return CEventLoop::Main();
    }
};

WASM_EXPORT int run() // native cube code searches for this function and runs as a main()
{
    //whatever you return here will just be recorded into logs
    return CEventLoopEx().Main();
}
