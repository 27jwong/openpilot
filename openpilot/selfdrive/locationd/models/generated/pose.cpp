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
void err_fun(double *nom_x, double *delta_x, double *out_3388086590141056177) {
   out_3388086590141056177[0] = delta_x[0] + nom_x[0];
   out_3388086590141056177[1] = delta_x[1] + nom_x[1];
   out_3388086590141056177[2] = delta_x[2] + nom_x[2];
   out_3388086590141056177[3] = delta_x[3] + nom_x[3];
   out_3388086590141056177[4] = delta_x[4] + nom_x[4];
   out_3388086590141056177[5] = delta_x[5] + nom_x[5];
   out_3388086590141056177[6] = delta_x[6] + nom_x[6];
   out_3388086590141056177[7] = delta_x[7] + nom_x[7];
   out_3388086590141056177[8] = delta_x[8] + nom_x[8];
   out_3388086590141056177[9] = delta_x[9] + nom_x[9];
   out_3388086590141056177[10] = delta_x[10] + nom_x[10];
   out_3388086590141056177[11] = delta_x[11] + nom_x[11];
   out_3388086590141056177[12] = delta_x[12] + nom_x[12];
   out_3388086590141056177[13] = delta_x[13] + nom_x[13];
   out_3388086590141056177[14] = delta_x[14] + nom_x[14];
   out_3388086590141056177[15] = delta_x[15] + nom_x[15];
   out_3388086590141056177[16] = delta_x[16] + nom_x[16];
   out_3388086590141056177[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_1633734341039361389) {
   out_1633734341039361389[0] = -nom_x[0] + true_x[0];
   out_1633734341039361389[1] = -nom_x[1] + true_x[1];
   out_1633734341039361389[2] = -nom_x[2] + true_x[2];
   out_1633734341039361389[3] = -nom_x[3] + true_x[3];
   out_1633734341039361389[4] = -nom_x[4] + true_x[4];
   out_1633734341039361389[5] = -nom_x[5] + true_x[5];
   out_1633734341039361389[6] = -nom_x[6] + true_x[6];
   out_1633734341039361389[7] = -nom_x[7] + true_x[7];
   out_1633734341039361389[8] = -nom_x[8] + true_x[8];
   out_1633734341039361389[9] = -nom_x[9] + true_x[9];
   out_1633734341039361389[10] = -nom_x[10] + true_x[10];
   out_1633734341039361389[11] = -nom_x[11] + true_x[11];
   out_1633734341039361389[12] = -nom_x[12] + true_x[12];
   out_1633734341039361389[13] = -nom_x[13] + true_x[13];
   out_1633734341039361389[14] = -nom_x[14] + true_x[14];
   out_1633734341039361389[15] = -nom_x[15] + true_x[15];
   out_1633734341039361389[16] = -nom_x[16] + true_x[16];
   out_1633734341039361389[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_3284972142892485625) {
   out_3284972142892485625[0] = 1.0;
   out_3284972142892485625[1] = 0.0;
   out_3284972142892485625[2] = 0.0;
   out_3284972142892485625[3] = 0.0;
   out_3284972142892485625[4] = 0.0;
   out_3284972142892485625[5] = 0.0;
   out_3284972142892485625[6] = 0.0;
   out_3284972142892485625[7] = 0.0;
   out_3284972142892485625[8] = 0.0;
   out_3284972142892485625[9] = 0.0;
   out_3284972142892485625[10] = 0.0;
   out_3284972142892485625[11] = 0.0;
   out_3284972142892485625[12] = 0.0;
   out_3284972142892485625[13] = 0.0;
   out_3284972142892485625[14] = 0.0;
   out_3284972142892485625[15] = 0.0;
   out_3284972142892485625[16] = 0.0;
   out_3284972142892485625[17] = 0.0;
   out_3284972142892485625[18] = 0.0;
   out_3284972142892485625[19] = 1.0;
   out_3284972142892485625[20] = 0.0;
   out_3284972142892485625[21] = 0.0;
   out_3284972142892485625[22] = 0.0;
   out_3284972142892485625[23] = 0.0;
   out_3284972142892485625[24] = 0.0;
   out_3284972142892485625[25] = 0.0;
   out_3284972142892485625[26] = 0.0;
   out_3284972142892485625[27] = 0.0;
   out_3284972142892485625[28] = 0.0;
   out_3284972142892485625[29] = 0.0;
   out_3284972142892485625[30] = 0.0;
   out_3284972142892485625[31] = 0.0;
   out_3284972142892485625[32] = 0.0;
   out_3284972142892485625[33] = 0.0;
   out_3284972142892485625[34] = 0.0;
   out_3284972142892485625[35] = 0.0;
   out_3284972142892485625[36] = 0.0;
   out_3284972142892485625[37] = 0.0;
   out_3284972142892485625[38] = 1.0;
   out_3284972142892485625[39] = 0.0;
   out_3284972142892485625[40] = 0.0;
   out_3284972142892485625[41] = 0.0;
   out_3284972142892485625[42] = 0.0;
   out_3284972142892485625[43] = 0.0;
   out_3284972142892485625[44] = 0.0;
   out_3284972142892485625[45] = 0.0;
   out_3284972142892485625[46] = 0.0;
   out_3284972142892485625[47] = 0.0;
   out_3284972142892485625[48] = 0.0;
   out_3284972142892485625[49] = 0.0;
   out_3284972142892485625[50] = 0.0;
   out_3284972142892485625[51] = 0.0;
   out_3284972142892485625[52] = 0.0;
   out_3284972142892485625[53] = 0.0;
   out_3284972142892485625[54] = 0.0;
   out_3284972142892485625[55] = 0.0;
   out_3284972142892485625[56] = 0.0;
   out_3284972142892485625[57] = 1.0;
   out_3284972142892485625[58] = 0.0;
   out_3284972142892485625[59] = 0.0;
   out_3284972142892485625[60] = 0.0;
   out_3284972142892485625[61] = 0.0;
   out_3284972142892485625[62] = 0.0;
   out_3284972142892485625[63] = 0.0;
   out_3284972142892485625[64] = 0.0;
   out_3284972142892485625[65] = 0.0;
   out_3284972142892485625[66] = 0.0;
   out_3284972142892485625[67] = 0.0;
   out_3284972142892485625[68] = 0.0;
   out_3284972142892485625[69] = 0.0;
   out_3284972142892485625[70] = 0.0;
   out_3284972142892485625[71] = 0.0;
   out_3284972142892485625[72] = 0.0;
   out_3284972142892485625[73] = 0.0;
   out_3284972142892485625[74] = 0.0;
   out_3284972142892485625[75] = 0.0;
   out_3284972142892485625[76] = 1.0;
   out_3284972142892485625[77] = 0.0;
   out_3284972142892485625[78] = 0.0;
   out_3284972142892485625[79] = 0.0;
   out_3284972142892485625[80] = 0.0;
   out_3284972142892485625[81] = 0.0;
   out_3284972142892485625[82] = 0.0;
   out_3284972142892485625[83] = 0.0;
   out_3284972142892485625[84] = 0.0;
   out_3284972142892485625[85] = 0.0;
   out_3284972142892485625[86] = 0.0;
   out_3284972142892485625[87] = 0.0;
   out_3284972142892485625[88] = 0.0;
   out_3284972142892485625[89] = 0.0;
   out_3284972142892485625[90] = 0.0;
   out_3284972142892485625[91] = 0.0;
   out_3284972142892485625[92] = 0.0;
   out_3284972142892485625[93] = 0.0;
   out_3284972142892485625[94] = 0.0;
   out_3284972142892485625[95] = 1.0;
   out_3284972142892485625[96] = 0.0;
   out_3284972142892485625[97] = 0.0;
   out_3284972142892485625[98] = 0.0;
   out_3284972142892485625[99] = 0.0;
   out_3284972142892485625[100] = 0.0;
   out_3284972142892485625[101] = 0.0;
   out_3284972142892485625[102] = 0.0;
   out_3284972142892485625[103] = 0.0;
   out_3284972142892485625[104] = 0.0;
   out_3284972142892485625[105] = 0.0;
   out_3284972142892485625[106] = 0.0;
   out_3284972142892485625[107] = 0.0;
   out_3284972142892485625[108] = 0.0;
   out_3284972142892485625[109] = 0.0;
   out_3284972142892485625[110] = 0.0;
   out_3284972142892485625[111] = 0.0;
   out_3284972142892485625[112] = 0.0;
   out_3284972142892485625[113] = 0.0;
   out_3284972142892485625[114] = 1.0;
   out_3284972142892485625[115] = 0.0;
   out_3284972142892485625[116] = 0.0;
   out_3284972142892485625[117] = 0.0;
   out_3284972142892485625[118] = 0.0;
   out_3284972142892485625[119] = 0.0;
   out_3284972142892485625[120] = 0.0;
   out_3284972142892485625[121] = 0.0;
   out_3284972142892485625[122] = 0.0;
   out_3284972142892485625[123] = 0.0;
   out_3284972142892485625[124] = 0.0;
   out_3284972142892485625[125] = 0.0;
   out_3284972142892485625[126] = 0.0;
   out_3284972142892485625[127] = 0.0;
   out_3284972142892485625[128] = 0.0;
   out_3284972142892485625[129] = 0.0;
   out_3284972142892485625[130] = 0.0;
   out_3284972142892485625[131] = 0.0;
   out_3284972142892485625[132] = 0.0;
   out_3284972142892485625[133] = 1.0;
   out_3284972142892485625[134] = 0.0;
   out_3284972142892485625[135] = 0.0;
   out_3284972142892485625[136] = 0.0;
   out_3284972142892485625[137] = 0.0;
   out_3284972142892485625[138] = 0.0;
   out_3284972142892485625[139] = 0.0;
   out_3284972142892485625[140] = 0.0;
   out_3284972142892485625[141] = 0.0;
   out_3284972142892485625[142] = 0.0;
   out_3284972142892485625[143] = 0.0;
   out_3284972142892485625[144] = 0.0;
   out_3284972142892485625[145] = 0.0;
   out_3284972142892485625[146] = 0.0;
   out_3284972142892485625[147] = 0.0;
   out_3284972142892485625[148] = 0.0;
   out_3284972142892485625[149] = 0.0;
   out_3284972142892485625[150] = 0.0;
   out_3284972142892485625[151] = 0.0;
   out_3284972142892485625[152] = 1.0;
   out_3284972142892485625[153] = 0.0;
   out_3284972142892485625[154] = 0.0;
   out_3284972142892485625[155] = 0.0;
   out_3284972142892485625[156] = 0.0;
   out_3284972142892485625[157] = 0.0;
   out_3284972142892485625[158] = 0.0;
   out_3284972142892485625[159] = 0.0;
   out_3284972142892485625[160] = 0.0;
   out_3284972142892485625[161] = 0.0;
   out_3284972142892485625[162] = 0.0;
   out_3284972142892485625[163] = 0.0;
   out_3284972142892485625[164] = 0.0;
   out_3284972142892485625[165] = 0.0;
   out_3284972142892485625[166] = 0.0;
   out_3284972142892485625[167] = 0.0;
   out_3284972142892485625[168] = 0.0;
   out_3284972142892485625[169] = 0.0;
   out_3284972142892485625[170] = 0.0;
   out_3284972142892485625[171] = 1.0;
   out_3284972142892485625[172] = 0.0;
   out_3284972142892485625[173] = 0.0;
   out_3284972142892485625[174] = 0.0;
   out_3284972142892485625[175] = 0.0;
   out_3284972142892485625[176] = 0.0;
   out_3284972142892485625[177] = 0.0;
   out_3284972142892485625[178] = 0.0;
   out_3284972142892485625[179] = 0.0;
   out_3284972142892485625[180] = 0.0;
   out_3284972142892485625[181] = 0.0;
   out_3284972142892485625[182] = 0.0;
   out_3284972142892485625[183] = 0.0;
   out_3284972142892485625[184] = 0.0;
   out_3284972142892485625[185] = 0.0;
   out_3284972142892485625[186] = 0.0;
   out_3284972142892485625[187] = 0.0;
   out_3284972142892485625[188] = 0.0;
   out_3284972142892485625[189] = 0.0;
   out_3284972142892485625[190] = 1.0;
   out_3284972142892485625[191] = 0.0;
   out_3284972142892485625[192] = 0.0;
   out_3284972142892485625[193] = 0.0;
   out_3284972142892485625[194] = 0.0;
   out_3284972142892485625[195] = 0.0;
   out_3284972142892485625[196] = 0.0;
   out_3284972142892485625[197] = 0.0;
   out_3284972142892485625[198] = 0.0;
   out_3284972142892485625[199] = 0.0;
   out_3284972142892485625[200] = 0.0;
   out_3284972142892485625[201] = 0.0;
   out_3284972142892485625[202] = 0.0;
   out_3284972142892485625[203] = 0.0;
   out_3284972142892485625[204] = 0.0;
   out_3284972142892485625[205] = 0.0;
   out_3284972142892485625[206] = 0.0;
   out_3284972142892485625[207] = 0.0;
   out_3284972142892485625[208] = 0.0;
   out_3284972142892485625[209] = 1.0;
   out_3284972142892485625[210] = 0.0;
   out_3284972142892485625[211] = 0.0;
   out_3284972142892485625[212] = 0.0;
   out_3284972142892485625[213] = 0.0;
   out_3284972142892485625[214] = 0.0;
   out_3284972142892485625[215] = 0.0;
   out_3284972142892485625[216] = 0.0;
   out_3284972142892485625[217] = 0.0;
   out_3284972142892485625[218] = 0.0;
   out_3284972142892485625[219] = 0.0;
   out_3284972142892485625[220] = 0.0;
   out_3284972142892485625[221] = 0.0;
   out_3284972142892485625[222] = 0.0;
   out_3284972142892485625[223] = 0.0;
   out_3284972142892485625[224] = 0.0;
   out_3284972142892485625[225] = 0.0;
   out_3284972142892485625[226] = 0.0;
   out_3284972142892485625[227] = 0.0;
   out_3284972142892485625[228] = 1.0;
   out_3284972142892485625[229] = 0.0;
   out_3284972142892485625[230] = 0.0;
   out_3284972142892485625[231] = 0.0;
   out_3284972142892485625[232] = 0.0;
   out_3284972142892485625[233] = 0.0;
   out_3284972142892485625[234] = 0.0;
   out_3284972142892485625[235] = 0.0;
   out_3284972142892485625[236] = 0.0;
   out_3284972142892485625[237] = 0.0;
   out_3284972142892485625[238] = 0.0;
   out_3284972142892485625[239] = 0.0;
   out_3284972142892485625[240] = 0.0;
   out_3284972142892485625[241] = 0.0;
   out_3284972142892485625[242] = 0.0;
   out_3284972142892485625[243] = 0.0;
   out_3284972142892485625[244] = 0.0;
   out_3284972142892485625[245] = 0.0;
   out_3284972142892485625[246] = 0.0;
   out_3284972142892485625[247] = 1.0;
   out_3284972142892485625[248] = 0.0;
   out_3284972142892485625[249] = 0.0;
   out_3284972142892485625[250] = 0.0;
   out_3284972142892485625[251] = 0.0;
   out_3284972142892485625[252] = 0.0;
   out_3284972142892485625[253] = 0.0;
   out_3284972142892485625[254] = 0.0;
   out_3284972142892485625[255] = 0.0;
   out_3284972142892485625[256] = 0.0;
   out_3284972142892485625[257] = 0.0;
   out_3284972142892485625[258] = 0.0;
   out_3284972142892485625[259] = 0.0;
   out_3284972142892485625[260] = 0.0;
   out_3284972142892485625[261] = 0.0;
   out_3284972142892485625[262] = 0.0;
   out_3284972142892485625[263] = 0.0;
   out_3284972142892485625[264] = 0.0;
   out_3284972142892485625[265] = 0.0;
   out_3284972142892485625[266] = 1.0;
   out_3284972142892485625[267] = 0.0;
   out_3284972142892485625[268] = 0.0;
   out_3284972142892485625[269] = 0.0;
   out_3284972142892485625[270] = 0.0;
   out_3284972142892485625[271] = 0.0;
   out_3284972142892485625[272] = 0.0;
   out_3284972142892485625[273] = 0.0;
   out_3284972142892485625[274] = 0.0;
   out_3284972142892485625[275] = 0.0;
   out_3284972142892485625[276] = 0.0;
   out_3284972142892485625[277] = 0.0;
   out_3284972142892485625[278] = 0.0;
   out_3284972142892485625[279] = 0.0;
   out_3284972142892485625[280] = 0.0;
   out_3284972142892485625[281] = 0.0;
   out_3284972142892485625[282] = 0.0;
   out_3284972142892485625[283] = 0.0;
   out_3284972142892485625[284] = 0.0;
   out_3284972142892485625[285] = 1.0;
   out_3284972142892485625[286] = 0.0;
   out_3284972142892485625[287] = 0.0;
   out_3284972142892485625[288] = 0.0;
   out_3284972142892485625[289] = 0.0;
   out_3284972142892485625[290] = 0.0;
   out_3284972142892485625[291] = 0.0;
   out_3284972142892485625[292] = 0.0;
   out_3284972142892485625[293] = 0.0;
   out_3284972142892485625[294] = 0.0;
   out_3284972142892485625[295] = 0.0;
   out_3284972142892485625[296] = 0.0;
   out_3284972142892485625[297] = 0.0;
   out_3284972142892485625[298] = 0.0;
   out_3284972142892485625[299] = 0.0;
   out_3284972142892485625[300] = 0.0;
   out_3284972142892485625[301] = 0.0;
   out_3284972142892485625[302] = 0.0;
   out_3284972142892485625[303] = 0.0;
   out_3284972142892485625[304] = 1.0;
   out_3284972142892485625[305] = 0.0;
   out_3284972142892485625[306] = 0.0;
   out_3284972142892485625[307] = 0.0;
   out_3284972142892485625[308] = 0.0;
   out_3284972142892485625[309] = 0.0;
   out_3284972142892485625[310] = 0.0;
   out_3284972142892485625[311] = 0.0;
   out_3284972142892485625[312] = 0.0;
   out_3284972142892485625[313] = 0.0;
   out_3284972142892485625[314] = 0.0;
   out_3284972142892485625[315] = 0.0;
   out_3284972142892485625[316] = 0.0;
   out_3284972142892485625[317] = 0.0;
   out_3284972142892485625[318] = 0.0;
   out_3284972142892485625[319] = 0.0;
   out_3284972142892485625[320] = 0.0;
   out_3284972142892485625[321] = 0.0;
   out_3284972142892485625[322] = 0.0;
   out_3284972142892485625[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_7294513523227393093) {
   out_7294513523227393093[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_7294513523227393093[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_7294513523227393093[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_7294513523227393093[3] = dt*state[12] + state[3];
   out_7294513523227393093[4] = dt*state[13] + state[4];
   out_7294513523227393093[5] = dt*state[14] + state[5];
   out_7294513523227393093[6] = state[6];
   out_7294513523227393093[7] = state[7];
   out_7294513523227393093[8] = state[8];
   out_7294513523227393093[9] = state[9];
   out_7294513523227393093[10] = state[10];
   out_7294513523227393093[11] = state[11];
   out_7294513523227393093[12] = state[12];
   out_7294513523227393093[13] = state[13];
   out_7294513523227393093[14] = state[14];
   out_7294513523227393093[15] = state[15];
   out_7294513523227393093[16] = state[16];
   out_7294513523227393093[17] = state[17];
}
void F_fun(double *state, double dt, double *out_1315688866139806167) {
   out_1315688866139806167[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_1315688866139806167[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_1315688866139806167[2] = 0;
   out_1315688866139806167[3] = 0;
   out_1315688866139806167[4] = 0;
   out_1315688866139806167[5] = 0;
   out_1315688866139806167[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_1315688866139806167[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_1315688866139806167[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_1315688866139806167[9] = 0;
   out_1315688866139806167[10] = 0;
   out_1315688866139806167[11] = 0;
   out_1315688866139806167[12] = 0;
   out_1315688866139806167[13] = 0;
   out_1315688866139806167[14] = 0;
   out_1315688866139806167[15] = 0;
   out_1315688866139806167[16] = 0;
   out_1315688866139806167[17] = 0;
   out_1315688866139806167[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_1315688866139806167[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_1315688866139806167[20] = 0;
   out_1315688866139806167[21] = 0;
   out_1315688866139806167[22] = 0;
   out_1315688866139806167[23] = 0;
   out_1315688866139806167[24] = 0;
   out_1315688866139806167[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_1315688866139806167[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_1315688866139806167[27] = 0;
   out_1315688866139806167[28] = 0;
   out_1315688866139806167[29] = 0;
   out_1315688866139806167[30] = 0;
   out_1315688866139806167[31] = 0;
   out_1315688866139806167[32] = 0;
   out_1315688866139806167[33] = 0;
   out_1315688866139806167[34] = 0;
   out_1315688866139806167[35] = 0;
   out_1315688866139806167[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_1315688866139806167[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_1315688866139806167[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_1315688866139806167[39] = 0;
   out_1315688866139806167[40] = 0;
   out_1315688866139806167[41] = 0;
   out_1315688866139806167[42] = 0;
   out_1315688866139806167[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_1315688866139806167[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_1315688866139806167[45] = 0;
   out_1315688866139806167[46] = 0;
   out_1315688866139806167[47] = 0;
   out_1315688866139806167[48] = 0;
   out_1315688866139806167[49] = 0;
   out_1315688866139806167[50] = 0;
   out_1315688866139806167[51] = 0;
   out_1315688866139806167[52] = 0;
   out_1315688866139806167[53] = 0;
   out_1315688866139806167[54] = 0;
   out_1315688866139806167[55] = 0;
   out_1315688866139806167[56] = 0;
   out_1315688866139806167[57] = 1;
   out_1315688866139806167[58] = 0;
   out_1315688866139806167[59] = 0;
   out_1315688866139806167[60] = 0;
   out_1315688866139806167[61] = 0;
   out_1315688866139806167[62] = 0;
   out_1315688866139806167[63] = 0;
   out_1315688866139806167[64] = 0;
   out_1315688866139806167[65] = 0;
   out_1315688866139806167[66] = dt;
   out_1315688866139806167[67] = 0;
   out_1315688866139806167[68] = 0;
   out_1315688866139806167[69] = 0;
   out_1315688866139806167[70] = 0;
   out_1315688866139806167[71] = 0;
   out_1315688866139806167[72] = 0;
   out_1315688866139806167[73] = 0;
   out_1315688866139806167[74] = 0;
   out_1315688866139806167[75] = 0;
   out_1315688866139806167[76] = 1;
   out_1315688866139806167[77] = 0;
   out_1315688866139806167[78] = 0;
   out_1315688866139806167[79] = 0;
   out_1315688866139806167[80] = 0;
   out_1315688866139806167[81] = 0;
   out_1315688866139806167[82] = 0;
   out_1315688866139806167[83] = 0;
   out_1315688866139806167[84] = 0;
   out_1315688866139806167[85] = dt;
   out_1315688866139806167[86] = 0;
   out_1315688866139806167[87] = 0;
   out_1315688866139806167[88] = 0;
   out_1315688866139806167[89] = 0;
   out_1315688866139806167[90] = 0;
   out_1315688866139806167[91] = 0;
   out_1315688866139806167[92] = 0;
   out_1315688866139806167[93] = 0;
   out_1315688866139806167[94] = 0;
   out_1315688866139806167[95] = 1;
   out_1315688866139806167[96] = 0;
   out_1315688866139806167[97] = 0;
   out_1315688866139806167[98] = 0;
   out_1315688866139806167[99] = 0;
   out_1315688866139806167[100] = 0;
   out_1315688866139806167[101] = 0;
   out_1315688866139806167[102] = 0;
   out_1315688866139806167[103] = 0;
   out_1315688866139806167[104] = dt;
   out_1315688866139806167[105] = 0;
   out_1315688866139806167[106] = 0;
   out_1315688866139806167[107] = 0;
   out_1315688866139806167[108] = 0;
   out_1315688866139806167[109] = 0;
   out_1315688866139806167[110] = 0;
   out_1315688866139806167[111] = 0;
   out_1315688866139806167[112] = 0;
   out_1315688866139806167[113] = 0;
   out_1315688866139806167[114] = 1;
   out_1315688866139806167[115] = 0;
   out_1315688866139806167[116] = 0;
   out_1315688866139806167[117] = 0;
   out_1315688866139806167[118] = 0;
   out_1315688866139806167[119] = 0;
   out_1315688866139806167[120] = 0;
   out_1315688866139806167[121] = 0;
   out_1315688866139806167[122] = 0;
   out_1315688866139806167[123] = 0;
   out_1315688866139806167[124] = 0;
   out_1315688866139806167[125] = 0;
   out_1315688866139806167[126] = 0;
   out_1315688866139806167[127] = 0;
   out_1315688866139806167[128] = 0;
   out_1315688866139806167[129] = 0;
   out_1315688866139806167[130] = 0;
   out_1315688866139806167[131] = 0;
   out_1315688866139806167[132] = 0;
   out_1315688866139806167[133] = 1;
   out_1315688866139806167[134] = 0;
   out_1315688866139806167[135] = 0;
   out_1315688866139806167[136] = 0;
   out_1315688866139806167[137] = 0;
   out_1315688866139806167[138] = 0;
   out_1315688866139806167[139] = 0;
   out_1315688866139806167[140] = 0;
   out_1315688866139806167[141] = 0;
   out_1315688866139806167[142] = 0;
   out_1315688866139806167[143] = 0;
   out_1315688866139806167[144] = 0;
   out_1315688866139806167[145] = 0;
   out_1315688866139806167[146] = 0;
   out_1315688866139806167[147] = 0;
   out_1315688866139806167[148] = 0;
   out_1315688866139806167[149] = 0;
   out_1315688866139806167[150] = 0;
   out_1315688866139806167[151] = 0;
   out_1315688866139806167[152] = 1;
   out_1315688866139806167[153] = 0;
   out_1315688866139806167[154] = 0;
   out_1315688866139806167[155] = 0;
   out_1315688866139806167[156] = 0;
   out_1315688866139806167[157] = 0;
   out_1315688866139806167[158] = 0;
   out_1315688866139806167[159] = 0;
   out_1315688866139806167[160] = 0;
   out_1315688866139806167[161] = 0;
   out_1315688866139806167[162] = 0;
   out_1315688866139806167[163] = 0;
   out_1315688866139806167[164] = 0;
   out_1315688866139806167[165] = 0;
   out_1315688866139806167[166] = 0;
   out_1315688866139806167[167] = 0;
   out_1315688866139806167[168] = 0;
   out_1315688866139806167[169] = 0;
   out_1315688866139806167[170] = 0;
   out_1315688866139806167[171] = 1;
   out_1315688866139806167[172] = 0;
   out_1315688866139806167[173] = 0;
   out_1315688866139806167[174] = 0;
   out_1315688866139806167[175] = 0;
   out_1315688866139806167[176] = 0;
   out_1315688866139806167[177] = 0;
   out_1315688866139806167[178] = 0;
   out_1315688866139806167[179] = 0;
   out_1315688866139806167[180] = 0;
   out_1315688866139806167[181] = 0;
   out_1315688866139806167[182] = 0;
   out_1315688866139806167[183] = 0;
   out_1315688866139806167[184] = 0;
   out_1315688866139806167[185] = 0;
   out_1315688866139806167[186] = 0;
   out_1315688866139806167[187] = 0;
   out_1315688866139806167[188] = 0;
   out_1315688866139806167[189] = 0;
   out_1315688866139806167[190] = 1;
   out_1315688866139806167[191] = 0;
   out_1315688866139806167[192] = 0;
   out_1315688866139806167[193] = 0;
   out_1315688866139806167[194] = 0;
   out_1315688866139806167[195] = 0;
   out_1315688866139806167[196] = 0;
   out_1315688866139806167[197] = 0;
   out_1315688866139806167[198] = 0;
   out_1315688866139806167[199] = 0;
   out_1315688866139806167[200] = 0;
   out_1315688866139806167[201] = 0;
   out_1315688866139806167[202] = 0;
   out_1315688866139806167[203] = 0;
   out_1315688866139806167[204] = 0;
   out_1315688866139806167[205] = 0;
   out_1315688866139806167[206] = 0;
   out_1315688866139806167[207] = 0;
   out_1315688866139806167[208] = 0;
   out_1315688866139806167[209] = 1;
   out_1315688866139806167[210] = 0;
   out_1315688866139806167[211] = 0;
   out_1315688866139806167[212] = 0;
   out_1315688866139806167[213] = 0;
   out_1315688866139806167[214] = 0;
   out_1315688866139806167[215] = 0;
   out_1315688866139806167[216] = 0;
   out_1315688866139806167[217] = 0;
   out_1315688866139806167[218] = 0;
   out_1315688866139806167[219] = 0;
   out_1315688866139806167[220] = 0;
   out_1315688866139806167[221] = 0;
   out_1315688866139806167[222] = 0;
   out_1315688866139806167[223] = 0;
   out_1315688866139806167[224] = 0;
   out_1315688866139806167[225] = 0;
   out_1315688866139806167[226] = 0;
   out_1315688866139806167[227] = 0;
   out_1315688866139806167[228] = 1;
   out_1315688866139806167[229] = 0;
   out_1315688866139806167[230] = 0;
   out_1315688866139806167[231] = 0;
   out_1315688866139806167[232] = 0;
   out_1315688866139806167[233] = 0;
   out_1315688866139806167[234] = 0;
   out_1315688866139806167[235] = 0;
   out_1315688866139806167[236] = 0;
   out_1315688866139806167[237] = 0;
   out_1315688866139806167[238] = 0;
   out_1315688866139806167[239] = 0;
   out_1315688866139806167[240] = 0;
   out_1315688866139806167[241] = 0;
   out_1315688866139806167[242] = 0;
   out_1315688866139806167[243] = 0;
   out_1315688866139806167[244] = 0;
   out_1315688866139806167[245] = 0;
   out_1315688866139806167[246] = 0;
   out_1315688866139806167[247] = 1;
   out_1315688866139806167[248] = 0;
   out_1315688866139806167[249] = 0;
   out_1315688866139806167[250] = 0;
   out_1315688866139806167[251] = 0;
   out_1315688866139806167[252] = 0;
   out_1315688866139806167[253] = 0;
   out_1315688866139806167[254] = 0;
   out_1315688866139806167[255] = 0;
   out_1315688866139806167[256] = 0;
   out_1315688866139806167[257] = 0;
   out_1315688866139806167[258] = 0;
   out_1315688866139806167[259] = 0;
   out_1315688866139806167[260] = 0;
   out_1315688866139806167[261] = 0;
   out_1315688866139806167[262] = 0;
   out_1315688866139806167[263] = 0;
   out_1315688866139806167[264] = 0;
   out_1315688866139806167[265] = 0;
   out_1315688866139806167[266] = 1;
   out_1315688866139806167[267] = 0;
   out_1315688866139806167[268] = 0;
   out_1315688866139806167[269] = 0;
   out_1315688866139806167[270] = 0;
   out_1315688866139806167[271] = 0;
   out_1315688866139806167[272] = 0;
   out_1315688866139806167[273] = 0;
   out_1315688866139806167[274] = 0;
   out_1315688866139806167[275] = 0;
   out_1315688866139806167[276] = 0;
   out_1315688866139806167[277] = 0;
   out_1315688866139806167[278] = 0;
   out_1315688866139806167[279] = 0;
   out_1315688866139806167[280] = 0;
   out_1315688866139806167[281] = 0;
   out_1315688866139806167[282] = 0;
   out_1315688866139806167[283] = 0;
   out_1315688866139806167[284] = 0;
   out_1315688866139806167[285] = 1;
   out_1315688866139806167[286] = 0;
   out_1315688866139806167[287] = 0;
   out_1315688866139806167[288] = 0;
   out_1315688866139806167[289] = 0;
   out_1315688866139806167[290] = 0;
   out_1315688866139806167[291] = 0;
   out_1315688866139806167[292] = 0;
   out_1315688866139806167[293] = 0;
   out_1315688866139806167[294] = 0;
   out_1315688866139806167[295] = 0;
   out_1315688866139806167[296] = 0;
   out_1315688866139806167[297] = 0;
   out_1315688866139806167[298] = 0;
   out_1315688866139806167[299] = 0;
   out_1315688866139806167[300] = 0;
   out_1315688866139806167[301] = 0;
   out_1315688866139806167[302] = 0;
   out_1315688866139806167[303] = 0;
   out_1315688866139806167[304] = 1;
   out_1315688866139806167[305] = 0;
   out_1315688866139806167[306] = 0;
   out_1315688866139806167[307] = 0;
   out_1315688866139806167[308] = 0;
   out_1315688866139806167[309] = 0;
   out_1315688866139806167[310] = 0;
   out_1315688866139806167[311] = 0;
   out_1315688866139806167[312] = 0;
   out_1315688866139806167[313] = 0;
   out_1315688866139806167[314] = 0;
   out_1315688866139806167[315] = 0;
   out_1315688866139806167[316] = 0;
   out_1315688866139806167[317] = 0;
   out_1315688866139806167[318] = 0;
   out_1315688866139806167[319] = 0;
   out_1315688866139806167[320] = 0;
   out_1315688866139806167[321] = 0;
   out_1315688866139806167[322] = 0;
   out_1315688866139806167[323] = 1;
}
void h_4(double *state, double *unused, double *out_6849640214971072753) {
   out_6849640214971072753[0] = state[6] + state[9];
   out_6849640214971072753[1] = state[7] + state[10];
   out_6849640214971072753[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_6217410264759244649) {
   out_6217410264759244649[0] = 0;
   out_6217410264759244649[1] = 0;
   out_6217410264759244649[2] = 0;
   out_6217410264759244649[3] = 0;
   out_6217410264759244649[4] = 0;
   out_6217410264759244649[5] = 0;
   out_6217410264759244649[6] = 1;
   out_6217410264759244649[7] = 0;
   out_6217410264759244649[8] = 0;
   out_6217410264759244649[9] = 1;
   out_6217410264759244649[10] = 0;
   out_6217410264759244649[11] = 0;
   out_6217410264759244649[12] = 0;
   out_6217410264759244649[13] = 0;
   out_6217410264759244649[14] = 0;
   out_6217410264759244649[15] = 0;
   out_6217410264759244649[16] = 0;
   out_6217410264759244649[17] = 0;
   out_6217410264759244649[18] = 0;
   out_6217410264759244649[19] = 0;
   out_6217410264759244649[20] = 0;
   out_6217410264759244649[21] = 0;
   out_6217410264759244649[22] = 0;
   out_6217410264759244649[23] = 0;
   out_6217410264759244649[24] = 0;
   out_6217410264759244649[25] = 1;
   out_6217410264759244649[26] = 0;
   out_6217410264759244649[27] = 0;
   out_6217410264759244649[28] = 1;
   out_6217410264759244649[29] = 0;
   out_6217410264759244649[30] = 0;
   out_6217410264759244649[31] = 0;
   out_6217410264759244649[32] = 0;
   out_6217410264759244649[33] = 0;
   out_6217410264759244649[34] = 0;
   out_6217410264759244649[35] = 0;
   out_6217410264759244649[36] = 0;
   out_6217410264759244649[37] = 0;
   out_6217410264759244649[38] = 0;
   out_6217410264759244649[39] = 0;
   out_6217410264759244649[40] = 0;
   out_6217410264759244649[41] = 0;
   out_6217410264759244649[42] = 0;
   out_6217410264759244649[43] = 0;
   out_6217410264759244649[44] = 1;
   out_6217410264759244649[45] = 0;
   out_6217410264759244649[46] = 0;
   out_6217410264759244649[47] = 1;
   out_6217410264759244649[48] = 0;
   out_6217410264759244649[49] = 0;
   out_6217410264759244649[50] = 0;
   out_6217410264759244649[51] = 0;
   out_6217410264759244649[52] = 0;
   out_6217410264759244649[53] = 0;
}
void h_10(double *state, double *unused, double *out_2538664534589622554) {
   out_2538664534589622554[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_2538664534589622554[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_2538664534589622554[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_5499394725381253526) {
   out_5499394725381253526[0] = 0;
   out_5499394725381253526[1] = 9.8100000000000005*cos(state[1]);
   out_5499394725381253526[2] = 0;
   out_5499394725381253526[3] = 0;
   out_5499394725381253526[4] = -state[8];
   out_5499394725381253526[5] = state[7];
   out_5499394725381253526[6] = 0;
   out_5499394725381253526[7] = state[5];
   out_5499394725381253526[8] = -state[4];
   out_5499394725381253526[9] = 0;
   out_5499394725381253526[10] = 0;
   out_5499394725381253526[11] = 0;
   out_5499394725381253526[12] = 1;
   out_5499394725381253526[13] = 0;
   out_5499394725381253526[14] = 0;
   out_5499394725381253526[15] = 1;
   out_5499394725381253526[16] = 0;
   out_5499394725381253526[17] = 0;
   out_5499394725381253526[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_5499394725381253526[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_5499394725381253526[20] = 0;
   out_5499394725381253526[21] = state[8];
   out_5499394725381253526[22] = 0;
   out_5499394725381253526[23] = -state[6];
   out_5499394725381253526[24] = -state[5];
   out_5499394725381253526[25] = 0;
   out_5499394725381253526[26] = state[3];
   out_5499394725381253526[27] = 0;
   out_5499394725381253526[28] = 0;
   out_5499394725381253526[29] = 0;
   out_5499394725381253526[30] = 0;
   out_5499394725381253526[31] = 1;
   out_5499394725381253526[32] = 0;
   out_5499394725381253526[33] = 0;
   out_5499394725381253526[34] = 1;
   out_5499394725381253526[35] = 0;
   out_5499394725381253526[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_5499394725381253526[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_5499394725381253526[38] = 0;
   out_5499394725381253526[39] = -state[7];
   out_5499394725381253526[40] = state[6];
   out_5499394725381253526[41] = 0;
   out_5499394725381253526[42] = state[4];
   out_5499394725381253526[43] = -state[3];
   out_5499394725381253526[44] = 0;
   out_5499394725381253526[45] = 0;
   out_5499394725381253526[46] = 0;
   out_5499394725381253526[47] = 0;
   out_5499394725381253526[48] = 0;
   out_5499394725381253526[49] = 0;
   out_5499394725381253526[50] = 1;
   out_5499394725381253526[51] = 0;
   out_5499394725381253526[52] = 0;
   out_5499394725381253526[53] = 1;
}
void h_13(double *state, double *unused, double *out_228516151720916839) {
   out_228516151720916839[0] = state[3];
   out_228516151720916839[1] = state[4];
   out_228516151720916839[2] = state[5];
}
void H_13(double *state, double *unused, double *out_9017059983617974166) {
   out_9017059983617974166[0] = 0;
   out_9017059983617974166[1] = 0;
   out_9017059983617974166[2] = 0;
   out_9017059983617974166[3] = 1;
   out_9017059983617974166[4] = 0;
   out_9017059983617974166[5] = 0;
   out_9017059983617974166[6] = 0;
   out_9017059983617974166[7] = 0;
   out_9017059983617974166[8] = 0;
   out_9017059983617974166[9] = 0;
   out_9017059983617974166[10] = 0;
   out_9017059983617974166[11] = 0;
   out_9017059983617974166[12] = 0;
   out_9017059983617974166[13] = 0;
   out_9017059983617974166[14] = 0;
   out_9017059983617974166[15] = 0;
   out_9017059983617974166[16] = 0;
   out_9017059983617974166[17] = 0;
   out_9017059983617974166[18] = 0;
   out_9017059983617974166[19] = 0;
   out_9017059983617974166[20] = 0;
   out_9017059983617974166[21] = 0;
   out_9017059983617974166[22] = 1;
   out_9017059983617974166[23] = 0;
   out_9017059983617974166[24] = 0;
   out_9017059983617974166[25] = 0;
   out_9017059983617974166[26] = 0;
   out_9017059983617974166[27] = 0;
   out_9017059983617974166[28] = 0;
   out_9017059983617974166[29] = 0;
   out_9017059983617974166[30] = 0;
   out_9017059983617974166[31] = 0;
   out_9017059983617974166[32] = 0;
   out_9017059983617974166[33] = 0;
   out_9017059983617974166[34] = 0;
   out_9017059983617974166[35] = 0;
   out_9017059983617974166[36] = 0;
   out_9017059983617974166[37] = 0;
   out_9017059983617974166[38] = 0;
   out_9017059983617974166[39] = 0;
   out_9017059983617974166[40] = 0;
   out_9017059983617974166[41] = 1;
   out_9017059983617974166[42] = 0;
   out_9017059983617974166[43] = 0;
   out_9017059983617974166[44] = 0;
   out_9017059983617974166[45] = 0;
   out_9017059983617974166[46] = 0;
   out_9017059983617974166[47] = 0;
   out_9017059983617974166[48] = 0;
   out_9017059983617974166[49] = 0;
   out_9017059983617974166[50] = 0;
   out_9017059983617974166[51] = 0;
   out_9017059983617974166[52] = 0;
   out_9017059983617974166[53] = 0;
}
void h_14(double *state, double *unused, double *out_4519675338605033391) {
   out_4519675338605033391[0] = state[6];
   out_4519675338605033391[1] = state[7];
   out_4519675338605033391[2] = state[8];
}
void H_14(double *state, double *unused, double *out_3134621832463872353) {
   out_3134621832463872353[0] = 0;
   out_3134621832463872353[1] = 0;
   out_3134621832463872353[2] = 0;
   out_3134621832463872353[3] = 0;
   out_3134621832463872353[4] = 0;
   out_3134621832463872353[5] = 0;
   out_3134621832463872353[6] = 1;
   out_3134621832463872353[7] = 0;
   out_3134621832463872353[8] = 0;
   out_3134621832463872353[9] = 0;
   out_3134621832463872353[10] = 0;
   out_3134621832463872353[11] = 0;
   out_3134621832463872353[12] = 0;
   out_3134621832463872353[13] = 0;
   out_3134621832463872353[14] = 0;
   out_3134621832463872353[15] = 0;
   out_3134621832463872353[16] = 0;
   out_3134621832463872353[17] = 0;
   out_3134621832463872353[18] = 0;
   out_3134621832463872353[19] = 0;
   out_3134621832463872353[20] = 0;
   out_3134621832463872353[21] = 0;
   out_3134621832463872353[22] = 0;
   out_3134621832463872353[23] = 0;
   out_3134621832463872353[24] = 0;
   out_3134621832463872353[25] = 1;
   out_3134621832463872353[26] = 0;
   out_3134621832463872353[27] = 0;
   out_3134621832463872353[28] = 0;
   out_3134621832463872353[29] = 0;
   out_3134621832463872353[30] = 0;
   out_3134621832463872353[31] = 0;
   out_3134621832463872353[32] = 0;
   out_3134621832463872353[33] = 0;
   out_3134621832463872353[34] = 0;
   out_3134621832463872353[35] = 0;
   out_3134621832463872353[36] = 0;
   out_3134621832463872353[37] = 0;
   out_3134621832463872353[38] = 0;
   out_3134621832463872353[39] = 0;
   out_3134621832463872353[40] = 0;
   out_3134621832463872353[41] = 0;
   out_3134621832463872353[42] = 0;
   out_3134621832463872353[43] = 0;
   out_3134621832463872353[44] = 1;
   out_3134621832463872353[45] = 0;
   out_3134621832463872353[46] = 0;
   out_3134621832463872353[47] = 0;
   out_3134621832463872353[48] = 0;
   out_3134621832463872353[49] = 0;
   out_3134621832463872353[50] = 0;
   out_3134621832463872353[51] = 0;
   out_3134621832463872353[52] = 0;
   out_3134621832463872353[53] = 0;
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
void pose_err_fun(double *nom_x, double *delta_x, double *out_3388086590141056177) {
  err_fun(nom_x, delta_x, out_3388086590141056177);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_1633734341039361389) {
  inv_err_fun(nom_x, true_x, out_1633734341039361389);
}
void pose_H_mod_fun(double *state, double *out_3284972142892485625) {
  H_mod_fun(state, out_3284972142892485625);
}
void pose_f_fun(double *state, double dt, double *out_7294513523227393093) {
  f_fun(state,  dt, out_7294513523227393093);
}
void pose_F_fun(double *state, double dt, double *out_1315688866139806167) {
  F_fun(state,  dt, out_1315688866139806167);
}
void pose_h_4(double *state, double *unused, double *out_6849640214971072753) {
  h_4(state, unused, out_6849640214971072753);
}
void pose_H_4(double *state, double *unused, double *out_6217410264759244649) {
  H_4(state, unused, out_6217410264759244649);
}
void pose_h_10(double *state, double *unused, double *out_2538664534589622554) {
  h_10(state, unused, out_2538664534589622554);
}
void pose_H_10(double *state, double *unused, double *out_5499394725381253526) {
  H_10(state, unused, out_5499394725381253526);
}
void pose_h_13(double *state, double *unused, double *out_228516151720916839) {
  h_13(state, unused, out_228516151720916839);
}
void pose_H_13(double *state, double *unused, double *out_9017059983617974166) {
  H_13(state, unused, out_9017059983617974166);
}
void pose_h_14(double *state, double *unused, double *out_4519675338605033391) {
  h_14(state, unused, out_4519675338605033391);
}
void pose_H_14(double *state, double *unused, double *out_3134621832463872353) {
  H_14(state, unused, out_3134621832463872353);
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
