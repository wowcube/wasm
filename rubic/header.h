
typedef int cell;
#define new signed
#define bool int
#define false 0
#define true 1
typedef int stock;

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