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
void err_fun(double *nom_x, double *delta_x, double *out_4013660299866450489) {
   out_4013660299866450489[0] = delta_x[0] + nom_x[0];
   out_4013660299866450489[1] = delta_x[1] + nom_x[1];
   out_4013660299866450489[2] = delta_x[2] + nom_x[2];
   out_4013660299866450489[3] = delta_x[3] + nom_x[3];
   out_4013660299866450489[4] = delta_x[4] + nom_x[4];
   out_4013660299866450489[5] = delta_x[5] + nom_x[5];
   out_4013660299866450489[6] = delta_x[6] + nom_x[6];
   out_4013660299866450489[7] = delta_x[7] + nom_x[7];
   out_4013660299866450489[8] = delta_x[8] + nom_x[8];
   out_4013660299866450489[9] = delta_x[9] + nom_x[9];
   out_4013660299866450489[10] = delta_x[10] + nom_x[10];
   out_4013660299866450489[11] = delta_x[11] + nom_x[11];
   out_4013660299866450489[12] = delta_x[12] + nom_x[12];
   out_4013660299866450489[13] = delta_x[13] + nom_x[13];
   out_4013660299866450489[14] = delta_x[14] + nom_x[14];
   out_4013660299866450489[15] = delta_x[15] + nom_x[15];
   out_4013660299866450489[16] = delta_x[16] + nom_x[16];
   out_4013660299866450489[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_4438646948403156526) {
   out_4438646948403156526[0] = -nom_x[0] + true_x[0];
   out_4438646948403156526[1] = -nom_x[1] + true_x[1];
   out_4438646948403156526[2] = -nom_x[2] + true_x[2];
   out_4438646948403156526[3] = -nom_x[3] + true_x[3];
   out_4438646948403156526[4] = -nom_x[4] + true_x[4];
   out_4438646948403156526[5] = -nom_x[5] + true_x[5];
   out_4438646948403156526[6] = -nom_x[6] + true_x[6];
   out_4438646948403156526[7] = -nom_x[7] + true_x[7];
   out_4438646948403156526[8] = -nom_x[8] + true_x[8];
   out_4438646948403156526[9] = -nom_x[9] + true_x[9];
   out_4438646948403156526[10] = -nom_x[10] + true_x[10];
   out_4438646948403156526[11] = -nom_x[11] + true_x[11];
   out_4438646948403156526[12] = -nom_x[12] + true_x[12];
   out_4438646948403156526[13] = -nom_x[13] + true_x[13];
   out_4438646948403156526[14] = -nom_x[14] + true_x[14];
   out_4438646948403156526[15] = -nom_x[15] + true_x[15];
   out_4438646948403156526[16] = -nom_x[16] + true_x[16];
   out_4438646948403156526[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_8172130496533507169) {
   out_8172130496533507169[0] = 1.0;
   out_8172130496533507169[1] = 0.0;
   out_8172130496533507169[2] = 0.0;
   out_8172130496533507169[3] = 0.0;
   out_8172130496533507169[4] = 0.0;
   out_8172130496533507169[5] = 0.0;
   out_8172130496533507169[6] = 0.0;
   out_8172130496533507169[7] = 0.0;
   out_8172130496533507169[8] = 0.0;
   out_8172130496533507169[9] = 0.0;
   out_8172130496533507169[10] = 0.0;
   out_8172130496533507169[11] = 0.0;
   out_8172130496533507169[12] = 0.0;
   out_8172130496533507169[13] = 0.0;
   out_8172130496533507169[14] = 0.0;
   out_8172130496533507169[15] = 0.0;
   out_8172130496533507169[16] = 0.0;
   out_8172130496533507169[17] = 0.0;
   out_8172130496533507169[18] = 0.0;
   out_8172130496533507169[19] = 1.0;
   out_8172130496533507169[20] = 0.0;
   out_8172130496533507169[21] = 0.0;
   out_8172130496533507169[22] = 0.0;
   out_8172130496533507169[23] = 0.0;
   out_8172130496533507169[24] = 0.0;
   out_8172130496533507169[25] = 0.0;
   out_8172130496533507169[26] = 0.0;
   out_8172130496533507169[27] = 0.0;
   out_8172130496533507169[28] = 0.0;
   out_8172130496533507169[29] = 0.0;
   out_8172130496533507169[30] = 0.0;
   out_8172130496533507169[31] = 0.0;
   out_8172130496533507169[32] = 0.0;
   out_8172130496533507169[33] = 0.0;
   out_8172130496533507169[34] = 0.0;
   out_8172130496533507169[35] = 0.0;
   out_8172130496533507169[36] = 0.0;
   out_8172130496533507169[37] = 0.0;
   out_8172130496533507169[38] = 1.0;
   out_8172130496533507169[39] = 0.0;
   out_8172130496533507169[40] = 0.0;
   out_8172130496533507169[41] = 0.0;
   out_8172130496533507169[42] = 0.0;
   out_8172130496533507169[43] = 0.0;
   out_8172130496533507169[44] = 0.0;
   out_8172130496533507169[45] = 0.0;
   out_8172130496533507169[46] = 0.0;
   out_8172130496533507169[47] = 0.0;
   out_8172130496533507169[48] = 0.0;
   out_8172130496533507169[49] = 0.0;
   out_8172130496533507169[50] = 0.0;
   out_8172130496533507169[51] = 0.0;
   out_8172130496533507169[52] = 0.0;
   out_8172130496533507169[53] = 0.0;
   out_8172130496533507169[54] = 0.0;
   out_8172130496533507169[55] = 0.0;
   out_8172130496533507169[56] = 0.0;
   out_8172130496533507169[57] = 1.0;
   out_8172130496533507169[58] = 0.0;
   out_8172130496533507169[59] = 0.0;
   out_8172130496533507169[60] = 0.0;
   out_8172130496533507169[61] = 0.0;
   out_8172130496533507169[62] = 0.0;
   out_8172130496533507169[63] = 0.0;
   out_8172130496533507169[64] = 0.0;
   out_8172130496533507169[65] = 0.0;
   out_8172130496533507169[66] = 0.0;
   out_8172130496533507169[67] = 0.0;
   out_8172130496533507169[68] = 0.0;
   out_8172130496533507169[69] = 0.0;
   out_8172130496533507169[70] = 0.0;
   out_8172130496533507169[71] = 0.0;
   out_8172130496533507169[72] = 0.0;
   out_8172130496533507169[73] = 0.0;
   out_8172130496533507169[74] = 0.0;
   out_8172130496533507169[75] = 0.0;
   out_8172130496533507169[76] = 1.0;
   out_8172130496533507169[77] = 0.0;
   out_8172130496533507169[78] = 0.0;
   out_8172130496533507169[79] = 0.0;
   out_8172130496533507169[80] = 0.0;
   out_8172130496533507169[81] = 0.0;
   out_8172130496533507169[82] = 0.0;
   out_8172130496533507169[83] = 0.0;
   out_8172130496533507169[84] = 0.0;
   out_8172130496533507169[85] = 0.0;
   out_8172130496533507169[86] = 0.0;
   out_8172130496533507169[87] = 0.0;
   out_8172130496533507169[88] = 0.0;
   out_8172130496533507169[89] = 0.0;
   out_8172130496533507169[90] = 0.0;
   out_8172130496533507169[91] = 0.0;
   out_8172130496533507169[92] = 0.0;
   out_8172130496533507169[93] = 0.0;
   out_8172130496533507169[94] = 0.0;
   out_8172130496533507169[95] = 1.0;
   out_8172130496533507169[96] = 0.0;
   out_8172130496533507169[97] = 0.0;
   out_8172130496533507169[98] = 0.0;
   out_8172130496533507169[99] = 0.0;
   out_8172130496533507169[100] = 0.0;
   out_8172130496533507169[101] = 0.0;
   out_8172130496533507169[102] = 0.0;
   out_8172130496533507169[103] = 0.0;
   out_8172130496533507169[104] = 0.0;
   out_8172130496533507169[105] = 0.0;
   out_8172130496533507169[106] = 0.0;
   out_8172130496533507169[107] = 0.0;
   out_8172130496533507169[108] = 0.0;
   out_8172130496533507169[109] = 0.0;
   out_8172130496533507169[110] = 0.0;
   out_8172130496533507169[111] = 0.0;
   out_8172130496533507169[112] = 0.0;
   out_8172130496533507169[113] = 0.0;
   out_8172130496533507169[114] = 1.0;
   out_8172130496533507169[115] = 0.0;
   out_8172130496533507169[116] = 0.0;
   out_8172130496533507169[117] = 0.0;
   out_8172130496533507169[118] = 0.0;
   out_8172130496533507169[119] = 0.0;
   out_8172130496533507169[120] = 0.0;
   out_8172130496533507169[121] = 0.0;
   out_8172130496533507169[122] = 0.0;
   out_8172130496533507169[123] = 0.0;
   out_8172130496533507169[124] = 0.0;
   out_8172130496533507169[125] = 0.0;
   out_8172130496533507169[126] = 0.0;
   out_8172130496533507169[127] = 0.0;
   out_8172130496533507169[128] = 0.0;
   out_8172130496533507169[129] = 0.0;
   out_8172130496533507169[130] = 0.0;
   out_8172130496533507169[131] = 0.0;
   out_8172130496533507169[132] = 0.0;
   out_8172130496533507169[133] = 1.0;
   out_8172130496533507169[134] = 0.0;
   out_8172130496533507169[135] = 0.0;
   out_8172130496533507169[136] = 0.0;
   out_8172130496533507169[137] = 0.0;
   out_8172130496533507169[138] = 0.0;
   out_8172130496533507169[139] = 0.0;
   out_8172130496533507169[140] = 0.0;
   out_8172130496533507169[141] = 0.0;
   out_8172130496533507169[142] = 0.0;
   out_8172130496533507169[143] = 0.0;
   out_8172130496533507169[144] = 0.0;
   out_8172130496533507169[145] = 0.0;
   out_8172130496533507169[146] = 0.0;
   out_8172130496533507169[147] = 0.0;
   out_8172130496533507169[148] = 0.0;
   out_8172130496533507169[149] = 0.0;
   out_8172130496533507169[150] = 0.0;
   out_8172130496533507169[151] = 0.0;
   out_8172130496533507169[152] = 1.0;
   out_8172130496533507169[153] = 0.0;
   out_8172130496533507169[154] = 0.0;
   out_8172130496533507169[155] = 0.0;
   out_8172130496533507169[156] = 0.0;
   out_8172130496533507169[157] = 0.0;
   out_8172130496533507169[158] = 0.0;
   out_8172130496533507169[159] = 0.0;
   out_8172130496533507169[160] = 0.0;
   out_8172130496533507169[161] = 0.0;
   out_8172130496533507169[162] = 0.0;
   out_8172130496533507169[163] = 0.0;
   out_8172130496533507169[164] = 0.0;
   out_8172130496533507169[165] = 0.0;
   out_8172130496533507169[166] = 0.0;
   out_8172130496533507169[167] = 0.0;
   out_8172130496533507169[168] = 0.0;
   out_8172130496533507169[169] = 0.0;
   out_8172130496533507169[170] = 0.0;
   out_8172130496533507169[171] = 1.0;
   out_8172130496533507169[172] = 0.0;
   out_8172130496533507169[173] = 0.0;
   out_8172130496533507169[174] = 0.0;
   out_8172130496533507169[175] = 0.0;
   out_8172130496533507169[176] = 0.0;
   out_8172130496533507169[177] = 0.0;
   out_8172130496533507169[178] = 0.0;
   out_8172130496533507169[179] = 0.0;
   out_8172130496533507169[180] = 0.0;
   out_8172130496533507169[181] = 0.0;
   out_8172130496533507169[182] = 0.0;
   out_8172130496533507169[183] = 0.0;
   out_8172130496533507169[184] = 0.0;
   out_8172130496533507169[185] = 0.0;
   out_8172130496533507169[186] = 0.0;
   out_8172130496533507169[187] = 0.0;
   out_8172130496533507169[188] = 0.0;
   out_8172130496533507169[189] = 0.0;
   out_8172130496533507169[190] = 1.0;
   out_8172130496533507169[191] = 0.0;
   out_8172130496533507169[192] = 0.0;
   out_8172130496533507169[193] = 0.0;
   out_8172130496533507169[194] = 0.0;
   out_8172130496533507169[195] = 0.0;
   out_8172130496533507169[196] = 0.0;
   out_8172130496533507169[197] = 0.0;
   out_8172130496533507169[198] = 0.0;
   out_8172130496533507169[199] = 0.0;
   out_8172130496533507169[200] = 0.0;
   out_8172130496533507169[201] = 0.0;
   out_8172130496533507169[202] = 0.0;
   out_8172130496533507169[203] = 0.0;
   out_8172130496533507169[204] = 0.0;
   out_8172130496533507169[205] = 0.0;
   out_8172130496533507169[206] = 0.0;
   out_8172130496533507169[207] = 0.0;
   out_8172130496533507169[208] = 0.0;
   out_8172130496533507169[209] = 1.0;
   out_8172130496533507169[210] = 0.0;
   out_8172130496533507169[211] = 0.0;
   out_8172130496533507169[212] = 0.0;
   out_8172130496533507169[213] = 0.0;
   out_8172130496533507169[214] = 0.0;
   out_8172130496533507169[215] = 0.0;
   out_8172130496533507169[216] = 0.0;
   out_8172130496533507169[217] = 0.0;
   out_8172130496533507169[218] = 0.0;
   out_8172130496533507169[219] = 0.0;
   out_8172130496533507169[220] = 0.0;
   out_8172130496533507169[221] = 0.0;
   out_8172130496533507169[222] = 0.0;
   out_8172130496533507169[223] = 0.0;
   out_8172130496533507169[224] = 0.0;
   out_8172130496533507169[225] = 0.0;
   out_8172130496533507169[226] = 0.0;
   out_8172130496533507169[227] = 0.0;
   out_8172130496533507169[228] = 1.0;
   out_8172130496533507169[229] = 0.0;
   out_8172130496533507169[230] = 0.0;
   out_8172130496533507169[231] = 0.0;
   out_8172130496533507169[232] = 0.0;
   out_8172130496533507169[233] = 0.0;
   out_8172130496533507169[234] = 0.0;
   out_8172130496533507169[235] = 0.0;
   out_8172130496533507169[236] = 0.0;
   out_8172130496533507169[237] = 0.0;
   out_8172130496533507169[238] = 0.0;
   out_8172130496533507169[239] = 0.0;
   out_8172130496533507169[240] = 0.0;
   out_8172130496533507169[241] = 0.0;
   out_8172130496533507169[242] = 0.0;
   out_8172130496533507169[243] = 0.0;
   out_8172130496533507169[244] = 0.0;
   out_8172130496533507169[245] = 0.0;
   out_8172130496533507169[246] = 0.0;
   out_8172130496533507169[247] = 1.0;
   out_8172130496533507169[248] = 0.0;
   out_8172130496533507169[249] = 0.0;
   out_8172130496533507169[250] = 0.0;
   out_8172130496533507169[251] = 0.0;
   out_8172130496533507169[252] = 0.0;
   out_8172130496533507169[253] = 0.0;
   out_8172130496533507169[254] = 0.0;
   out_8172130496533507169[255] = 0.0;
   out_8172130496533507169[256] = 0.0;
   out_8172130496533507169[257] = 0.0;
   out_8172130496533507169[258] = 0.0;
   out_8172130496533507169[259] = 0.0;
   out_8172130496533507169[260] = 0.0;
   out_8172130496533507169[261] = 0.0;
   out_8172130496533507169[262] = 0.0;
   out_8172130496533507169[263] = 0.0;
   out_8172130496533507169[264] = 0.0;
   out_8172130496533507169[265] = 0.0;
   out_8172130496533507169[266] = 1.0;
   out_8172130496533507169[267] = 0.0;
   out_8172130496533507169[268] = 0.0;
   out_8172130496533507169[269] = 0.0;
   out_8172130496533507169[270] = 0.0;
   out_8172130496533507169[271] = 0.0;
   out_8172130496533507169[272] = 0.0;
   out_8172130496533507169[273] = 0.0;
   out_8172130496533507169[274] = 0.0;
   out_8172130496533507169[275] = 0.0;
   out_8172130496533507169[276] = 0.0;
   out_8172130496533507169[277] = 0.0;
   out_8172130496533507169[278] = 0.0;
   out_8172130496533507169[279] = 0.0;
   out_8172130496533507169[280] = 0.0;
   out_8172130496533507169[281] = 0.0;
   out_8172130496533507169[282] = 0.0;
   out_8172130496533507169[283] = 0.0;
   out_8172130496533507169[284] = 0.0;
   out_8172130496533507169[285] = 1.0;
   out_8172130496533507169[286] = 0.0;
   out_8172130496533507169[287] = 0.0;
   out_8172130496533507169[288] = 0.0;
   out_8172130496533507169[289] = 0.0;
   out_8172130496533507169[290] = 0.0;
   out_8172130496533507169[291] = 0.0;
   out_8172130496533507169[292] = 0.0;
   out_8172130496533507169[293] = 0.0;
   out_8172130496533507169[294] = 0.0;
   out_8172130496533507169[295] = 0.0;
   out_8172130496533507169[296] = 0.0;
   out_8172130496533507169[297] = 0.0;
   out_8172130496533507169[298] = 0.0;
   out_8172130496533507169[299] = 0.0;
   out_8172130496533507169[300] = 0.0;
   out_8172130496533507169[301] = 0.0;
   out_8172130496533507169[302] = 0.0;
   out_8172130496533507169[303] = 0.0;
   out_8172130496533507169[304] = 1.0;
   out_8172130496533507169[305] = 0.0;
   out_8172130496533507169[306] = 0.0;
   out_8172130496533507169[307] = 0.0;
   out_8172130496533507169[308] = 0.0;
   out_8172130496533507169[309] = 0.0;
   out_8172130496533507169[310] = 0.0;
   out_8172130496533507169[311] = 0.0;
   out_8172130496533507169[312] = 0.0;
   out_8172130496533507169[313] = 0.0;
   out_8172130496533507169[314] = 0.0;
   out_8172130496533507169[315] = 0.0;
   out_8172130496533507169[316] = 0.0;
   out_8172130496533507169[317] = 0.0;
   out_8172130496533507169[318] = 0.0;
   out_8172130496533507169[319] = 0.0;
   out_8172130496533507169[320] = 0.0;
   out_8172130496533507169[321] = 0.0;
   out_8172130496533507169[322] = 0.0;
   out_8172130496533507169[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_5004053374026539222) {
   out_5004053374026539222[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_5004053374026539222[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_5004053374026539222[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_5004053374026539222[3] = dt*state[12] + state[3];
   out_5004053374026539222[4] = dt*state[13] + state[4];
   out_5004053374026539222[5] = dt*state[14] + state[5];
   out_5004053374026539222[6] = state[6];
   out_5004053374026539222[7] = state[7];
   out_5004053374026539222[8] = state[8];
   out_5004053374026539222[9] = state[9];
   out_5004053374026539222[10] = state[10];
   out_5004053374026539222[11] = state[11];
   out_5004053374026539222[12] = state[12];
   out_5004053374026539222[13] = state[13];
   out_5004053374026539222[14] = state[14];
   out_5004053374026539222[15] = state[15];
   out_5004053374026539222[16] = state[16];
   out_5004053374026539222[17] = state[17];
}
void F_fun(double *state, double dt, double *out_4632470071587184948) {
   out_4632470071587184948[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_4632470071587184948[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_4632470071587184948[2] = 0;
   out_4632470071587184948[3] = 0;
   out_4632470071587184948[4] = 0;
   out_4632470071587184948[5] = 0;
   out_4632470071587184948[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_4632470071587184948[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_4632470071587184948[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_4632470071587184948[9] = 0;
   out_4632470071587184948[10] = 0;
   out_4632470071587184948[11] = 0;
   out_4632470071587184948[12] = 0;
   out_4632470071587184948[13] = 0;
   out_4632470071587184948[14] = 0;
   out_4632470071587184948[15] = 0;
   out_4632470071587184948[16] = 0;
   out_4632470071587184948[17] = 0;
   out_4632470071587184948[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_4632470071587184948[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_4632470071587184948[20] = 0;
   out_4632470071587184948[21] = 0;
   out_4632470071587184948[22] = 0;
   out_4632470071587184948[23] = 0;
   out_4632470071587184948[24] = 0;
   out_4632470071587184948[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_4632470071587184948[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_4632470071587184948[27] = 0;
   out_4632470071587184948[28] = 0;
   out_4632470071587184948[29] = 0;
   out_4632470071587184948[30] = 0;
   out_4632470071587184948[31] = 0;
   out_4632470071587184948[32] = 0;
   out_4632470071587184948[33] = 0;
   out_4632470071587184948[34] = 0;
   out_4632470071587184948[35] = 0;
   out_4632470071587184948[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_4632470071587184948[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_4632470071587184948[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_4632470071587184948[39] = 0;
   out_4632470071587184948[40] = 0;
   out_4632470071587184948[41] = 0;
   out_4632470071587184948[42] = 0;
   out_4632470071587184948[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_4632470071587184948[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_4632470071587184948[45] = 0;
   out_4632470071587184948[46] = 0;
   out_4632470071587184948[47] = 0;
   out_4632470071587184948[48] = 0;
   out_4632470071587184948[49] = 0;
   out_4632470071587184948[50] = 0;
   out_4632470071587184948[51] = 0;
   out_4632470071587184948[52] = 0;
   out_4632470071587184948[53] = 0;
   out_4632470071587184948[54] = 0;
   out_4632470071587184948[55] = 0;
   out_4632470071587184948[56] = 0;
   out_4632470071587184948[57] = 1;
   out_4632470071587184948[58] = 0;
   out_4632470071587184948[59] = 0;
   out_4632470071587184948[60] = 0;
   out_4632470071587184948[61] = 0;
   out_4632470071587184948[62] = 0;
   out_4632470071587184948[63] = 0;
   out_4632470071587184948[64] = 0;
   out_4632470071587184948[65] = 0;
   out_4632470071587184948[66] = dt;
   out_4632470071587184948[67] = 0;
   out_4632470071587184948[68] = 0;
   out_4632470071587184948[69] = 0;
   out_4632470071587184948[70] = 0;
   out_4632470071587184948[71] = 0;
   out_4632470071587184948[72] = 0;
   out_4632470071587184948[73] = 0;
   out_4632470071587184948[74] = 0;
   out_4632470071587184948[75] = 0;
   out_4632470071587184948[76] = 1;
   out_4632470071587184948[77] = 0;
   out_4632470071587184948[78] = 0;
   out_4632470071587184948[79] = 0;
   out_4632470071587184948[80] = 0;
   out_4632470071587184948[81] = 0;
   out_4632470071587184948[82] = 0;
   out_4632470071587184948[83] = 0;
   out_4632470071587184948[84] = 0;
   out_4632470071587184948[85] = dt;
   out_4632470071587184948[86] = 0;
   out_4632470071587184948[87] = 0;
   out_4632470071587184948[88] = 0;
   out_4632470071587184948[89] = 0;
   out_4632470071587184948[90] = 0;
   out_4632470071587184948[91] = 0;
   out_4632470071587184948[92] = 0;
   out_4632470071587184948[93] = 0;
   out_4632470071587184948[94] = 0;
   out_4632470071587184948[95] = 1;
   out_4632470071587184948[96] = 0;
   out_4632470071587184948[97] = 0;
   out_4632470071587184948[98] = 0;
   out_4632470071587184948[99] = 0;
   out_4632470071587184948[100] = 0;
   out_4632470071587184948[101] = 0;
   out_4632470071587184948[102] = 0;
   out_4632470071587184948[103] = 0;
   out_4632470071587184948[104] = dt;
   out_4632470071587184948[105] = 0;
   out_4632470071587184948[106] = 0;
   out_4632470071587184948[107] = 0;
   out_4632470071587184948[108] = 0;
   out_4632470071587184948[109] = 0;
   out_4632470071587184948[110] = 0;
   out_4632470071587184948[111] = 0;
   out_4632470071587184948[112] = 0;
   out_4632470071587184948[113] = 0;
   out_4632470071587184948[114] = 1;
   out_4632470071587184948[115] = 0;
   out_4632470071587184948[116] = 0;
   out_4632470071587184948[117] = 0;
   out_4632470071587184948[118] = 0;
   out_4632470071587184948[119] = 0;
   out_4632470071587184948[120] = 0;
   out_4632470071587184948[121] = 0;
   out_4632470071587184948[122] = 0;
   out_4632470071587184948[123] = 0;
   out_4632470071587184948[124] = 0;
   out_4632470071587184948[125] = 0;
   out_4632470071587184948[126] = 0;
   out_4632470071587184948[127] = 0;
   out_4632470071587184948[128] = 0;
   out_4632470071587184948[129] = 0;
   out_4632470071587184948[130] = 0;
   out_4632470071587184948[131] = 0;
   out_4632470071587184948[132] = 0;
   out_4632470071587184948[133] = 1;
   out_4632470071587184948[134] = 0;
   out_4632470071587184948[135] = 0;
   out_4632470071587184948[136] = 0;
   out_4632470071587184948[137] = 0;
   out_4632470071587184948[138] = 0;
   out_4632470071587184948[139] = 0;
   out_4632470071587184948[140] = 0;
   out_4632470071587184948[141] = 0;
   out_4632470071587184948[142] = 0;
   out_4632470071587184948[143] = 0;
   out_4632470071587184948[144] = 0;
   out_4632470071587184948[145] = 0;
   out_4632470071587184948[146] = 0;
   out_4632470071587184948[147] = 0;
   out_4632470071587184948[148] = 0;
   out_4632470071587184948[149] = 0;
   out_4632470071587184948[150] = 0;
   out_4632470071587184948[151] = 0;
   out_4632470071587184948[152] = 1;
   out_4632470071587184948[153] = 0;
   out_4632470071587184948[154] = 0;
   out_4632470071587184948[155] = 0;
   out_4632470071587184948[156] = 0;
   out_4632470071587184948[157] = 0;
   out_4632470071587184948[158] = 0;
   out_4632470071587184948[159] = 0;
   out_4632470071587184948[160] = 0;
   out_4632470071587184948[161] = 0;
   out_4632470071587184948[162] = 0;
   out_4632470071587184948[163] = 0;
   out_4632470071587184948[164] = 0;
   out_4632470071587184948[165] = 0;
   out_4632470071587184948[166] = 0;
   out_4632470071587184948[167] = 0;
   out_4632470071587184948[168] = 0;
   out_4632470071587184948[169] = 0;
   out_4632470071587184948[170] = 0;
   out_4632470071587184948[171] = 1;
   out_4632470071587184948[172] = 0;
   out_4632470071587184948[173] = 0;
   out_4632470071587184948[174] = 0;
   out_4632470071587184948[175] = 0;
   out_4632470071587184948[176] = 0;
   out_4632470071587184948[177] = 0;
   out_4632470071587184948[178] = 0;
   out_4632470071587184948[179] = 0;
   out_4632470071587184948[180] = 0;
   out_4632470071587184948[181] = 0;
   out_4632470071587184948[182] = 0;
   out_4632470071587184948[183] = 0;
   out_4632470071587184948[184] = 0;
   out_4632470071587184948[185] = 0;
   out_4632470071587184948[186] = 0;
   out_4632470071587184948[187] = 0;
   out_4632470071587184948[188] = 0;
   out_4632470071587184948[189] = 0;
   out_4632470071587184948[190] = 1;
   out_4632470071587184948[191] = 0;
   out_4632470071587184948[192] = 0;
   out_4632470071587184948[193] = 0;
   out_4632470071587184948[194] = 0;
   out_4632470071587184948[195] = 0;
   out_4632470071587184948[196] = 0;
   out_4632470071587184948[197] = 0;
   out_4632470071587184948[198] = 0;
   out_4632470071587184948[199] = 0;
   out_4632470071587184948[200] = 0;
   out_4632470071587184948[201] = 0;
   out_4632470071587184948[202] = 0;
   out_4632470071587184948[203] = 0;
   out_4632470071587184948[204] = 0;
   out_4632470071587184948[205] = 0;
   out_4632470071587184948[206] = 0;
   out_4632470071587184948[207] = 0;
   out_4632470071587184948[208] = 0;
   out_4632470071587184948[209] = 1;
   out_4632470071587184948[210] = 0;
   out_4632470071587184948[211] = 0;
   out_4632470071587184948[212] = 0;
   out_4632470071587184948[213] = 0;
   out_4632470071587184948[214] = 0;
   out_4632470071587184948[215] = 0;
   out_4632470071587184948[216] = 0;
   out_4632470071587184948[217] = 0;
   out_4632470071587184948[218] = 0;
   out_4632470071587184948[219] = 0;
   out_4632470071587184948[220] = 0;
   out_4632470071587184948[221] = 0;
   out_4632470071587184948[222] = 0;
   out_4632470071587184948[223] = 0;
   out_4632470071587184948[224] = 0;
   out_4632470071587184948[225] = 0;
   out_4632470071587184948[226] = 0;
   out_4632470071587184948[227] = 0;
   out_4632470071587184948[228] = 1;
   out_4632470071587184948[229] = 0;
   out_4632470071587184948[230] = 0;
   out_4632470071587184948[231] = 0;
   out_4632470071587184948[232] = 0;
   out_4632470071587184948[233] = 0;
   out_4632470071587184948[234] = 0;
   out_4632470071587184948[235] = 0;
   out_4632470071587184948[236] = 0;
   out_4632470071587184948[237] = 0;
   out_4632470071587184948[238] = 0;
   out_4632470071587184948[239] = 0;
   out_4632470071587184948[240] = 0;
   out_4632470071587184948[241] = 0;
   out_4632470071587184948[242] = 0;
   out_4632470071587184948[243] = 0;
   out_4632470071587184948[244] = 0;
   out_4632470071587184948[245] = 0;
   out_4632470071587184948[246] = 0;
   out_4632470071587184948[247] = 1;
   out_4632470071587184948[248] = 0;
   out_4632470071587184948[249] = 0;
   out_4632470071587184948[250] = 0;
   out_4632470071587184948[251] = 0;
   out_4632470071587184948[252] = 0;
   out_4632470071587184948[253] = 0;
   out_4632470071587184948[254] = 0;
   out_4632470071587184948[255] = 0;
   out_4632470071587184948[256] = 0;
   out_4632470071587184948[257] = 0;
   out_4632470071587184948[258] = 0;
   out_4632470071587184948[259] = 0;
   out_4632470071587184948[260] = 0;
   out_4632470071587184948[261] = 0;
   out_4632470071587184948[262] = 0;
   out_4632470071587184948[263] = 0;
   out_4632470071587184948[264] = 0;
   out_4632470071587184948[265] = 0;
   out_4632470071587184948[266] = 1;
   out_4632470071587184948[267] = 0;
   out_4632470071587184948[268] = 0;
   out_4632470071587184948[269] = 0;
   out_4632470071587184948[270] = 0;
   out_4632470071587184948[271] = 0;
   out_4632470071587184948[272] = 0;
   out_4632470071587184948[273] = 0;
   out_4632470071587184948[274] = 0;
   out_4632470071587184948[275] = 0;
   out_4632470071587184948[276] = 0;
   out_4632470071587184948[277] = 0;
   out_4632470071587184948[278] = 0;
   out_4632470071587184948[279] = 0;
   out_4632470071587184948[280] = 0;
   out_4632470071587184948[281] = 0;
   out_4632470071587184948[282] = 0;
   out_4632470071587184948[283] = 0;
   out_4632470071587184948[284] = 0;
   out_4632470071587184948[285] = 1;
   out_4632470071587184948[286] = 0;
   out_4632470071587184948[287] = 0;
   out_4632470071587184948[288] = 0;
   out_4632470071587184948[289] = 0;
   out_4632470071587184948[290] = 0;
   out_4632470071587184948[291] = 0;
   out_4632470071587184948[292] = 0;
   out_4632470071587184948[293] = 0;
   out_4632470071587184948[294] = 0;
   out_4632470071587184948[295] = 0;
   out_4632470071587184948[296] = 0;
   out_4632470071587184948[297] = 0;
   out_4632470071587184948[298] = 0;
   out_4632470071587184948[299] = 0;
   out_4632470071587184948[300] = 0;
   out_4632470071587184948[301] = 0;
   out_4632470071587184948[302] = 0;
   out_4632470071587184948[303] = 0;
   out_4632470071587184948[304] = 1;
   out_4632470071587184948[305] = 0;
   out_4632470071587184948[306] = 0;
   out_4632470071587184948[307] = 0;
   out_4632470071587184948[308] = 0;
   out_4632470071587184948[309] = 0;
   out_4632470071587184948[310] = 0;
   out_4632470071587184948[311] = 0;
   out_4632470071587184948[312] = 0;
   out_4632470071587184948[313] = 0;
   out_4632470071587184948[314] = 0;
   out_4632470071587184948[315] = 0;
   out_4632470071587184948[316] = 0;
   out_4632470071587184948[317] = 0;
   out_4632470071587184948[318] = 0;
   out_4632470071587184948[319] = 0;
   out_4632470071587184948[320] = 0;
   out_4632470071587184948[321] = 0;
   out_4632470071587184948[322] = 0;
   out_4632470071587184948[323] = 1;
}
void h_4(double *state, double *unused, double *out_5642004684309586938) {
   out_5642004684309586938[0] = state[6] + state[9];
   out_5642004684309586938[1] = state[7] + state[10];
   out_5642004684309586938[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_2173071738203631897) {
   out_2173071738203631897[0] = 0;
   out_2173071738203631897[1] = 0;
   out_2173071738203631897[2] = 0;
   out_2173071738203631897[3] = 0;
   out_2173071738203631897[4] = 0;
   out_2173071738203631897[5] = 0;
   out_2173071738203631897[6] = 1;
   out_2173071738203631897[7] = 0;
   out_2173071738203631897[8] = 0;
   out_2173071738203631897[9] = 1;
   out_2173071738203631897[10] = 0;
   out_2173071738203631897[11] = 0;
   out_2173071738203631897[12] = 0;
   out_2173071738203631897[13] = 0;
   out_2173071738203631897[14] = 0;
   out_2173071738203631897[15] = 0;
   out_2173071738203631897[16] = 0;
   out_2173071738203631897[17] = 0;
   out_2173071738203631897[18] = 0;
   out_2173071738203631897[19] = 0;
   out_2173071738203631897[20] = 0;
   out_2173071738203631897[21] = 0;
   out_2173071738203631897[22] = 0;
   out_2173071738203631897[23] = 0;
   out_2173071738203631897[24] = 0;
   out_2173071738203631897[25] = 1;
   out_2173071738203631897[26] = 0;
   out_2173071738203631897[27] = 0;
   out_2173071738203631897[28] = 1;
   out_2173071738203631897[29] = 0;
   out_2173071738203631897[30] = 0;
   out_2173071738203631897[31] = 0;
   out_2173071738203631897[32] = 0;
   out_2173071738203631897[33] = 0;
   out_2173071738203631897[34] = 0;
   out_2173071738203631897[35] = 0;
   out_2173071738203631897[36] = 0;
   out_2173071738203631897[37] = 0;
   out_2173071738203631897[38] = 0;
   out_2173071738203631897[39] = 0;
   out_2173071738203631897[40] = 0;
   out_2173071738203631897[41] = 0;
   out_2173071738203631897[42] = 0;
   out_2173071738203631897[43] = 0;
   out_2173071738203631897[44] = 1;
   out_2173071738203631897[45] = 0;
   out_2173071738203631897[46] = 0;
   out_2173071738203631897[47] = 1;
   out_2173071738203631897[48] = 0;
   out_2173071738203631897[49] = 0;
   out_2173071738203631897[50] = 0;
   out_2173071738203631897[51] = 0;
   out_2173071738203631897[52] = 0;
   out_2173071738203631897[53] = 0;
}
void h_10(double *state, double *unused, double *out_6635142105328933338) {
   out_6635142105328933338[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_6635142105328933338[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_6635142105328933338[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_8640756480625150988) {
   out_8640756480625150988[0] = 0;
   out_8640756480625150988[1] = 9.8100000000000005*cos(state[1]);
   out_8640756480625150988[2] = 0;
   out_8640756480625150988[3] = 0;
   out_8640756480625150988[4] = -state[8];
   out_8640756480625150988[5] = state[7];
   out_8640756480625150988[6] = 0;
   out_8640756480625150988[7] = state[5];
   out_8640756480625150988[8] = -state[4];
   out_8640756480625150988[9] = 0;
   out_8640756480625150988[10] = 0;
   out_8640756480625150988[11] = 0;
   out_8640756480625150988[12] = 1;
   out_8640756480625150988[13] = 0;
   out_8640756480625150988[14] = 0;
   out_8640756480625150988[15] = 1;
   out_8640756480625150988[16] = 0;
   out_8640756480625150988[17] = 0;
   out_8640756480625150988[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_8640756480625150988[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_8640756480625150988[20] = 0;
   out_8640756480625150988[21] = state[8];
   out_8640756480625150988[22] = 0;
   out_8640756480625150988[23] = -state[6];
   out_8640756480625150988[24] = -state[5];
   out_8640756480625150988[25] = 0;
   out_8640756480625150988[26] = state[3];
   out_8640756480625150988[27] = 0;
   out_8640756480625150988[28] = 0;
   out_8640756480625150988[29] = 0;
   out_8640756480625150988[30] = 0;
   out_8640756480625150988[31] = 1;
   out_8640756480625150988[32] = 0;
   out_8640756480625150988[33] = 0;
   out_8640756480625150988[34] = 1;
   out_8640756480625150988[35] = 0;
   out_8640756480625150988[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_8640756480625150988[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_8640756480625150988[38] = 0;
   out_8640756480625150988[39] = -state[7];
   out_8640756480625150988[40] = state[6];
   out_8640756480625150988[41] = 0;
   out_8640756480625150988[42] = state[4];
   out_8640756480625150988[43] = -state[3];
   out_8640756480625150988[44] = 0;
   out_8640756480625150988[45] = 0;
   out_8640756480625150988[46] = 0;
   out_8640756480625150988[47] = 0;
   out_8640756480625150988[48] = 0;
   out_8640756480625150988[49] = 0;
   out_8640756480625150988[50] = 1;
   out_8640756480625150988[51] = 0;
   out_8640756480625150988[52] = 0;
   out_8640756480625150988[53] = 1;
}
void h_13(double *state, double *unused, double *out_2521909184006486810) {
   out_2521909184006486810[0] = state[3];
   out_2521909184006486810[1] = state[4];
   out_2521909184006486810[2] = state[5];
}
void H_13(double *state, double *unused, double *out_5385345563535964698) {
   out_5385345563535964698[0] = 0;
   out_5385345563535964698[1] = 0;
   out_5385345563535964698[2] = 0;
   out_5385345563535964698[3] = 1;
   out_5385345563535964698[4] = 0;
   out_5385345563535964698[5] = 0;
   out_5385345563535964698[6] = 0;
   out_5385345563535964698[7] = 0;
   out_5385345563535964698[8] = 0;
   out_5385345563535964698[9] = 0;
   out_5385345563535964698[10] = 0;
   out_5385345563535964698[11] = 0;
   out_5385345563535964698[12] = 0;
   out_5385345563535964698[13] = 0;
   out_5385345563535964698[14] = 0;
   out_5385345563535964698[15] = 0;
   out_5385345563535964698[16] = 0;
   out_5385345563535964698[17] = 0;
   out_5385345563535964698[18] = 0;
   out_5385345563535964698[19] = 0;
   out_5385345563535964698[20] = 0;
   out_5385345563535964698[21] = 0;
   out_5385345563535964698[22] = 1;
   out_5385345563535964698[23] = 0;
   out_5385345563535964698[24] = 0;
   out_5385345563535964698[25] = 0;
   out_5385345563535964698[26] = 0;
   out_5385345563535964698[27] = 0;
   out_5385345563535964698[28] = 0;
   out_5385345563535964698[29] = 0;
   out_5385345563535964698[30] = 0;
   out_5385345563535964698[31] = 0;
   out_5385345563535964698[32] = 0;
   out_5385345563535964698[33] = 0;
   out_5385345563535964698[34] = 0;
   out_5385345563535964698[35] = 0;
   out_5385345563535964698[36] = 0;
   out_5385345563535964698[37] = 0;
   out_5385345563535964698[38] = 0;
   out_5385345563535964698[39] = 0;
   out_5385345563535964698[40] = 0;
   out_5385345563535964698[41] = 1;
   out_5385345563535964698[42] = 0;
   out_5385345563535964698[43] = 0;
   out_5385345563535964698[44] = 0;
   out_5385345563535964698[45] = 0;
   out_5385345563535964698[46] = 0;
   out_5385345563535964698[47] = 0;
   out_5385345563535964698[48] = 0;
   out_5385345563535964698[49] = 0;
   out_5385345563535964698[50] = 0;
   out_5385345563535964698[51] = 0;
   out_5385345563535964698[52] = 0;
   out_5385345563535964698[53] = 0;
}
void h_14(double *state, double *unused, double *out_1314376768125748605) {
   out_1314376768125748605[0] = state[6];
   out_1314376768125748605[1] = state[7];
   out_1314376768125748605[2] = state[8];
}
void H_14(double *state, double *unused, double *out_6136312594543116426) {
   out_6136312594543116426[0] = 0;
   out_6136312594543116426[1] = 0;
   out_6136312594543116426[2] = 0;
   out_6136312594543116426[3] = 0;
   out_6136312594543116426[4] = 0;
   out_6136312594543116426[5] = 0;
   out_6136312594543116426[6] = 1;
   out_6136312594543116426[7] = 0;
   out_6136312594543116426[8] = 0;
   out_6136312594543116426[9] = 0;
   out_6136312594543116426[10] = 0;
   out_6136312594543116426[11] = 0;
   out_6136312594543116426[12] = 0;
   out_6136312594543116426[13] = 0;
   out_6136312594543116426[14] = 0;
   out_6136312594543116426[15] = 0;
   out_6136312594543116426[16] = 0;
   out_6136312594543116426[17] = 0;
   out_6136312594543116426[18] = 0;
   out_6136312594543116426[19] = 0;
   out_6136312594543116426[20] = 0;
   out_6136312594543116426[21] = 0;
   out_6136312594543116426[22] = 0;
   out_6136312594543116426[23] = 0;
   out_6136312594543116426[24] = 0;
   out_6136312594543116426[25] = 1;
   out_6136312594543116426[26] = 0;
   out_6136312594543116426[27] = 0;
   out_6136312594543116426[28] = 0;
   out_6136312594543116426[29] = 0;
   out_6136312594543116426[30] = 0;
   out_6136312594543116426[31] = 0;
   out_6136312594543116426[32] = 0;
   out_6136312594543116426[33] = 0;
   out_6136312594543116426[34] = 0;
   out_6136312594543116426[35] = 0;
   out_6136312594543116426[36] = 0;
   out_6136312594543116426[37] = 0;
   out_6136312594543116426[38] = 0;
   out_6136312594543116426[39] = 0;
   out_6136312594543116426[40] = 0;
   out_6136312594543116426[41] = 0;
   out_6136312594543116426[42] = 0;
   out_6136312594543116426[43] = 0;
   out_6136312594543116426[44] = 1;
   out_6136312594543116426[45] = 0;
   out_6136312594543116426[46] = 0;
   out_6136312594543116426[47] = 0;
   out_6136312594543116426[48] = 0;
   out_6136312594543116426[49] = 0;
   out_6136312594543116426[50] = 0;
   out_6136312594543116426[51] = 0;
   out_6136312594543116426[52] = 0;
   out_6136312594543116426[53] = 0;
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
void pose_err_fun(double *nom_x, double *delta_x, double *out_4013660299866450489) {
  err_fun(nom_x, delta_x, out_4013660299866450489);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_4438646948403156526) {
  inv_err_fun(nom_x, true_x, out_4438646948403156526);
}
void pose_H_mod_fun(double *state, double *out_8172130496533507169) {
  H_mod_fun(state, out_8172130496533507169);
}
void pose_f_fun(double *state, double dt, double *out_5004053374026539222) {
  f_fun(state,  dt, out_5004053374026539222);
}
void pose_F_fun(double *state, double dt, double *out_4632470071587184948) {
  F_fun(state,  dt, out_4632470071587184948);
}
void pose_h_4(double *state, double *unused, double *out_5642004684309586938) {
  h_4(state, unused, out_5642004684309586938);
}
void pose_H_4(double *state, double *unused, double *out_2173071738203631897) {
  H_4(state, unused, out_2173071738203631897);
}
void pose_h_10(double *state, double *unused, double *out_6635142105328933338) {
  h_10(state, unused, out_6635142105328933338);
}
void pose_H_10(double *state, double *unused, double *out_8640756480625150988) {
  H_10(state, unused, out_8640756480625150988);
}
void pose_h_13(double *state, double *unused, double *out_2521909184006486810) {
  h_13(state, unused, out_2521909184006486810);
}
void pose_H_13(double *state, double *unused, double *out_5385345563535964698) {
  H_13(state, unused, out_5385345563535964698);
}
void pose_h_14(double *state, double *unused, double *out_1314376768125748605) {
  h_14(state, unused, out_1314376768125748605);
}
void pose_H_14(double *state, double *unused, double *out_6136312594543116426) {
  H_14(state, unused, out_6136312594543116426);
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
