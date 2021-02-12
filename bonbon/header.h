#include "pawn2c.h"
#define new signed
#define bool int
#define false 0
#define true 1



cell abi_GetCubeN();
cell abi_SetCubeN(cell cubeN);
cell abi_ByteN(cell* arr, cell n);
cell abi_TRBL_Deserialize(cell* pkt);
cell abi_check_rotate();
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
cell abi_exit();
//cell abi_trigger_debug_info();
//cell abi_trigger_bluetooth();
cell abi_CMD_FILL(cell R, cell G, cell B);
cell abi_CMD_FILL_2(cell rgb);
cell abi_CMD_TEXT(cell* text, cell fontResID, cell x, cell y, cell scale, cell angle, cell r, cell g, cell b, cell size);
// cell abi_CMD_BITMAP(cell resID, cell x, cell y, cell angle, cell mirror, cell bool g2d);
cell abi_CMD_PLAYSND(cell id, cell volume);
cell abi_CMD_LINE(cell x1, cell y1, cell x2, cell y2, cell R, cell G, cell B);
cell abi_CMD_RECT(cell x1, cell y1, cell x2, cell y2, cell R, cell G, cell B);
cell abi_CMD_NET_TX(cell line_tx, cell TTL, cell* data, cell with_pool);
cell abi_CMD_SAVE_STATE(cell* data, cell size);
cell abi_CMD_LOAD_STATE();
cell abi_CMD_CHANGE_SCRIPT(cell scriptID);
cell abi_CMD_SLEEP();
cell abi_CMD_G2D_BEGIN_BITMAP(cell resID, cell width, cell height, cell bool replace);
cell abi_CMD_G2D_BEGIN_DISPLAY(cell display, cell bool replace);
cell abi_CMD_G2D_ADD_SPRITE(cell resID, cell bool g2d, cell x, cell y, cell alpha, cell color, cell rotation, cell mirror);
cell abi_CMD_G2D_ADD_RECTANGLE(cell x, cell y, cell width, cell height, cell color);
cell abi_CMD_G2D_END();
cell abi_trigger_nightlamp(cell c_mode);
cell abi_CMD_DYNAMIC_TEXTURE(cell effectId, cell time, cell* args, cell argsCount, cell bool g2d);
cell Draw_Results(cell _type);
cell abi_show_results(cell thiscommand, cell toCubeN, cell toFaceN, cell level);
cell abi_send_level(cell thiscommand, cell level);
cell abi_respose_to_request_results(cell thiscommand, cell fromCube, cell completed);
cell abi_set_result_from_RX(cell cubeN, cell value, cell total);
cell abi_clearing_total(cell total);
cell getCubeFaceToResults(cell thisCubeN, cell thisFaceN, cell cubeN, cell faceN);
cell getShake(cell num, cell shift);
cell send_info();
cell check_shake(cell script, cell face);
cell trbl_clearing();
cell trbl_on_tick();
cell trbl_send(cell ThisCubeN);
cell trbl_init();
cell get_rotation_side();
cell get_top_cube(cell c_faceN);
cell get_left_cube(cell c_faceN);
cell get_right_cube(cell c_faceN);
cell update_trbl_record_for_rotation();
cell trbl_clear_after_rotation();
cell Random(cell min, cell max);
cell Round(cell number, cell base);
cell pow(cell a, cell n);
cell int_sqrt(cell x);
cell run(cell* pkt, cell size, cell* src);
cell RandomRange(cell min, cell max);
cell Parse(cell number, cell xCenterPos, cell yCenterPos, cell angle, cell axis, cell reverse);
cell DrawHUD(cell angle);
cell GeneratePartiles();
cell ResendLastMessage(cell messageNumber);
cell CheckAngles();
cell TransferChip(cell direction, cell face);
cell ONTICK();
cell ON_LOAD_GAME_DATA();
cell ON_INIT();
cell ON_CHECK_ROTATE();
//cell main();
