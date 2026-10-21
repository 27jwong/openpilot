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
void err_fun(double *nom_x, double *delta_x, double *out_7121613786142052873) {
   out_7121613786142052873[0] = delta_x[0] + nom_x[0];
   out_7121613786142052873[1] = delta_x[1] + nom_x[1];
   out_7121613786142052873[2] = delta_x[2] + nom_x[2];
   out_7121613786142052873[3] = delta_x[3] + nom_x[3];
   out_7121613786142052873[4] = delta_x[4] + nom_x[4];
   out_7121613786142052873[5] = delta_x[5] + nom_x[5];
   out_7121613786142052873[6] = delta_x[6] + nom_x[6];
   out_7121613786142052873[7] = delta_x[7] + nom_x[7];
   out_7121613786142052873[8] = delta_x[8] + nom_x[8];
   out_7121613786142052873[9] = delta_x[9] + nom_x[9];
   out_7121613786142052873[10] = delta_x[10] + nom_x[10];
   out_7121613786142052873[11] = delta_x[11] + nom_x[11];
   out_7121613786142052873[12] = delta_x[12] + nom_x[12];
   out_7121613786142052873[13] = delta_x[13] + nom_x[13];
   out_7121613786142052873[14] = delta_x[14] + nom_x[14];
   out_7121613786142052873[15] = delta_x[15] + nom_x[15];
   out_7121613786142052873[16] = delta_x[16] + nom_x[16];
   out_7121613786142052873[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_4360274276000378130) {
   out_4360274276000378130[0] = -nom_x[0] + true_x[0];
   out_4360274276000378130[1] = -nom_x[1] + true_x[1];
   out_4360274276000378130[2] = -nom_x[2] + true_x[2];
   out_4360274276000378130[3] = -nom_x[3] + true_x[3];
   out_4360274276000378130[4] = -nom_x[4] + true_x[4];
   out_4360274276000378130[5] = -nom_x[5] + true_x[5];
   out_4360274276000378130[6] = -nom_x[6] + true_x[6];
   out_4360274276000378130[7] = -nom_x[7] + true_x[7];
   out_4360274276000378130[8] = -nom_x[8] + true_x[8];
   out_4360274276000378130[9] = -nom_x[9] + true_x[9];
   out_4360274276000378130[10] = -nom_x[10] + true_x[10];
   out_4360274276000378130[11] = -nom_x[11] + true_x[11];
   out_4360274276000378130[12] = -nom_x[12] + true_x[12];
   out_4360274276000378130[13] = -nom_x[13] + true_x[13];
   out_4360274276000378130[14] = -nom_x[14] + true_x[14];
   out_4360274276000378130[15] = -nom_x[15] + true_x[15];
   out_4360274276000378130[16] = -nom_x[16] + true_x[16];
   out_4360274276000378130[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_6957806766333672764) {
   out_6957806766333672764[0] = 1.0;
   out_6957806766333672764[1] = 0.0;
   out_6957806766333672764[2] = 0.0;
   out_6957806766333672764[3] = 0.0;
   out_6957806766333672764[4] = 0.0;
   out_6957806766333672764[5] = 0.0;
   out_6957806766333672764[6] = 0.0;
   out_6957806766333672764[7] = 0.0;
   out_6957806766333672764[8] = 0.0;
   out_6957806766333672764[9] = 0.0;
   out_6957806766333672764[10] = 0.0;
   out_6957806766333672764[11] = 0.0;
   out_6957806766333672764[12] = 0.0;
   out_6957806766333672764[13] = 0.0;
   out_6957806766333672764[14] = 0.0;
   out_6957806766333672764[15] = 0.0;
   out_6957806766333672764[16] = 0.0;
   out_6957806766333672764[17] = 0.0;
   out_6957806766333672764[18] = 0.0;
   out_6957806766333672764[19] = 1.0;
   out_6957806766333672764[20] = 0.0;
   out_6957806766333672764[21] = 0.0;
   out_6957806766333672764[22] = 0.0;
   out_6957806766333672764[23] = 0.0;
   out_6957806766333672764[24] = 0.0;
   out_6957806766333672764[25] = 0.0;
   out_6957806766333672764[26] = 0.0;
   out_6957806766333672764[27] = 0.0;
   out_6957806766333672764[28] = 0.0;
   out_6957806766333672764[29] = 0.0;
   out_6957806766333672764[30] = 0.0;
   out_6957806766333672764[31] = 0.0;
   out_6957806766333672764[32] = 0.0;
   out_6957806766333672764[33] = 0.0;
   out_6957806766333672764[34] = 0.0;
   out_6957806766333672764[35] = 0.0;
   out_6957806766333672764[36] = 0.0;
   out_6957806766333672764[37] = 0.0;
   out_6957806766333672764[38] = 1.0;
   out_6957806766333672764[39] = 0.0;
   out_6957806766333672764[40] = 0.0;
   out_6957806766333672764[41] = 0.0;
   out_6957806766333672764[42] = 0.0;
   out_6957806766333672764[43] = 0.0;
   out_6957806766333672764[44] = 0.0;
   out_6957806766333672764[45] = 0.0;
   out_6957806766333672764[46] = 0.0;
   out_6957806766333672764[47] = 0.0;
   out_6957806766333672764[48] = 0.0;
   out_6957806766333672764[49] = 0.0;
   out_6957806766333672764[50] = 0.0;
   out_6957806766333672764[51] = 0.0;
   out_6957806766333672764[52] = 0.0;
   out_6957806766333672764[53] = 0.0;
   out_6957806766333672764[54] = 0.0;
   out_6957806766333672764[55] = 0.0;
   out_6957806766333672764[56] = 0.0;
   out_6957806766333672764[57] = 1.0;
   out_6957806766333672764[58] = 0.0;
   out_6957806766333672764[59] = 0.0;
   out_6957806766333672764[60] = 0.0;
   out_6957806766333672764[61] = 0.0;
   out_6957806766333672764[62] = 0.0;
   out_6957806766333672764[63] = 0.0;
   out_6957806766333672764[64] = 0.0;
   out_6957806766333672764[65] = 0.0;
   out_6957806766333672764[66] = 0.0;
   out_6957806766333672764[67] = 0.0;
   out_6957806766333672764[68] = 0.0;
   out_6957806766333672764[69] = 0.0;
   out_6957806766333672764[70] = 0.0;
   out_6957806766333672764[71] = 0.0;
   out_6957806766333672764[72] = 0.0;
   out_6957806766333672764[73] = 0.0;
   out_6957806766333672764[74] = 0.0;
   out_6957806766333672764[75] = 0.0;
   out_6957806766333672764[76] = 1.0;
   out_6957806766333672764[77] = 0.0;
   out_6957806766333672764[78] = 0.0;
   out_6957806766333672764[79] = 0.0;
   out_6957806766333672764[80] = 0.0;
   out_6957806766333672764[81] = 0.0;
   out_6957806766333672764[82] = 0.0;
   out_6957806766333672764[83] = 0.0;
   out_6957806766333672764[84] = 0.0;
   out_6957806766333672764[85] = 0.0;
   out_6957806766333672764[86] = 0.0;
   out_6957806766333672764[87] = 0.0;
   out_6957806766333672764[88] = 0.0;
   out_6957806766333672764[89] = 0.0;
   out_6957806766333672764[90] = 0.0;
   out_6957806766333672764[91] = 0.0;
   out_6957806766333672764[92] = 0.0;
   out_6957806766333672764[93] = 0.0;
   out_6957806766333672764[94] = 0.0;
   out_6957806766333672764[95] = 1.0;
   out_6957806766333672764[96] = 0.0;
   out_6957806766333672764[97] = 0.0;
   out_6957806766333672764[98] = 0.0;
   out_6957806766333672764[99] = 0.0;
   out_6957806766333672764[100] = 0.0;
   out_6957806766333672764[101] = 0.0;
   out_6957806766333672764[102] = 0.0;
   out_6957806766333672764[103] = 0.0;
   out_6957806766333672764[104] = 0.0;
   out_6957806766333672764[105] = 0.0;
   out_6957806766333672764[106] = 0.0;
   out_6957806766333672764[107] = 0.0;
   out_6957806766333672764[108] = 0.0;
   out_6957806766333672764[109] = 0.0;
   out_6957806766333672764[110] = 0.0;
   out_6957806766333672764[111] = 0.0;
   out_6957806766333672764[112] = 0.0;
   out_6957806766333672764[113] = 0.0;
   out_6957806766333672764[114] = 1.0;
   out_6957806766333672764[115] = 0.0;
   out_6957806766333672764[116] = 0.0;
   out_6957806766333672764[117] = 0.0;
   out_6957806766333672764[118] = 0.0;
   out_6957806766333672764[119] = 0.0;
   out_6957806766333672764[120] = 0.0;
   out_6957806766333672764[121] = 0.0;
   out_6957806766333672764[122] = 0.0;
   out_6957806766333672764[123] = 0.0;
   out_6957806766333672764[124] = 0.0;
   out_6957806766333672764[125] = 0.0;
   out_6957806766333672764[126] = 0.0;
   out_6957806766333672764[127] = 0.0;
   out_6957806766333672764[128] = 0.0;
   out_6957806766333672764[129] = 0.0;
   out_6957806766333672764[130] = 0.0;
   out_6957806766333672764[131] = 0.0;
   out_6957806766333672764[132] = 0.0;
   out_6957806766333672764[133] = 1.0;
   out_6957806766333672764[134] = 0.0;
   out_6957806766333672764[135] = 0.0;
   out_6957806766333672764[136] = 0.0;
   out_6957806766333672764[137] = 0.0;
   out_6957806766333672764[138] = 0.0;
   out_6957806766333672764[139] = 0.0;
   out_6957806766333672764[140] = 0.0;
   out_6957806766333672764[141] = 0.0;
   out_6957806766333672764[142] = 0.0;
   out_6957806766333672764[143] = 0.0;
   out_6957806766333672764[144] = 0.0;
   out_6957806766333672764[145] = 0.0;
   out_6957806766333672764[146] = 0.0;
   out_6957806766333672764[147] = 0.0;
   out_6957806766333672764[148] = 0.0;
   out_6957806766333672764[149] = 0.0;
   out_6957806766333672764[150] = 0.0;
   out_6957806766333672764[151] = 0.0;
   out_6957806766333672764[152] = 1.0;
   out_6957806766333672764[153] = 0.0;
   out_6957806766333672764[154] = 0.0;
   out_6957806766333672764[155] = 0.0;
   out_6957806766333672764[156] = 0.0;
   out_6957806766333672764[157] = 0.0;
   out_6957806766333672764[158] = 0.0;
   out_6957806766333672764[159] = 0.0;
   out_6957806766333672764[160] = 0.0;
   out_6957806766333672764[161] = 0.0;
   out_6957806766333672764[162] = 0.0;
   out_6957806766333672764[163] = 0.0;
   out_6957806766333672764[164] = 0.0;
   out_6957806766333672764[165] = 0.0;
   out_6957806766333672764[166] = 0.0;
   out_6957806766333672764[167] = 0.0;
   out_6957806766333672764[168] = 0.0;
   out_6957806766333672764[169] = 0.0;
   out_6957806766333672764[170] = 0.0;
   out_6957806766333672764[171] = 1.0;
   out_6957806766333672764[172] = 0.0;
   out_6957806766333672764[173] = 0.0;
   out_6957806766333672764[174] = 0.0;
   out_6957806766333672764[175] = 0.0;
   out_6957806766333672764[176] = 0.0;
   out_6957806766333672764[177] = 0.0;
   out_6957806766333672764[178] = 0.0;
   out_6957806766333672764[179] = 0.0;
   out_6957806766333672764[180] = 0.0;
   out_6957806766333672764[181] = 0.0;
   out_6957806766333672764[182] = 0.0;
   out_6957806766333672764[183] = 0.0;
   out_6957806766333672764[184] = 0.0;
   out_6957806766333672764[185] = 0.0;
   out_6957806766333672764[186] = 0.0;
   out_6957806766333672764[187] = 0.0;
   out_6957806766333672764[188] = 0.0;
   out_6957806766333672764[189] = 0.0;
   out_6957806766333672764[190] = 1.0;
   out_6957806766333672764[191] = 0.0;
   out_6957806766333672764[192] = 0.0;
   out_6957806766333672764[193] = 0.0;
   out_6957806766333672764[194] = 0.0;
   out_6957806766333672764[195] = 0.0;
   out_6957806766333672764[196] = 0.0;
   out_6957806766333672764[197] = 0.0;
   out_6957806766333672764[198] = 0.0;
   out_6957806766333672764[199] = 0.0;
   out_6957806766333672764[200] = 0.0;
   out_6957806766333672764[201] = 0.0;
   out_6957806766333672764[202] = 0.0;
   out_6957806766333672764[203] = 0.0;
   out_6957806766333672764[204] = 0.0;
   out_6957806766333672764[205] = 0.0;
   out_6957806766333672764[206] = 0.0;
   out_6957806766333672764[207] = 0.0;
   out_6957806766333672764[208] = 0.0;
   out_6957806766333672764[209] = 1.0;
   out_6957806766333672764[210] = 0.0;
   out_6957806766333672764[211] = 0.0;
   out_6957806766333672764[212] = 0.0;
   out_6957806766333672764[213] = 0.0;
   out_6957806766333672764[214] = 0.0;
   out_6957806766333672764[215] = 0.0;
   out_6957806766333672764[216] = 0.0;
   out_6957806766333672764[217] = 0.0;
   out_6957806766333672764[218] = 0.0;
   out_6957806766333672764[219] = 0.0;
   out_6957806766333672764[220] = 0.0;
   out_6957806766333672764[221] = 0.0;
   out_6957806766333672764[222] = 0.0;
   out_6957806766333672764[223] = 0.0;
   out_6957806766333672764[224] = 0.0;
   out_6957806766333672764[225] = 0.0;
   out_6957806766333672764[226] = 0.0;
   out_6957806766333672764[227] = 0.0;
   out_6957806766333672764[228] = 1.0;
   out_6957806766333672764[229] = 0.0;
   out_6957806766333672764[230] = 0.0;
   out_6957806766333672764[231] = 0.0;
   out_6957806766333672764[232] = 0.0;
   out_6957806766333672764[233] = 0.0;
   out_6957806766333672764[234] = 0.0;
   out_6957806766333672764[235] = 0.0;
   out_6957806766333672764[236] = 0.0;
   out_6957806766333672764[237] = 0.0;
   out_6957806766333672764[238] = 0.0;
   out_6957806766333672764[239] = 0.0;
   out_6957806766333672764[240] = 0.0;
   out_6957806766333672764[241] = 0.0;
   out_6957806766333672764[242] = 0.0;
   out_6957806766333672764[243] = 0.0;
   out_6957806766333672764[244] = 0.0;
   out_6957806766333672764[245] = 0.0;
   out_6957806766333672764[246] = 0.0;
   out_6957806766333672764[247] = 1.0;
   out_6957806766333672764[248] = 0.0;
   out_6957806766333672764[249] = 0.0;
   out_6957806766333672764[250] = 0.0;
   out_6957806766333672764[251] = 0.0;
   out_6957806766333672764[252] = 0.0;
   out_6957806766333672764[253] = 0.0;
   out_6957806766333672764[254] = 0.0;
   out_6957806766333672764[255] = 0.0;
   out_6957806766333672764[256] = 0.0;
   out_6957806766333672764[257] = 0.0;
   out_6957806766333672764[258] = 0.0;
   out_6957806766333672764[259] = 0.0;
   out_6957806766333672764[260] = 0.0;
   out_6957806766333672764[261] = 0.0;
   out_6957806766333672764[262] = 0.0;
   out_6957806766333672764[263] = 0.0;
   out_6957806766333672764[264] = 0.0;
   out_6957806766333672764[265] = 0.0;
   out_6957806766333672764[266] = 1.0;
   out_6957806766333672764[267] = 0.0;
   out_6957806766333672764[268] = 0.0;
   out_6957806766333672764[269] = 0.0;
   out_6957806766333672764[270] = 0.0;
   out_6957806766333672764[271] = 0.0;
   out_6957806766333672764[272] = 0.0;
   out_6957806766333672764[273] = 0.0;
   out_6957806766333672764[274] = 0.0;
   out_6957806766333672764[275] = 0.0;
   out_6957806766333672764[276] = 0.0;
   out_6957806766333672764[277] = 0.0;
   out_6957806766333672764[278] = 0.0;
   out_6957806766333672764[279] = 0.0;
   out_6957806766333672764[280] = 0.0;
   out_6957806766333672764[281] = 0.0;
   out_6957806766333672764[282] = 0.0;
   out_6957806766333672764[283] = 0.0;
   out_6957806766333672764[284] = 0.0;
   out_6957806766333672764[285] = 1.0;
   out_6957806766333672764[286] = 0.0;
   out_6957806766333672764[287] = 0.0;
   out_6957806766333672764[288] = 0.0;
   out_6957806766333672764[289] = 0.0;
   out_6957806766333672764[290] = 0.0;
   out_6957806766333672764[291] = 0.0;
   out_6957806766333672764[292] = 0.0;
   out_6957806766333672764[293] = 0.0;
   out_6957806766333672764[294] = 0.0;
   out_6957806766333672764[295] = 0.0;
   out_6957806766333672764[296] = 0.0;
   out_6957806766333672764[297] = 0.0;
   out_6957806766333672764[298] = 0.0;
   out_6957806766333672764[299] = 0.0;
   out_6957806766333672764[300] = 0.0;
   out_6957806766333672764[301] = 0.0;
   out_6957806766333672764[302] = 0.0;
   out_6957806766333672764[303] = 0.0;
   out_6957806766333672764[304] = 1.0;
   out_6957806766333672764[305] = 0.0;
   out_6957806766333672764[306] = 0.0;
   out_6957806766333672764[307] = 0.0;
   out_6957806766333672764[308] = 0.0;
   out_6957806766333672764[309] = 0.0;
   out_6957806766333672764[310] = 0.0;
   out_6957806766333672764[311] = 0.0;
   out_6957806766333672764[312] = 0.0;
   out_6957806766333672764[313] = 0.0;
   out_6957806766333672764[314] = 0.0;
   out_6957806766333672764[315] = 0.0;
   out_6957806766333672764[316] = 0.0;
   out_6957806766333672764[317] = 0.0;
   out_6957806766333672764[318] = 0.0;
   out_6957806766333672764[319] = 0.0;
   out_6957806766333672764[320] = 0.0;
   out_6957806766333672764[321] = 0.0;
   out_6957806766333672764[322] = 0.0;
   out_6957806766333672764[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_5556993601589438608) {
   out_5556993601589438608[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_5556993601589438608[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_5556993601589438608[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_5556993601589438608[3] = dt*state[12] + state[3];
   out_5556993601589438608[4] = dt*state[13] + state[4];
   out_5556993601589438608[5] = dt*state[14] + state[5];
   out_5556993601589438608[6] = state[6];
   out_5556993601589438608[7] = state[7];
   out_5556993601589438608[8] = state[8];
   out_5556993601589438608[9] = state[9];
   out_5556993601589438608[10] = state[10];
   out_5556993601589438608[11] = state[11];
   out_5556993601589438608[12] = state[12];
   out_5556993601589438608[13] = state[13];
   out_5556993601589438608[14] = state[14];
   out_5556993601589438608[15] = state[15];
   out_5556993601589438608[16] = state[16];
   out_5556993601589438608[17] = state[17];
}
void F_fun(double *state, double dt, double *out_5351909948517241954) {
   out_5351909948517241954[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_5351909948517241954[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_5351909948517241954[2] = 0;
   out_5351909948517241954[3] = 0;
   out_5351909948517241954[4] = 0;
   out_5351909948517241954[5] = 0;
   out_5351909948517241954[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_5351909948517241954[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_5351909948517241954[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_5351909948517241954[9] = 0;
   out_5351909948517241954[10] = 0;
   out_5351909948517241954[11] = 0;
   out_5351909948517241954[12] = 0;
   out_5351909948517241954[13] = 0;
   out_5351909948517241954[14] = 0;
   out_5351909948517241954[15] = 0;
   out_5351909948517241954[16] = 0;
   out_5351909948517241954[17] = 0;
   out_5351909948517241954[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_5351909948517241954[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_5351909948517241954[20] = 0;
   out_5351909948517241954[21] = 0;
   out_5351909948517241954[22] = 0;
   out_5351909948517241954[23] = 0;
   out_5351909948517241954[24] = 0;
   out_5351909948517241954[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_5351909948517241954[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_5351909948517241954[27] = 0;
   out_5351909948517241954[28] = 0;
   out_5351909948517241954[29] = 0;
   out_5351909948517241954[30] = 0;
   out_5351909948517241954[31] = 0;
   out_5351909948517241954[32] = 0;
   out_5351909948517241954[33] = 0;
   out_5351909948517241954[34] = 0;
   out_5351909948517241954[35] = 0;
   out_5351909948517241954[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_5351909948517241954[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_5351909948517241954[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_5351909948517241954[39] = 0;
   out_5351909948517241954[40] = 0;
   out_5351909948517241954[41] = 0;
   out_5351909948517241954[42] = 0;
   out_5351909948517241954[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_5351909948517241954[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_5351909948517241954[45] = 0;
   out_5351909948517241954[46] = 0;
   out_5351909948517241954[47] = 0;
   out_5351909948517241954[48] = 0;
   out_5351909948517241954[49] = 0;
   out_5351909948517241954[50] = 0;
   out_5351909948517241954[51] = 0;
   out_5351909948517241954[52] = 0;
   out_5351909948517241954[53] = 0;
   out_5351909948517241954[54] = 0;
   out_5351909948517241954[55] = 0;
   out_5351909948517241954[56] = 0;
   out_5351909948517241954[57] = 1;
   out_5351909948517241954[58] = 0;
   out_5351909948517241954[59] = 0;
   out_5351909948517241954[60] = 0;
   out_5351909948517241954[61] = 0;
   out_5351909948517241954[62] = 0;
   out_5351909948517241954[63] = 0;
   out_5351909948517241954[64] = 0;
   out_5351909948517241954[65] = 0;
   out_5351909948517241954[66] = dt;
   out_5351909948517241954[67] = 0;
   out_5351909948517241954[68] = 0;
   out_5351909948517241954[69] = 0;
   out_5351909948517241954[70] = 0;
   out_5351909948517241954[71] = 0;
   out_5351909948517241954[72] = 0;
   out_5351909948517241954[73] = 0;
   out_5351909948517241954[74] = 0;
   out_5351909948517241954[75] = 0;
   out_5351909948517241954[76] = 1;
   out_5351909948517241954[77] = 0;
   out_5351909948517241954[78] = 0;
   out_5351909948517241954[79] = 0;
   out_5351909948517241954[80] = 0;
   out_5351909948517241954[81] = 0;
   out_5351909948517241954[82] = 0;
   out_5351909948517241954[83] = 0;
   out_5351909948517241954[84] = 0;
   out_5351909948517241954[85] = dt;
   out_5351909948517241954[86] = 0;
   out_5351909948517241954[87] = 0;
   out_5351909948517241954[88] = 0;
   out_5351909948517241954[89] = 0;
   out_5351909948517241954[90] = 0;
   out_5351909948517241954[91] = 0;
   out_5351909948517241954[92] = 0;
   out_5351909948517241954[93] = 0;
   out_5351909948517241954[94] = 0;
   out_5351909948517241954[95] = 1;
   out_5351909948517241954[96] = 0;
   out_5351909948517241954[97] = 0;
   out_5351909948517241954[98] = 0;
   out_5351909948517241954[99] = 0;
   out_5351909948517241954[100] = 0;
   out_5351909948517241954[101] = 0;
   out_5351909948517241954[102] = 0;
   out_5351909948517241954[103] = 0;
   out_5351909948517241954[104] = dt;
   out_5351909948517241954[105] = 0;
   out_5351909948517241954[106] = 0;
   out_5351909948517241954[107] = 0;
   out_5351909948517241954[108] = 0;
   out_5351909948517241954[109] = 0;
   out_5351909948517241954[110] = 0;
   out_5351909948517241954[111] = 0;
   out_5351909948517241954[112] = 0;
   out_5351909948517241954[113] = 0;
   out_5351909948517241954[114] = 1;
   out_5351909948517241954[115] = 0;
   out_5351909948517241954[116] = 0;
   out_5351909948517241954[117] = 0;
   out_5351909948517241954[118] = 0;
   out_5351909948517241954[119] = 0;
   out_5351909948517241954[120] = 0;
   out_5351909948517241954[121] = 0;
   out_5351909948517241954[122] = 0;
   out_5351909948517241954[123] = 0;
   out_5351909948517241954[124] = 0;
   out_5351909948517241954[125] = 0;
   out_5351909948517241954[126] = 0;
   out_5351909948517241954[127] = 0;
   out_5351909948517241954[128] = 0;
   out_5351909948517241954[129] = 0;
   out_5351909948517241954[130] = 0;
   out_5351909948517241954[131] = 0;
   out_5351909948517241954[132] = 0;
   out_5351909948517241954[133] = 1;
   out_5351909948517241954[134] = 0;
   out_5351909948517241954[135] = 0;
   out_5351909948517241954[136] = 0;
   out_5351909948517241954[137] = 0;
   out_5351909948517241954[138] = 0;
   out_5351909948517241954[139] = 0;
   out_5351909948517241954[140] = 0;
   out_5351909948517241954[141] = 0;
   out_5351909948517241954[142] = 0;
   out_5351909948517241954[143] = 0;
   out_5351909948517241954[144] = 0;
   out_5351909948517241954[145] = 0;
   out_5351909948517241954[146] = 0;
   out_5351909948517241954[147] = 0;
   out_5351909948517241954[148] = 0;
   out_5351909948517241954[149] = 0;
   out_5351909948517241954[150] = 0;
   out_5351909948517241954[151] = 0;
   out_5351909948517241954[152] = 1;
   out_5351909948517241954[153] = 0;
   out_5351909948517241954[154] = 0;
   out_5351909948517241954[155] = 0;
   out_5351909948517241954[156] = 0;
   out_5351909948517241954[157] = 0;
   out_5351909948517241954[158] = 0;
   out_5351909948517241954[159] = 0;
   out_5351909948517241954[160] = 0;
   out_5351909948517241954[161] = 0;
   out_5351909948517241954[162] = 0;
   out_5351909948517241954[163] = 0;
   out_5351909948517241954[164] = 0;
   out_5351909948517241954[165] = 0;
   out_5351909948517241954[166] = 0;
   out_5351909948517241954[167] = 0;
   out_5351909948517241954[168] = 0;
   out_5351909948517241954[169] = 0;
   out_5351909948517241954[170] = 0;
   out_5351909948517241954[171] = 1;
   out_5351909948517241954[172] = 0;
   out_5351909948517241954[173] = 0;
   out_5351909948517241954[174] = 0;
   out_5351909948517241954[175] = 0;
   out_5351909948517241954[176] = 0;
   out_5351909948517241954[177] = 0;
   out_5351909948517241954[178] = 0;
   out_5351909948517241954[179] = 0;
   out_5351909948517241954[180] = 0;
   out_5351909948517241954[181] = 0;
   out_5351909948517241954[182] = 0;
   out_5351909948517241954[183] = 0;
   out_5351909948517241954[184] = 0;
   out_5351909948517241954[185] = 0;
   out_5351909948517241954[186] = 0;
   out_5351909948517241954[187] = 0;
   out_5351909948517241954[188] = 0;
   out_5351909948517241954[189] = 0;
   out_5351909948517241954[190] = 1;
   out_5351909948517241954[191] = 0;
   out_5351909948517241954[192] = 0;
   out_5351909948517241954[193] = 0;
   out_5351909948517241954[194] = 0;
   out_5351909948517241954[195] = 0;
   out_5351909948517241954[196] = 0;
   out_5351909948517241954[197] = 0;
   out_5351909948517241954[198] = 0;
   out_5351909948517241954[199] = 0;
   out_5351909948517241954[200] = 0;
   out_5351909948517241954[201] = 0;
   out_5351909948517241954[202] = 0;
   out_5351909948517241954[203] = 0;
   out_5351909948517241954[204] = 0;
   out_5351909948517241954[205] = 0;
   out_5351909948517241954[206] = 0;
   out_5351909948517241954[207] = 0;
   out_5351909948517241954[208] = 0;
   out_5351909948517241954[209] = 1;
   out_5351909948517241954[210] = 0;
   out_5351909948517241954[211] = 0;
   out_5351909948517241954[212] = 0;
   out_5351909948517241954[213] = 0;
   out_5351909948517241954[214] = 0;
   out_5351909948517241954[215] = 0;
   out_5351909948517241954[216] = 0;
   out_5351909948517241954[217] = 0;
   out_5351909948517241954[218] = 0;
   out_5351909948517241954[219] = 0;
   out_5351909948517241954[220] = 0;
   out_5351909948517241954[221] = 0;
   out_5351909948517241954[222] = 0;
   out_5351909948517241954[223] = 0;
   out_5351909948517241954[224] = 0;
   out_5351909948517241954[225] = 0;
   out_5351909948517241954[226] = 0;
   out_5351909948517241954[227] = 0;
   out_5351909948517241954[228] = 1;
   out_5351909948517241954[229] = 0;
   out_5351909948517241954[230] = 0;
   out_5351909948517241954[231] = 0;
   out_5351909948517241954[232] = 0;
   out_5351909948517241954[233] = 0;
   out_5351909948517241954[234] = 0;
   out_5351909948517241954[235] = 0;
   out_5351909948517241954[236] = 0;
   out_5351909948517241954[237] = 0;
   out_5351909948517241954[238] = 0;
   out_5351909948517241954[239] = 0;
   out_5351909948517241954[240] = 0;
   out_5351909948517241954[241] = 0;
   out_5351909948517241954[242] = 0;
   out_5351909948517241954[243] = 0;
   out_5351909948517241954[244] = 0;
   out_5351909948517241954[245] = 0;
   out_5351909948517241954[246] = 0;
   out_5351909948517241954[247] = 1;
   out_5351909948517241954[248] = 0;
   out_5351909948517241954[249] = 0;
   out_5351909948517241954[250] = 0;
   out_5351909948517241954[251] = 0;
   out_5351909948517241954[252] = 0;
   out_5351909948517241954[253] = 0;
   out_5351909948517241954[254] = 0;
   out_5351909948517241954[255] = 0;
   out_5351909948517241954[256] = 0;
   out_5351909948517241954[257] = 0;
   out_5351909948517241954[258] = 0;
   out_5351909948517241954[259] = 0;
   out_5351909948517241954[260] = 0;
   out_5351909948517241954[261] = 0;
   out_5351909948517241954[262] = 0;
   out_5351909948517241954[263] = 0;
   out_5351909948517241954[264] = 0;
   out_5351909948517241954[265] = 0;
   out_5351909948517241954[266] = 1;
   out_5351909948517241954[267] = 0;
   out_5351909948517241954[268] = 0;
   out_5351909948517241954[269] = 0;
   out_5351909948517241954[270] = 0;
   out_5351909948517241954[271] = 0;
   out_5351909948517241954[272] = 0;
   out_5351909948517241954[273] = 0;
   out_5351909948517241954[274] = 0;
   out_5351909948517241954[275] = 0;
   out_5351909948517241954[276] = 0;
   out_5351909948517241954[277] = 0;
   out_5351909948517241954[278] = 0;
   out_5351909948517241954[279] = 0;
   out_5351909948517241954[280] = 0;
   out_5351909948517241954[281] = 0;
   out_5351909948517241954[282] = 0;
   out_5351909948517241954[283] = 0;
   out_5351909948517241954[284] = 0;
   out_5351909948517241954[285] = 1;
   out_5351909948517241954[286] = 0;
   out_5351909948517241954[287] = 0;
   out_5351909948517241954[288] = 0;
   out_5351909948517241954[289] = 0;
   out_5351909948517241954[290] = 0;
   out_5351909948517241954[291] = 0;
   out_5351909948517241954[292] = 0;
   out_5351909948517241954[293] = 0;
   out_5351909948517241954[294] = 0;
   out_5351909948517241954[295] = 0;
   out_5351909948517241954[296] = 0;
   out_5351909948517241954[297] = 0;
   out_5351909948517241954[298] = 0;
   out_5351909948517241954[299] = 0;
   out_5351909948517241954[300] = 0;
   out_5351909948517241954[301] = 0;
   out_5351909948517241954[302] = 0;
   out_5351909948517241954[303] = 0;
   out_5351909948517241954[304] = 1;
   out_5351909948517241954[305] = 0;
   out_5351909948517241954[306] = 0;
   out_5351909948517241954[307] = 0;
   out_5351909948517241954[308] = 0;
   out_5351909948517241954[309] = 0;
   out_5351909948517241954[310] = 0;
   out_5351909948517241954[311] = 0;
   out_5351909948517241954[312] = 0;
   out_5351909948517241954[313] = 0;
   out_5351909948517241954[314] = 0;
   out_5351909948517241954[315] = 0;
   out_5351909948517241954[316] = 0;
   out_5351909948517241954[317] = 0;
   out_5351909948517241954[318] = 0;
   out_5351909948517241954[319] = 0;
   out_5351909948517241954[320] = 0;
   out_5351909948517241954[321] = 0;
   out_5351909948517241954[322] = 0;
   out_5351909948517241954[323] = 1;
}
void h_4(double *state, double *unused, double *out_358390589776990143) {
   out_358390589776990143[0] = state[6] + state[9];
   out_358390589776990143[1] = state[7] + state[10];
   out_358390589776990143[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_1907921248207480922) {
   out_1907921248207480922[0] = 0;
   out_1907921248207480922[1] = 0;
   out_1907921248207480922[2] = 0;
   out_1907921248207480922[3] = 0;
   out_1907921248207480922[4] = 0;
   out_1907921248207480922[5] = 0;
   out_1907921248207480922[6] = 1;
   out_1907921248207480922[7] = 0;
   out_1907921248207480922[8] = 0;
   out_1907921248207480922[9] = 1;
   out_1907921248207480922[10] = 0;
   out_1907921248207480922[11] = 0;
   out_1907921248207480922[12] = 0;
   out_1907921248207480922[13] = 0;
   out_1907921248207480922[14] = 0;
   out_1907921248207480922[15] = 0;
   out_1907921248207480922[16] = 0;
   out_1907921248207480922[17] = 0;
   out_1907921248207480922[18] = 0;
   out_1907921248207480922[19] = 0;
   out_1907921248207480922[20] = 0;
   out_1907921248207480922[21] = 0;
   out_1907921248207480922[22] = 0;
   out_1907921248207480922[23] = 0;
   out_1907921248207480922[24] = 0;
   out_1907921248207480922[25] = 1;
   out_1907921248207480922[26] = 0;
   out_1907921248207480922[27] = 0;
   out_1907921248207480922[28] = 1;
   out_1907921248207480922[29] = 0;
   out_1907921248207480922[30] = 0;
   out_1907921248207480922[31] = 0;
   out_1907921248207480922[32] = 0;
   out_1907921248207480922[33] = 0;
   out_1907921248207480922[34] = 0;
   out_1907921248207480922[35] = 0;
   out_1907921248207480922[36] = 0;
   out_1907921248207480922[37] = 0;
   out_1907921248207480922[38] = 0;
   out_1907921248207480922[39] = 0;
   out_1907921248207480922[40] = 0;
   out_1907921248207480922[41] = 0;
   out_1907921248207480922[42] = 0;
   out_1907921248207480922[43] = 0;
   out_1907921248207480922[44] = 1;
   out_1907921248207480922[45] = 0;
   out_1907921248207480922[46] = 0;
   out_1907921248207480922[47] = 1;
   out_1907921248207480922[48] = 0;
   out_1907921248207480922[49] = 0;
   out_1907921248207480922[50] = 0;
   out_1907921248207480922[51] = 0;
   out_1907921248207480922[52] = 0;
   out_1907921248207480922[53] = 0;
}
void h_10(double *state, double *unused, double *out_3316959461566506895) {
   out_3316959461566506895[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_3316959461566506895[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_3316959461566506895[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_913840009049565840) {
   out_913840009049565840[0] = 0;
   out_913840009049565840[1] = 9.8100000000000005*cos(state[1]);
   out_913840009049565840[2] = 0;
   out_913840009049565840[3] = 0;
   out_913840009049565840[4] = -state[8];
   out_913840009049565840[5] = state[7];
   out_913840009049565840[6] = 0;
   out_913840009049565840[7] = state[5];
   out_913840009049565840[8] = -state[4];
   out_913840009049565840[9] = 0;
   out_913840009049565840[10] = 0;
   out_913840009049565840[11] = 0;
   out_913840009049565840[12] = 1;
   out_913840009049565840[13] = 0;
   out_913840009049565840[14] = 0;
   out_913840009049565840[15] = 1;
   out_913840009049565840[16] = 0;
   out_913840009049565840[17] = 0;
   out_913840009049565840[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_913840009049565840[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_913840009049565840[20] = 0;
   out_913840009049565840[21] = state[8];
   out_913840009049565840[22] = 0;
   out_913840009049565840[23] = -state[6];
   out_913840009049565840[24] = -state[5];
   out_913840009049565840[25] = 0;
   out_913840009049565840[26] = state[3];
   out_913840009049565840[27] = 0;
   out_913840009049565840[28] = 0;
   out_913840009049565840[29] = 0;
   out_913840009049565840[30] = 0;
   out_913840009049565840[31] = 1;
   out_913840009049565840[32] = 0;
   out_913840009049565840[33] = 0;
   out_913840009049565840[34] = 1;
   out_913840009049565840[35] = 0;
   out_913840009049565840[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_913840009049565840[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_913840009049565840[38] = 0;
   out_913840009049565840[39] = -state[7];
   out_913840009049565840[40] = state[6];
   out_913840009049565840[41] = 0;
   out_913840009049565840[42] = state[4];
   out_913840009049565840[43] = -state[3];
   out_913840009049565840[44] = 0;
   out_913840009049565840[45] = 0;
   out_913840009049565840[46] = 0;
   out_913840009049565840[47] = 0;
   out_913840009049565840[48] = 0;
   out_913840009049565840[49] = 0;
   out_913840009049565840[50] = 1;
   out_913840009049565840[51] = 0;
   out_913840009049565840[52] = 0;
   out_913840009049565840[53] = 1;
}
void h_13(double *state, double *unused, double *out_2811325568776580864) {
   out_2811325568776580864[0] = state[3];
   out_2811325568776580864[1] = state[4];
   out_2811325568776580864[2] = state[5];
}
void H_13(double *state, double *unused, double *out_1304352577124851879) {
   out_1304352577124851879[0] = 0;
   out_1304352577124851879[1] = 0;
   out_1304352577124851879[2] = 0;
   out_1304352577124851879[3] = 1;
   out_1304352577124851879[4] = 0;
   out_1304352577124851879[5] = 0;
   out_1304352577124851879[6] = 0;
   out_1304352577124851879[7] = 0;
   out_1304352577124851879[8] = 0;
   out_1304352577124851879[9] = 0;
   out_1304352577124851879[10] = 0;
   out_1304352577124851879[11] = 0;
   out_1304352577124851879[12] = 0;
   out_1304352577124851879[13] = 0;
   out_1304352577124851879[14] = 0;
   out_1304352577124851879[15] = 0;
   out_1304352577124851879[16] = 0;
   out_1304352577124851879[17] = 0;
   out_1304352577124851879[18] = 0;
   out_1304352577124851879[19] = 0;
   out_1304352577124851879[20] = 0;
   out_1304352577124851879[21] = 0;
   out_1304352577124851879[22] = 1;
   out_1304352577124851879[23] = 0;
   out_1304352577124851879[24] = 0;
   out_1304352577124851879[25] = 0;
   out_1304352577124851879[26] = 0;
   out_1304352577124851879[27] = 0;
   out_1304352577124851879[28] = 0;
   out_1304352577124851879[29] = 0;
   out_1304352577124851879[30] = 0;
   out_1304352577124851879[31] = 0;
   out_1304352577124851879[32] = 0;
   out_1304352577124851879[33] = 0;
   out_1304352577124851879[34] = 0;
   out_1304352577124851879[35] = 0;
   out_1304352577124851879[36] = 0;
   out_1304352577124851879[37] = 0;
   out_1304352577124851879[38] = 0;
   out_1304352577124851879[39] = 0;
   out_1304352577124851879[40] = 0;
   out_1304352577124851879[41] = 1;
   out_1304352577124851879[42] = 0;
   out_1304352577124851879[43] = 0;
   out_1304352577124851879[44] = 0;
   out_1304352577124851879[45] = 0;
   out_1304352577124851879[46] = 0;
   out_1304352577124851879[47] = 0;
   out_1304352577124851879[48] = 0;
   out_1304352577124851879[49] = 0;
   out_1304352577124851879[50] = 0;
   out_1304352577124851879[51] = 0;
   out_1304352577124851879[52] = 0;
   out_1304352577124851879[53] = 0;
}
void h_14(double *state, double *unused, double *out_470700127672570802) {
   out_470700127672570802[0] = state[6];
   out_470700127672570802[1] = state[7];
   out_470700127672570802[2] = state[8];
}
void H_14(double *state, double *unused, double *out_2055319608132003607) {
   out_2055319608132003607[0] = 0;
   out_2055319608132003607[1] = 0;
   out_2055319608132003607[2] = 0;
   out_2055319608132003607[3] = 0;
   out_2055319608132003607[4] = 0;
   out_2055319608132003607[5] = 0;
   out_2055319608132003607[6] = 1;
   out_2055319608132003607[7] = 0;
   out_2055319608132003607[8] = 0;
   out_2055319608132003607[9] = 0;
   out_2055319608132003607[10] = 0;
   out_2055319608132003607[11] = 0;
   out_2055319608132003607[12] = 0;
   out_2055319608132003607[13] = 0;
   out_2055319608132003607[14] = 0;
   out_2055319608132003607[15] = 0;
   out_2055319608132003607[16] = 0;
   out_2055319608132003607[17] = 0;
   out_2055319608132003607[18] = 0;
   out_2055319608132003607[19] = 0;
   out_2055319608132003607[20] = 0;
   out_2055319608132003607[21] = 0;
   out_2055319608132003607[22] = 0;
   out_2055319608132003607[23] = 0;
   out_2055319608132003607[24] = 0;
   out_2055319608132003607[25] = 1;
   out_2055319608132003607[26] = 0;
   out_2055319608132003607[27] = 0;
   out_2055319608132003607[28] = 0;
   out_2055319608132003607[29] = 0;
   out_2055319608132003607[30] = 0;
   out_2055319608132003607[31] = 0;
   out_2055319608132003607[32] = 0;
   out_2055319608132003607[33] = 0;
   out_2055319608132003607[34] = 0;
   out_2055319608132003607[35] = 0;
   out_2055319608132003607[36] = 0;
   out_2055319608132003607[37] = 0;
   out_2055319608132003607[38] = 0;
   out_2055319608132003607[39] = 0;
   out_2055319608132003607[40] = 0;
   out_2055319608132003607[41] = 0;
   out_2055319608132003607[42] = 0;
   out_2055319608132003607[43] = 0;
   out_2055319608132003607[44] = 1;
   out_2055319608132003607[45] = 0;
   out_2055319608132003607[46] = 0;
   out_2055319608132003607[47] = 0;
   out_2055319608132003607[48] = 0;
   out_2055319608132003607[49] = 0;
   out_2055319608132003607[50] = 0;
   out_2055319608132003607[51] = 0;
   out_2055319608132003607[52] = 0;
   out_2055319608132003607[53] = 0;
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
void pose_err_fun(double *nom_x, double *delta_x, double *out_7121613786142052873) {
  err_fun(nom_x, delta_x, out_7121613786142052873);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_4360274276000378130) {
  inv_err_fun(nom_x, true_x, out_4360274276000378130);
}
void pose_H_mod_fun(double *state, double *out_6957806766333672764) {
  H_mod_fun(state, out_6957806766333672764);
}
void pose_f_fun(double *state, double dt, double *out_5556993601589438608) {
  f_fun(state,  dt, out_5556993601589438608);
}
void pose_F_fun(double *state, double dt, double *out_5351909948517241954) {
  F_fun(state,  dt, out_5351909948517241954);
}
void pose_h_4(double *state, double *unused, double *out_358390589776990143) {
  h_4(state, unused, out_358390589776990143);
}
void pose_H_4(double *state, double *unused, double *out_1907921248207480922) {
  H_4(state, unused, out_1907921248207480922);
}
void pose_h_10(double *state, double *unused, double *out_3316959461566506895) {
  h_10(state, unused, out_3316959461566506895);
}
void pose_H_10(double *state, double *unused, double *out_913840009049565840) {
  H_10(state, unused, out_913840009049565840);
}
void pose_h_13(double *state, double *unused, double *out_2811325568776580864) {
  h_13(state, unused, out_2811325568776580864);
}
void pose_H_13(double *state, double *unused, double *out_1304352577124851879) {
  H_13(state, unused, out_1304352577124851879);
}
void pose_h_14(double *state, double *unused, double *out_470700127672570802) {
  h_14(state, unused, out_470700127672570802);
}
void pose_H_14(double *state, double *unused, double *out_2055319608132003607) {
  H_14(state, unused, out_2055319608132003607);
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
