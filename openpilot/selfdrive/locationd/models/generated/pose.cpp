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
void err_fun(double *nom_x, double *delta_x, double *out_1586140269824446038) {
   out_1586140269824446038[0] = delta_x[0] + nom_x[0];
   out_1586140269824446038[1] = delta_x[1] + nom_x[1];
   out_1586140269824446038[2] = delta_x[2] + nom_x[2];
   out_1586140269824446038[3] = delta_x[3] + nom_x[3];
   out_1586140269824446038[4] = delta_x[4] + nom_x[4];
   out_1586140269824446038[5] = delta_x[5] + nom_x[5];
   out_1586140269824446038[6] = delta_x[6] + nom_x[6];
   out_1586140269824446038[7] = delta_x[7] + nom_x[7];
   out_1586140269824446038[8] = delta_x[8] + nom_x[8];
   out_1586140269824446038[9] = delta_x[9] + nom_x[9];
   out_1586140269824446038[10] = delta_x[10] + nom_x[10];
   out_1586140269824446038[11] = delta_x[11] + nom_x[11];
   out_1586140269824446038[12] = delta_x[12] + nom_x[12];
   out_1586140269824446038[13] = delta_x[13] + nom_x[13];
   out_1586140269824446038[14] = delta_x[14] + nom_x[14];
   out_1586140269824446038[15] = delta_x[15] + nom_x[15];
   out_1586140269824446038[16] = delta_x[16] + nom_x[16];
   out_1586140269824446038[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_9166849630934128431) {
   out_9166849630934128431[0] = -nom_x[0] + true_x[0];
   out_9166849630934128431[1] = -nom_x[1] + true_x[1];
   out_9166849630934128431[2] = -nom_x[2] + true_x[2];
   out_9166849630934128431[3] = -nom_x[3] + true_x[3];
   out_9166849630934128431[4] = -nom_x[4] + true_x[4];
   out_9166849630934128431[5] = -nom_x[5] + true_x[5];
   out_9166849630934128431[6] = -nom_x[6] + true_x[6];
   out_9166849630934128431[7] = -nom_x[7] + true_x[7];
   out_9166849630934128431[8] = -nom_x[8] + true_x[8];
   out_9166849630934128431[9] = -nom_x[9] + true_x[9];
   out_9166849630934128431[10] = -nom_x[10] + true_x[10];
   out_9166849630934128431[11] = -nom_x[11] + true_x[11];
   out_9166849630934128431[12] = -nom_x[12] + true_x[12];
   out_9166849630934128431[13] = -nom_x[13] + true_x[13];
   out_9166849630934128431[14] = -nom_x[14] + true_x[14];
   out_9166849630934128431[15] = -nom_x[15] + true_x[15];
   out_9166849630934128431[16] = -nom_x[16] + true_x[16];
   out_9166849630934128431[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_1085718029145294855) {
   out_1085718029145294855[0] = 1.0;
   out_1085718029145294855[1] = 0.0;
   out_1085718029145294855[2] = 0.0;
   out_1085718029145294855[3] = 0.0;
   out_1085718029145294855[4] = 0.0;
   out_1085718029145294855[5] = 0.0;
   out_1085718029145294855[6] = 0.0;
   out_1085718029145294855[7] = 0.0;
   out_1085718029145294855[8] = 0.0;
   out_1085718029145294855[9] = 0.0;
   out_1085718029145294855[10] = 0.0;
   out_1085718029145294855[11] = 0.0;
   out_1085718029145294855[12] = 0.0;
   out_1085718029145294855[13] = 0.0;
   out_1085718029145294855[14] = 0.0;
   out_1085718029145294855[15] = 0.0;
   out_1085718029145294855[16] = 0.0;
   out_1085718029145294855[17] = 0.0;
   out_1085718029145294855[18] = 0.0;
   out_1085718029145294855[19] = 1.0;
   out_1085718029145294855[20] = 0.0;
   out_1085718029145294855[21] = 0.0;
   out_1085718029145294855[22] = 0.0;
   out_1085718029145294855[23] = 0.0;
   out_1085718029145294855[24] = 0.0;
   out_1085718029145294855[25] = 0.0;
   out_1085718029145294855[26] = 0.0;
   out_1085718029145294855[27] = 0.0;
   out_1085718029145294855[28] = 0.0;
   out_1085718029145294855[29] = 0.0;
   out_1085718029145294855[30] = 0.0;
   out_1085718029145294855[31] = 0.0;
   out_1085718029145294855[32] = 0.0;
   out_1085718029145294855[33] = 0.0;
   out_1085718029145294855[34] = 0.0;
   out_1085718029145294855[35] = 0.0;
   out_1085718029145294855[36] = 0.0;
   out_1085718029145294855[37] = 0.0;
   out_1085718029145294855[38] = 1.0;
   out_1085718029145294855[39] = 0.0;
   out_1085718029145294855[40] = 0.0;
   out_1085718029145294855[41] = 0.0;
   out_1085718029145294855[42] = 0.0;
   out_1085718029145294855[43] = 0.0;
   out_1085718029145294855[44] = 0.0;
   out_1085718029145294855[45] = 0.0;
   out_1085718029145294855[46] = 0.0;
   out_1085718029145294855[47] = 0.0;
   out_1085718029145294855[48] = 0.0;
   out_1085718029145294855[49] = 0.0;
   out_1085718029145294855[50] = 0.0;
   out_1085718029145294855[51] = 0.0;
   out_1085718029145294855[52] = 0.0;
   out_1085718029145294855[53] = 0.0;
   out_1085718029145294855[54] = 0.0;
   out_1085718029145294855[55] = 0.0;
   out_1085718029145294855[56] = 0.0;
   out_1085718029145294855[57] = 1.0;
   out_1085718029145294855[58] = 0.0;
   out_1085718029145294855[59] = 0.0;
   out_1085718029145294855[60] = 0.0;
   out_1085718029145294855[61] = 0.0;
   out_1085718029145294855[62] = 0.0;
   out_1085718029145294855[63] = 0.0;
   out_1085718029145294855[64] = 0.0;
   out_1085718029145294855[65] = 0.0;
   out_1085718029145294855[66] = 0.0;
   out_1085718029145294855[67] = 0.0;
   out_1085718029145294855[68] = 0.0;
   out_1085718029145294855[69] = 0.0;
   out_1085718029145294855[70] = 0.0;
   out_1085718029145294855[71] = 0.0;
   out_1085718029145294855[72] = 0.0;
   out_1085718029145294855[73] = 0.0;
   out_1085718029145294855[74] = 0.0;
   out_1085718029145294855[75] = 0.0;
   out_1085718029145294855[76] = 1.0;
   out_1085718029145294855[77] = 0.0;
   out_1085718029145294855[78] = 0.0;
   out_1085718029145294855[79] = 0.0;
   out_1085718029145294855[80] = 0.0;
   out_1085718029145294855[81] = 0.0;
   out_1085718029145294855[82] = 0.0;
   out_1085718029145294855[83] = 0.0;
   out_1085718029145294855[84] = 0.0;
   out_1085718029145294855[85] = 0.0;
   out_1085718029145294855[86] = 0.0;
   out_1085718029145294855[87] = 0.0;
   out_1085718029145294855[88] = 0.0;
   out_1085718029145294855[89] = 0.0;
   out_1085718029145294855[90] = 0.0;
   out_1085718029145294855[91] = 0.0;
   out_1085718029145294855[92] = 0.0;
   out_1085718029145294855[93] = 0.0;
   out_1085718029145294855[94] = 0.0;
   out_1085718029145294855[95] = 1.0;
   out_1085718029145294855[96] = 0.0;
   out_1085718029145294855[97] = 0.0;
   out_1085718029145294855[98] = 0.0;
   out_1085718029145294855[99] = 0.0;
   out_1085718029145294855[100] = 0.0;
   out_1085718029145294855[101] = 0.0;
   out_1085718029145294855[102] = 0.0;
   out_1085718029145294855[103] = 0.0;
   out_1085718029145294855[104] = 0.0;
   out_1085718029145294855[105] = 0.0;
   out_1085718029145294855[106] = 0.0;
   out_1085718029145294855[107] = 0.0;
   out_1085718029145294855[108] = 0.0;
   out_1085718029145294855[109] = 0.0;
   out_1085718029145294855[110] = 0.0;
   out_1085718029145294855[111] = 0.0;
   out_1085718029145294855[112] = 0.0;
   out_1085718029145294855[113] = 0.0;
   out_1085718029145294855[114] = 1.0;
   out_1085718029145294855[115] = 0.0;
   out_1085718029145294855[116] = 0.0;
   out_1085718029145294855[117] = 0.0;
   out_1085718029145294855[118] = 0.0;
   out_1085718029145294855[119] = 0.0;
   out_1085718029145294855[120] = 0.0;
   out_1085718029145294855[121] = 0.0;
   out_1085718029145294855[122] = 0.0;
   out_1085718029145294855[123] = 0.0;
   out_1085718029145294855[124] = 0.0;
   out_1085718029145294855[125] = 0.0;
   out_1085718029145294855[126] = 0.0;
   out_1085718029145294855[127] = 0.0;
   out_1085718029145294855[128] = 0.0;
   out_1085718029145294855[129] = 0.0;
   out_1085718029145294855[130] = 0.0;
   out_1085718029145294855[131] = 0.0;
   out_1085718029145294855[132] = 0.0;
   out_1085718029145294855[133] = 1.0;
   out_1085718029145294855[134] = 0.0;
   out_1085718029145294855[135] = 0.0;
   out_1085718029145294855[136] = 0.0;
   out_1085718029145294855[137] = 0.0;
   out_1085718029145294855[138] = 0.0;
   out_1085718029145294855[139] = 0.0;
   out_1085718029145294855[140] = 0.0;
   out_1085718029145294855[141] = 0.0;
   out_1085718029145294855[142] = 0.0;
   out_1085718029145294855[143] = 0.0;
   out_1085718029145294855[144] = 0.0;
   out_1085718029145294855[145] = 0.0;
   out_1085718029145294855[146] = 0.0;
   out_1085718029145294855[147] = 0.0;
   out_1085718029145294855[148] = 0.0;
   out_1085718029145294855[149] = 0.0;
   out_1085718029145294855[150] = 0.0;
   out_1085718029145294855[151] = 0.0;
   out_1085718029145294855[152] = 1.0;
   out_1085718029145294855[153] = 0.0;
   out_1085718029145294855[154] = 0.0;
   out_1085718029145294855[155] = 0.0;
   out_1085718029145294855[156] = 0.0;
   out_1085718029145294855[157] = 0.0;
   out_1085718029145294855[158] = 0.0;
   out_1085718029145294855[159] = 0.0;
   out_1085718029145294855[160] = 0.0;
   out_1085718029145294855[161] = 0.0;
   out_1085718029145294855[162] = 0.0;
   out_1085718029145294855[163] = 0.0;
   out_1085718029145294855[164] = 0.0;
   out_1085718029145294855[165] = 0.0;
   out_1085718029145294855[166] = 0.0;
   out_1085718029145294855[167] = 0.0;
   out_1085718029145294855[168] = 0.0;
   out_1085718029145294855[169] = 0.0;
   out_1085718029145294855[170] = 0.0;
   out_1085718029145294855[171] = 1.0;
   out_1085718029145294855[172] = 0.0;
   out_1085718029145294855[173] = 0.0;
   out_1085718029145294855[174] = 0.0;
   out_1085718029145294855[175] = 0.0;
   out_1085718029145294855[176] = 0.0;
   out_1085718029145294855[177] = 0.0;
   out_1085718029145294855[178] = 0.0;
   out_1085718029145294855[179] = 0.0;
   out_1085718029145294855[180] = 0.0;
   out_1085718029145294855[181] = 0.0;
   out_1085718029145294855[182] = 0.0;
   out_1085718029145294855[183] = 0.0;
   out_1085718029145294855[184] = 0.0;
   out_1085718029145294855[185] = 0.0;
   out_1085718029145294855[186] = 0.0;
   out_1085718029145294855[187] = 0.0;
   out_1085718029145294855[188] = 0.0;
   out_1085718029145294855[189] = 0.0;
   out_1085718029145294855[190] = 1.0;
   out_1085718029145294855[191] = 0.0;
   out_1085718029145294855[192] = 0.0;
   out_1085718029145294855[193] = 0.0;
   out_1085718029145294855[194] = 0.0;
   out_1085718029145294855[195] = 0.0;
   out_1085718029145294855[196] = 0.0;
   out_1085718029145294855[197] = 0.0;
   out_1085718029145294855[198] = 0.0;
   out_1085718029145294855[199] = 0.0;
   out_1085718029145294855[200] = 0.0;
   out_1085718029145294855[201] = 0.0;
   out_1085718029145294855[202] = 0.0;
   out_1085718029145294855[203] = 0.0;
   out_1085718029145294855[204] = 0.0;
   out_1085718029145294855[205] = 0.0;
   out_1085718029145294855[206] = 0.0;
   out_1085718029145294855[207] = 0.0;
   out_1085718029145294855[208] = 0.0;
   out_1085718029145294855[209] = 1.0;
   out_1085718029145294855[210] = 0.0;
   out_1085718029145294855[211] = 0.0;
   out_1085718029145294855[212] = 0.0;
   out_1085718029145294855[213] = 0.0;
   out_1085718029145294855[214] = 0.0;
   out_1085718029145294855[215] = 0.0;
   out_1085718029145294855[216] = 0.0;
   out_1085718029145294855[217] = 0.0;
   out_1085718029145294855[218] = 0.0;
   out_1085718029145294855[219] = 0.0;
   out_1085718029145294855[220] = 0.0;
   out_1085718029145294855[221] = 0.0;
   out_1085718029145294855[222] = 0.0;
   out_1085718029145294855[223] = 0.0;
   out_1085718029145294855[224] = 0.0;
   out_1085718029145294855[225] = 0.0;
   out_1085718029145294855[226] = 0.0;
   out_1085718029145294855[227] = 0.0;
   out_1085718029145294855[228] = 1.0;
   out_1085718029145294855[229] = 0.0;
   out_1085718029145294855[230] = 0.0;
   out_1085718029145294855[231] = 0.0;
   out_1085718029145294855[232] = 0.0;
   out_1085718029145294855[233] = 0.0;
   out_1085718029145294855[234] = 0.0;
   out_1085718029145294855[235] = 0.0;
   out_1085718029145294855[236] = 0.0;
   out_1085718029145294855[237] = 0.0;
   out_1085718029145294855[238] = 0.0;
   out_1085718029145294855[239] = 0.0;
   out_1085718029145294855[240] = 0.0;
   out_1085718029145294855[241] = 0.0;
   out_1085718029145294855[242] = 0.0;
   out_1085718029145294855[243] = 0.0;
   out_1085718029145294855[244] = 0.0;
   out_1085718029145294855[245] = 0.0;
   out_1085718029145294855[246] = 0.0;
   out_1085718029145294855[247] = 1.0;
   out_1085718029145294855[248] = 0.0;
   out_1085718029145294855[249] = 0.0;
   out_1085718029145294855[250] = 0.0;
   out_1085718029145294855[251] = 0.0;
   out_1085718029145294855[252] = 0.0;
   out_1085718029145294855[253] = 0.0;
   out_1085718029145294855[254] = 0.0;
   out_1085718029145294855[255] = 0.0;
   out_1085718029145294855[256] = 0.0;
   out_1085718029145294855[257] = 0.0;
   out_1085718029145294855[258] = 0.0;
   out_1085718029145294855[259] = 0.0;
   out_1085718029145294855[260] = 0.0;
   out_1085718029145294855[261] = 0.0;
   out_1085718029145294855[262] = 0.0;
   out_1085718029145294855[263] = 0.0;
   out_1085718029145294855[264] = 0.0;
   out_1085718029145294855[265] = 0.0;
   out_1085718029145294855[266] = 1.0;
   out_1085718029145294855[267] = 0.0;
   out_1085718029145294855[268] = 0.0;
   out_1085718029145294855[269] = 0.0;
   out_1085718029145294855[270] = 0.0;
   out_1085718029145294855[271] = 0.0;
   out_1085718029145294855[272] = 0.0;
   out_1085718029145294855[273] = 0.0;
   out_1085718029145294855[274] = 0.0;
   out_1085718029145294855[275] = 0.0;
   out_1085718029145294855[276] = 0.0;
   out_1085718029145294855[277] = 0.0;
   out_1085718029145294855[278] = 0.0;
   out_1085718029145294855[279] = 0.0;
   out_1085718029145294855[280] = 0.0;
   out_1085718029145294855[281] = 0.0;
   out_1085718029145294855[282] = 0.0;
   out_1085718029145294855[283] = 0.0;
   out_1085718029145294855[284] = 0.0;
   out_1085718029145294855[285] = 1.0;
   out_1085718029145294855[286] = 0.0;
   out_1085718029145294855[287] = 0.0;
   out_1085718029145294855[288] = 0.0;
   out_1085718029145294855[289] = 0.0;
   out_1085718029145294855[290] = 0.0;
   out_1085718029145294855[291] = 0.0;
   out_1085718029145294855[292] = 0.0;
   out_1085718029145294855[293] = 0.0;
   out_1085718029145294855[294] = 0.0;
   out_1085718029145294855[295] = 0.0;
   out_1085718029145294855[296] = 0.0;
   out_1085718029145294855[297] = 0.0;
   out_1085718029145294855[298] = 0.0;
   out_1085718029145294855[299] = 0.0;
   out_1085718029145294855[300] = 0.0;
   out_1085718029145294855[301] = 0.0;
   out_1085718029145294855[302] = 0.0;
   out_1085718029145294855[303] = 0.0;
   out_1085718029145294855[304] = 1.0;
   out_1085718029145294855[305] = 0.0;
   out_1085718029145294855[306] = 0.0;
   out_1085718029145294855[307] = 0.0;
   out_1085718029145294855[308] = 0.0;
   out_1085718029145294855[309] = 0.0;
   out_1085718029145294855[310] = 0.0;
   out_1085718029145294855[311] = 0.0;
   out_1085718029145294855[312] = 0.0;
   out_1085718029145294855[313] = 0.0;
   out_1085718029145294855[314] = 0.0;
   out_1085718029145294855[315] = 0.0;
   out_1085718029145294855[316] = 0.0;
   out_1085718029145294855[317] = 0.0;
   out_1085718029145294855[318] = 0.0;
   out_1085718029145294855[319] = 0.0;
   out_1085718029145294855[320] = 0.0;
   out_1085718029145294855[321] = 0.0;
   out_1085718029145294855[322] = 0.0;
   out_1085718029145294855[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_1181361488543532072) {
   out_1181361488543532072[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_1181361488543532072[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_1181361488543532072[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_1181361488543532072[3] = dt*state[12] + state[3];
   out_1181361488543532072[4] = dt*state[13] + state[4];
   out_1181361488543532072[5] = dt*state[14] + state[5];
   out_1181361488543532072[6] = state[6];
   out_1181361488543532072[7] = state[7];
   out_1181361488543532072[8] = state[8];
   out_1181361488543532072[9] = state[9];
   out_1181361488543532072[10] = state[10];
   out_1181361488543532072[11] = state[11];
   out_1181361488543532072[12] = state[12];
   out_1181361488543532072[13] = state[13];
   out_1181361488543532072[14] = state[14];
   out_1181361488543532072[15] = state[15];
   out_1181361488543532072[16] = state[16];
   out_1181361488543532072[17] = state[17];
}
void F_fun(double *state, double dt, double *out_6679695245946987401) {
   out_6679695245946987401[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6679695245946987401[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6679695245946987401[2] = 0;
   out_6679695245946987401[3] = 0;
   out_6679695245946987401[4] = 0;
   out_6679695245946987401[5] = 0;
   out_6679695245946987401[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6679695245946987401[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6679695245946987401[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6679695245946987401[9] = 0;
   out_6679695245946987401[10] = 0;
   out_6679695245946987401[11] = 0;
   out_6679695245946987401[12] = 0;
   out_6679695245946987401[13] = 0;
   out_6679695245946987401[14] = 0;
   out_6679695245946987401[15] = 0;
   out_6679695245946987401[16] = 0;
   out_6679695245946987401[17] = 0;
   out_6679695245946987401[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6679695245946987401[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6679695245946987401[20] = 0;
   out_6679695245946987401[21] = 0;
   out_6679695245946987401[22] = 0;
   out_6679695245946987401[23] = 0;
   out_6679695245946987401[24] = 0;
   out_6679695245946987401[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6679695245946987401[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6679695245946987401[27] = 0;
   out_6679695245946987401[28] = 0;
   out_6679695245946987401[29] = 0;
   out_6679695245946987401[30] = 0;
   out_6679695245946987401[31] = 0;
   out_6679695245946987401[32] = 0;
   out_6679695245946987401[33] = 0;
   out_6679695245946987401[34] = 0;
   out_6679695245946987401[35] = 0;
   out_6679695245946987401[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6679695245946987401[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6679695245946987401[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6679695245946987401[39] = 0;
   out_6679695245946987401[40] = 0;
   out_6679695245946987401[41] = 0;
   out_6679695245946987401[42] = 0;
   out_6679695245946987401[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6679695245946987401[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6679695245946987401[45] = 0;
   out_6679695245946987401[46] = 0;
   out_6679695245946987401[47] = 0;
   out_6679695245946987401[48] = 0;
   out_6679695245946987401[49] = 0;
   out_6679695245946987401[50] = 0;
   out_6679695245946987401[51] = 0;
   out_6679695245946987401[52] = 0;
   out_6679695245946987401[53] = 0;
   out_6679695245946987401[54] = 0;
   out_6679695245946987401[55] = 0;
   out_6679695245946987401[56] = 0;
   out_6679695245946987401[57] = 1;
   out_6679695245946987401[58] = 0;
   out_6679695245946987401[59] = 0;
   out_6679695245946987401[60] = 0;
   out_6679695245946987401[61] = 0;
   out_6679695245946987401[62] = 0;
   out_6679695245946987401[63] = 0;
   out_6679695245946987401[64] = 0;
   out_6679695245946987401[65] = 0;
   out_6679695245946987401[66] = dt;
   out_6679695245946987401[67] = 0;
   out_6679695245946987401[68] = 0;
   out_6679695245946987401[69] = 0;
   out_6679695245946987401[70] = 0;
   out_6679695245946987401[71] = 0;
   out_6679695245946987401[72] = 0;
   out_6679695245946987401[73] = 0;
   out_6679695245946987401[74] = 0;
   out_6679695245946987401[75] = 0;
   out_6679695245946987401[76] = 1;
   out_6679695245946987401[77] = 0;
   out_6679695245946987401[78] = 0;
   out_6679695245946987401[79] = 0;
   out_6679695245946987401[80] = 0;
   out_6679695245946987401[81] = 0;
   out_6679695245946987401[82] = 0;
   out_6679695245946987401[83] = 0;
   out_6679695245946987401[84] = 0;
   out_6679695245946987401[85] = dt;
   out_6679695245946987401[86] = 0;
   out_6679695245946987401[87] = 0;
   out_6679695245946987401[88] = 0;
   out_6679695245946987401[89] = 0;
   out_6679695245946987401[90] = 0;
   out_6679695245946987401[91] = 0;
   out_6679695245946987401[92] = 0;
   out_6679695245946987401[93] = 0;
   out_6679695245946987401[94] = 0;
   out_6679695245946987401[95] = 1;
   out_6679695245946987401[96] = 0;
   out_6679695245946987401[97] = 0;
   out_6679695245946987401[98] = 0;
   out_6679695245946987401[99] = 0;
   out_6679695245946987401[100] = 0;
   out_6679695245946987401[101] = 0;
   out_6679695245946987401[102] = 0;
   out_6679695245946987401[103] = 0;
   out_6679695245946987401[104] = dt;
   out_6679695245946987401[105] = 0;
   out_6679695245946987401[106] = 0;
   out_6679695245946987401[107] = 0;
   out_6679695245946987401[108] = 0;
   out_6679695245946987401[109] = 0;
   out_6679695245946987401[110] = 0;
   out_6679695245946987401[111] = 0;
   out_6679695245946987401[112] = 0;
   out_6679695245946987401[113] = 0;
   out_6679695245946987401[114] = 1;
   out_6679695245946987401[115] = 0;
   out_6679695245946987401[116] = 0;
   out_6679695245946987401[117] = 0;
   out_6679695245946987401[118] = 0;
   out_6679695245946987401[119] = 0;
   out_6679695245946987401[120] = 0;
   out_6679695245946987401[121] = 0;
   out_6679695245946987401[122] = 0;
   out_6679695245946987401[123] = 0;
   out_6679695245946987401[124] = 0;
   out_6679695245946987401[125] = 0;
   out_6679695245946987401[126] = 0;
   out_6679695245946987401[127] = 0;
   out_6679695245946987401[128] = 0;
   out_6679695245946987401[129] = 0;
   out_6679695245946987401[130] = 0;
   out_6679695245946987401[131] = 0;
   out_6679695245946987401[132] = 0;
   out_6679695245946987401[133] = 1;
   out_6679695245946987401[134] = 0;
   out_6679695245946987401[135] = 0;
   out_6679695245946987401[136] = 0;
   out_6679695245946987401[137] = 0;
   out_6679695245946987401[138] = 0;
   out_6679695245946987401[139] = 0;
   out_6679695245946987401[140] = 0;
   out_6679695245946987401[141] = 0;
   out_6679695245946987401[142] = 0;
   out_6679695245946987401[143] = 0;
   out_6679695245946987401[144] = 0;
   out_6679695245946987401[145] = 0;
   out_6679695245946987401[146] = 0;
   out_6679695245946987401[147] = 0;
   out_6679695245946987401[148] = 0;
   out_6679695245946987401[149] = 0;
   out_6679695245946987401[150] = 0;
   out_6679695245946987401[151] = 0;
   out_6679695245946987401[152] = 1;
   out_6679695245946987401[153] = 0;
   out_6679695245946987401[154] = 0;
   out_6679695245946987401[155] = 0;
   out_6679695245946987401[156] = 0;
   out_6679695245946987401[157] = 0;
   out_6679695245946987401[158] = 0;
   out_6679695245946987401[159] = 0;
   out_6679695245946987401[160] = 0;
   out_6679695245946987401[161] = 0;
   out_6679695245946987401[162] = 0;
   out_6679695245946987401[163] = 0;
   out_6679695245946987401[164] = 0;
   out_6679695245946987401[165] = 0;
   out_6679695245946987401[166] = 0;
   out_6679695245946987401[167] = 0;
   out_6679695245946987401[168] = 0;
   out_6679695245946987401[169] = 0;
   out_6679695245946987401[170] = 0;
   out_6679695245946987401[171] = 1;
   out_6679695245946987401[172] = 0;
   out_6679695245946987401[173] = 0;
   out_6679695245946987401[174] = 0;
   out_6679695245946987401[175] = 0;
   out_6679695245946987401[176] = 0;
   out_6679695245946987401[177] = 0;
   out_6679695245946987401[178] = 0;
   out_6679695245946987401[179] = 0;
   out_6679695245946987401[180] = 0;
   out_6679695245946987401[181] = 0;
   out_6679695245946987401[182] = 0;
   out_6679695245946987401[183] = 0;
   out_6679695245946987401[184] = 0;
   out_6679695245946987401[185] = 0;
   out_6679695245946987401[186] = 0;
   out_6679695245946987401[187] = 0;
   out_6679695245946987401[188] = 0;
   out_6679695245946987401[189] = 0;
   out_6679695245946987401[190] = 1;
   out_6679695245946987401[191] = 0;
   out_6679695245946987401[192] = 0;
   out_6679695245946987401[193] = 0;
   out_6679695245946987401[194] = 0;
   out_6679695245946987401[195] = 0;
   out_6679695245946987401[196] = 0;
   out_6679695245946987401[197] = 0;
   out_6679695245946987401[198] = 0;
   out_6679695245946987401[199] = 0;
   out_6679695245946987401[200] = 0;
   out_6679695245946987401[201] = 0;
   out_6679695245946987401[202] = 0;
   out_6679695245946987401[203] = 0;
   out_6679695245946987401[204] = 0;
   out_6679695245946987401[205] = 0;
   out_6679695245946987401[206] = 0;
   out_6679695245946987401[207] = 0;
   out_6679695245946987401[208] = 0;
   out_6679695245946987401[209] = 1;
   out_6679695245946987401[210] = 0;
   out_6679695245946987401[211] = 0;
   out_6679695245946987401[212] = 0;
   out_6679695245946987401[213] = 0;
   out_6679695245946987401[214] = 0;
   out_6679695245946987401[215] = 0;
   out_6679695245946987401[216] = 0;
   out_6679695245946987401[217] = 0;
   out_6679695245946987401[218] = 0;
   out_6679695245946987401[219] = 0;
   out_6679695245946987401[220] = 0;
   out_6679695245946987401[221] = 0;
   out_6679695245946987401[222] = 0;
   out_6679695245946987401[223] = 0;
   out_6679695245946987401[224] = 0;
   out_6679695245946987401[225] = 0;
   out_6679695245946987401[226] = 0;
   out_6679695245946987401[227] = 0;
   out_6679695245946987401[228] = 1;
   out_6679695245946987401[229] = 0;
   out_6679695245946987401[230] = 0;
   out_6679695245946987401[231] = 0;
   out_6679695245946987401[232] = 0;
   out_6679695245946987401[233] = 0;
   out_6679695245946987401[234] = 0;
   out_6679695245946987401[235] = 0;
   out_6679695245946987401[236] = 0;
   out_6679695245946987401[237] = 0;
   out_6679695245946987401[238] = 0;
   out_6679695245946987401[239] = 0;
   out_6679695245946987401[240] = 0;
   out_6679695245946987401[241] = 0;
   out_6679695245946987401[242] = 0;
   out_6679695245946987401[243] = 0;
   out_6679695245946987401[244] = 0;
   out_6679695245946987401[245] = 0;
   out_6679695245946987401[246] = 0;
   out_6679695245946987401[247] = 1;
   out_6679695245946987401[248] = 0;
   out_6679695245946987401[249] = 0;
   out_6679695245946987401[250] = 0;
   out_6679695245946987401[251] = 0;
   out_6679695245946987401[252] = 0;
   out_6679695245946987401[253] = 0;
   out_6679695245946987401[254] = 0;
   out_6679695245946987401[255] = 0;
   out_6679695245946987401[256] = 0;
   out_6679695245946987401[257] = 0;
   out_6679695245946987401[258] = 0;
   out_6679695245946987401[259] = 0;
   out_6679695245946987401[260] = 0;
   out_6679695245946987401[261] = 0;
   out_6679695245946987401[262] = 0;
   out_6679695245946987401[263] = 0;
   out_6679695245946987401[264] = 0;
   out_6679695245946987401[265] = 0;
   out_6679695245946987401[266] = 1;
   out_6679695245946987401[267] = 0;
   out_6679695245946987401[268] = 0;
   out_6679695245946987401[269] = 0;
   out_6679695245946987401[270] = 0;
   out_6679695245946987401[271] = 0;
   out_6679695245946987401[272] = 0;
   out_6679695245946987401[273] = 0;
   out_6679695245946987401[274] = 0;
   out_6679695245946987401[275] = 0;
   out_6679695245946987401[276] = 0;
   out_6679695245946987401[277] = 0;
   out_6679695245946987401[278] = 0;
   out_6679695245946987401[279] = 0;
   out_6679695245946987401[280] = 0;
   out_6679695245946987401[281] = 0;
   out_6679695245946987401[282] = 0;
   out_6679695245946987401[283] = 0;
   out_6679695245946987401[284] = 0;
   out_6679695245946987401[285] = 1;
   out_6679695245946987401[286] = 0;
   out_6679695245946987401[287] = 0;
   out_6679695245946987401[288] = 0;
   out_6679695245946987401[289] = 0;
   out_6679695245946987401[290] = 0;
   out_6679695245946987401[291] = 0;
   out_6679695245946987401[292] = 0;
   out_6679695245946987401[293] = 0;
   out_6679695245946987401[294] = 0;
   out_6679695245946987401[295] = 0;
   out_6679695245946987401[296] = 0;
   out_6679695245946987401[297] = 0;
   out_6679695245946987401[298] = 0;
   out_6679695245946987401[299] = 0;
   out_6679695245946987401[300] = 0;
   out_6679695245946987401[301] = 0;
   out_6679695245946987401[302] = 0;
   out_6679695245946987401[303] = 0;
   out_6679695245946987401[304] = 1;
   out_6679695245946987401[305] = 0;
   out_6679695245946987401[306] = 0;
   out_6679695245946987401[307] = 0;
   out_6679695245946987401[308] = 0;
   out_6679695245946987401[309] = 0;
   out_6679695245946987401[310] = 0;
   out_6679695245946987401[311] = 0;
   out_6679695245946987401[312] = 0;
   out_6679695245946987401[313] = 0;
   out_6679695245946987401[314] = 0;
   out_6679695245946987401[315] = 0;
   out_6679695245946987401[316] = 0;
   out_6679695245946987401[317] = 0;
   out_6679695245946987401[318] = 0;
   out_6679695245946987401[319] = 0;
   out_6679695245946987401[320] = 0;
   out_6679695245946987401[321] = 0;
   out_6679695245946987401[322] = 0;
   out_6679695245946987401[323] = 1;
}
void h_4(double *state, double *unused, double *out_564364644927851281) {
   out_564364644927851281[0] = state[6] + state[9];
   out_564364644927851281[1] = state[7] + state[10];
   out_564364644927851281[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_4857525238989202427) {
   out_4857525238989202427[0] = 0;
   out_4857525238989202427[1] = 0;
   out_4857525238989202427[2] = 0;
   out_4857525238989202427[3] = 0;
   out_4857525238989202427[4] = 0;
   out_4857525238989202427[5] = 0;
   out_4857525238989202427[6] = 1;
   out_4857525238989202427[7] = 0;
   out_4857525238989202427[8] = 0;
   out_4857525238989202427[9] = 1;
   out_4857525238989202427[10] = 0;
   out_4857525238989202427[11] = 0;
   out_4857525238989202427[12] = 0;
   out_4857525238989202427[13] = 0;
   out_4857525238989202427[14] = 0;
   out_4857525238989202427[15] = 0;
   out_4857525238989202427[16] = 0;
   out_4857525238989202427[17] = 0;
   out_4857525238989202427[18] = 0;
   out_4857525238989202427[19] = 0;
   out_4857525238989202427[20] = 0;
   out_4857525238989202427[21] = 0;
   out_4857525238989202427[22] = 0;
   out_4857525238989202427[23] = 0;
   out_4857525238989202427[24] = 0;
   out_4857525238989202427[25] = 1;
   out_4857525238989202427[26] = 0;
   out_4857525238989202427[27] = 0;
   out_4857525238989202427[28] = 1;
   out_4857525238989202427[29] = 0;
   out_4857525238989202427[30] = 0;
   out_4857525238989202427[31] = 0;
   out_4857525238989202427[32] = 0;
   out_4857525238989202427[33] = 0;
   out_4857525238989202427[34] = 0;
   out_4857525238989202427[35] = 0;
   out_4857525238989202427[36] = 0;
   out_4857525238989202427[37] = 0;
   out_4857525238989202427[38] = 0;
   out_4857525238989202427[39] = 0;
   out_4857525238989202427[40] = 0;
   out_4857525238989202427[41] = 0;
   out_4857525238989202427[42] = 0;
   out_4857525238989202427[43] = 0;
   out_4857525238989202427[44] = 1;
   out_4857525238989202427[45] = 0;
   out_4857525238989202427[46] = 0;
   out_4857525238989202427[47] = 1;
   out_4857525238989202427[48] = 0;
   out_4857525238989202427[49] = 0;
   out_4857525238989202427[50] = 0;
   out_4857525238989202427[51] = 0;
   out_4857525238989202427[52] = 0;
   out_4857525238989202427[53] = 0;
}
void h_10(double *state, double *unused, double *out_4709684773907818943) {
   out_4709684773907818943[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_4709684773907818943[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_4709684773907818943[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_2903583333991674534) {
   out_2903583333991674534[0] = 0;
   out_2903583333991674534[1] = 9.8100000000000005*cos(state[1]);
   out_2903583333991674534[2] = 0;
   out_2903583333991674534[3] = 0;
   out_2903583333991674534[4] = -state[8];
   out_2903583333991674534[5] = state[7];
   out_2903583333991674534[6] = 0;
   out_2903583333991674534[7] = state[5];
   out_2903583333991674534[8] = -state[4];
   out_2903583333991674534[9] = 0;
   out_2903583333991674534[10] = 0;
   out_2903583333991674534[11] = 0;
   out_2903583333991674534[12] = 1;
   out_2903583333991674534[13] = 0;
   out_2903583333991674534[14] = 0;
   out_2903583333991674534[15] = 1;
   out_2903583333991674534[16] = 0;
   out_2903583333991674534[17] = 0;
   out_2903583333991674534[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_2903583333991674534[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_2903583333991674534[20] = 0;
   out_2903583333991674534[21] = state[8];
   out_2903583333991674534[22] = 0;
   out_2903583333991674534[23] = -state[6];
   out_2903583333991674534[24] = -state[5];
   out_2903583333991674534[25] = 0;
   out_2903583333991674534[26] = state[3];
   out_2903583333991674534[27] = 0;
   out_2903583333991674534[28] = 0;
   out_2903583333991674534[29] = 0;
   out_2903583333991674534[30] = 0;
   out_2903583333991674534[31] = 1;
   out_2903583333991674534[32] = 0;
   out_2903583333991674534[33] = 0;
   out_2903583333991674534[34] = 1;
   out_2903583333991674534[35] = 0;
   out_2903583333991674534[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_2903583333991674534[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_2903583333991674534[38] = 0;
   out_2903583333991674534[39] = -state[7];
   out_2903583333991674534[40] = state[6];
   out_2903583333991674534[41] = 0;
   out_2903583333991674534[42] = state[4];
   out_2903583333991674534[43] = -state[3];
   out_2903583333991674534[44] = 0;
   out_2903583333991674534[45] = 0;
   out_2903583333991674534[46] = 0;
   out_2903583333991674534[47] = 0;
   out_2903583333991674534[48] = 0;
   out_2903583333991674534[49] = 0;
   out_2903583333991674534[50] = 1;
   out_2903583333991674534[51] = 0;
   out_2903583333991674534[52] = 0;
   out_2903583333991674534[53] = 1;
}
void h_13(double *state, double *unused, double *out_9183852149661263008) {
   out_9183852149661263008[0] = state[3];
   out_9183852149661263008[1] = state[4];
   out_9183852149661263008[2] = state[5];
}
void H_13(double *state, double *unused, double *out_8069799064321535228) {
   out_8069799064321535228[0] = 0;
   out_8069799064321535228[1] = 0;
   out_8069799064321535228[2] = 0;
   out_8069799064321535228[3] = 1;
   out_8069799064321535228[4] = 0;
   out_8069799064321535228[5] = 0;
   out_8069799064321535228[6] = 0;
   out_8069799064321535228[7] = 0;
   out_8069799064321535228[8] = 0;
   out_8069799064321535228[9] = 0;
   out_8069799064321535228[10] = 0;
   out_8069799064321535228[11] = 0;
   out_8069799064321535228[12] = 0;
   out_8069799064321535228[13] = 0;
   out_8069799064321535228[14] = 0;
   out_8069799064321535228[15] = 0;
   out_8069799064321535228[16] = 0;
   out_8069799064321535228[17] = 0;
   out_8069799064321535228[18] = 0;
   out_8069799064321535228[19] = 0;
   out_8069799064321535228[20] = 0;
   out_8069799064321535228[21] = 0;
   out_8069799064321535228[22] = 1;
   out_8069799064321535228[23] = 0;
   out_8069799064321535228[24] = 0;
   out_8069799064321535228[25] = 0;
   out_8069799064321535228[26] = 0;
   out_8069799064321535228[27] = 0;
   out_8069799064321535228[28] = 0;
   out_8069799064321535228[29] = 0;
   out_8069799064321535228[30] = 0;
   out_8069799064321535228[31] = 0;
   out_8069799064321535228[32] = 0;
   out_8069799064321535228[33] = 0;
   out_8069799064321535228[34] = 0;
   out_8069799064321535228[35] = 0;
   out_8069799064321535228[36] = 0;
   out_8069799064321535228[37] = 0;
   out_8069799064321535228[38] = 0;
   out_8069799064321535228[39] = 0;
   out_8069799064321535228[40] = 0;
   out_8069799064321535228[41] = 1;
   out_8069799064321535228[42] = 0;
   out_8069799064321535228[43] = 0;
   out_8069799064321535228[44] = 0;
   out_8069799064321535228[45] = 0;
   out_8069799064321535228[46] = 0;
   out_8069799064321535228[47] = 0;
   out_8069799064321535228[48] = 0;
   out_8069799064321535228[49] = 0;
   out_8069799064321535228[50] = 0;
   out_8069799064321535228[51] = 0;
   out_8069799064321535228[52] = 0;
   out_8069799064321535228[53] = 0;
}
void h_14(double *state, double *unused, double *out_1305949636277284093) {
   out_1305949636277284093[0] = state[6];
   out_1305949636277284093[1] = state[7];
   out_1305949636277284093[2] = state[8];
}
void H_14(double *state, double *unused, double *out_1774736806693830131) {
   out_1774736806693830131[0] = 0;
   out_1774736806693830131[1] = 0;
   out_1774736806693830131[2] = 0;
   out_1774736806693830131[3] = 0;
   out_1774736806693830131[4] = 0;
   out_1774736806693830131[5] = 0;
   out_1774736806693830131[6] = 1;
   out_1774736806693830131[7] = 0;
   out_1774736806693830131[8] = 0;
   out_1774736806693830131[9] = 0;
   out_1774736806693830131[10] = 0;
   out_1774736806693830131[11] = 0;
   out_1774736806693830131[12] = 0;
   out_1774736806693830131[13] = 0;
   out_1774736806693830131[14] = 0;
   out_1774736806693830131[15] = 0;
   out_1774736806693830131[16] = 0;
   out_1774736806693830131[17] = 0;
   out_1774736806693830131[18] = 0;
   out_1774736806693830131[19] = 0;
   out_1774736806693830131[20] = 0;
   out_1774736806693830131[21] = 0;
   out_1774736806693830131[22] = 0;
   out_1774736806693830131[23] = 0;
   out_1774736806693830131[24] = 0;
   out_1774736806693830131[25] = 1;
   out_1774736806693830131[26] = 0;
   out_1774736806693830131[27] = 0;
   out_1774736806693830131[28] = 0;
   out_1774736806693830131[29] = 0;
   out_1774736806693830131[30] = 0;
   out_1774736806693830131[31] = 0;
   out_1774736806693830131[32] = 0;
   out_1774736806693830131[33] = 0;
   out_1774736806693830131[34] = 0;
   out_1774736806693830131[35] = 0;
   out_1774736806693830131[36] = 0;
   out_1774736806693830131[37] = 0;
   out_1774736806693830131[38] = 0;
   out_1774736806693830131[39] = 0;
   out_1774736806693830131[40] = 0;
   out_1774736806693830131[41] = 0;
   out_1774736806693830131[42] = 0;
   out_1774736806693830131[43] = 0;
   out_1774736806693830131[44] = 1;
   out_1774736806693830131[45] = 0;
   out_1774736806693830131[46] = 0;
   out_1774736806693830131[47] = 0;
   out_1774736806693830131[48] = 0;
   out_1774736806693830131[49] = 0;
   out_1774736806693830131[50] = 0;
   out_1774736806693830131[51] = 0;
   out_1774736806693830131[52] = 0;
   out_1774736806693830131[53] = 0;
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
void pose_err_fun(double *nom_x, double *delta_x, double *out_1586140269824446038) {
  err_fun(nom_x, delta_x, out_1586140269824446038);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_9166849630934128431) {
  inv_err_fun(nom_x, true_x, out_9166849630934128431);
}
void pose_H_mod_fun(double *state, double *out_1085718029145294855) {
  H_mod_fun(state, out_1085718029145294855);
}
void pose_f_fun(double *state, double dt, double *out_1181361488543532072) {
  f_fun(state,  dt, out_1181361488543532072);
}
void pose_F_fun(double *state, double dt, double *out_6679695245946987401) {
  F_fun(state,  dt, out_6679695245946987401);
}
void pose_h_4(double *state, double *unused, double *out_564364644927851281) {
  h_4(state, unused, out_564364644927851281);
}
void pose_H_4(double *state, double *unused, double *out_4857525238989202427) {
  H_4(state, unused, out_4857525238989202427);
}
void pose_h_10(double *state, double *unused, double *out_4709684773907818943) {
  h_10(state, unused, out_4709684773907818943);
}
void pose_H_10(double *state, double *unused, double *out_2903583333991674534) {
  H_10(state, unused, out_2903583333991674534);
}
void pose_h_13(double *state, double *unused, double *out_9183852149661263008) {
  h_13(state, unused, out_9183852149661263008);
}
void pose_H_13(double *state, double *unused, double *out_8069799064321535228) {
  H_13(state, unused, out_8069799064321535228);
}
void pose_h_14(double *state, double *unused, double *out_1305949636277284093) {
  h_14(state, unused, out_1305949636277284093);
}
void pose_H_14(double *state, double *unused, double *out_1774736806693830131) {
  H_14(state, unused, out_1774736806693830131);
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
