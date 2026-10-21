#include "pose.h"

namespace {
#define DIM 18
#define EDIM 18
#define MEDIM 18
typedef void (*Hfun)(double *, double *, double *);
const static double MAHA_THRESH_4 = 7.814727903251177;
const static double MAHA_THRESH_10 = 7.814727903251177;
const static double MAHA_THRESH_13 = 7.814727903251177;
const static double MAHA_THRESH_14 = 7.814727903251177;

/******************************************************************************
 *                      Code generated with SymPy 1.14.0                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_5537634756637633787) {
   out_5537634756637633787[0] = delta_x[0] + nom_x[0];
   out_5537634756637633787[1] = delta_x[1] + nom_x[1];
   out_5537634756637633787[2] = delta_x[2] + nom_x[2];
   out_5537634756637633787[3] = delta_x[3] + nom_x[3];
   out_5537634756637633787[4] = delta_x[4] + nom_x[4];
   out_5537634756637633787[5] = delta_x[5] + nom_x[5];
   out_5537634756637633787[6] = delta_x[6] + nom_x[6];
   out_5537634756637633787[7] = delta_x[7] + nom_x[7];
   out_5537634756637633787[8] = delta_x[8] + nom_x[8];
   out_5537634756637633787[9] = delta_x[9] + nom_x[9];
   out_5537634756637633787[10] = delta_x[10] + nom_x[10];
   out_5537634756637633787[11] = delta_x[11] + nom_x[11];
   out_5537634756637633787[12] = delta_x[12] + nom_x[12];
   out_5537634756637633787[13] = delta_x[13] + nom_x[13];
   out_5537634756637633787[14] = delta_x[14] + nom_x[14];
   out_5537634756637633787[15] = delta_x[15] + nom_x[15];
   out_5537634756637633787[16] = delta_x[16] + nom_x[16];
   out_5537634756637633787[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_6641296844440448094) {
   out_6641296844440448094[0] = -nom_x[0] + true_x[0];
   out_6641296844440448094[1] = -nom_x[1] + true_x[1];
   out_6641296844440448094[2] = -nom_x[2] + true_x[2];
   out_6641296844440448094[3] = -nom_x[3] + true_x[3];
   out_6641296844440448094[4] = -nom_x[4] + true_x[4];
   out_6641296844440448094[5] = -nom_x[5] + true_x[5];
   out_6641296844440448094[6] = -nom_x[6] + true_x[6];
   out_6641296844440448094[7] = -nom_x[7] + true_x[7];
   out_6641296844440448094[8] = -nom_x[8] + true_x[8];
   out_6641296844440448094[9] = -nom_x[9] + true_x[9];
   out_6641296844440448094[10] = -nom_x[10] + true_x[10];
   out_6641296844440448094[11] = -nom_x[11] + true_x[11];
   out_6641296844440448094[12] = -nom_x[12] + true_x[12];
   out_6641296844440448094[13] = -nom_x[13] + true_x[13];
   out_6641296844440448094[14] = -nom_x[14] + true_x[14];
   out_6641296844440448094[15] = -nom_x[15] + true_x[15];
   out_6641296844440448094[16] = -nom_x[16] + true_x[16];
   out_6641296844440448094[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_1952919505077041985) {
   out_1952919505077041985[0] = 1.0;
   out_1952919505077041985[1] = 0.0;
   out_1952919505077041985[2] = 0.0;
   out_1952919505077041985[3] = 0.0;
   out_1952919505077041985[4] = 0.0;
   out_1952919505077041985[5] = 0.0;
   out_1952919505077041985[6] = 0.0;
   out_1952919505077041985[7] = 0.0;
   out_1952919505077041985[8] = 0.0;
   out_1952919505077041985[9] = 0.0;
   out_1952919505077041985[10] = 0.0;
   out_1952919505077041985[11] = 0.0;
   out_1952919505077041985[12] = 0.0;
   out_1952919505077041985[13] = 0.0;
   out_1952919505077041985[14] = 0.0;
   out_1952919505077041985[15] = 0.0;
   out_1952919505077041985[16] = 0.0;
   out_1952919505077041985[17] = 0.0;
   out_1952919505077041985[18] = 0.0;
   out_1952919505077041985[19] = 1.0;
   out_1952919505077041985[20] = 0.0;
   out_1952919505077041985[21] = 0.0;
   out_1952919505077041985[22] = 0.0;
   out_1952919505077041985[23] = 0.0;
   out_1952919505077041985[24] = 0.0;
   out_1952919505077041985[25] = 0.0;
   out_1952919505077041985[26] = 0.0;
   out_1952919505077041985[27] = 0.0;
   out_1952919505077041985[28] = 0.0;
   out_1952919505077041985[29] = 0.0;
   out_1952919505077041985[30] = 0.0;
   out_1952919505077041985[31] = 0.0;
   out_1952919505077041985[32] = 0.0;
   out_1952919505077041985[33] = 0.0;
   out_1952919505077041985[34] = 0.0;
   out_1952919505077041985[35] = 0.0;
   out_1952919505077041985[36] = 0.0;
   out_1952919505077041985[37] = 0.0;
   out_1952919505077041985[38] = 1.0;
   out_1952919505077041985[39] = 0.0;
   out_1952919505077041985[40] = 0.0;
   out_1952919505077041985[41] = 0.0;
   out_1952919505077041985[42] = 0.0;
   out_1952919505077041985[43] = 0.0;
   out_1952919505077041985[44] = 0.0;
   out_1952919505077041985[45] = 0.0;
   out_1952919505077041985[46] = 0.0;
   out_1952919505077041985[47] = 0.0;
   out_1952919505077041985[48] = 0.0;
   out_1952919505077041985[49] = 0.0;
   out_1952919505077041985[50] = 0.0;
   out_1952919505077041985[51] = 0.0;
   out_1952919505077041985[52] = 0.0;
   out_1952919505077041985[53] = 0.0;
   out_1952919505077041985[54] = 0.0;
   out_1952919505077041985[55] = 0.0;
   out_1952919505077041985[56] = 0.0;
   out_1952919505077041985[57] = 1.0;
   out_1952919505077041985[58] = 0.0;
   out_1952919505077041985[59] = 0.0;
   out_1952919505077041985[60] = 0.0;
   out_1952919505077041985[61] = 0.0;
   out_1952919505077041985[62] = 0.0;
   out_1952919505077041985[63] = 0.0;
   out_1952919505077041985[64] = 0.0;
   out_1952919505077041985[65] = 0.0;
   out_1952919505077041985[66] = 0.0;
   out_1952919505077041985[67] = 0.0;
   out_1952919505077041985[68] = 0.0;
   out_1952919505077041985[69] = 0.0;
   out_1952919505077041985[70] = 0.0;
   out_1952919505077041985[71] = 0.0;
   out_1952919505077041985[72] = 0.0;
   out_1952919505077041985[73] = 0.0;
   out_1952919505077041985[74] = 0.0;
   out_1952919505077041985[75] = 0.0;
   out_1952919505077041985[76] = 1.0;
   out_1952919505077041985[77] = 0.0;
   out_1952919505077041985[78] = 0.0;
   out_1952919505077041985[79] = 0.0;
   out_1952919505077041985[80] = 0.0;
   out_1952919505077041985[81] = 0.0;
   out_1952919505077041985[82] = 0.0;
   out_1952919505077041985[83] = 0.0;
   out_1952919505077041985[84] = 0.0;
   out_1952919505077041985[85] = 0.0;
   out_1952919505077041985[86] = 0.0;
   out_1952919505077041985[87] = 0.0;
   out_1952919505077041985[88] = 0.0;
   out_1952919505077041985[89] = 0.0;
   out_1952919505077041985[90] = 0.0;
   out_1952919505077041985[91] = 0.0;
   out_1952919505077041985[92] = 0.0;
   out_1952919505077041985[93] = 0.0;
   out_1952919505077041985[94] = 0.0;
   out_1952919505077041985[95] = 1.0;
   out_1952919505077041985[96] = 0.0;
   out_1952919505077041985[97] = 0.0;
   out_1952919505077041985[98] = 0.0;
   out_1952919505077041985[99] = 0.0;
   out_1952919505077041985[100] = 0.0;
   out_1952919505077041985[101] = 0.0;
   out_1952919505077041985[102] = 0.0;
   out_1952919505077041985[103] = 0.0;
   out_1952919505077041985[104] = 0.0;
   out_1952919505077041985[105] = 0.0;
   out_1952919505077041985[106] = 0.0;
   out_1952919505077041985[107] = 0.0;
   out_1952919505077041985[108] = 0.0;
   out_1952919505077041985[109] = 0.0;
   out_1952919505077041985[110] = 0.0;
   out_1952919505077041985[111] = 0.0;
   out_1952919505077041985[112] = 0.0;
   out_1952919505077041985[113] = 0.0;
   out_1952919505077041985[114] = 1.0;
   out_1952919505077041985[115] = 0.0;
   out_1952919505077041985[116] = 0.0;
   out_1952919505077041985[117] = 0.0;
   out_1952919505077041985[118] = 0.0;
   out_1952919505077041985[119] = 0.0;
   out_1952919505077041985[120] = 0.0;
   out_1952919505077041985[121] = 0.0;
   out_1952919505077041985[122] = 0.0;
   out_1952919505077041985[123] = 0.0;
   out_1952919505077041985[124] = 0.0;
   out_1952919505077041985[125] = 0.0;
   out_1952919505077041985[126] = 0.0;
   out_1952919505077041985[127] = 0.0;
   out_1952919505077041985[128] = 0.0;
   out_1952919505077041985[129] = 0.0;
   out_1952919505077041985[130] = 0.0;
   out_1952919505077041985[131] = 0.0;
   out_1952919505077041985[132] = 0.0;
   out_1952919505077041985[133] = 1.0;
   out_1952919505077041985[134] = 0.0;
   out_1952919505077041985[135] = 0.0;
   out_1952919505077041985[136] = 0.0;
   out_1952919505077041985[137] = 0.0;
   out_1952919505077041985[138] = 0.0;
   out_1952919505077041985[139] = 0.0;
   out_1952919505077041985[140] = 0.0;
   out_1952919505077041985[141] = 0.0;
   out_1952919505077041985[142] = 0.0;
   out_1952919505077041985[143] = 0.0;
   out_1952919505077041985[144] = 0.0;
   out_1952919505077041985[145] = 0.0;
   out_1952919505077041985[146] = 0.0;
   out_1952919505077041985[147] = 0.0;
   out_1952919505077041985[148] = 0.0;
   out_1952919505077041985[149] = 0.0;
   out_1952919505077041985[150] = 0.0;
   out_1952919505077041985[151] = 0.0;
   out_1952919505077041985[152] = 1.0;
   out_1952919505077041985[153] = 0.0;
   out_1952919505077041985[154] = 0.0;
   out_1952919505077041985[155] = 0.0;
   out_1952919505077041985[156] = 0.0;
   out_1952919505077041985[157] = 0.0;
   out_1952919505077041985[158] = 0.0;
   out_1952919505077041985[159] = 0.0;
   out_1952919505077041985[160] = 0.0;
   out_1952919505077041985[161] = 0.0;
   out_1952919505077041985[162] = 0.0;
   out_1952919505077041985[163] = 0.0;
   out_1952919505077041985[164] = 0.0;
   out_1952919505077041985[165] = 0.0;
   out_1952919505077041985[166] = 0.0;
   out_1952919505077041985[167] = 0.0;
   out_1952919505077041985[168] = 0.0;
   out_1952919505077041985[169] = 0.0;
   out_1952919505077041985[170] = 0.0;
   out_1952919505077041985[171] = 1.0;
   out_1952919505077041985[172] = 0.0;
   out_1952919505077041985[173] = 0.0;
   out_1952919505077041985[174] = 0.0;
   out_1952919505077041985[175] = 0.0;
   out_1952919505077041985[176] = 0.0;
   out_1952919505077041985[177] = 0.0;
   out_1952919505077041985[178] = 0.0;
   out_1952919505077041985[179] = 0.0;
   out_1952919505077041985[180] = 0.0;
   out_1952919505077041985[181] = 0.0;
   out_1952919505077041985[182] = 0.0;
   out_1952919505077041985[183] = 0.0;
   out_1952919505077041985[184] = 0.0;
   out_1952919505077041985[185] = 0.0;
   out_1952919505077041985[186] = 0.0;
   out_1952919505077041985[187] = 0.0;
   out_1952919505077041985[188] = 0.0;
   out_1952919505077041985[189] = 0.0;
   out_1952919505077041985[190] = 1.0;
   out_1952919505077041985[191] = 0.0;
   out_1952919505077041985[192] = 0.0;
   out_1952919505077041985[193] = 0.0;
   out_1952919505077041985[194] = 0.0;
   out_1952919505077041985[195] = 0.0;
   out_1952919505077041985[196] = 0.0;
   out_1952919505077041985[197] = 0.0;
   out_1952919505077041985[198] = 0.0;
   out_1952919505077041985[199] = 0.0;
   out_1952919505077041985[200] = 0.0;
   out_1952919505077041985[201] = 0.0;
   out_1952919505077041985[202] = 0.0;
   out_1952919505077041985[203] = 0.0;
   out_1952919505077041985[204] = 0.0;
   out_1952919505077041985[205] = 0.0;
   out_1952919505077041985[206] = 0.0;
   out_1952919505077041985[207] = 0.0;
   out_1952919505077041985[208] = 0.0;
   out_1952919505077041985[209] = 1.0;
   out_1952919505077041985[210] = 0.0;
   out_1952919505077041985[211] = 0.0;
   out_1952919505077041985[212] = 0.0;
   out_1952919505077041985[213] = 0.0;
   out_1952919505077041985[214] = 0.0;
   out_1952919505077041985[215] = 0.0;
   out_1952919505077041985[216] = 0.0;
   out_1952919505077041985[217] = 0.0;
   out_1952919505077041985[218] = 0.0;
   out_1952919505077041985[219] = 0.0;
   out_1952919505077041985[220] = 0.0;
   out_1952919505077041985[221] = 0.0;
   out_1952919505077041985[222] = 0.0;
   out_1952919505077041985[223] = 0.0;
   out_1952919505077041985[224] = 0.0;
   out_1952919505077041985[225] = 0.0;
   out_1952919505077041985[226] = 0.0;
   out_1952919505077041985[227] = 0.0;
   out_1952919505077041985[228] = 1.0;
   out_1952919505077041985[229] = 0.0;
   out_1952919505077041985[230] = 0.0;
   out_1952919505077041985[231] = 0.0;
   out_1952919505077041985[232] = 0.0;
   out_1952919505077041985[233] = 0.0;
   out_1952919505077041985[234] = 0.0;
   out_1952919505077041985[235] = 0.0;
   out_1952919505077041985[236] = 0.0;
   out_1952919505077041985[237] = 0.0;
   out_1952919505077041985[238] = 0.0;
   out_1952919505077041985[239] = 0.0;
   out_1952919505077041985[240] = 0.0;
   out_1952919505077041985[241] = 0.0;
   out_1952919505077041985[242] = 0.0;
   out_1952919505077041985[243] = 0.0;
   out_1952919505077041985[244] = 0.0;
   out_1952919505077041985[245] = 0.0;
   out_1952919505077041985[246] = 0.0;
   out_1952919505077041985[247] = 1.0;
   out_1952919505077041985[248] = 0.0;
   out_1952919505077041985[249] = 0.0;
   out_1952919505077041985[250] = 0.0;
   out_1952919505077041985[251] = 0.0;
   out_1952919505077041985[252] = 0.0;
   out_1952919505077041985[253] = 0.0;
   out_1952919505077041985[254] = 0.0;
   out_1952919505077041985[255] = 0.0;
   out_1952919505077041985[256] = 0.0;
   out_1952919505077041985[257] = 0.0;
   out_1952919505077041985[258] = 0.0;
   out_1952919505077041985[259] = 0.0;
   out_1952919505077041985[260] = 0.0;
   out_1952919505077041985[261] = 0.0;
   out_1952919505077041985[262] = 0.0;
   out_1952919505077041985[263] = 0.0;
   out_1952919505077041985[264] = 0.0;
   out_1952919505077041985[265] = 0.0;
   out_1952919505077041985[266] = 1.0;
   out_1952919505077041985[267] = 0.0;
   out_1952919505077041985[268] = 0.0;
   out_1952919505077041985[269] = 0.0;
   out_1952919505077041985[270] = 0.0;
   out_1952919505077041985[271] = 0.0;
   out_1952919505077041985[272] = 0.0;
   out_1952919505077041985[273] = 0.0;
   out_1952919505077041985[274] = 0.0;
   out_1952919505077041985[275] = 0.0;
   out_1952919505077041985[276] = 0.0;
   out_1952919505077041985[277] = 0.0;
   out_1952919505077041985[278] = 0.0;
   out_1952919505077041985[279] = 0.0;
   out_1952919505077041985[280] = 0.0;
   out_1952919505077041985[281] = 0.0;
   out_1952919505077041985[282] = 0.0;
   out_1952919505077041985[283] = 0.0;
   out_1952919505077041985[284] = 0.0;
   out_1952919505077041985[285] = 1.0;
   out_1952919505077041985[286] = 0.0;
   out_1952919505077041985[287] = 0.0;
   out_1952919505077041985[288] = 0.0;
   out_1952919505077041985[289] = 0.0;
   out_1952919505077041985[290] = 0.0;
   out_1952919505077041985[291] = 0.0;
   out_1952919505077041985[292] = 0.0;
   out_1952919505077041985[293] = 0.0;
   out_1952919505077041985[294] = 0.0;
   out_1952919505077041985[295] = 0.0;
   out_1952919505077041985[296] = 0.0;
   out_1952919505077041985[297] = 0.0;
   out_1952919505077041985[298] = 0.0;
   out_1952919505077041985[299] = 0.0;
   out_1952919505077041985[300] = 0.0;
   out_1952919505077041985[301] = 0.0;
   out_1952919505077041985[302] = 0.0;
   out_1952919505077041985[303] = 0.0;
   out_1952919505077041985[304] = 1.0;
   out_1952919505077041985[305] = 0.0;
   out_1952919505077041985[306] = 0.0;
   out_1952919505077041985[307] = 0.0;
   out_1952919505077041985[308] = 0.0;
   out_1952919505077041985[309] = 0.0;
   out_1952919505077041985[310] = 0.0;
   out_1952919505077041985[311] = 0.0;
   out_1952919505077041985[312] = 0.0;
   out_1952919505077041985[313] = 0.0;
   out_1952919505077041985[314] = 0.0;
   out_1952919505077041985[315] = 0.0;
   out_1952919505077041985[316] = 0.0;
   out_1952919505077041985[317] = 0.0;
   out_1952919505077041985[318] = 0.0;
   out_1952919505077041985[319] = 0.0;
   out_1952919505077041985[320] = 0.0;
   out_1952919505077041985[321] = 0.0;
   out_1952919505077041985[322] = 0.0;
   out_1952919505077041985[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_5673854303566172016) {
   out_5673854303566172016[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_5673854303566172016[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_5673854303566172016[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_5673854303566172016[3] = dt*state[12] + state[3];
   out_5673854303566172016[4] = dt*state[13] + state[4];
   out_5673854303566172016[5] = dt*state[14] + state[5];
   out_5673854303566172016[6] = state[6];
   out_5673854303566172016[7] = state[7];
   out_5673854303566172016[8] = state[8];
   out_5673854303566172016[9] = state[9];
   out_5673854303566172016[10] = state[10];
   out_5673854303566172016[11] = state[11];
   out_5673854303566172016[12] = state[12];
   out_5673854303566172016[13] = state[13];
   out_5673854303566172016[14] = state[14];
   out_5673854303566172016[15] = state[15];
   out_5673854303566172016[16] = state[16];
   out_5673854303566172016[17] = state[17];
}
void F_fun(double *state, double dt, double *out_2879053897486576154) {
   out_2879053897486576154[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2879053897486576154[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2879053897486576154[2] = 0;
   out_2879053897486576154[3] = 0;
   out_2879053897486576154[4] = 0;
   out_2879053897486576154[5] = 0;
   out_2879053897486576154[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2879053897486576154[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2879053897486576154[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2879053897486576154[9] = 0;
   out_2879053897486576154[10] = 0;
   out_2879053897486576154[11] = 0;
   out_2879053897486576154[12] = 0;
   out_2879053897486576154[13] = 0;
   out_2879053897486576154[14] = 0;
   out_2879053897486576154[15] = 0;
   out_2879053897486576154[16] = 0;
   out_2879053897486576154[17] = 0;
   out_2879053897486576154[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_2879053897486576154[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_2879053897486576154[20] = 0;
   out_2879053897486576154[21] = 0;
   out_2879053897486576154[22] = 0;
   out_2879053897486576154[23] = 0;
   out_2879053897486576154[24] = 0;
   out_2879053897486576154[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_2879053897486576154[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_2879053897486576154[27] = 0;
   out_2879053897486576154[28] = 0;
   out_2879053897486576154[29] = 0;
   out_2879053897486576154[30] = 0;
   out_2879053897486576154[31] = 0;
   out_2879053897486576154[32] = 0;
   out_2879053897486576154[33] = 0;
   out_2879053897486576154[34] = 0;
   out_2879053897486576154[35] = 0;
   out_2879053897486576154[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2879053897486576154[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2879053897486576154[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2879053897486576154[39] = 0;
   out_2879053897486576154[40] = 0;
   out_2879053897486576154[41] = 0;
   out_2879053897486576154[42] = 0;
   out_2879053897486576154[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2879053897486576154[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2879053897486576154[45] = 0;
   out_2879053897486576154[46] = 0;
   out_2879053897486576154[47] = 0;
   out_2879053897486576154[48] = 0;
   out_2879053897486576154[49] = 0;
   out_2879053897486576154[50] = 0;
   out_2879053897486576154[51] = 0;
   out_2879053897486576154[52] = 0;
   out_2879053897486576154[53] = 0;
   out_2879053897486576154[54] = 0;
   out_2879053897486576154[55] = 0;
   out_2879053897486576154[56] = 0;
   out_2879053897486576154[57] = 1;
   out_2879053897486576154[58] = 0;
   out_2879053897486576154[59] = 0;
   out_2879053897486576154[60] = 0;
   out_2879053897486576154[61] = 0;
   out_2879053897486576154[62] = 0;
   out_2879053897486576154[63] = 0;
   out_2879053897486576154[64] = 0;
   out_2879053897486576154[65] = 0;
   out_2879053897486576154[66] = dt;
   out_2879053897486576154[67] = 0;
   out_2879053897486576154[68] = 0;
   out_2879053897486576154[69] = 0;
   out_2879053897486576154[70] = 0;
   out_2879053897486576154[71] = 0;
   out_2879053897486576154[72] = 0;
   out_2879053897486576154[73] = 0;
   out_2879053897486576154[74] = 0;
   out_2879053897486576154[75] = 0;
   out_2879053897486576154[76] = 1;
   out_2879053897486576154[77] = 0;
   out_2879053897486576154[78] = 0;
   out_2879053897486576154[79] = 0;
   out_2879053897486576154[80] = 0;
   out_2879053897486576154[81] = 0;
   out_2879053897486576154[82] = 0;
   out_2879053897486576154[83] = 0;
   out_2879053897486576154[84] = 0;
   out_2879053897486576154[85] = dt;
   out_2879053897486576154[86] = 0;
   out_2879053897486576154[87] = 0;
   out_2879053897486576154[88] = 0;
   out_2879053897486576154[89] = 0;
   out_2879053897486576154[90] = 0;
   out_2879053897486576154[91] = 0;
   out_2879053897486576154[92] = 0;
   out_2879053897486576154[93] = 0;
   out_2879053897486576154[94] = 0;
   out_2879053897486576154[95] = 1;
   out_2879053897486576154[96] = 0;
   out_2879053897486576154[97] = 0;
   out_2879053897486576154[98] = 0;
   out_2879053897486576154[99] = 0;
   out_2879053897486576154[100] = 0;
   out_2879053897486576154[101] = 0;
   out_2879053897486576154[102] = 0;
   out_2879053897486576154[103] = 0;
   out_2879053897486576154[104] = dt;
   out_2879053897486576154[105] = 0;
   out_2879053897486576154[106] = 0;
   out_2879053897486576154[107] = 0;
   out_2879053897486576154[108] = 0;
   out_2879053897486576154[109] = 0;
   out_2879053897486576154[110] = 0;
   out_2879053897486576154[111] = 0;
   out_2879053897486576154[112] = 0;
   out_2879053897486576154[113] = 0;
   out_2879053897486576154[114] = 1;
   out_2879053897486576154[115] = 0;
   out_2879053897486576154[116] = 0;
   out_2879053897486576154[117] = 0;
   out_2879053897486576154[118] = 0;
   out_2879053897486576154[119] = 0;
   out_2879053897486576154[120] = 0;
   out_2879053897486576154[121] = 0;
   out_2879053897486576154[122] = 0;
   out_2879053897486576154[123] = 0;
   out_2879053897486576154[124] = 0;
   out_2879053897486576154[125] = 0;
   out_2879053897486576154[126] = 0;
   out_2879053897486576154[127] = 0;
   out_2879053897486576154[128] = 0;
   out_2879053897486576154[129] = 0;
   out_2879053897486576154[130] = 0;
   out_2879053897486576154[131] = 0;
   out_2879053897486576154[132] = 0;
   out_2879053897486576154[133] = 1;
   out_2879053897486576154[134] = 0;
   out_2879053897486576154[135] = 0;
   out_2879053897486576154[136] = 0;
   out_2879053897486576154[137] = 0;
   out_2879053897486576154[138] = 0;
   out_2879053897486576154[139] = 0;
   out_2879053897486576154[140] = 0;
   out_2879053897486576154[141] = 0;
   out_2879053897486576154[142] = 0;
   out_2879053897486576154[143] = 0;
   out_2879053897486576154[144] = 0;
   out_2879053897486576154[145] = 0;
   out_2879053897486576154[146] = 0;
   out_2879053897486576154[147] = 0;
   out_2879053897486576154[148] = 0;
   out_2879053897486576154[149] = 0;
   out_2879053897486576154[150] = 0;
   out_2879053897486576154[151] = 0;
   out_2879053897486576154[152] = 1;
   out_2879053897486576154[153] = 0;
   out_2879053897486576154[154] = 0;
   out_2879053897486576154[155] = 0;
   out_2879053897486576154[156] = 0;
   out_2879053897486576154[157] = 0;
   out_2879053897486576154[158] = 0;
   out_2879053897486576154[159] = 0;
   out_2879053897486576154[160] = 0;
   out_2879053897486576154[161] = 0;
   out_2879053897486576154[162] = 0;
   out_2879053897486576154[163] = 0;
   out_2879053897486576154[164] = 0;
   out_2879053897486576154[165] = 0;
   out_2879053897486576154[166] = 0;
   out_2879053897486576154[167] = 0;
   out_2879053897486576154[168] = 0;
   out_2879053897486576154[169] = 0;
   out_2879053897486576154[170] = 0;
   out_2879053897486576154[171] = 1;
   out_2879053897486576154[172] = 0;
   out_2879053897486576154[173] = 0;
   out_2879053897486576154[174] = 0;
   out_2879053897486576154[175] = 0;
   out_2879053897486576154[176] = 0;
   out_2879053897486576154[177] = 0;
   out_2879053897486576154[178] = 0;
   out_2879053897486576154[179] = 0;
   out_2879053897486576154[180] = 0;
   out_2879053897486576154[181] = 0;
   out_2879053897486576154[182] = 0;
   out_2879053897486576154[183] = 0;
   out_2879053897486576154[184] = 0;
   out_2879053897486576154[185] = 0;
   out_2879053897486576154[186] = 0;
   out_2879053897486576154[187] = 0;
   out_2879053897486576154[188] = 0;
   out_2879053897486576154[189] = 0;
   out_2879053897486576154[190] = 1;
   out_2879053897486576154[191] = 0;
   out_2879053897486576154[192] = 0;
   out_2879053897486576154[193] = 0;
   out_2879053897486576154[194] = 0;
   out_2879053897486576154[195] = 0;
   out_2879053897486576154[196] = 0;
   out_2879053897486576154[197] = 0;
   out_2879053897486576154[198] = 0;
   out_2879053897486576154[199] = 0;
   out_2879053897486576154[200] = 0;
   out_2879053897486576154[201] = 0;
   out_2879053897486576154[202] = 0;
   out_2879053897486576154[203] = 0;
   out_2879053897486576154[204] = 0;
   out_2879053897486576154[205] = 0;
   out_2879053897486576154[206] = 0;
   out_2879053897486576154[207] = 0;
   out_2879053897486576154[208] = 0;
   out_2879053897486576154[209] = 1;
   out_2879053897486576154[210] = 0;
   out_2879053897486576154[211] = 0;
   out_2879053897486576154[212] = 0;
   out_2879053897486576154[213] = 0;
   out_2879053897486576154[214] = 0;
   out_2879053897486576154[215] = 0;
   out_2879053897486576154[216] = 0;
   out_2879053897486576154[217] = 0;
   out_2879053897486576154[218] = 0;
   out_2879053897486576154[219] = 0;
   out_2879053897486576154[220] = 0;
   out_2879053897486576154[221] = 0;
   out_2879053897486576154[222] = 0;
   out_2879053897486576154[223] = 0;
   out_2879053897486576154[224] = 0;
   out_2879053897486576154[225] = 0;
   out_2879053897486576154[226] = 0;
   out_2879053897486576154[227] = 0;
   out_2879053897486576154[228] = 1;
   out_2879053897486576154[229] = 0;
   out_2879053897486576154[230] = 0;
   out_2879053897486576154[231] = 0;
   out_2879053897486576154[232] = 0;
   out_2879053897486576154[233] = 0;
   out_2879053897486576154[234] = 0;
   out_2879053897486576154[235] = 0;
   out_2879053897486576154[236] = 0;
   out_2879053897486576154[237] = 0;
   out_2879053897486576154[238] = 0;
   out_2879053897486576154[239] = 0;
   out_2879053897486576154[240] = 0;
   out_2879053897486576154[241] = 0;
   out_2879053897486576154[242] = 0;
   out_2879053897486576154[243] = 0;
   out_2879053897486576154[244] = 0;
   out_2879053897486576154[245] = 0;
   out_2879053897486576154[246] = 0;
   out_2879053897486576154[247] = 1;
   out_2879053897486576154[248] = 0;
   out_2879053897486576154[249] = 0;
   out_2879053897486576154[250] = 0;
   out_2879053897486576154[251] = 0;
   out_2879053897486576154[252] = 0;
   out_2879053897486576154[253] = 0;
   out_2879053897486576154[254] = 0;
   out_2879053897486576154[255] = 0;
   out_2879053897486576154[256] = 0;
   out_2879053897486576154[257] = 0;
   out_2879053897486576154[258] = 0;
   out_2879053897486576154[259] = 0;
   out_2879053897486576154[260] = 0;
   out_2879053897486576154[261] = 0;
   out_2879053897486576154[262] = 0;
   out_2879053897486576154[263] = 0;
   out_2879053897486576154[264] = 0;
   out_2879053897486576154[265] = 0;
   out_2879053897486576154[266] = 1;
   out_2879053897486576154[267] = 0;
   out_2879053897486576154[268] = 0;
   out_2879053897486576154[269] = 0;
   out_2879053897486576154[270] = 0;
   out_2879053897486576154[271] = 0;
   out_2879053897486576154[272] = 0;
   out_2879053897486576154[273] = 0;
   out_2879053897486576154[274] = 0;
   out_2879053897486576154[275] = 0;
   out_2879053897486576154[276] = 0;
   out_2879053897486576154[277] = 0;
   out_2879053897486576154[278] = 0;
   out_2879053897486576154[279] = 0;
   out_2879053897486576154[280] = 0;
   out_2879053897486576154[281] = 0;
   out_2879053897486576154[282] = 0;
   out_2879053897486576154[283] = 0;
   out_2879053897486576154[284] = 0;
   out_2879053897486576154[285] = 1;
   out_2879053897486576154[286] = 0;
   out_2879053897486576154[287] = 0;
   out_2879053897486576154[288] = 0;
   out_2879053897486576154[289] = 0;
   out_2879053897486576154[290] = 0;
   out_2879053897486576154[291] = 0;
   out_2879053897486576154[292] = 0;
   out_2879053897486576154[293] = 0;
   out_2879053897486576154[294] = 0;
   out_2879053897486576154[295] = 0;
   out_2879053897486576154[296] = 0;
   out_2879053897486576154[297] = 0;
   out_2879053897486576154[298] = 0;
   out_2879053897486576154[299] = 0;
   out_2879053897486576154[300] = 0;
   out_2879053897486576154[301] = 0;
   out_2879053897486576154[302] = 0;
   out_2879053897486576154[303] = 0;
   out_2879053897486576154[304] = 1;
   out_2879053897486576154[305] = 0;
   out_2879053897486576154[306] = 0;
   out_2879053897486576154[307] = 0;
   out_2879053897486576154[308] = 0;
   out_2879053897486576154[309] = 0;
   out_2879053897486576154[310] = 0;
   out_2879053897486576154[311] = 0;
   out_2879053897486576154[312] = 0;
   out_2879053897486576154[313] = 0;
   out_2879053897486576154[314] = 0;
   out_2879053897486576154[315] = 0;
   out_2879053897486576154[316] = 0;
   out_2879053897486576154[317] = 0;
   out_2879053897486576154[318] = 0;
   out_2879053897486576154[319] = 0;
   out_2879053897486576154[320] = 0;
   out_2879053897486576154[321] = 0;
   out_2879053897486576154[322] = 0;
   out_2879053897486576154[323] = 1;
}
void h_4(double *state, double *unused, double *out_7225039158807375856) {
   out_7225039158807375856[0] = state[6] + state[9];
   out_7225039158807375856[1] = state[7] + state[10];
   out_7225039158807375856[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_7801001311829000706) {
   out_7801001311829000706[0] = 0;
   out_7801001311829000706[1] = 0;
   out_7801001311829000706[2] = 0;
   out_7801001311829000706[3] = 0;
   out_7801001311829000706[4] = 0;
   out_7801001311829000706[5] = 0;
   out_7801001311829000706[6] = 1;
   out_7801001311829000706[7] = 0;
   out_7801001311829000706[8] = 0;
   out_7801001311829000706[9] = 1;
   out_7801001311829000706[10] = 0;
   out_7801001311829000706[11] = 0;
   out_7801001311829000706[12] = 0;
   out_7801001311829000706[13] = 0;
   out_7801001311829000706[14] = 0;
   out_7801001311829000706[15] = 0;
   out_7801001311829000706[16] = 0;
   out_7801001311829000706[17] = 0;
   out_7801001311829000706[18] = 0;
   out_7801001311829000706[19] = 0;
   out_7801001311829000706[20] = 0;
   out_7801001311829000706[21] = 0;
   out_7801001311829000706[22] = 0;
   out_7801001311829000706[23] = 0;
   out_7801001311829000706[24] = 0;
   out_7801001311829000706[25] = 1;
   out_7801001311829000706[26] = 0;
   out_7801001311829000706[27] = 0;
   out_7801001311829000706[28] = 1;
   out_7801001311829000706[29] = 0;
   out_7801001311829000706[30] = 0;
   out_7801001311829000706[31] = 0;
   out_7801001311829000706[32] = 0;
   out_7801001311829000706[33] = 0;
   out_7801001311829000706[34] = 0;
   out_7801001311829000706[35] = 0;
   out_7801001311829000706[36] = 0;
   out_7801001311829000706[37] = 0;
   out_7801001311829000706[38] = 0;
   out_7801001311829000706[39] = 0;
   out_7801001311829000706[40] = 0;
   out_7801001311829000706[41] = 0;
   out_7801001311829000706[42] = 0;
   out_7801001311829000706[43] = 0;
   out_7801001311829000706[44] = 1;
   out_7801001311829000706[45] = 0;
   out_7801001311829000706[46] = 0;
   out_7801001311829000706[47] = 1;
   out_7801001311829000706[48] = 0;
   out_7801001311829000706[49] = 0;
   out_7801001311829000706[50] = 0;
   out_7801001311829000706[51] = 0;
   out_7801001311829000706[52] = 0;
   out_7801001311829000706[53] = 0;
}
void h_10(double *state, double *unused, double *out_1527157142141944609) {
   out_1527157142141944609[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_1527157142141944609[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_1527157142141944609[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_8499975729958205534) {
   out_8499975729958205534[0] = 0;
   out_8499975729958205534[1] = 9.8100000000000005*cos(state[1]);
   out_8499975729958205534[2] = 0;
   out_8499975729958205534[3] = 0;
   out_8499975729958205534[4] = -state[8];
   out_8499975729958205534[5] = state[7];
   out_8499975729958205534[6] = 0;
   out_8499975729958205534[7] = state[5];
   out_8499975729958205534[8] = -state[4];
   out_8499975729958205534[9] = 0;
   out_8499975729958205534[10] = 0;
   out_8499975729958205534[11] = 0;
   out_8499975729958205534[12] = 1;
   out_8499975729958205534[13] = 0;
   out_8499975729958205534[14] = 0;
   out_8499975729958205534[15] = 1;
   out_8499975729958205534[16] = 0;
   out_8499975729958205534[17] = 0;
   out_8499975729958205534[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_8499975729958205534[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_8499975729958205534[20] = 0;
   out_8499975729958205534[21] = state[8];
   out_8499975729958205534[22] = 0;
   out_8499975729958205534[23] = -state[6];
   out_8499975729958205534[24] = -state[5];
   out_8499975729958205534[25] = 0;
   out_8499975729958205534[26] = state[3];
   out_8499975729958205534[27] = 0;
   out_8499975729958205534[28] = 0;
   out_8499975729958205534[29] = 0;
   out_8499975729958205534[30] = 0;
   out_8499975729958205534[31] = 1;
   out_8499975729958205534[32] = 0;
   out_8499975729958205534[33] = 0;
   out_8499975729958205534[34] = 1;
   out_8499975729958205534[35] = 0;
   out_8499975729958205534[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_8499975729958205534[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_8499975729958205534[38] = 0;
   out_8499975729958205534[39] = -state[7];
   out_8499975729958205534[40] = state[6];
   out_8499975729958205534[41] = 0;
   out_8499975729958205534[42] = state[4];
   out_8499975729958205534[43] = -state[3];
   out_8499975729958205534[44] = 0;
   out_8499975729958205534[45] = 0;
   out_8499975729958205534[46] = 0;
   out_8499975729958205534[47] = 0;
   out_8499975729958205534[48] = 0;
   out_8499975729958205534[49] = 0;
   out_8499975729958205534[50] = 1;
   out_8499975729958205534[51] = 0;
   out_8499975729958205534[52] = 0;
   out_8499975729958205534[53] = 1;
}
void h_13(double *state, double *unused, double *out_4139399010471460228) {
   out_4139399010471460228[0] = state[3];
   out_4139399010471460228[1] = state[4];
   out_4139399010471460228[2] = state[5];
}
void H_13(double *state, double *unused, double *out_4588727486496667905) {
   out_4588727486496667905[0] = 0;
   out_4588727486496667905[1] = 0;
   out_4588727486496667905[2] = 0;
   out_4588727486496667905[3] = 1;
   out_4588727486496667905[4] = 0;
   out_4588727486496667905[5] = 0;
   out_4588727486496667905[6] = 0;
   out_4588727486496667905[7] = 0;
   out_4588727486496667905[8] = 0;
   out_4588727486496667905[9] = 0;
   out_4588727486496667905[10] = 0;
   out_4588727486496667905[11] = 0;
   out_4588727486496667905[12] = 0;
   out_4588727486496667905[13] = 0;
   out_4588727486496667905[14] = 0;
   out_4588727486496667905[15] = 0;
   out_4588727486496667905[16] = 0;
   out_4588727486496667905[17] = 0;
   out_4588727486496667905[18] = 0;
   out_4588727486496667905[19] = 0;
   out_4588727486496667905[20] = 0;
   out_4588727486496667905[21] = 0;
   out_4588727486496667905[22] = 1;
   out_4588727486496667905[23] = 0;
   out_4588727486496667905[24] = 0;
   out_4588727486496667905[25] = 0;
   out_4588727486496667905[26] = 0;
   out_4588727486496667905[27] = 0;
   out_4588727486496667905[28] = 0;
   out_4588727486496667905[29] = 0;
   out_4588727486496667905[30] = 0;
   out_4588727486496667905[31] = 0;
   out_4588727486496667905[32] = 0;
   out_4588727486496667905[33] = 0;
   out_4588727486496667905[34] = 0;
   out_4588727486496667905[35] = 0;
   out_4588727486496667905[36] = 0;
   out_4588727486496667905[37] = 0;
   out_4588727486496667905[38] = 0;
   out_4588727486496667905[39] = 0;
   out_4588727486496667905[40] = 0;
   out_4588727486496667905[41] = 1;
   out_4588727486496667905[42] = 0;
   out_4588727486496667905[43] = 0;
   out_4588727486496667905[44] = 0;
   out_4588727486496667905[45] = 0;
   out_4588727486496667905[46] = 0;
   out_4588727486496667905[47] = 0;
   out_4588727486496667905[48] = 0;
   out_4588727486496667905[49] = 0;
   out_4588727486496667905[50] = 0;
   out_4588727486496667905[51] = 0;
   out_4588727486496667905[52] = 0;
   out_4588727486496667905[53] = 0;
}
void h_14(double *state, double *unused, double *out_6562515905070331452) {
   out_6562515905070331452[0] = state[6];
   out_6562515905070331452[1] = state[7];
   out_6562515905070331452[2] = state[8];
}
void H_14(double *state, double *unused, double *out_3837760455489516177) {
   out_3837760455489516177[0] = 0;
   out_3837760455489516177[1] = 0;
   out_3837760455489516177[2] = 0;
   out_3837760455489516177[3] = 0;
   out_3837760455489516177[4] = 0;
   out_3837760455489516177[5] = 0;
   out_3837760455489516177[6] = 1;
   out_3837760455489516177[7] = 0;
   out_3837760455489516177[8] = 0;
   out_3837760455489516177[9] = 0;
   out_3837760455489516177[10] = 0;
   out_3837760455489516177[11] = 0;
   out_3837760455489516177[12] = 0;
   out_3837760455489516177[13] = 0;
   out_3837760455489516177[14] = 0;
   out_3837760455489516177[15] = 0;
   out_3837760455489516177[16] = 0;
   out_3837760455489516177[17] = 0;
   out_3837760455489516177[18] = 0;
   out_3837760455489516177[19] = 0;
   out_3837760455489516177[20] = 0;
   out_3837760455489516177[21] = 0;
   out_3837760455489516177[22] = 0;
   out_3837760455489516177[23] = 0;
   out_3837760455489516177[24] = 0;
   out_3837760455489516177[25] = 1;
   out_3837760455489516177[26] = 0;
   out_3837760455489516177[27] = 0;
   out_3837760455489516177[28] = 0;
   out_3837760455489516177[29] = 0;
   out_3837760455489516177[30] = 0;
   out_3837760455489516177[31] = 0;
   out_3837760455489516177[32] = 0;
   out_3837760455489516177[33] = 0;
   out_3837760455489516177[34] = 0;
   out_3837760455489516177[35] = 0;
   out_3837760455489516177[36] = 0;
   out_3837760455489516177[37] = 0;
   out_3837760455489516177[38] = 0;
   out_3837760455489516177[39] = 0;
   out_3837760455489516177[40] = 0;
   out_3837760455489516177[41] = 0;
   out_3837760455489516177[42] = 0;
   out_3837760455489516177[43] = 0;
   out_3837760455489516177[44] = 1;
   out_3837760455489516177[45] = 0;
   out_3837760455489516177[46] = 0;
   out_3837760455489516177[47] = 0;
   out_3837760455489516177[48] = 0;
   out_3837760455489516177[49] = 0;
   out_3837760455489516177[50] = 0;
   out_3837760455489516177[51] = 0;
   out_3837760455489516177[52] = 0;
   out_3837760455489516177[53] = 0;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_4, H_4, NULL, in_z, in_R, in_ea, MAHA_THRESH_4);
}
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_10, H_10, NULL, in_z, in_R, in_ea, MAHA_THRESH_10);
}
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_13, H_13, NULL, in_z, in_R, in_ea, MAHA_THRESH_13);
}
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_14, H_14, NULL, in_z, in_R, in_ea, MAHA_THRESH_14);
}
void pose_err_fun(double *nom_x, double *delta_x, double *out_5537634756637633787) {
  err_fun(nom_x, delta_x, out_5537634756637633787);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_6641296844440448094) {
  inv_err_fun(nom_x, true_x, out_6641296844440448094);
}
void pose_H_mod_fun(double *state, double *out_1952919505077041985) {
  H_mod_fun(state, out_1952919505077041985);
}
void pose_f_fun(double *state, double dt, double *out_5673854303566172016) {
  f_fun(state,  dt, out_5673854303566172016);
}
void pose_F_fun(double *state, double dt, double *out_2879053897486576154) {
  F_fun(state,  dt, out_2879053897486576154);
}
void pose_h_4(double *state, double *unused, double *out_7225039158807375856) {
  h_4(state, unused, out_7225039158807375856);
}
void pose_H_4(double *state, double *unused, double *out_7801001311829000706) {
  H_4(state, unused, out_7801001311829000706);
}
void pose_h_10(double *state, double *unused, double *out_1527157142141944609) {
  h_10(state, unused, out_1527157142141944609);
}
void pose_H_10(double *state, double *unused, double *out_8499975729958205534) {
  H_10(state, unused, out_8499975729958205534);
}
void pose_h_13(double *state, double *unused, double *out_4139399010471460228) {
  h_13(state, unused, out_4139399010471460228);
}
void pose_H_13(double *state, double *unused, double *out_4588727486496667905) {
  H_13(state, unused, out_4588727486496667905);
}
void pose_h_14(double *state, double *unused, double *out_6562515905070331452) {
  h_14(state, unused, out_6562515905070331452);
}
void pose_H_14(double *state, double *unused, double *out_3837760455489516177) {
  H_14(state, unused, out_3837760455489516177);
}
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
}

const EKF pose = {
  .name = "pose",
  .kinds = { 4, 10, 13, 14 },
  .feature_kinds = {  },
  .f_fun = pose_f_fun,
  .F_fun = pose_F_fun,
  .err_fun = pose_err_fun,
  .inv_err_fun = pose_inv_err_fun,
  .H_mod_fun = pose_H_mod_fun,
  .predict = pose_predict,
  .hs = {
    { 4, pose_h_4 },
    { 10, pose_h_10 },
    { 13, pose_h_13 },
    { 14, pose_h_14 },
  },
  .Hs = {
    { 4, pose_H_4 },
    { 10, pose_H_10 },
    { 13, pose_H_13 },
    { 14, pose_H_14 },
  },
  .updates = {
    { 4, pose_update_4 },
    { 10, pose_update_10 },
    { 13, pose_update_13 },
    { 14, pose_update_14 },
  },
  .Hes = {
  },
  .sets = {
  },
  .extra_routines = {
  },
};

ekf_lib_init(pose)
