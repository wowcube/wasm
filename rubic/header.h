typedef int cell;
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

cell abi_GetCubeN();
cell abi_SetCubeN(cell cubeN);
cell abi_ByteN(cell* arr, cell n);
cell abi_TRBL_Deserialize(cell* pkt);
cell abi_topCubeN(cell _cubeN, cell _faceN);
cell abi_topFaceN(cell _cubeN, cell _faceN);
cell abi_rightCubeN(cell _cubeN, cell _faceN);
cell abi_rightFaceN(cell _cubeN, cell _faceN);
cell abi_bottomCubeN(cell _cubeN, cell _faceN);
cell abi_bottomFaceN(cell _cubeN, cell _faceN);
cell abi_leftCubeN(cell _cubeN, cell _faceN);
cell abi_leftFaceN(cell _cubeN, cell _faceN);
cell abi_MTD_Deserialize(cell* pkt);
cell abi_MTD_GetFaceAccelX(cell faceN);
cell abi_MTD_GetFaceAccelY(cell faceN);
cell abi_MTD_GetFaceAccelZ(cell faceN);
cell abi_MTD_GetFaceGyroX(cell faceN);
cell abi_MTD_GetFaceGyroY(cell faceN);
cell abi_MTD_GetFaceGyroZ(cell faceN);
cell abi_CMD_REDRAW(cell faceN);
cell abi_trigger_debug_info();
cell abi_trigger_bluetooth();
cell abi_CMD_FILL(cell R, cell G, cell B);
cell abi_CMD_FILL_2(cell rgb);
cell abi_CMD_BITMAP(cell resID, cell x, cell y, cell angle, cell mirror);
cell abi_CMD_LINE(cell x1, cell y1, cell x2, cell y2, cell R, cell G, cell B);
cell abi_CMD_RECT(cell x1, cell y1, cell x2, cell y2, cell R, cell G, cell B);
cell abi_CMD_NET_TX(cell line_tx, cell TTL, cell* data, cell with_pool);
cell abi_CMD_CHANGE_SCRIPT(cell scriptID);
cell abi_CMD_SLEEP();
cell getShake(cell num, cell shift);
cell send_info();
cell check_shake(cell script, cell face);
cell Draw_Results(cell _type);
cell abi_show_results(cell thiscommand, cell toCubeN, cell toFaceN, cell level);
cell abi_send_level(cell thiscommand, cell level);
cell abi_respose_to_request_results(cell thiscommand, cell fromCube, cell completed);
cell abi_set_result_from_RX(cell cubeN, cell value, cell total);
cell abi_clearing_total(cell total);
cell getCubeFaceToResults(cell thisCubeN, cell thisFaceN, cell cubeN, cell faceN);
cell trbl_clearing();
cell trbl_on_tick();
cell trbl_send(cell ThisCubeN);
cell trbl_init();
cell Random(cell min, cell max);
cell Round(cell number, cell base);
cell run(cell* pkt, cell size, cell* src);
cell platform_TwoWayLineToFaceOpp(cell face_or_line);
cell platform_TwoWayLineToFaceLeft(cell face_or_line);
cell rubik_initCfmMap();
cell rubik_mapLinesAndFaces(cell receivedFromLine);
cell rubik_assignNumberToNeihbours();
cell rubik_paintFaces();
cell ONTICK();
cell ON_INIT();
cell ON_CMD_NET_RX(cell* pkt);
cell ON_CHECK_ROTATE();