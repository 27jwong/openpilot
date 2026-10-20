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
void err_fun(double *nom_x, double *delta_x, double *out_2510165527669125945) {
   out_2510165527669125945[0] = delta_x[0] + nom_x[0];
   out_2510165527669125945[1] = delta_x[1] + nom_x[1];
   out_2510165527669125945[2] = delta_x[2] + nom_x[2];
   out_2510165527669125945[3] = delta_x[3] + nom_x[3];
   out_2510165527669125945[4] = delta_x[4] + nom_x[4];
   out_2510165527669125945[5] = delta_x[5] + nom_x[5];
   out_2510165527669125945[6] = delta_x[6] + nom_x[6];
   out_2510165527669125945[7] = delta_x[7] + nom_x[7];
   out_2510165527669125945[8] = delta_x[8] + nom_x[8];
   out_2510165527669125945[9] = delta_x[9] + nom_x[9];
   out_2510165527669125945[10] = delta_x[10] + nom_x[10];
   out_2510165527669125945[11] = delta_x[11] + nom_x[11];
   out_2510165527669125945[12] = delta_x[12] + nom_x[12];
   out_2510165527669125945[13] = delta_x[13] + nom_x[13];
   out_2510165527669125945[14] = delta_x[14] + nom_x[14];
   out_2510165527669125945[15] = delta_x[15] + nom_x[15];
   out_2510165527669125945[16] = delta_x[16] + nom_x[16];
   out_2510165527669125945[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_2827775544900411455) {
   out_2827775544900411455[0] = -nom_x[0] + true_x[0];
   out_2827775544900411455[1] = -nom_x[1] + true_x[1];
   out_2827775544900411455[2] = -nom_x[2] + true_x[2];
   out_2827775544900411455[3] = -nom_x[3] + true_x[3];
   out_2827775544900411455[4] = -nom_x[4] + true_x[4];
   out_2827775544900411455[5] = -nom_x[5] + true_x[5];
   out_2827775544900411455[6] = -nom_x[6] + true_x[6];
   out_2827775544900411455[7] = -nom_x[7] + true_x[7];
   out_2827775544900411455[8] = -nom_x[8] + true_x[8];
   out_2827775544900411455[9] = -nom_x[9] + true_x[9];
   out_2827775544900411455[10] = -nom_x[10] + true_x[10];
   out_2827775544900411455[11] = -nom_x[11] + true_x[11];
   out_2827775544900411455[12] = -nom_x[12] + true_x[12];
   out_2827775544900411455[13] = -nom_x[13] + true_x[13];
   out_2827775544900411455[14] = -nom_x[14] + true_x[14];
   out_2827775544900411455[15] = -nom_x[15] + true_x[15];
   out_2827775544900411455[16] = -nom_x[16] + true_x[16];
   out_2827775544900411455[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_6581160706960496502) {
   out_6581160706960496502[0] = 1.0;
   out_6581160706960496502[1] = 0.0;
   out_6581160706960496502[2] = 0.0;
   out_6581160706960496502[3] = 0.0;
   out_6581160706960496502[4] = 0.0;
   out_6581160706960496502[5] = 0.0;
   out_6581160706960496502[6] = 0.0;
   out_6581160706960496502[7] = 0.0;
   out_6581160706960496502[8] = 0.0;
   out_6581160706960496502[9] = 0.0;
   out_6581160706960496502[10] = 0.0;
   out_6581160706960496502[11] = 0.0;
   out_6581160706960496502[12] = 0.0;
   out_6581160706960496502[13] = 0.0;
   out_6581160706960496502[14] = 0.0;
   out_6581160706960496502[15] = 0.0;
   out_6581160706960496502[16] = 0.0;
   out_6581160706960496502[17] = 0.0;
   out_6581160706960496502[18] = 0.0;
   out_6581160706960496502[19] = 1.0;
   out_6581160706960496502[20] = 0.0;
   out_6581160706960496502[21] = 0.0;
   out_6581160706960496502[22] = 0.0;
   out_6581160706960496502[23] = 0.0;
   out_6581160706960496502[24] = 0.0;
   out_6581160706960496502[25] = 0.0;
   out_6581160706960496502[26] = 0.0;
   out_6581160706960496502[27] = 0.0;
   out_6581160706960496502[28] = 0.0;
   out_6581160706960496502[29] = 0.0;
   out_6581160706960496502[30] = 0.0;
   out_6581160706960496502[31] = 0.0;
   out_6581160706960496502[32] = 0.0;
   out_6581160706960496502[33] = 0.0;
   out_6581160706960496502[34] = 0.0;
   out_6581160706960496502[35] = 0.0;
   out_6581160706960496502[36] = 0.0;
   out_6581160706960496502[37] = 0.0;
   out_6581160706960496502[38] = 1.0;
   out_6581160706960496502[39] = 0.0;
   out_6581160706960496502[40] = 0.0;
   out_6581160706960496502[41] = 0.0;
   out_6581160706960496502[42] = 0.0;
   out_6581160706960496502[43] = 0.0;
   out_6581160706960496502[44] = 0.0;
   out_6581160706960496502[45] = 0.0;
   out_6581160706960496502[46] = 0.0;
   out_6581160706960496502[47] = 0.0;
   out_6581160706960496502[48] = 0.0;
   out_6581160706960496502[49] = 0.0;
   out_6581160706960496502[50] = 0.0;
   out_6581160706960496502[51] = 0.0;
   out_6581160706960496502[52] = 0.0;
   out_6581160706960496502[53] = 0.0;
   out_6581160706960496502[54] = 0.0;
   out_6581160706960496502[55] = 0.0;
   out_6581160706960496502[56] = 0.0;
   out_6581160706960496502[57] = 1.0;
   out_6581160706960496502[58] = 0.0;
   out_6581160706960496502[59] = 0.0;
   out_6581160706960496502[60] = 0.0;
   out_6581160706960496502[61] = 0.0;
   out_6581160706960496502[62] = 0.0;
   out_6581160706960496502[63] = 0.0;
   out_6581160706960496502[64] = 0.0;
   out_6581160706960496502[65] = 0.0;
   out_6581160706960496502[66] = 0.0;
   out_6581160706960496502[67] = 0.0;
   out_6581160706960496502[68] = 0.0;
   out_6581160706960496502[69] = 0.0;
   out_6581160706960496502[70] = 0.0;
   out_6581160706960496502[71] = 0.0;
   out_6581160706960496502[72] = 0.0;
   out_6581160706960496502[73] = 0.0;
   out_6581160706960496502[74] = 0.0;
   out_6581160706960496502[75] = 0.0;
   out_6581160706960496502[76] = 1.0;
   out_6581160706960496502[77] = 0.0;
   out_6581160706960496502[78] = 0.0;
   out_6581160706960496502[79] = 0.0;
   out_6581160706960496502[80] = 0.0;
   out_6581160706960496502[81] = 0.0;
   out_6581160706960496502[82] = 0.0;
   out_6581160706960496502[83] = 0.0;
   out_6581160706960496502[84] = 0.0;
   out_6581160706960496502[85] = 0.0;
   out_6581160706960496502[86] = 0.0;
   out_6581160706960496502[87] = 0.0;
   out_6581160706960496502[88] = 0.0;
   out_6581160706960496502[89] = 0.0;
   out_6581160706960496502[90] = 0.0;
   out_6581160706960496502[91] = 0.0;
   out_6581160706960496502[92] = 0.0;
   out_6581160706960496502[93] = 0.0;
   out_6581160706960496502[94] = 0.0;
   out_6581160706960496502[95] = 1.0;
   out_6581160706960496502[96] = 0.0;
   out_6581160706960496502[97] = 0.0;
   out_6581160706960496502[98] = 0.0;
   out_6581160706960496502[99] = 0.0;
   out_6581160706960496502[100] = 0.0;
   out_6581160706960496502[101] = 0.0;
   out_6581160706960496502[102] = 0.0;
   out_6581160706960496502[103] = 0.0;
   out_6581160706960496502[104] = 0.0;
   out_6581160706960496502[105] = 0.0;
   out_6581160706960496502[106] = 0.0;
   out_6581160706960496502[107] = 0.0;
   out_6581160706960496502[108] = 0.0;
   out_6581160706960496502[109] = 0.0;
   out_6581160706960496502[110] = 0.0;
   out_6581160706960496502[111] = 0.0;
   out_6581160706960496502[112] = 0.0;
   out_6581160706960496502[113] = 0.0;
   out_6581160706960496502[114] = 1.0;
   out_6581160706960496502[115] = 0.0;
   out_6581160706960496502[116] = 0.0;
   out_6581160706960496502[117] = 0.0;
   out_6581160706960496502[118] = 0.0;
   out_6581160706960496502[119] = 0.0;
   out_6581160706960496502[120] = 0.0;
   out_6581160706960496502[121] = 0.0;
   out_6581160706960496502[122] = 0.0;
   out_6581160706960496502[123] = 0.0;
   out_6581160706960496502[124] = 0.0;
   out_6581160706960496502[125] = 0.0;
   out_6581160706960496502[126] = 0.0;
   out_6581160706960496502[127] = 0.0;
   out_6581160706960496502[128] = 0.0;
   out_6581160706960496502[129] = 0.0;
   out_6581160706960496502[130] = 0.0;
   out_6581160706960496502[131] = 0.0;
   out_6581160706960496502[132] = 0.0;
   out_6581160706960496502[133] = 1.0;
   out_6581160706960496502[134] = 0.0;
   out_6581160706960496502[135] = 0.0;
   out_6581160706960496502[136] = 0.0;
   out_6581160706960496502[137] = 0.0;
   out_6581160706960496502[138] = 0.0;
   out_6581160706960496502[139] = 0.0;
   out_6581160706960496502[140] = 0.0;
   out_6581160706960496502[141] = 0.0;
   out_6581160706960496502[142] = 0.0;
   out_6581160706960496502[143] = 0.0;
   out_6581160706960496502[144] = 0.0;
   out_6581160706960496502[145] = 0.0;
   out_6581160706960496502[146] = 0.0;
   out_6581160706960496502[147] = 0.0;
   out_6581160706960496502[148] = 0.0;
   out_6581160706960496502[149] = 0.0;
   out_6581160706960496502[150] = 0.0;
   out_6581160706960496502[151] = 0.0;
   out_6581160706960496502[152] = 1.0;
   out_6581160706960496502[153] = 0.0;
   out_6581160706960496502[154] = 0.0;
   out_6581160706960496502[155] = 0.0;
   out_6581160706960496502[156] = 0.0;
   out_6581160706960496502[157] = 0.0;
   out_6581160706960496502[158] = 0.0;
   out_6581160706960496502[159] = 0.0;
   out_6581160706960496502[160] = 0.0;
   out_6581160706960496502[161] = 0.0;
   out_6581160706960496502[162] = 0.0;
   out_6581160706960496502[163] = 0.0;
   out_6581160706960496502[164] = 0.0;
   out_6581160706960496502[165] = 0.0;
   out_6581160706960496502[166] = 0.0;
   out_6581160706960496502[167] = 0.0;
   out_6581160706960496502[168] = 0.0;
   out_6581160706960496502[169] = 0.0;
   out_6581160706960496502[170] = 0.0;
   out_6581160706960496502[171] = 1.0;
   out_6581160706960496502[172] = 0.0;
   out_6581160706960496502[173] = 0.0;
   out_6581160706960496502[174] = 0.0;
   out_6581160706960496502[175] = 0.0;
   out_6581160706960496502[176] = 0.0;
   out_6581160706960496502[177] = 0.0;
   out_6581160706960496502[178] = 0.0;
   out_6581160706960496502[179] = 0.0;
   out_6581160706960496502[180] = 0.0;
   out_6581160706960496502[181] = 0.0;
   out_6581160706960496502[182] = 0.0;
   out_6581160706960496502[183] = 0.0;
   out_6581160706960496502[184] = 0.0;
   out_6581160706960496502[185] = 0.0;
   out_6581160706960496502[186] = 0.0;
   out_6581160706960496502[187] = 0.0;
   out_6581160706960496502[188] = 0.0;
   out_6581160706960496502[189] = 0.0;
   out_6581160706960496502[190] = 1.0;
   out_6581160706960496502[191] = 0.0;
   out_6581160706960496502[192] = 0.0;
   out_6581160706960496502[193] = 0.0;
   out_6581160706960496502[194] = 0.0;
   out_6581160706960496502[195] = 0.0;
   out_6581160706960496502[196] = 0.0;
   out_6581160706960496502[197] = 0.0;
   out_6581160706960496502[198] = 0.0;
   out_6581160706960496502[199] = 0.0;
   out_6581160706960496502[200] = 0.0;
   out_6581160706960496502[201] = 0.0;
   out_6581160706960496502[202] = 0.0;
   out_6581160706960496502[203] = 0.0;
   out_6581160706960496502[204] = 0.0;
   out_6581160706960496502[205] = 0.0;
   out_6581160706960496502[206] = 0.0;
   out_6581160706960496502[207] = 0.0;
   out_6581160706960496502[208] = 0.0;
   out_6581160706960496502[209] = 1.0;
   out_6581160706960496502[210] = 0.0;
   out_6581160706960496502[211] = 0.0;
   out_6581160706960496502[212] = 0.0;
   out_6581160706960496502[213] = 0.0;
   out_6581160706960496502[214] = 0.0;
   out_6581160706960496502[215] = 0.0;
   out_6581160706960496502[216] = 0.0;
   out_6581160706960496502[217] = 0.0;
   out_6581160706960496502[218] = 0.0;
   out_6581160706960496502[219] = 0.0;
   out_6581160706960496502[220] = 0.0;
   out_6581160706960496502[221] = 0.0;
   out_6581160706960496502[222] = 0.0;
   out_6581160706960496502[223] = 0.0;
   out_6581160706960496502[224] = 0.0;
   out_6581160706960496502[225] = 0.0;
   out_6581160706960496502[226] = 0.0;
   out_6581160706960496502[227] = 0.0;
   out_6581160706960496502[228] = 1.0;
   out_6581160706960496502[229] = 0.0;
   out_6581160706960496502[230] = 0.0;
   out_6581160706960496502[231] = 0.0;
   out_6581160706960496502[232] = 0.0;
   out_6581160706960496502[233] = 0.0;
   out_6581160706960496502[234] = 0.0;
   out_6581160706960496502[235] = 0.0;
   out_6581160706960496502[236] = 0.0;
   out_6581160706960496502[237] = 0.0;
   out_6581160706960496502[238] = 0.0;
   out_6581160706960496502[239] = 0.0;
   out_6581160706960496502[240] = 0.0;
   out_6581160706960496502[241] = 0.0;
   out_6581160706960496502[242] = 0.0;
   out_6581160706960496502[243] = 0.0;
   out_6581160706960496502[244] = 0.0;
   out_6581160706960496502[245] = 0.0;
   out_6581160706960496502[246] = 0.0;
   out_6581160706960496502[247] = 1.0;
   out_6581160706960496502[248] = 0.0;
   out_6581160706960496502[249] = 0.0;
   out_6581160706960496502[250] = 0.0;
   out_6581160706960496502[251] = 0.0;
   out_6581160706960496502[252] = 0.0;
   out_6581160706960496502[253] = 0.0;
   out_6581160706960496502[254] = 0.0;
   out_6581160706960496502[255] = 0.0;
   out_6581160706960496502[256] = 0.0;
   out_6581160706960496502[257] = 0.0;
   out_6581160706960496502[258] = 0.0;
   out_6581160706960496502[259] = 0.0;
   out_6581160706960496502[260] = 0.0;
   out_6581160706960496502[261] = 0.0;
   out_6581160706960496502[262] = 0.0;
   out_6581160706960496502[263] = 0.0;
   out_6581160706960496502[264] = 0.0;
   out_6581160706960496502[265] = 0.0;
   out_6581160706960496502[266] = 1.0;
   out_6581160706960496502[267] = 0.0;
   out_6581160706960496502[268] = 0.0;
   out_6581160706960496502[269] = 0.0;
   out_6581160706960496502[270] = 0.0;
   out_6581160706960496502[271] = 0.0;
   out_6581160706960496502[272] = 0.0;
   out_6581160706960496502[273] = 0.0;
   out_6581160706960496502[274] = 0.0;
   out_6581160706960496502[275] = 0.0;
   out_6581160706960496502[276] = 0.0;
   out_6581160706960496502[277] = 0.0;
   out_6581160706960496502[278] = 0.0;
   out_6581160706960496502[279] = 0.0;
   out_6581160706960496502[280] = 0.0;
   out_6581160706960496502[281] = 0.0;
   out_6581160706960496502[282] = 0.0;
   out_6581160706960496502[283] = 0.0;
   out_6581160706960496502[284] = 0.0;
   out_6581160706960496502[285] = 1.0;
   out_6581160706960496502[286] = 0.0;
   out_6581160706960496502[287] = 0.0;
   out_6581160706960496502[288] = 0.0;
   out_6581160706960496502[289] = 0.0;
   out_6581160706960496502[290] = 0.0;
   out_6581160706960496502[291] = 0.0;
   out_6581160706960496502[292] = 0.0;
   out_6581160706960496502[293] = 0.0;
   out_6581160706960496502[294] = 0.0;
   out_6581160706960496502[295] = 0.0;
   out_6581160706960496502[296] = 0.0;
   out_6581160706960496502[297] = 0.0;
   out_6581160706960496502[298] = 0.0;
   out_6581160706960496502[299] = 0.0;
   out_6581160706960496502[300] = 0.0;
   out_6581160706960496502[301] = 0.0;
   out_6581160706960496502[302] = 0.0;
   out_6581160706960496502[303] = 0.0;
   out_6581160706960496502[304] = 1.0;
   out_6581160706960496502[305] = 0.0;
   out_6581160706960496502[306] = 0.0;
   out_6581160706960496502[307] = 0.0;
   out_6581160706960496502[308] = 0.0;
   out_6581160706960496502[309] = 0.0;
   out_6581160706960496502[310] = 0.0;
   out_6581160706960496502[311] = 0.0;
   out_6581160706960496502[312] = 0.0;
   out_6581160706960496502[313] = 0.0;
   out_6581160706960496502[314] = 0.0;
   out_6581160706960496502[315] = 0.0;
   out_6581160706960496502[316] = 0.0;
   out_6581160706960496502[317] = 0.0;
   out_6581160706960496502[318] = 0.0;
   out_6581160706960496502[319] = 0.0;
   out_6581160706960496502[320] = 0.0;
   out_6581160706960496502[321] = 0.0;
   out_6581160706960496502[322] = 0.0;
   out_6581160706960496502[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_1715652979675739070) {
   out_1715652979675739070[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_1715652979675739070[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_1715652979675739070[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_1715652979675739070[3] = dt*state[12] + state[3];
   out_1715652979675739070[4] = dt*state[13] + state[4];
   out_1715652979675739070[5] = dt*state[14] + state[5];
   out_1715652979675739070[6] = state[6];
   out_1715652979675739070[7] = state[7];
   out_1715652979675739070[8] = state[8];
   out_1715652979675739070[9] = state[9];
   out_1715652979675739070[10] = state[10];
   out_1715652979675739070[11] = state[11];
   out_1715652979675739070[12] = state[12];
   out_1715652979675739070[13] = state[13];
   out_1715652979675739070[14] = state[14];
   out_1715652979675739070[15] = state[15];
   out_1715652979675739070[16] = state[16];
   out_1715652979675739070[17] = state[17];
}
void F_fun(double *state, double dt, double *out_7854120589136729219) {
   out_7854120589136729219[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_7854120589136729219[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_7854120589136729219[2] = 0;
   out_7854120589136729219[3] = 0;
   out_7854120589136729219[4] = 0;
   out_7854120589136729219[5] = 0;
   out_7854120589136729219[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_7854120589136729219[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_7854120589136729219[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_7854120589136729219[9] = 0;
   out_7854120589136729219[10] = 0;
   out_7854120589136729219[11] = 0;
   out_7854120589136729219[12] = 0;
   out_7854120589136729219[13] = 0;
   out_7854120589136729219[14] = 0;
   out_7854120589136729219[15] = 0;
   out_7854120589136729219[16] = 0;
   out_7854120589136729219[17] = 0;
   out_7854120589136729219[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_7854120589136729219[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_7854120589136729219[20] = 0;
   out_7854120589136729219[21] = 0;
   out_7854120589136729219[22] = 0;
   out_7854120589136729219[23] = 0;
   out_7854120589136729219[24] = 0;
   out_7854120589136729219[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_7854120589136729219[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_7854120589136729219[27] = 0;
   out_7854120589136729219[28] = 0;
   out_7854120589136729219[29] = 0;
   out_7854120589136729219[30] = 0;
   out_7854120589136729219[31] = 0;
   out_7854120589136729219[32] = 0;
   out_7854120589136729219[33] = 0;
   out_7854120589136729219[34] = 0;
   out_7854120589136729219[35] = 0;
   out_7854120589136729219[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_7854120589136729219[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_7854120589136729219[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_7854120589136729219[39] = 0;
   out_7854120589136729219[40] = 0;
   out_7854120589136729219[41] = 0;
   out_7854120589136729219[42] = 0;
   out_7854120589136729219[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_7854120589136729219[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_7854120589136729219[45] = 0;
   out_7854120589136729219[46] = 0;
   out_7854120589136729219[47] = 0;
   out_7854120589136729219[48] = 0;
   out_7854120589136729219[49] = 0;
   out_7854120589136729219[50] = 0;
   out_7854120589136729219[51] = 0;
   out_7854120589136729219[52] = 0;
   out_7854120589136729219[53] = 0;
   out_7854120589136729219[54] = 0;
   out_7854120589136729219[55] = 0;
   out_7854120589136729219[56] = 0;
   out_7854120589136729219[57] = 1;
   out_7854120589136729219[58] = 0;
   out_7854120589136729219[59] = 0;
   out_7854120589136729219[60] = 0;
   out_7854120589136729219[61] = 0;
   out_7854120589136729219[62] = 0;
   out_7854120589136729219[63] = 0;
   out_7854120589136729219[64] = 0;
   out_7854120589136729219[65] = 0;
   out_7854120589136729219[66] = dt;
   out_7854120589136729219[67] = 0;
   out_7854120589136729219[68] = 0;
   out_7854120589136729219[69] = 0;
   out_7854120589136729219[70] = 0;
   out_7854120589136729219[71] = 0;
   out_7854120589136729219[72] = 0;
   out_7854120589136729219[73] = 0;
   out_7854120589136729219[74] = 0;
   out_7854120589136729219[75] = 0;
   out_7854120589136729219[76] = 1;
   out_7854120589136729219[77] = 0;
   out_7854120589136729219[78] = 0;
   out_7854120589136729219[79] = 0;
   out_7854120589136729219[80] = 0;
   out_7854120589136729219[81] = 0;
   out_7854120589136729219[82] = 0;
   out_7854120589136729219[83] = 0;
   out_7854120589136729219[84] = 0;
   out_7854120589136729219[85] = dt;
   out_7854120589136729219[86] = 0;
   out_7854120589136729219[87] = 0;
   out_7854120589136729219[88] = 0;
   out_7854120589136729219[89] = 0;
   out_7854120589136729219[90] = 0;
   out_7854120589136729219[91] = 0;
   out_7854120589136729219[92] = 0;
   out_7854120589136729219[93] = 0;
   out_7854120589136729219[94] = 0;
   out_7854120589136729219[95] = 1;
   out_7854120589136729219[96] = 0;
   out_7854120589136729219[97] = 0;
   out_7854120589136729219[98] = 0;
   out_7854120589136729219[99] = 0;
   out_7854120589136729219[100] = 0;
   out_7854120589136729219[101] = 0;
   out_7854120589136729219[102] = 0;
   out_7854120589136729219[103] = 0;
   out_7854120589136729219[104] = dt;
   out_7854120589136729219[105] = 0;
   out_7854120589136729219[106] = 0;
   out_7854120589136729219[107] = 0;
   out_7854120589136729219[108] = 0;
   out_7854120589136729219[109] = 0;
   out_7854120589136729219[110] = 0;
   out_7854120589136729219[111] = 0;
   out_7854120589136729219[112] = 0;
   out_7854120589136729219[113] = 0;
   out_7854120589136729219[114] = 1;
   out_7854120589136729219[115] = 0;
   out_7854120589136729219[116] = 0;
   out_7854120589136729219[117] = 0;
   out_7854120589136729219[118] = 0;
   out_7854120589136729219[119] = 0;
   out_7854120589136729219[120] = 0;
   out_7854120589136729219[121] = 0;
   out_7854120589136729219[122] = 0;
   out_7854120589136729219[123] = 0;
   out_7854120589136729219[124] = 0;
   out_7854120589136729219[125] = 0;
   out_7854120589136729219[126] = 0;
   out_7854120589136729219[127] = 0;
   out_7854120589136729219[128] = 0;
   out_7854120589136729219[129] = 0;
   out_7854120589136729219[130] = 0;
   out_7854120589136729219[131] = 0;
   out_7854120589136729219[132] = 0;
   out_7854120589136729219[133] = 1;
   out_7854120589136729219[134] = 0;
   out_7854120589136729219[135] = 0;
   out_7854120589136729219[136] = 0;
   out_7854120589136729219[137] = 0;
   out_7854120589136729219[138] = 0;
   out_7854120589136729219[139] = 0;
   out_7854120589136729219[140] = 0;
   out_7854120589136729219[141] = 0;
   out_7854120589136729219[142] = 0;
   out_7854120589136729219[143] = 0;
   out_7854120589136729219[144] = 0;
   out_7854120589136729219[145] = 0;
   out_7854120589136729219[146] = 0;
   out_7854120589136729219[147] = 0;
   out_7854120589136729219[148] = 0;
   out_7854120589136729219[149] = 0;
   out_7854120589136729219[150] = 0;
   out_7854120589136729219[151] = 0;
   out_7854120589136729219[152] = 1;
   out_7854120589136729219[153] = 0;
   out_7854120589136729219[154] = 0;
   out_7854120589136729219[155] = 0;
   out_7854120589136729219[156] = 0;
   out_7854120589136729219[157] = 0;
   out_7854120589136729219[158] = 0;
   out_7854120589136729219[159] = 0;
   out_7854120589136729219[160] = 0;
   out_7854120589136729219[161] = 0;
   out_7854120589136729219[162] = 0;
   out_7854120589136729219[163] = 0;
   out_7854120589136729219[164] = 0;
   out_7854120589136729219[165] = 0;
   out_7854120589136729219[166] = 0;
   out_7854120589136729219[167] = 0;
   out_7854120589136729219[168] = 0;
   out_7854120589136729219[169] = 0;
   out_7854120589136729219[170] = 0;
   out_7854120589136729219[171] = 1;
   out_7854120589136729219[172] = 0;
   out_7854120589136729219[173] = 0;
   out_7854120589136729219[174] = 0;
   out_7854120589136729219[175] = 0;
   out_7854120589136729219[176] = 0;
   out_7854120589136729219[177] = 0;
   out_7854120589136729219[178] = 0;
   out_7854120589136729219[179] = 0;
   out_7854120589136729219[180] = 0;
   out_7854120589136729219[181] = 0;
   out_7854120589136729219[182] = 0;
   out_7854120589136729219[183] = 0;
   out_7854120589136729219[184] = 0;
   out_7854120589136729219[185] = 0;
   out_7854120589136729219[186] = 0;
   out_7854120589136729219[187] = 0;
   out_7854120589136729219[188] = 0;
   out_7854120589136729219[189] = 0;
   out_7854120589136729219[190] = 1;
   out_7854120589136729219[191] = 0;
   out_7854120589136729219[192] = 0;
   out_7854120589136729219[193] = 0;
   out_7854120589136729219[194] = 0;
   out_7854120589136729219[195] = 0;
   out_7854120589136729219[196] = 0;
   out_7854120589136729219[197] = 0;
   out_7854120589136729219[198] = 0;
   out_7854120589136729219[199] = 0;
   out_7854120589136729219[200] = 0;
   out_7854120589136729219[201] = 0;
   out_7854120589136729219[202] = 0;
   out_7854120589136729219[203] = 0;
   out_7854120589136729219[204] = 0;
   out_7854120589136729219[205] = 0;
   out_7854120589136729219[206] = 0;
   out_7854120589136729219[207] = 0;
   out_7854120589136729219[208] = 0;
   out_7854120589136729219[209] = 1;
   out_7854120589136729219[210] = 0;
   out_7854120589136729219[211] = 0;
   out_7854120589136729219[212] = 0;
   out_7854120589136729219[213] = 0;
   out_7854120589136729219[214] = 0;
   out_7854120589136729219[215] = 0;
   out_7854120589136729219[216] = 0;
   out_7854120589136729219[217] = 0;
   out_7854120589136729219[218] = 0;
   out_7854120589136729219[219] = 0;
   out_7854120589136729219[220] = 0;
   out_7854120589136729219[221] = 0;
   out_7854120589136729219[222] = 0;
   out_7854120589136729219[223] = 0;
   out_7854120589136729219[224] = 0;
   out_7854120589136729219[225] = 0;
   out_7854120589136729219[226] = 0;
   out_7854120589136729219[227] = 0;
   out_7854120589136729219[228] = 1;
   out_7854120589136729219[229] = 0;
   out_7854120589136729219[230] = 0;
   out_7854120589136729219[231] = 0;
   out_7854120589136729219[232] = 0;
   out_7854120589136729219[233] = 0;
   out_7854120589136729219[234] = 0;
   out_7854120589136729219[235] = 0;
   out_7854120589136729219[236] = 0;
   out_7854120589136729219[237] = 0;
   out_7854120589136729219[238] = 0;
   out_7854120589136729219[239] = 0;
   out_7854120589136729219[240] = 0;
   out_7854120589136729219[241] = 0;
   out_7854120589136729219[242] = 0;
   out_7854120589136729219[243] = 0;
   out_7854120589136729219[244] = 0;
   out_7854120589136729219[245] = 0;
   out_7854120589136729219[246] = 0;
   out_7854120589136729219[247] = 1;
   out_7854120589136729219[248] = 0;
   out_7854120589136729219[249] = 0;
   out_7854120589136729219[250] = 0;
   out_7854120589136729219[251] = 0;
   out_7854120589136729219[252] = 0;
   out_7854120589136729219[253] = 0;
   out_7854120589136729219[254] = 0;
   out_7854120589136729219[255] = 0;
   out_7854120589136729219[256] = 0;
   out_7854120589136729219[257] = 0;
   out_7854120589136729219[258] = 0;
   out_7854120589136729219[259] = 0;
   out_7854120589136729219[260] = 0;
   out_7854120589136729219[261] = 0;
   out_7854120589136729219[262] = 0;
   out_7854120589136729219[263] = 0;
   out_7854120589136729219[264] = 0;
   out_7854120589136729219[265] = 0;
   out_7854120589136729219[266] = 1;
   out_7854120589136729219[267] = 0;
   out_7854120589136729219[268] = 0;
   out_7854120589136729219[269] = 0;
   out_7854120589136729219[270] = 0;
   out_7854120589136729219[271] = 0;
   out_7854120589136729219[272] = 0;
   out_7854120589136729219[273] = 0;
   out_7854120589136729219[274] = 0;
   out_7854120589136729219[275] = 0;
   out_7854120589136729219[276] = 0;
   out_7854120589136729219[277] = 0;
   out_7854120589136729219[278] = 0;
   out_7854120589136729219[279] = 0;
   out_7854120589136729219[280] = 0;
   out_7854120589136729219[281] = 0;
   out_7854120589136729219[282] = 0;
   out_7854120589136729219[283] = 0;
   out_7854120589136729219[284] = 0;
   out_7854120589136729219[285] = 1;
   out_7854120589136729219[286] = 0;
   out_7854120589136729219[287] = 0;
   out_7854120589136729219[288] = 0;
   out_7854120589136729219[289] = 0;
   out_7854120589136729219[290] = 0;
   out_7854120589136729219[291] = 0;
   out_7854120589136729219[292] = 0;
   out_7854120589136729219[293] = 0;
   out_7854120589136729219[294] = 0;
   out_7854120589136729219[295] = 0;
   out_7854120589136729219[296] = 0;
   out_7854120589136729219[297] = 0;
   out_7854120589136729219[298] = 0;
   out_7854120589136729219[299] = 0;
   out_7854120589136729219[300] = 0;
   out_7854120589136729219[301] = 0;
   out_7854120589136729219[302] = 0;
   out_7854120589136729219[303] = 0;
   out_7854120589136729219[304] = 1;
   out_7854120589136729219[305] = 0;
   out_7854120589136729219[306] = 0;
   out_7854120589136729219[307] = 0;
   out_7854120589136729219[308] = 0;
   out_7854120589136729219[309] = 0;
   out_7854120589136729219[310] = 0;
   out_7854120589136729219[311] = 0;
   out_7854120589136729219[312] = 0;
   out_7854120589136729219[313] = 0;
   out_7854120589136729219[314] = 0;
   out_7854120589136729219[315] = 0;
   out_7854120589136729219[316] = 0;
   out_7854120589136729219[317] = 0;
   out_7854120589136729219[318] = 0;
   out_7854120589136729219[319] = 0;
   out_7854120589136729219[320] = 0;
   out_7854120589136729219[321] = 0;
   out_7854120589136729219[322] = 0;
   out_7854120589136729219[323] = 1;
}
void h_4(double *state, double *unused, double *out_3221272873403501279) {
   out_3221272873403501279[0] = state[6] + state[9];
   out_3221272873403501279[1] = state[7] + state[10];
   out_3221272873403501279[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_5826999704905783895) {
   out_5826999704905783895[0] = 0;
   out_5826999704905783895[1] = 0;
   out_5826999704905783895[2] = 0;
   out_5826999704905783895[3] = 0;
   out_5826999704905783895[4] = 0;
   out_5826999704905783895[5] = 0;
   out_5826999704905783895[6] = 1;
   out_5826999704905783895[7] = 0;
   out_5826999704905783895[8] = 0;
   out_5826999704905783895[9] = 1;
   out_5826999704905783895[10] = 0;
   out_5826999704905783895[11] = 0;
   out_5826999704905783895[12] = 0;
   out_5826999704905783895[13] = 0;
   out_5826999704905783895[14] = 0;
   out_5826999704905783895[15] = 0;
   out_5826999704905783895[16] = 0;
   out_5826999704905783895[17] = 0;
   out_5826999704905783895[18] = 0;
   out_5826999704905783895[19] = 0;
   out_5826999704905783895[20] = 0;
   out_5826999704905783895[21] = 0;
   out_5826999704905783895[22] = 0;
   out_5826999704905783895[23] = 0;
   out_5826999704905783895[24] = 0;
   out_5826999704905783895[25] = 1;
   out_5826999704905783895[26] = 0;
   out_5826999704905783895[27] = 0;
   out_5826999704905783895[28] = 1;
   out_5826999704905783895[29] = 0;
   out_5826999704905783895[30] = 0;
   out_5826999704905783895[31] = 0;
   out_5826999704905783895[32] = 0;
   out_5826999704905783895[33] = 0;
   out_5826999704905783895[34] = 0;
   out_5826999704905783895[35] = 0;
   out_5826999704905783895[36] = 0;
   out_5826999704905783895[37] = 0;
   out_5826999704905783895[38] = 0;
   out_5826999704905783895[39] = 0;
   out_5826999704905783895[40] = 0;
   out_5826999704905783895[41] = 0;
   out_5826999704905783895[42] = 0;
   out_5826999704905783895[43] = 0;
   out_5826999704905783895[44] = 1;
   out_5826999704905783895[45] = 0;
   out_5826999704905783895[46] = 0;
   out_5826999704905783895[47] = 1;
   out_5826999704905783895[48] = 0;
   out_5826999704905783895[49] = 0;
   out_5826999704905783895[50] = 0;
   out_5826999704905783895[51] = 0;
   out_5826999704905783895[52] = 0;
   out_5826999704905783895[53] = 0;
}
void h_10(double *state, double *unused, double *out_3970686424555567202) {
   out_3970686424555567202[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_3970686424555567202[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_3970686424555567202[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_4590171680468794279) {
   out_4590171680468794279[0] = 0;
   out_4590171680468794279[1] = 9.8100000000000005*cos(state[1]);
   out_4590171680468794279[2] = 0;
   out_4590171680468794279[3] = 0;
   out_4590171680468794279[4] = -state[8];
   out_4590171680468794279[5] = state[7];
   out_4590171680468794279[6] = 0;
   out_4590171680468794279[7] = state[5];
   out_4590171680468794279[8] = -state[4];
   out_4590171680468794279[9] = 0;
   out_4590171680468794279[10] = 0;
   out_4590171680468794279[11] = 0;
   out_4590171680468794279[12] = 1;
   out_4590171680468794279[13] = 0;
   out_4590171680468794279[14] = 0;
   out_4590171680468794279[15] = 1;
   out_4590171680468794279[16] = 0;
   out_4590171680468794279[17] = 0;
   out_4590171680468794279[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_4590171680468794279[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_4590171680468794279[20] = 0;
   out_4590171680468794279[21] = state[8];
   out_4590171680468794279[22] = 0;
   out_4590171680468794279[23] = -state[6];
   out_4590171680468794279[24] = -state[5];
   out_4590171680468794279[25] = 0;
   out_4590171680468794279[26] = state[3];
   out_4590171680468794279[27] = 0;
   out_4590171680468794279[28] = 0;
   out_4590171680468794279[29] = 0;
   out_4590171680468794279[30] = 0;
   out_4590171680468794279[31] = 1;
   out_4590171680468794279[32] = 0;
   out_4590171680468794279[33] = 0;
   out_4590171680468794279[34] = 1;
   out_4590171680468794279[35] = 0;
   out_4590171680468794279[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_4590171680468794279[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_4590171680468794279[38] = 0;
   out_4590171680468794279[39] = -state[7];
   out_4590171680468794279[40] = state[6];
   out_4590171680468794279[41] = 0;
   out_4590171680468794279[42] = state[4];
   out_4590171680468794279[43] = -state[3];
   out_4590171680468794279[44] = 0;
   out_4590171680468794279[45] = 0;
   out_4590171680468794279[46] = 0;
   out_4590171680468794279[47] = 0;
   out_4590171680468794279[48] = 0;
   out_4590171680468794279[49] = 0;
   out_4590171680468794279[50] = 1;
   out_4590171680468794279[51] = 0;
   out_4590171680468794279[52] = 0;
   out_4590171680468794279[53] = 1;
}
void h_13(double *state, double *unused, double *out_649122268436072591) {
   out_649122268436072591[0] = state[3];
   out_649122268436072591[1] = state[4];
   out_649122268436072591[2] = state[5];
}
void H_13(double *state, double *unused, double *out_2614725879573451094) {
   out_2614725879573451094[0] = 0;
   out_2614725879573451094[1] = 0;
   out_2614725879573451094[2] = 0;
   out_2614725879573451094[3] = 1;
   out_2614725879573451094[4] = 0;
   out_2614725879573451094[5] = 0;
   out_2614725879573451094[6] = 0;
   out_2614725879573451094[7] = 0;
   out_2614725879573451094[8] = 0;
   out_2614725879573451094[9] = 0;
   out_2614725879573451094[10] = 0;
   out_2614725879573451094[11] = 0;
   out_2614725879573451094[12] = 0;
   out_2614725879573451094[13] = 0;
   out_2614725879573451094[14] = 0;
   out_2614725879573451094[15] = 0;
   out_2614725879573451094[16] = 0;
   out_2614725879573451094[17] = 0;
   out_2614725879573451094[18] = 0;
   out_2614725879573451094[19] = 0;
   out_2614725879573451094[20] = 0;
   out_2614725879573451094[21] = 0;
   out_2614725879573451094[22] = 1;
   out_2614725879573451094[23] = 0;
   out_2614725879573451094[24] = 0;
   out_2614725879573451094[25] = 0;
   out_2614725879573451094[26] = 0;
   out_2614725879573451094[27] = 0;
   out_2614725879573451094[28] = 0;
   out_2614725879573451094[29] = 0;
   out_2614725879573451094[30] = 0;
   out_2614725879573451094[31] = 0;
   out_2614725879573451094[32] = 0;
   out_2614725879573451094[33] = 0;
   out_2614725879573451094[34] = 0;
   out_2614725879573451094[35] = 0;
   out_2614725879573451094[36] = 0;
   out_2614725879573451094[37] = 0;
   out_2614725879573451094[38] = 0;
   out_2614725879573451094[39] = 0;
   out_2614725879573451094[40] = 0;
   out_2614725879573451094[41] = 1;
   out_2614725879573451094[42] = 0;
   out_2614725879573451094[43] = 0;
   out_2614725879573451094[44] = 0;
   out_2614725879573451094[45] = 0;
   out_2614725879573451094[46] = 0;
   out_2614725879573451094[47] = 0;
   out_2614725879573451094[48] = 0;
   out_2614725879573451094[49] = 0;
   out_2614725879573451094[50] = 0;
   out_2614725879573451094[51] = 0;
   out_2614725879573451094[52] = 0;
   out_2614725879573451094[53] = 0;
}
void h_14(double *state, double *unused, double *out_113050053768907397) {
   out_113050053768907397[0] = state[6];
   out_113050053768907397[1] = state[7];
   out_113050053768907397[2] = state[8];
}
void H_14(double *state, double *unused, double *out_1863758848566299366) {
   out_1863758848566299366[0] = 0;
   out_1863758848566299366[1] = 0;
   out_1863758848566299366[2] = 0;
   out_1863758848566299366[3] = 0;
   out_1863758848566299366[4] = 0;
   out_1863758848566299366[5] = 0;
   out_1863758848566299366[6] = 1;
   out_1863758848566299366[7] = 0;
   out_1863758848566299366[8] = 0;
   out_1863758848566299366[9] = 0;
   out_1863758848566299366[10] = 0;
   out_1863758848566299366[11] = 0;
   out_1863758848566299366[12] = 0;
   out_1863758848566299366[13] = 0;
   out_1863758848566299366[14] = 0;
   out_1863758848566299366[15] = 0;
   out_1863758848566299366[16] = 0;
   out_1863758848566299366[17] = 0;
   out_1863758848566299366[18] = 0;
   out_1863758848566299366[19] = 0;
   out_1863758848566299366[20] = 0;
   out_1863758848566299366[21] = 0;
   out_1863758848566299366[22] = 0;
   out_1863758848566299366[23] = 0;
   out_1863758848566299366[24] = 0;
   out_1863758848566299366[25] = 1;
   out_1863758848566299366[26] = 0;
   out_1863758848566299366[27] = 0;
   out_1863758848566299366[28] = 0;
   out_1863758848566299366[29] = 0;
   out_1863758848566299366[30] = 0;
   out_1863758848566299366[31] = 0;
   out_1863758848566299366[32] = 0;
   out_1863758848566299366[33] = 0;
   out_1863758848566299366[34] = 0;
   out_1863758848566299366[35] = 0;
   out_1863758848566299366[36] = 0;
   out_1863758848566299366[37] = 0;
   out_1863758848566299366[38] = 0;
   out_1863758848566299366[39] = 0;
   out_1863758848566299366[40] = 0;
   out_1863758848566299366[41] = 0;
   out_1863758848566299366[42] = 0;
   out_1863758848566299366[43] = 0;
   out_1863758848566299366[44] = 1;
   out_1863758848566299366[45] = 0;
   out_1863758848566299366[46] = 0;
   out_1863758848566299366[47] = 0;
   out_1863758848566299366[48] = 0;
   out_1863758848566299366[49] = 0;
   out_1863758848566299366[50] = 0;
   out_1863758848566299366[51] = 0;
   out_1863758848566299366[52] = 0;
   out_1863758848566299366[53] = 0;
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
void pose_err_fun(double *nom_x, double *delta_x, double *out_2510165527669125945) {
  err_fun(nom_x, delta_x, out_2510165527669125945);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_2827775544900411455) {
  inv_err_fun(nom_x, true_x, out_2827775544900411455);
}
void pose_H_mod_fun(double *state, double *out_6581160706960496502) {
  H_mod_fun(state, out_6581160706960496502);
}
void pose_f_fun(double *state, double dt, double *out_1715652979675739070) {
  f_fun(state,  dt, out_1715652979675739070);
}
void pose_F_fun(double *state, double dt, double *out_7854120589136729219) {
  F_fun(state,  dt, out_7854120589136729219);
}
void pose_h_4(double *state, double *unused, double *out_3221272873403501279) {
  h_4(state, unused, out_3221272873403501279);
}
void pose_H_4(double *state, double *unused, double *out_5826999704905783895) {
  H_4(state, unused, out_5826999704905783895);
}
void pose_h_10(double *state, double *unused, double *out_3970686424555567202) {
  h_10(state, unused, out_3970686424555567202);
}
void pose_H_10(double *state, double *unused, double *out_4590171680468794279) {
  H_10(state, unused, out_4590171680468794279);
}
void pose_h_13(double *state, double *unused, double *out_649122268436072591) {
  h_13(state, unused, out_649122268436072591);
}
void pose_H_13(double *state, double *unused, double *out_2614725879573451094) {
  H_13(state, unused, out_2614725879573451094);
}
void pose_h_14(double *state, double *unused, double *out_113050053768907397) {
  h_14(state, unused, out_113050053768907397);
}
void pose_H_14(double *state, double *unused, double *out_1863758848566299366) {
  H_14(state, unused, out_1863758848566299366);
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
