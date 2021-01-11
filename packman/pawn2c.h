#define cell signed
typedef int stock;


// from AMX, {N} - N is number of bits
#define CMD_GUI_BASE           0 /* TO DISPLAY */
#define CMD_REDRAW             CMD_GUI_BASE+1 /* CMD_REDRAW{8},faceN{8} - copy framebuffer contents to the face specified */
#define CMD_FILL               CMD_GUI_BASE+2 /* CMD_FILL{8},R{8},G{8},B{8} - to framebuffer, RGB565 */
#define CMD_BITMAP             CMD_GUI_BASE+3 /* CMD_BITMAP{8},resID{16},X{16},Y{16},angle{16},mirror{8} - to framebuffer */
#define CMD_LINE               CMD_GUI_BASE+4 /* CMD_LINE{8},x1{16},y1{16},x2{16},y2{16},R{8},G{8},B{8} - to framebuffer, RGB565 */
#define CMD_RECT               CMD_GUI_BASE+5 /* CMD_RECT{8},x1{16},y1{16},x2{16},y2{16},R{8},G{8},B{8} - to framebuffer, RGB565 */
#define CMD_SLEEP              CMD_GUI_BASE+6 /* power off */
#define CMD_DRAW_OVERLAY       CMD_GUI_BASE+7 /* show/off overlay */
#define CMD_TRIGGER_BLUETOOTH  CMD_GUI_BASE+8 /* enable/disable bluetooth */
#define CMD_G2D_BEGIN_BITMAP   CMD_GUI_BASE+9 /* start to add G2D layers to blend */
#define CMD_G2D_ADD_SPRITE     CMD_GUI_BASE+10 /* add bitmap layer */
#define CMD_G2D_ADD_RECTANGLE  CMD_GUI_BASE+11 /* add rectangle layer */
#define CMD_G2D_END            CMD_GUI_BASE+12 /* stop adding G2D layers and start blendind process */
#define CMD_TEXT               CMD_GUI_BASE+13 /* CMD_TEXT{8},fontResID{16},x{16},y{16},scale{16},angle{16},r{8},g{8},b{8},text{8...} - to framebuffer, RGB565 */
#define CMD_G2D_BEGIN_DISPLAY  CMD_GUI_BASE+14 /* start to add G2D layers to blend them into display framebuffer */
#define CMD_PLAYSND            CMD_GUI_BASE+15 /* play sound {sound_id} */
#define CMD_TRIGGER_NIGHTLAMP  CMD_GUI_BASE+16 /* switch to night lamp mode */ 
#define CMD_G2D_DYNAMIC_TEXTURE CMD_GUI_BASE+17 /* draw dynamic texture to pawn buffer or g2d */ 
#define CMD_STATE_SAVE         CMD_GUI_BASE+18 /* store game save */
#define CMD_STATE_SYNC         CMD_GUI_BASE+19 /* Sync game save from master module to other modules */
#define CMD_EXIT               CMD_GUI_BASE+20 

#define CMD_NET_BASE        20 /* TO UARTs 0-2 */
#define CMD_NET_TX          CMD_NET_BASE+1 /* CMD_NET_TX{8},line_tx{8},TTL{8},RESERVED{8},<4 CELLs of arbitrary data here>{128} */
#define CMD_SCHEDULER_BASE  30 /* TO SCRIPTS SCHEDULER */
#define CMD_CHANGE_SCRIPT    CMD_SCHEDULER_BASE+1 /* CMD_CHANGE_SCRIPT{8},scriptID{8} - change script before next tick */
// to AMX
#define CMD_PAWN_BASE   100
#define CMD_TICK        CMD_PAWN_BASE+1 /* CMD_TICK{8} */
#define CMD_GEO         CMD_PAWN_BASE+2 /* CMD_GEO{8},n_records{8},CID[0]{8},screen[0]{8},...,CID[N]{8},screen[N]{8} */
#define CMD_NET_RX      CMD_PAWN_BASE+3 /* CMD_NET_RX{8},line_rx{8},neighbor_line_tx{8},TTL{8},<4 CELLs of arbitrary data here>{128} */
#define CMD_MTD         CMD_PAWN_BASE+4 /* CMD_MTD{8},
                                           accel_X_pos{8},accel_X_neg{8},accel_Y_pos{8},accel_Y_neg{8},accel_Z_pos{8},accel_Z_neg{8},
                                           gyro_X_pos{8},gyro_X_neg{8},gyro_Y_pos{8},gyro_Y_neg{8},gyro_Z_pos{8},gyro_Z_neg{8} */
#define CMD_STATE_LOAD  CMD_PAWN_BASE+5

#ifdef __cplusplus
    #define WC_EXTERN_C extern "C"
#else
    #define WC_EXTERN_C
#endif


WC_EXTERN_C int sendpacket(int* packet, int size);
cell ABS(cell);
cell GetSign(cell);