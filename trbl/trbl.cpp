#include "cube_api.h"
#include <cstdio>
#include <string>

struct Vec3f {
    float x;
    float y;
    float z;
} accel, gyro;

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
            
            static char buf[64] = {};
            snprintf(buf, sizeof(buf), "DISPLAY: %d", display);
            disp.DrawText(0, 40, buf, fColor(0, 0, 0), 2, 0);

            snprintf(buf, sizeof(buf), "MODULE: %d", m_cid);
            disp.DrawText(0, 60, buf, fColor(0, 0, 0), 2, 0);

            snprintf(buf, sizeof(buf), "GYRO %.2f:%.2f:%.2f", gyro.x, gyro.y, gyro.z);
            disp.DrawText(0, 140, buf, fColor(0, 0, 0), 2, 0);

            snprintf(buf, sizeof(buf), "ACCL %.2f:%.2f:%.2f", accel.x, accel.y, accel.z);
            disp.DrawText(0, 160, buf, fColor(0, 0, 0), 2, 0);

            auto print_it = [](const char* title, auto& arr) {
                std::string res = title;
                for (uint8_t cid : arr)
                    res += std::to_string(cid) + " ";
                return res;
            };

            disp.DrawText(0, 180, print_it("CID:", m_trbl.CID[m_cid]).c_str(), fColor(0, 0, 0), 2, 0);
            disp.DrawText(0, 200, print_it("CFID:", m_trbl.CFID[m_cid]).c_str(), fColor(0, 0, 0), 2, 0);
            disp.DrawText(0, 220, print_it("FMID:", m_trbl.CFMID[m_cid]).c_str(), fColor(0, 0, 0), 2, 0);

            disp.DrawLine(120, 0, 120, 120, fColor(1, 0, 0));
            disp.DrawLine(0, 120, 120, 120, fColor(0, 1, 0));
        }
        return CEventLoop::OnTick(time);
    }

    
    void OnGyroChanged(const Get_Gyro_1_0& _gyro) override {
        //NativePrint("GYRO CHANGED");
        gyro.x = _gyro.axis_X;
        gyro.y = _gyro.axis_Y;
        gyro.z = _gyro.axis_Z;
        //NativePrint("GYRO: X:%f, Y:%f, Z:%f", gyro.x, gyro.y, gyro.z);
    }
    void OnAccelChanged(const Get_Accel_1_0& _accel) override {
        //NativePrint("ACCEL CHANGED");
        accel.x = _accel.axis_X;
        accel.y = _accel.axis_Y;
        accel.z = _accel.axis_Z;
        //NativePrint("ACCEL: X:%f, Y:%f, Z:%f", accel.x, accel.y, accel.z);
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
        /*
        NativePrint("on trbl changed \n");
        for (int i = 0; i < 8; i++) {
            NativePrint("CID %d %d %d", trbl.CID[i][0], trbl.CID[i][1], trbl.CID[i][2]);
            NativePrint("CFID %d %d %d", trbl.CFID[i][0], trbl.CFID[i][1], trbl.CFID[i][2]);
            NativePrint("CFMID %d %d %d", trbl.CFMID[i][0], trbl.CFMID[i][1], trbl.CFMID[i][2]);
        }
        */
        m_trbl = trbl;
        auto pkt = GetSharableMem(20);// CMD_GEO{8},n_records{8},CID[0]{8},screen[0]{8},...,CID[N]{8},screen[N]{8}

        Get_Legacy_1_0 glTRBL= {Get_Legacy_1_0::eglTRBL, pkt.get(), 20};
        NativeInvoke(glTRBL);
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
