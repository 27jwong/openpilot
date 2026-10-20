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
void err_fun(double *nom_x, double *delta_x, double *out_3945905870871886629) {
   out_3945905870871886629[0] = delta_x[0] + nom_x[0];
   out_3945905870871886629[1] = delta_x[1] + nom_x[1];
   out_3945905870871886629[2] = delta_x[2] + nom_x[2];
   out_3945905870871886629[3] = delta_x[3] + nom_x[3];
   out_3945905870871886629[4] = delta_x[4] + nom_x[4];
   out_3945905870871886629[5] = delta_x[5] + nom_x[5];
   out_3945905870871886629[6] = delta_x[6] + nom_x[6];
   out_3945905870871886629[7] = delta_x[7] + nom_x[7];
   out_3945905870871886629[8] = delta_x[8] + nom_x[8];
   out_3945905870871886629[9] = delta_x[9] + nom_x[9];
   out_3945905870871886629[10] = delta_x[10] + nom_x[10];
   out_3945905870871886629[11] = delta_x[11] + nom_x[11];
   out_3945905870871886629[12] = delta_x[12] + nom_x[12];
   out_3945905870871886629[13] = delta_x[13] + nom_x[13];
   out_3945905870871886629[14] = delta_x[14] + nom_x[14];
   out_3945905870871886629[15] = delta_x[15] + nom_x[15];
   out_3945905870871886629[16] = delta_x[16] + nom_x[16];
   out_3945905870871886629[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_3515743109708110314) {
   out_3515743109708110314[0] = -nom_x[0] + true_x[0];
   out_3515743109708110314[1] = -nom_x[1] + true_x[1];
   out_3515743109708110314[2] = -nom_x[2] + true_x[2];
   out_3515743109708110314[3] = -nom_x[3] + true_x[3];
   out_3515743109708110314[4] = -nom_x[4] + true_x[4];
   out_3515743109708110314[5] = -nom_x[5] + true_x[5];
   out_3515743109708110314[6] = -nom_x[6] + true_x[6];
   out_3515743109708110314[7] = -nom_x[7] + true_x[7];
   out_3515743109708110314[8] = -nom_x[8] + true_x[8];
   out_3515743109708110314[9] = -nom_x[9] + true_x[9];
   out_3515743109708110314[10] = -nom_x[10] + true_x[10];
   out_3515743109708110314[11] = -nom_x[11] + true_x[11];
   out_3515743109708110314[12] = -nom_x[12] + true_x[12];
   out_3515743109708110314[13] = -nom_x[13] + true_x[13];
   out_3515743109708110314[14] = -nom_x[14] + true_x[14];
   out_3515743109708110314[15] = -nom_x[15] + true_x[15];
   out_3515743109708110314[16] = -nom_x[16] + true_x[16];
   out_3515743109708110314[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_2942188750116967597) {
   out_2942188750116967597[0] = 1.0;
   out_2942188750116967597[1] = 0.0;
   out_2942188750116967597[2] = 0.0;
   out_2942188750116967597[3] = 0.0;
   out_2942188750116967597[4] = 0.0;
   out_2942188750116967597[5] = 0.0;
   out_2942188750116967597[6] = 0.0;
   out_2942188750116967597[7] = 0.0;
   out_2942188750116967597[8] = 0.0;
   out_2942188750116967597[9] = 0.0;
   out_2942188750116967597[10] = 0.0;
   out_2942188750116967597[11] = 0.0;
   out_2942188750116967597[12] = 0.0;
   out_2942188750116967597[13] = 0.0;
   out_2942188750116967597[14] = 0.0;
   out_2942188750116967597[15] = 0.0;
   out_2942188750116967597[16] = 0.0;
   out_2942188750116967597[17] = 0.0;
   out_2942188750116967597[18] = 0.0;
   out_2942188750116967597[19] = 1.0;
   out_2942188750116967597[20] = 0.0;
   out_2942188750116967597[21] = 0.0;
   out_2942188750116967597[22] = 0.0;
   out_2942188750116967597[23] = 0.0;
   out_2942188750116967597[24] = 0.0;
   out_2942188750116967597[25] = 0.0;
   out_2942188750116967597[26] = 0.0;
   out_2942188750116967597[27] = 0.0;
   out_2942188750116967597[28] = 0.0;
   out_2942188750116967597[29] = 0.0;
   out_2942188750116967597[30] = 0.0;
   out_2942188750116967597[31] = 0.0;
   out_2942188750116967597[32] = 0.0;
   out_2942188750116967597[33] = 0.0;
   out_2942188750116967597[34] = 0.0;
   out_2942188750116967597[35] = 0.0;
   out_2942188750116967597[36] = 0.0;
   out_2942188750116967597[37] = 0.0;
   out_2942188750116967597[38] = 1.0;
   out_2942188750116967597[39] = 0.0;
   out_2942188750116967597[40] = 0.0;
   out_2942188750116967597[41] = 0.0;
   out_2942188750116967597[42] = 0.0;
   out_2942188750116967597[43] = 0.0;
   out_2942188750116967597[44] = 0.0;
   out_2942188750116967597[45] = 0.0;
   out_2942188750116967597[46] = 0.0;
   out_2942188750116967597[47] = 0.0;
   out_2942188750116967597[48] = 0.0;
   out_2942188750116967597[49] = 0.0;
   out_2942188750116967597[50] = 0.0;
   out_2942188750116967597[51] = 0.0;
   out_2942188750116967597[52] = 0.0;
   out_2942188750116967597[53] = 0.0;
   out_2942188750116967597[54] = 0.0;
   out_2942188750116967597[55] = 0.0;
   out_2942188750116967597[56] = 0.0;
   out_2942188750116967597[57] = 1.0;
   out_2942188750116967597[58] = 0.0;
   out_2942188750116967597[59] = 0.0;
   out_2942188750116967597[60] = 0.0;
   out_2942188750116967597[61] = 0.0;
   out_2942188750116967597[62] = 0.0;
   out_2942188750116967597[63] = 0.0;
   out_2942188750116967597[64] = 0.0;
   out_2942188750116967597[65] = 0.0;
   out_2942188750116967597[66] = 0.0;
   out_2942188750116967597[67] = 0.0;
   out_2942188750116967597[68] = 0.0;
   out_2942188750116967597[69] = 0.0;
   out_2942188750116967597[70] = 0.0;
   out_2942188750116967597[71] = 0.0;
   out_2942188750116967597[72] = 0.0;
   out_2942188750116967597[73] = 0.0;
   out_2942188750116967597[74] = 0.0;
   out_2942188750116967597[75] = 0.0;
   out_2942188750116967597[76] = 1.0;
   out_2942188750116967597[77] = 0.0;
   out_2942188750116967597[78] = 0.0;
   out_2942188750116967597[79] = 0.0;
   out_2942188750116967597[80] = 0.0;
   out_2942188750116967597[81] = 0.0;
   out_2942188750116967597[82] = 0.0;
   out_2942188750116967597[83] = 0.0;
   out_2942188750116967597[84] = 0.0;
   out_2942188750116967597[85] = 0.0;
   out_2942188750116967597[86] = 0.0;
   out_2942188750116967597[87] = 0.0;
   out_2942188750116967597[88] = 0.0;
   out_2942188750116967597[89] = 0.0;
   out_2942188750116967597[90] = 0.0;
   out_2942188750116967597[91] = 0.0;
   out_2942188750116967597[92] = 0.0;
   out_2942188750116967597[93] = 0.0;
   out_2942188750116967597[94] = 0.0;
   out_2942188750116967597[95] = 1.0;
   out_2942188750116967597[96] = 0.0;
   out_2942188750116967597[97] = 0.0;
   out_2942188750116967597[98] = 0.0;
   out_2942188750116967597[99] = 0.0;
   out_2942188750116967597[100] = 0.0;
   out_2942188750116967597[101] = 0.0;
   out_2942188750116967597[102] = 0.0;
   out_2942188750116967597[103] = 0.0;
   out_2942188750116967597[104] = 0.0;
   out_2942188750116967597[105] = 0.0;
   out_2942188750116967597[106] = 0.0;
   out_2942188750116967597[107] = 0.0;
   out_2942188750116967597[108] = 0.0;
   out_2942188750116967597[109] = 0.0;
   out_2942188750116967597[110] = 0.0;
   out_2942188750116967597[111] = 0.0;
   out_2942188750116967597[112] = 0.0;
   out_2942188750116967597[113] = 0.0;
   out_2942188750116967597[114] = 1.0;
   out_2942188750116967597[115] = 0.0;
   out_2942188750116967597[116] = 0.0;
   out_2942188750116967597[117] = 0.0;
   out_2942188750116967597[118] = 0.0;
   out_2942188750116967597[119] = 0.0;
   out_2942188750116967597[120] = 0.0;
   out_2942188750116967597[121] = 0.0;
   out_2942188750116967597[122] = 0.0;
   out_2942188750116967597[123] = 0.0;
   out_2942188750116967597[124] = 0.0;
   out_2942188750116967597[125] = 0.0;
   out_2942188750116967597[126] = 0.0;
   out_2942188750116967597[127] = 0.0;
   out_2942188750116967597[128] = 0.0;
   out_2942188750116967597[129] = 0.0;
   out_2942188750116967597[130] = 0.0;
   out_2942188750116967597[131] = 0.0;
   out_2942188750116967597[132] = 0.0;
   out_2942188750116967597[133] = 1.0;
   out_2942188750116967597[134] = 0.0;
   out_2942188750116967597[135] = 0.0;
   out_2942188750116967597[136] = 0.0;
   out_2942188750116967597[137] = 0.0;
   out_2942188750116967597[138] = 0.0;
   out_2942188750116967597[139] = 0.0;
   out_2942188750116967597[140] = 0.0;
   out_2942188750116967597[141] = 0.0;
   out_2942188750116967597[142] = 0.0;
   out_2942188750116967597[143] = 0.0;
   out_2942188750116967597[144] = 0.0;
   out_2942188750116967597[145] = 0.0;
   out_2942188750116967597[146] = 0.0;
   out_2942188750116967597[147] = 0.0;
   out_2942188750116967597[148] = 0.0;
   out_2942188750116967597[149] = 0.0;
   out_2942188750116967597[150] = 0.0;
   out_2942188750116967597[151] = 0.0;
   out_2942188750116967597[152] = 1.0;
   out_2942188750116967597[153] = 0.0;
   out_2942188750116967597[154] = 0.0;
   out_2942188750116967597[155] = 0.0;
   out_2942188750116967597[156] = 0.0;
   out_2942188750116967597[157] = 0.0;
   out_2942188750116967597[158] = 0.0;
   out_2942188750116967597[159] = 0.0;
   out_2942188750116967597[160] = 0.0;
   out_2942188750116967597[161] = 0.0;
   out_2942188750116967597[162] = 0.0;
   out_2942188750116967597[163] = 0.0;
   out_2942188750116967597[164] = 0.0;
   out_2942188750116967597[165] = 0.0;
   out_2942188750116967597[166] = 0.0;
   out_2942188750116967597[167] = 0.0;
   out_2942188750116967597[168] = 0.0;
   out_2942188750116967597[169] = 0.0;
   out_2942188750116967597[170] = 0.0;
   out_2942188750116967597[171] = 1.0;
   out_2942188750116967597[172] = 0.0;
   out_2942188750116967597[173] = 0.0;
   out_2942188750116967597[174] = 0.0;
   out_2942188750116967597[175] = 0.0;
   out_2942188750116967597[176] = 0.0;
   out_2942188750116967597[177] = 0.0;
   out_2942188750116967597[178] = 0.0;
   out_2942188750116967597[179] = 0.0;
   out_2942188750116967597[180] = 0.0;
   out_2942188750116967597[181] = 0.0;
   out_2942188750116967597[182] = 0.0;
   out_2942188750116967597[183] = 0.0;
   out_2942188750116967597[184] = 0.0;
   out_2942188750116967597[185] = 0.0;
   out_2942188750116967597[186] = 0.0;
   out_2942188750116967597[187] = 0.0;
   out_2942188750116967597[188] = 0.0;
   out_2942188750116967597[189] = 0.0;
   out_2942188750116967597[190] = 1.0;
   out_2942188750116967597[191] = 0.0;
   out_2942188750116967597[192] = 0.0;
   out_2942188750116967597[193] = 0.0;
   out_2942188750116967597[194] = 0.0;
   out_2942188750116967597[195] = 0.0;
   out_2942188750116967597[196] = 0.0;
   out_2942188750116967597[197] = 0.0;
   out_2942188750116967597[198] = 0.0;
   out_2942188750116967597[199] = 0.0;
   out_2942188750116967597[200] = 0.0;
   out_2942188750116967597[201] = 0.0;
   out_2942188750116967597[202] = 0.0;
   out_2942188750116967597[203] = 0.0;
   out_2942188750116967597[204] = 0.0;
   out_2942188750116967597[205] = 0.0;
   out_2942188750116967597[206] = 0.0;
   out_2942188750116967597[207] = 0.0;
   out_2942188750116967597[208] = 0.0;
   out_2942188750116967597[209] = 1.0;
   out_2942188750116967597[210] = 0.0;
   out_2942188750116967597[211] = 0.0;
   out_2942188750116967597[212] = 0.0;
   out_2942188750116967597[213] = 0.0;
   out_2942188750116967597[214] = 0.0;
   out_2942188750116967597[215] = 0.0;
   out_2942188750116967597[216] = 0.0;
   out_2942188750116967597[217] = 0.0;
   out_2942188750116967597[218] = 0.0;
   out_2942188750116967597[219] = 0.0;
   out_2942188750116967597[220] = 0.0;
   out_2942188750116967597[221] = 0.0;
   out_2942188750116967597[222] = 0.0;
   out_2942188750116967597[223] = 0.0;
   out_2942188750116967597[224] = 0.0;
   out_2942188750116967597[225] = 0.0;
   out_2942188750116967597[226] = 0.0;
   out_2942188750116967597[227] = 0.0;
   out_2942188750116967597[228] = 1.0;
   out_2942188750116967597[229] = 0.0;
   out_2942188750116967597[230] = 0.0;
   out_2942188750116967597[231] = 0.0;
   out_2942188750116967597[232] = 0.0;
   out_2942188750116967597[233] = 0.0;
   out_2942188750116967597[234] = 0.0;
   out_2942188750116967597[235] = 0.0;
   out_2942188750116967597[236] = 0.0;
   out_2942188750116967597[237] = 0.0;
   out_2942188750116967597[238] = 0.0;
   out_2942188750116967597[239] = 0.0;
   out_2942188750116967597[240] = 0.0;
   out_2942188750116967597[241] = 0.0;
   out_2942188750116967597[242] = 0.0;
   out_2942188750116967597[243] = 0.0;
   out_2942188750116967597[244] = 0.0;
   out_2942188750116967597[245] = 0.0;
   out_2942188750116967597[246] = 0.0;
   out_2942188750116967597[247] = 1.0;
   out_2942188750116967597[248] = 0.0;
   out_2942188750116967597[249] = 0.0;
   out_2942188750116967597[250] = 0.0;
   out_2942188750116967597[251] = 0.0;
   out_2942188750116967597[252] = 0.0;
   out_2942188750116967597[253] = 0.0;
   out_2942188750116967597[254] = 0.0;
   out_2942188750116967597[255] = 0.0;
   out_2942188750116967597[256] = 0.0;
   out_2942188750116967597[257] = 0.0;
   out_2942188750116967597[258] = 0.0;
   out_2942188750116967597[259] = 0.0;
   out_2942188750116967597[260] = 0.0;
   out_2942188750116967597[261] = 0.0;
   out_2942188750116967597[262] = 0.0;
   out_2942188750116967597[263] = 0.0;
   out_2942188750116967597[264] = 0.0;
   out_2942188750116967597[265] = 0.0;
   out_2942188750116967597[266] = 1.0;
   out_2942188750116967597[267] = 0.0;
   out_2942188750116967597[268] = 0.0;
   out_2942188750116967597[269] = 0.0;
   out_2942188750116967597[270] = 0.0;
   out_2942188750116967597[271] = 0.0;
   out_2942188750116967597[272] = 0.0;
   out_2942188750116967597[273] = 0.0;
   out_2942188750116967597[274] = 0.0;
   out_2942188750116967597[275] = 0.0;
   out_2942188750116967597[276] = 0.0;
   out_2942188750116967597[277] = 0.0;
   out_2942188750116967597[278] = 0.0;
   out_2942188750116967597[279] = 0.0;
   out_2942188750116967597[280] = 0.0;
   out_2942188750116967597[281] = 0.0;
   out_2942188750116967597[282] = 0.0;
   out_2942188750116967597[283] = 0.0;
   out_2942188750116967597[284] = 0.0;
   out_2942188750116967597[285] = 1.0;
   out_2942188750116967597[286] = 0.0;
   out_2942188750116967597[287] = 0.0;
   out_2942188750116967597[288] = 0.0;
   out_2942188750116967597[289] = 0.0;
   out_2942188750116967597[290] = 0.0;
   out_2942188750116967597[291] = 0.0;
   out_2942188750116967597[292] = 0.0;
   out_2942188750116967597[293] = 0.0;
   out_2942188750116967597[294] = 0.0;
   out_2942188750116967597[295] = 0.0;
   out_2942188750116967597[296] = 0.0;
   out_2942188750116967597[297] = 0.0;
   out_2942188750116967597[298] = 0.0;
   out_2942188750116967597[299] = 0.0;
   out_2942188750116967597[300] = 0.0;
   out_2942188750116967597[301] = 0.0;
   out_2942188750116967597[302] = 0.0;
   out_2942188750116967597[303] = 0.0;
   out_2942188750116967597[304] = 1.0;
   out_2942188750116967597[305] = 0.0;
   out_2942188750116967597[306] = 0.0;
   out_2942188750116967597[307] = 0.0;
   out_2942188750116967597[308] = 0.0;
   out_2942188750116967597[309] = 0.0;
   out_2942188750116967597[310] = 0.0;
   out_2942188750116967597[311] = 0.0;
   out_2942188750116967597[312] = 0.0;
   out_2942188750116967597[313] = 0.0;
   out_2942188750116967597[314] = 0.0;
   out_2942188750116967597[315] = 0.0;
   out_2942188750116967597[316] = 0.0;
   out_2942188750116967597[317] = 0.0;
   out_2942188750116967597[318] = 0.0;
   out_2942188750116967597[319] = 0.0;
   out_2942188750116967597[320] = 0.0;
   out_2942188750116967597[321] = 0.0;
   out_2942188750116967597[322] = 0.0;
   out_2942188750116967597[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_8009815601597605847) {
   out_8009815601597605847[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_8009815601597605847[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_8009815601597605847[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_8009815601597605847[3] = dt*state[12] + state[3];
   out_8009815601597605847[4] = dt*state[13] + state[4];
   out_8009815601597605847[5] = dt*state[14] + state[5];
   out_8009815601597605847[6] = state[6];
   out_8009815601597605847[7] = state[7];
   out_8009815601597605847[8] = state[8];
   out_8009815601597605847[9] = state[9];
   out_8009815601597605847[10] = state[10];
   out_8009815601597605847[11] = state[11];
   out_8009815601597605847[12] = state[12];
   out_8009815601597605847[13] = state[13];
   out_8009815601597605847[14] = state[14];
   out_8009815601597605847[15] = state[15];
   out_8009815601597605847[16] = state[16];
   out_8009815601597605847[17] = state[17];
}
void F_fun(double *state, double dt, double *out_9201886331103740614) {
   out_9201886331103740614[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_9201886331103740614[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_9201886331103740614[2] = 0;
   out_9201886331103740614[3] = 0;
   out_9201886331103740614[4] = 0;
   out_9201886331103740614[5] = 0;
   out_9201886331103740614[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_9201886331103740614[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_9201886331103740614[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_9201886331103740614[9] = 0;
   out_9201886331103740614[10] = 0;
   out_9201886331103740614[11] = 0;
   out_9201886331103740614[12] = 0;
   out_9201886331103740614[13] = 0;
   out_9201886331103740614[14] = 0;
   out_9201886331103740614[15] = 0;
   out_9201886331103740614[16] = 0;
   out_9201886331103740614[17] = 0;
   out_9201886331103740614[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_9201886331103740614[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_9201886331103740614[20] = 0;
   out_9201886331103740614[21] = 0;
   out_9201886331103740614[22] = 0;
   out_9201886331103740614[23] = 0;
   out_9201886331103740614[24] = 0;
   out_9201886331103740614[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_9201886331103740614[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_9201886331103740614[27] = 0;
   out_9201886331103740614[28] = 0;
   out_9201886331103740614[29] = 0;
   out_9201886331103740614[30] = 0;
   out_9201886331103740614[31] = 0;
   out_9201886331103740614[32] = 0;
   out_9201886331103740614[33] = 0;
   out_9201886331103740614[34] = 0;
   out_9201886331103740614[35] = 0;
   out_9201886331103740614[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_9201886331103740614[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_9201886331103740614[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_9201886331103740614[39] = 0;
   out_9201886331103740614[40] = 0;
   out_9201886331103740614[41] = 0;
   out_9201886331103740614[42] = 0;
   out_9201886331103740614[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_9201886331103740614[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_9201886331103740614[45] = 0;
   out_9201886331103740614[46] = 0;
   out_9201886331103740614[47] = 0;
   out_9201886331103740614[48] = 0;
   out_9201886331103740614[49] = 0;
   out_9201886331103740614[50] = 0;
   out_9201886331103740614[51] = 0;
   out_9201886331103740614[52] = 0;
   out_9201886331103740614[53] = 0;
   out_9201886331103740614[54] = 0;
   out_9201886331103740614[55] = 0;
   out_9201886331103740614[56] = 0;
   out_9201886331103740614[57] = 1;
   out_9201886331103740614[58] = 0;
   out_9201886331103740614[59] = 0;
   out_9201886331103740614[60] = 0;
   out_9201886331103740614[61] = 0;
   out_9201886331103740614[62] = 0;
   out_9201886331103740614[63] = 0;
   out_9201886331103740614[64] = 0;
   out_9201886331103740614[65] = 0;
   out_9201886331103740614[66] = dt;
   out_9201886331103740614[67] = 0;
   out_9201886331103740614[68] = 0;
   out_9201886331103740614[69] = 0;
   out_9201886331103740614[70] = 0;
   out_9201886331103740614[71] = 0;
   out_9201886331103740614[72] = 0;
   out_9201886331103740614[73] = 0;
   out_9201886331103740614[74] = 0;
   out_9201886331103740614[75] = 0;
   out_9201886331103740614[76] = 1;
   out_9201886331103740614[77] = 0;
   out_9201886331103740614[78] = 0;
   out_9201886331103740614[79] = 0;
   out_9201886331103740614[80] = 0;
   out_9201886331103740614[81] = 0;
   out_9201886331103740614[82] = 0;
   out_9201886331103740614[83] = 0;
   out_9201886331103740614[84] = 0;
   out_9201886331103740614[85] = dt;
   out_9201886331103740614[86] = 0;
   out_9201886331103740614[87] = 0;
   out_9201886331103740614[88] = 0;
   out_9201886331103740614[89] = 0;
   out_9201886331103740614[90] = 0;
   out_9201886331103740614[91] = 0;
   out_9201886331103740614[92] = 0;
   out_9201886331103740614[93] = 0;
   out_9201886331103740614[94] = 0;
   out_9201886331103740614[95] = 1;
   out_9201886331103740614[96] = 0;
   out_9201886331103740614[97] = 0;
   out_9201886331103740614[98] = 0;
   out_9201886331103740614[99] = 0;
   out_9201886331103740614[100] = 0;
   out_9201886331103740614[101] = 0;
   out_9201886331103740614[102] = 0;
   out_9201886331103740614[103] = 0;
   out_9201886331103740614[104] = dt;
   out_9201886331103740614[105] = 0;
   out_9201886331103740614[106] = 0;
   out_9201886331103740614[107] = 0;
   out_9201886331103740614[108] = 0;
   out_9201886331103740614[109] = 0;
   out_9201886331103740614[110] = 0;
   out_9201886331103740614[111] = 0;
   out_9201886331103740614[112] = 0;
   out_9201886331103740614[113] = 0;
   out_9201886331103740614[114] = 1;
   out_9201886331103740614[115] = 0;
   out_9201886331103740614[116] = 0;
   out_9201886331103740614[117] = 0;
   out_9201886331103740614[118] = 0;
   out_9201886331103740614[119] = 0;
   out_9201886331103740614[120] = 0;
   out_9201886331103740614[121] = 0;
   out_9201886331103740614[122] = 0;
   out_9201886331103740614[123] = 0;
   out_9201886331103740614[124] = 0;
   out_9201886331103740614[125] = 0;
   out_9201886331103740614[126] = 0;
   out_9201886331103740614[127] = 0;
   out_9201886331103740614[128] = 0;
   out_9201886331103740614[129] = 0;
   out_9201886331103740614[130] = 0;
   out_9201886331103740614[131] = 0;
   out_9201886331103740614[132] = 0;
   out_9201886331103740614[133] = 1;
   out_9201886331103740614[134] = 0;
   out_9201886331103740614[135] = 0;
   out_9201886331103740614[136] = 0;
   out_9201886331103740614[137] = 0;
   out_9201886331103740614[138] = 0;
   out_9201886331103740614[139] = 0;
   out_9201886331103740614[140] = 0;
   out_9201886331103740614[141] = 0;
   out_9201886331103740614[142] = 0;
   out_9201886331103740614[143] = 0;
   out_9201886331103740614[144] = 0;
   out_9201886331103740614[145] = 0;
   out_9201886331103740614[146] = 0;
   out_9201886331103740614[147] = 0;
   out_9201886331103740614[148] = 0;
   out_9201886331103740614[149] = 0;
   out_9201886331103740614[150] = 0;
   out_9201886331103740614[151] = 0;
   out_9201886331103740614[152] = 1;
   out_9201886331103740614[153] = 0;
   out_9201886331103740614[154] = 0;
   out_9201886331103740614[155] = 0;
   out_9201886331103740614[156] = 0;
   out_9201886331103740614[157] = 0;
   out_9201886331103740614[158] = 0;
   out_9201886331103740614[159] = 0;
   out_9201886331103740614[160] = 0;
   out_9201886331103740614[161] = 0;
   out_9201886331103740614[162] = 0;
   out_9201886331103740614[163] = 0;
   out_9201886331103740614[164] = 0;
   out_9201886331103740614[165] = 0;
   out_9201886331103740614[166] = 0;
   out_9201886331103740614[167] = 0;
   out_9201886331103740614[168] = 0;
   out_9201886331103740614[169] = 0;
   out_9201886331103740614[170] = 0;
   out_9201886331103740614[171] = 1;
   out_9201886331103740614[172] = 0;
   out_9201886331103740614[173] = 0;
   out_9201886331103740614[174] = 0;
   out_9201886331103740614[175] = 0;
   out_9201886331103740614[176] = 0;
   out_9201886331103740614[177] = 0;
   out_9201886331103740614[178] = 0;
   out_9201886331103740614[179] = 0;
   out_9201886331103740614[180] = 0;
   out_9201886331103740614[181] = 0;
   out_9201886331103740614[182] = 0;
   out_9201886331103740614[183] = 0;
   out_9201886331103740614[184] = 0;
   out_9201886331103740614[185] = 0;
   out_9201886331103740614[186] = 0;
   out_9201886331103740614[187] = 0;
   out_9201886331103740614[188] = 0;
   out_9201886331103740614[189] = 0;
   out_9201886331103740614[190] = 1;
   out_9201886331103740614[191] = 0;
   out_9201886331103740614[192] = 0;
   out_9201886331103740614[193] = 0;
   out_9201886331103740614[194] = 0;
   out_9201886331103740614[195] = 0;
   out_9201886331103740614[196] = 0;
   out_9201886331103740614[197] = 0;
   out_9201886331103740614[198] = 0;
   out_9201886331103740614[199] = 0;
   out_9201886331103740614[200] = 0;
   out_9201886331103740614[201] = 0;
   out_9201886331103740614[202] = 0;
   out_9201886331103740614[203] = 0;
   out_9201886331103740614[204] = 0;
   out_9201886331103740614[205] = 0;
   out_9201886331103740614[206] = 0;
   out_9201886331103740614[207] = 0;
   out_9201886331103740614[208] = 0;
   out_9201886331103740614[209] = 1;
   out_9201886331103740614[210] = 0;
   out_9201886331103740614[211] = 0;
   out_9201886331103740614[212] = 0;
   out_9201886331103740614[213] = 0;
   out_9201886331103740614[214] = 0;
   out_9201886331103740614[215] = 0;
   out_9201886331103740614[216] = 0;
   out_9201886331103740614[217] = 0;
   out_9201886331103740614[218] = 0;
   out_9201886331103740614[219] = 0;
   out_9201886331103740614[220] = 0;
   out_9201886331103740614[221] = 0;
   out_9201886331103740614[222] = 0;
   out_9201886331103740614[223] = 0;
   out_9201886331103740614[224] = 0;
   out_9201886331103740614[225] = 0;
   out_9201886331103740614[226] = 0;
   out_9201886331103740614[227] = 0;
   out_9201886331103740614[228] = 1;
   out_9201886331103740614[229] = 0;
   out_9201886331103740614[230] = 0;
   out_9201886331103740614[231] = 0;
   out_9201886331103740614[232] = 0;
   out_9201886331103740614[233] = 0;
   out_9201886331103740614[234] = 0;
   out_9201886331103740614[235] = 0;
   out_9201886331103740614[236] = 0;
   out_9201886331103740614[237] = 0;
   out_9201886331103740614[238] = 0;
   out_9201886331103740614[239] = 0;
   out_9201886331103740614[240] = 0;
   out_9201886331103740614[241] = 0;
   out_9201886331103740614[242] = 0;
   out_9201886331103740614[243] = 0;
   out_9201886331103740614[244] = 0;
   out_9201886331103740614[245] = 0;
   out_9201886331103740614[246] = 0;
   out_9201886331103740614[247] = 1;
   out_9201886331103740614[248] = 0;
   out_9201886331103740614[249] = 0;
   out_9201886331103740614[250] = 0;
   out_9201886331103740614[251] = 0;
   out_9201886331103740614[252] = 0;
   out_9201886331103740614[253] = 0;
   out_9201886331103740614[254] = 0;
   out_9201886331103740614[255] = 0;
   out_9201886331103740614[256] = 0;
   out_9201886331103740614[257] = 0;
   out_9201886331103740614[258] = 0;
   out_9201886331103740614[259] = 0;
   out_9201886331103740614[260] = 0;
   out_9201886331103740614[261] = 0;
   out_9201886331103740614[262] = 0;
   out_9201886331103740614[263] = 0;
   out_9201886331103740614[264] = 0;
   out_9201886331103740614[265] = 0;
   out_9201886331103740614[266] = 1;
   out_9201886331103740614[267] = 0;
   out_9201886331103740614[268] = 0;
   out_9201886331103740614[269] = 0;
   out_9201886331103740614[270] = 0;
   out_9201886331103740614[271] = 0;
   out_9201886331103740614[272] = 0;
   out_9201886331103740614[273] = 0;
   out_9201886331103740614[274] = 0;
   out_9201886331103740614[275] = 0;
   out_9201886331103740614[276] = 0;
   out_9201886331103740614[277] = 0;
   out_9201886331103740614[278] = 0;
   out_9201886331103740614[279] = 0;
   out_9201886331103740614[280] = 0;
   out_9201886331103740614[281] = 0;
   out_9201886331103740614[282] = 0;
   out_9201886331103740614[283] = 0;
   out_9201886331103740614[284] = 0;
   out_9201886331103740614[285] = 1;
   out_9201886331103740614[286] = 0;
   out_9201886331103740614[287] = 0;
   out_9201886331103740614[288] = 0;
   out_9201886331103740614[289] = 0;
   out_9201886331103740614[290] = 0;
   out_9201886331103740614[291] = 0;
   out_9201886331103740614[292] = 0;
   out_9201886331103740614[293] = 0;
   out_9201886331103740614[294] = 0;
   out_9201886331103740614[295] = 0;
   out_9201886331103740614[296] = 0;
   out_9201886331103740614[297] = 0;
   out_9201886331103740614[298] = 0;
   out_9201886331103740614[299] = 0;
   out_9201886331103740614[300] = 0;
   out_9201886331103740614[301] = 0;
   out_9201886331103740614[302] = 0;
   out_9201886331103740614[303] = 0;
   out_9201886331103740614[304] = 1;
   out_9201886331103740614[305] = 0;
   out_9201886331103740614[306] = 0;
   out_9201886331103740614[307] = 0;
   out_9201886331103740614[308] = 0;
   out_9201886331103740614[309] = 0;
   out_9201886331103740614[310] = 0;
   out_9201886331103740614[311] = 0;
   out_9201886331103740614[312] = 0;
   out_9201886331103740614[313] = 0;
   out_9201886331103740614[314] = 0;
   out_9201886331103740614[315] = 0;
   out_9201886331103740614[316] = 0;
   out_9201886331103740614[317] = 0;
   out_9201886331103740614[318] = 0;
   out_9201886331103740614[319] = 0;
   out_9201886331103740614[320] = 0;
   out_9201886331103740614[321] = 0;
   out_9201886331103740614[322] = 0;
   out_9201886331103740614[323] = 1;
}
void h_4(double *state, double *unused, double *out_3270913839475586541) {
   out_3270913839475586541[0] = state[6] + state[9];
   out_3270913839475586541[1] = state[7] + state[10];
   out_3270913839475586541[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_6766359091509594881) {
   out_6766359091509594881[0] = 0;
   out_6766359091509594881[1] = 0;
   out_6766359091509594881[2] = 0;
   out_6766359091509594881[3] = 0;
   out_6766359091509594881[4] = 0;
   out_6766359091509594881[5] = 0;
   out_6766359091509594881[6] = 1;
   out_6766359091509594881[7] = 0;
   out_6766359091509594881[8] = 0;
   out_6766359091509594881[9] = 1;
   out_6766359091509594881[10] = 0;
   out_6766359091509594881[11] = 0;
   out_6766359091509594881[12] = 0;
   out_6766359091509594881[13] = 0;
   out_6766359091509594881[14] = 0;
   out_6766359091509594881[15] = 0;
   out_6766359091509594881[16] = 0;
   out_6766359091509594881[17] = 0;
   out_6766359091509594881[18] = 0;
   out_6766359091509594881[19] = 0;
   out_6766359091509594881[20] = 0;
   out_6766359091509594881[21] = 0;
   out_6766359091509594881[22] = 0;
   out_6766359091509594881[23] = 0;
   out_6766359091509594881[24] = 0;
   out_6766359091509594881[25] = 1;
   out_6766359091509594881[26] = 0;
   out_6766359091509594881[27] = 0;
   out_6766359091509594881[28] = 1;
   out_6766359091509594881[29] = 0;
   out_6766359091509594881[30] = 0;
   out_6766359091509594881[31] = 0;
   out_6766359091509594881[32] = 0;
   out_6766359091509594881[33] = 0;
   out_6766359091509594881[34] = 0;
   out_6766359091509594881[35] = 0;
   out_6766359091509594881[36] = 0;
   out_6766359091509594881[37] = 0;
   out_6766359091509594881[38] = 0;
   out_6766359091509594881[39] = 0;
   out_6766359091509594881[40] = 0;
   out_6766359091509594881[41] = 0;
   out_6766359091509594881[42] = 0;
   out_6766359091509594881[43] = 0;
   out_6766359091509594881[44] = 1;
   out_6766359091509594881[45] = 0;
   out_6766359091509594881[46] = 0;
   out_6766359091509594881[47] = 1;
   out_6766359091509594881[48] = 0;
   out_6766359091509594881[49] = 0;
   out_6766359091509594881[50] = 0;
   out_6766359091509594881[51] = 0;
   out_6766359091509594881[52] = 0;
   out_6766359091509594881[53] = 0;
}
void h_10(double *state, double *unused, double *out_7309022143129653139) {
   out_7309022143129653139[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_7309022143129653139[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_7309022143129653139[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_8881959942302034495) {
   out_8881959942302034495[0] = 0;
   out_8881959942302034495[1] = 9.8100000000000005*cos(state[1]);
   out_8881959942302034495[2] = 0;
   out_8881959942302034495[3] = 0;
   out_8881959942302034495[4] = -state[8];
   out_8881959942302034495[5] = state[7];
   out_8881959942302034495[6] = 0;
   out_8881959942302034495[7] = state[5];
   out_8881959942302034495[8] = -state[4];
   out_8881959942302034495[9] = 0;
   out_8881959942302034495[10] = 0;
   out_8881959942302034495[11] = 0;
   out_8881959942302034495[12] = 1;
   out_8881959942302034495[13] = 0;
   out_8881959942302034495[14] = 0;
   out_8881959942302034495[15] = 1;
   out_8881959942302034495[16] = 0;
   out_8881959942302034495[17] = 0;
   out_8881959942302034495[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_8881959942302034495[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_8881959942302034495[20] = 0;
   out_8881959942302034495[21] = state[8];
   out_8881959942302034495[22] = 0;
   out_8881959942302034495[23] = -state[6];
   out_8881959942302034495[24] = -state[5];
   out_8881959942302034495[25] = 0;
   out_8881959942302034495[26] = state[3];
   out_8881959942302034495[27] = 0;
   out_8881959942302034495[28] = 0;
   out_8881959942302034495[29] = 0;
   out_8881959942302034495[30] = 0;
   out_8881959942302034495[31] = 1;
   out_8881959942302034495[32] = 0;
   out_8881959942302034495[33] = 0;
   out_8881959942302034495[34] = 1;
   out_8881959942302034495[35] = 0;
   out_8881959942302034495[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_8881959942302034495[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_8881959942302034495[38] = 0;
   out_8881959942302034495[39] = -state[7];
   out_8881959942302034495[40] = state[6];
   out_8881959942302034495[41] = 0;
   out_8881959942302034495[42] = state[4];
   out_8881959942302034495[43] = -state[3];
   out_8881959942302034495[44] = 0;
   out_8881959942302034495[45] = 0;
   out_8881959942302034495[46] = 0;
   out_8881959942302034495[47] = 0;
   out_8881959942302034495[48] = 0;
   out_8881959942302034495[49] = 0;
   out_8881959942302034495[50] = 1;
   out_8881959942302034495[51] = 0;
   out_8881959942302034495[52] = 0;
   out_8881959942302034495[53] = 1;
}
void h_13(double *state, double *unused, double *out_2388800272598135721) {
   out_2388800272598135721[0] = state[3];
   out_2388800272598135721[1] = state[4];
   out_2388800272598135721[2] = state[5];
}
void H_13(double *state, double *unused, double *out_3554085266177262080) {
   out_3554085266177262080[0] = 0;
   out_3554085266177262080[1] = 0;
   out_3554085266177262080[2] = 0;
   out_3554085266177262080[3] = 1;
   out_3554085266177262080[4] = 0;
   out_3554085266177262080[5] = 0;
   out_3554085266177262080[6] = 0;
   out_3554085266177262080[7] = 0;
   out_3554085266177262080[8] = 0;
   out_3554085266177262080[9] = 0;
   out_3554085266177262080[10] = 0;
   out_3554085266177262080[11] = 0;
   out_3554085266177262080[12] = 0;
   out_3554085266177262080[13] = 0;
   out_3554085266177262080[14] = 0;
   out_3554085266177262080[15] = 0;
   out_3554085266177262080[16] = 0;
   out_3554085266177262080[17] = 0;
   out_3554085266177262080[18] = 0;
   out_3554085266177262080[19] = 0;
   out_3554085266177262080[20] = 0;
   out_3554085266177262080[21] = 0;
   out_3554085266177262080[22] = 1;
   out_3554085266177262080[23] = 0;
   out_3554085266177262080[24] = 0;
   out_3554085266177262080[25] = 0;
   out_3554085266177262080[26] = 0;
   out_3554085266177262080[27] = 0;
   out_3554085266177262080[28] = 0;
   out_3554085266177262080[29] = 0;
   out_3554085266177262080[30] = 0;
   out_3554085266177262080[31] = 0;
   out_3554085266177262080[32] = 0;
   out_3554085266177262080[33] = 0;
   out_3554085266177262080[34] = 0;
   out_3554085266177262080[35] = 0;
   out_3554085266177262080[36] = 0;
   out_3554085266177262080[37] = 0;
   out_3554085266177262080[38] = 0;
   out_3554085266177262080[39] = 0;
   out_3554085266177262080[40] = 0;
   out_3554085266177262080[41] = 1;
   out_3554085266177262080[42] = 0;
   out_3554085266177262080[43] = 0;
   out_3554085266177262080[44] = 0;
   out_3554085266177262080[45] = 0;
   out_3554085266177262080[46] = 0;
   out_3554085266177262080[47] = 0;
   out_3554085266177262080[48] = 0;
   out_3554085266177262080[49] = 0;
   out_3554085266177262080[50] = 0;
   out_3554085266177262080[51] = 0;
   out_3554085266177262080[52] = 0;
   out_3554085266177262080[53] = 0;
}
void h_14(double *state, double *unused, double *out_2407078509949300141) {
   out_2407078509949300141[0] = state[6];
   out_2407078509949300141[1] = state[7];
   out_2407078509949300141[2] = state[8];
}
void H_14(double *state, double *unused, double *out_2803118235170110352) {
   out_2803118235170110352[0] = 0;
   out_2803118235170110352[1] = 0;
   out_2803118235170110352[2] = 0;
   out_2803118235170110352[3] = 0;
   out_2803118235170110352[4] = 0;
   out_2803118235170110352[5] = 0;
   out_2803118235170110352[6] = 1;
   out_2803118235170110352[7] = 0;
   out_2803118235170110352[8] = 0;
   out_2803118235170110352[9] = 0;
   out_2803118235170110352[10] = 0;
   out_2803118235170110352[11] = 0;
   out_2803118235170110352[12] = 0;
   out_2803118235170110352[13] = 0;
   out_2803118235170110352[14] = 0;
   out_2803118235170110352[15] = 0;
   out_2803118235170110352[16] = 0;
   out_2803118235170110352[17] = 0;
   out_2803118235170110352[18] = 0;
   out_2803118235170110352[19] = 0;
   out_2803118235170110352[20] = 0;
   out_2803118235170110352[21] = 0;
   out_2803118235170110352[22] = 0;
   out_2803118235170110352[23] = 0;
   out_2803118235170110352[24] = 0;
   out_2803118235170110352[25] = 1;
   out_2803118235170110352[26] = 0;
   out_2803118235170110352[27] = 0;
   out_2803118235170110352[28] = 0;
   out_2803118235170110352[29] = 0;
   out_2803118235170110352[30] = 0;
   out_2803118235170110352[31] = 0;
   out_2803118235170110352[32] = 0;
   out_2803118235170110352[33] = 0;
   out_2803118235170110352[34] = 0;
   out_2803118235170110352[35] = 0;
   out_2803118235170110352[36] = 0;
   out_2803118235170110352[37] = 0;
   out_2803118235170110352[38] = 0;
   out_2803118235170110352[39] = 0;
   out_2803118235170110352[40] = 0;
   out_2803118235170110352[41] = 0;
   out_2803118235170110352[42] = 0;
   out_2803118235170110352[43] = 0;
   out_2803118235170110352[44] = 1;
   out_2803118235170110352[45] = 0;
   out_2803118235170110352[46] = 0;
   out_2803118235170110352[47] = 0;
   out_2803118235170110352[48] = 0;
   out_2803118235170110352[49] = 0;
   out_2803118235170110352[50] = 0;
   out_2803118235170110352[51] = 0;
   out_2803118235170110352[52] = 0;
   out_2803118235170110352[53] = 0;
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
void pose_err_fun(double *nom_x, double *delta_x, double *out_3945905870871886629) {
  err_fun(nom_x, delta_x, out_3945905870871886629);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_3515743109708110314) {
  inv_err_fun(nom_x, true_x, out_3515743109708110314);
}
void pose_H_mod_fun(double *state, double *out_2942188750116967597) {
  H_mod_fun(state, out_2942188750116967597);
}
void pose_f_fun(double *state, double dt, double *out_8009815601597605847) {
  f_fun(state,  dt, out_8009815601597605847);
}
void pose_F_fun(double *state, double dt, double *out_9201886331103740614) {
  F_fun(state,  dt, out_9201886331103740614);
}
void pose_h_4(double *state, double *unused, double *out_3270913839475586541) {
  h_4(state, unused, out_3270913839475586541);
}
void pose_H_4(double *state, double *unused, double *out_6766359091509594881) {
  H_4(state, unused, out_6766359091509594881);
}
void pose_h_10(double *state, double *unused, double *out_7309022143129653139) {
  h_10(state, unused, out_7309022143129653139);
}
void pose_H_10(double *state, double *unused, double *out_8881959942302034495) {
  H_10(state, unused, out_8881959942302034495);
}
void pose_h_13(double *state, double *unused, double *out_2388800272598135721) {
  h_13(state, unused, out_2388800272598135721);
}
void pose_H_13(double *state, double *unused, double *out_3554085266177262080) {
  H_13(state, unused, out_3554085266177262080);
}
void pose_h_14(double *state, double *unused, double *out_2407078509949300141) {
  h_14(state, unused, out_2407078509949300141);
}
void pose_H_14(double *state, double *unused, double *out_2803118235170110352) {
  H_14(state, unused, out_2803118235170110352);
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
