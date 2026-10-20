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
void err_fun(double *nom_x, double *delta_x, double *out_1111971644641323647) {
   out_1111971644641323647[0] = delta_x[0] + nom_x[0];
   out_1111971644641323647[1] = delta_x[1] + nom_x[1];
   out_1111971644641323647[2] = delta_x[2] + nom_x[2];
   out_1111971644641323647[3] = delta_x[3] + nom_x[3];
   out_1111971644641323647[4] = delta_x[4] + nom_x[4];
   out_1111971644641323647[5] = delta_x[5] + nom_x[5];
   out_1111971644641323647[6] = delta_x[6] + nom_x[6];
   out_1111971644641323647[7] = delta_x[7] + nom_x[7];
   out_1111971644641323647[8] = delta_x[8] + nom_x[8];
   out_1111971644641323647[9] = delta_x[9] + nom_x[9];
   out_1111971644641323647[10] = delta_x[10] + nom_x[10];
   out_1111971644641323647[11] = delta_x[11] + nom_x[11];
   out_1111971644641323647[12] = delta_x[12] + nom_x[12];
   out_1111971644641323647[13] = delta_x[13] + nom_x[13];
   out_1111971644641323647[14] = delta_x[14] + nom_x[14];
   out_1111971644641323647[15] = delta_x[15] + nom_x[15];
   out_1111971644641323647[16] = delta_x[16] + nom_x[16];
   out_1111971644641323647[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_2323187048211860073) {
   out_2323187048211860073[0] = -nom_x[0] + true_x[0];
   out_2323187048211860073[1] = -nom_x[1] + true_x[1];
   out_2323187048211860073[2] = -nom_x[2] + true_x[2];
   out_2323187048211860073[3] = -nom_x[3] + true_x[3];
   out_2323187048211860073[4] = -nom_x[4] + true_x[4];
   out_2323187048211860073[5] = -nom_x[5] + true_x[5];
   out_2323187048211860073[6] = -nom_x[6] + true_x[6];
   out_2323187048211860073[7] = -nom_x[7] + true_x[7];
   out_2323187048211860073[8] = -nom_x[8] + true_x[8];
   out_2323187048211860073[9] = -nom_x[9] + true_x[9];
   out_2323187048211860073[10] = -nom_x[10] + true_x[10];
   out_2323187048211860073[11] = -nom_x[11] + true_x[11];
   out_2323187048211860073[12] = -nom_x[12] + true_x[12];
   out_2323187048211860073[13] = -nom_x[13] + true_x[13];
   out_2323187048211860073[14] = -nom_x[14] + true_x[14];
   out_2323187048211860073[15] = -nom_x[15] + true_x[15];
   out_2323187048211860073[16] = -nom_x[16] + true_x[16];
   out_2323187048211860073[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_5105792754762297630) {
   out_5105792754762297630[0] = 1.0;
   out_5105792754762297630[1] = 0.0;
   out_5105792754762297630[2] = 0.0;
   out_5105792754762297630[3] = 0.0;
   out_5105792754762297630[4] = 0.0;
   out_5105792754762297630[5] = 0.0;
   out_5105792754762297630[6] = 0.0;
   out_5105792754762297630[7] = 0.0;
   out_5105792754762297630[8] = 0.0;
   out_5105792754762297630[9] = 0.0;
   out_5105792754762297630[10] = 0.0;
   out_5105792754762297630[11] = 0.0;
   out_5105792754762297630[12] = 0.0;
   out_5105792754762297630[13] = 0.0;
   out_5105792754762297630[14] = 0.0;
   out_5105792754762297630[15] = 0.0;
   out_5105792754762297630[16] = 0.0;
   out_5105792754762297630[17] = 0.0;
   out_5105792754762297630[18] = 0.0;
   out_5105792754762297630[19] = 1.0;
   out_5105792754762297630[20] = 0.0;
   out_5105792754762297630[21] = 0.0;
   out_5105792754762297630[22] = 0.0;
   out_5105792754762297630[23] = 0.0;
   out_5105792754762297630[24] = 0.0;
   out_5105792754762297630[25] = 0.0;
   out_5105792754762297630[26] = 0.0;
   out_5105792754762297630[27] = 0.0;
   out_5105792754762297630[28] = 0.0;
   out_5105792754762297630[29] = 0.0;
   out_5105792754762297630[30] = 0.0;
   out_5105792754762297630[31] = 0.0;
   out_5105792754762297630[32] = 0.0;
   out_5105792754762297630[33] = 0.0;
   out_5105792754762297630[34] = 0.0;
   out_5105792754762297630[35] = 0.0;
   out_5105792754762297630[36] = 0.0;
   out_5105792754762297630[37] = 0.0;
   out_5105792754762297630[38] = 1.0;
   out_5105792754762297630[39] = 0.0;
   out_5105792754762297630[40] = 0.0;
   out_5105792754762297630[41] = 0.0;
   out_5105792754762297630[42] = 0.0;
   out_5105792754762297630[43] = 0.0;
   out_5105792754762297630[44] = 0.0;
   out_5105792754762297630[45] = 0.0;
   out_5105792754762297630[46] = 0.0;
   out_5105792754762297630[47] = 0.0;
   out_5105792754762297630[48] = 0.0;
   out_5105792754762297630[49] = 0.0;
   out_5105792754762297630[50] = 0.0;
   out_5105792754762297630[51] = 0.0;
   out_5105792754762297630[52] = 0.0;
   out_5105792754762297630[53] = 0.0;
   out_5105792754762297630[54] = 0.0;
   out_5105792754762297630[55] = 0.0;
   out_5105792754762297630[56] = 0.0;
   out_5105792754762297630[57] = 1.0;
   out_5105792754762297630[58] = 0.0;
   out_5105792754762297630[59] = 0.0;
   out_5105792754762297630[60] = 0.0;
   out_5105792754762297630[61] = 0.0;
   out_5105792754762297630[62] = 0.0;
   out_5105792754762297630[63] = 0.0;
   out_5105792754762297630[64] = 0.0;
   out_5105792754762297630[65] = 0.0;
   out_5105792754762297630[66] = 0.0;
   out_5105792754762297630[67] = 0.0;
   out_5105792754762297630[68] = 0.0;
   out_5105792754762297630[69] = 0.0;
   out_5105792754762297630[70] = 0.0;
   out_5105792754762297630[71] = 0.0;
   out_5105792754762297630[72] = 0.0;
   out_5105792754762297630[73] = 0.0;
   out_5105792754762297630[74] = 0.0;
   out_5105792754762297630[75] = 0.0;
   out_5105792754762297630[76] = 1.0;
   out_5105792754762297630[77] = 0.0;
   out_5105792754762297630[78] = 0.0;
   out_5105792754762297630[79] = 0.0;
   out_5105792754762297630[80] = 0.0;
   out_5105792754762297630[81] = 0.0;
   out_5105792754762297630[82] = 0.0;
   out_5105792754762297630[83] = 0.0;
   out_5105792754762297630[84] = 0.0;
   out_5105792754762297630[85] = 0.0;
   out_5105792754762297630[86] = 0.0;
   out_5105792754762297630[87] = 0.0;
   out_5105792754762297630[88] = 0.0;
   out_5105792754762297630[89] = 0.0;
   out_5105792754762297630[90] = 0.0;
   out_5105792754762297630[91] = 0.0;
   out_5105792754762297630[92] = 0.0;
   out_5105792754762297630[93] = 0.0;
   out_5105792754762297630[94] = 0.0;
   out_5105792754762297630[95] = 1.0;
   out_5105792754762297630[96] = 0.0;
   out_5105792754762297630[97] = 0.0;
   out_5105792754762297630[98] = 0.0;
   out_5105792754762297630[99] = 0.0;
   out_5105792754762297630[100] = 0.0;
   out_5105792754762297630[101] = 0.0;
   out_5105792754762297630[102] = 0.0;
   out_5105792754762297630[103] = 0.0;
   out_5105792754762297630[104] = 0.0;
   out_5105792754762297630[105] = 0.0;
   out_5105792754762297630[106] = 0.0;
   out_5105792754762297630[107] = 0.0;
   out_5105792754762297630[108] = 0.0;
   out_5105792754762297630[109] = 0.0;
   out_5105792754762297630[110] = 0.0;
   out_5105792754762297630[111] = 0.0;
   out_5105792754762297630[112] = 0.0;
   out_5105792754762297630[113] = 0.0;
   out_5105792754762297630[114] = 1.0;
   out_5105792754762297630[115] = 0.0;
   out_5105792754762297630[116] = 0.0;
   out_5105792754762297630[117] = 0.0;
   out_5105792754762297630[118] = 0.0;
   out_5105792754762297630[119] = 0.0;
   out_5105792754762297630[120] = 0.0;
   out_5105792754762297630[121] = 0.0;
   out_5105792754762297630[122] = 0.0;
   out_5105792754762297630[123] = 0.0;
   out_5105792754762297630[124] = 0.0;
   out_5105792754762297630[125] = 0.0;
   out_5105792754762297630[126] = 0.0;
   out_5105792754762297630[127] = 0.0;
   out_5105792754762297630[128] = 0.0;
   out_5105792754762297630[129] = 0.0;
   out_5105792754762297630[130] = 0.0;
   out_5105792754762297630[131] = 0.0;
   out_5105792754762297630[132] = 0.0;
   out_5105792754762297630[133] = 1.0;
   out_5105792754762297630[134] = 0.0;
   out_5105792754762297630[135] = 0.0;
   out_5105792754762297630[136] = 0.0;
   out_5105792754762297630[137] = 0.0;
   out_5105792754762297630[138] = 0.0;
   out_5105792754762297630[139] = 0.0;
   out_5105792754762297630[140] = 0.0;
   out_5105792754762297630[141] = 0.0;
   out_5105792754762297630[142] = 0.0;
   out_5105792754762297630[143] = 0.0;
   out_5105792754762297630[144] = 0.0;
   out_5105792754762297630[145] = 0.0;
   out_5105792754762297630[146] = 0.0;
   out_5105792754762297630[147] = 0.0;
   out_5105792754762297630[148] = 0.0;
   out_5105792754762297630[149] = 0.0;
   out_5105792754762297630[150] = 0.0;
   out_5105792754762297630[151] = 0.0;
   out_5105792754762297630[152] = 1.0;
   out_5105792754762297630[153] = 0.0;
   out_5105792754762297630[154] = 0.0;
   out_5105792754762297630[155] = 0.0;
   out_5105792754762297630[156] = 0.0;
   out_5105792754762297630[157] = 0.0;
   out_5105792754762297630[158] = 0.0;
   out_5105792754762297630[159] = 0.0;
   out_5105792754762297630[160] = 0.0;
   out_5105792754762297630[161] = 0.0;
   out_5105792754762297630[162] = 0.0;
   out_5105792754762297630[163] = 0.0;
   out_5105792754762297630[164] = 0.0;
   out_5105792754762297630[165] = 0.0;
   out_5105792754762297630[166] = 0.0;
   out_5105792754762297630[167] = 0.0;
   out_5105792754762297630[168] = 0.0;
   out_5105792754762297630[169] = 0.0;
   out_5105792754762297630[170] = 0.0;
   out_5105792754762297630[171] = 1.0;
   out_5105792754762297630[172] = 0.0;
   out_5105792754762297630[173] = 0.0;
   out_5105792754762297630[174] = 0.0;
   out_5105792754762297630[175] = 0.0;
   out_5105792754762297630[176] = 0.0;
   out_5105792754762297630[177] = 0.0;
   out_5105792754762297630[178] = 0.0;
   out_5105792754762297630[179] = 0.0;
   out_5105792754762297630[180] = 0.0;
   out_5105792754762297630[181] = 0.0;
   out_5105792754762297630[182] = 0.0;
   out_5105792754762297630[183] = 0.0;
   out_5105792754762297630[184] = 0.0;
   out_5105792754762297630[185] = 0.0;
   out_5105792754762297630[186] = 0.0;
   out_5105792754762297630[187] = 0.0;
   out_5105792754762297630[188] = 0.0;
   out_5105792754762297630[189] = 0.0;
   out_5105792754762297630[190] = 1.0;
   out_5105792754762297630[191] = 0.0;
   out_5105792754762297630[192] = 0.0;
   out_5105792754762297630[193] = 0.0;
   out_5105792754762297630[194] = 0.0;
   out_5105792754762297630[195] = 0.0;
   out_5105792754762297630[196] = 0.0;
   out_5105792754762297630[197] = 0.0;
   out_5105792754762297630[198] = 0.0;
   out_5105792754762297630[199] = 0.0;
   out_5105792754762297630[200] = 0.0;
   out_5105792754762297630[201] = 0.0;
   out_5105792754762297630[202] = 0.0;
   out_5105792754762297630[203] = 0.0;
   out_5105792754762297630[204] = 0.0;
   out_5105792754762297630[205] = 0.0;
   out_5105792754762297630[206] = 0.0;
   out_5105792754762297630[207] = 0.0;
   out_5105792754762297630[208] = 0.0;
   out_5105792754762297630[209] = 1.0;
   out_5105792754762297630[210] = 0.0;
   out_5105792754762297630[211] = 0.0;
   out_5105792754762297630[212] = 0.0;
   out_5105792754762297630[213] = 0.0;
   out_5105792754762297630[214] = 0.0;
   out_5105792754762297630[215] = 0.0;
   out_5105792754762297630[216] = 0.0;
   out_5105792754762297630[217] = 0.0;
   out_5105792754762297630[218] = 0.0;
   out_5105792754762297630[219] = 0.0;
   out_5105792754762297630[220] = 0.0;
   out_5105792754762297630[221] = 0.0;
   out_5105792754762297630[222] = 0.0;
   out_5105792754762297630[223] = 0.0;
   out_5105792754762297630[224] = 0.0;
   out_5105792754762297630[225] = 0.0;
   out_5105792754762297630[226] = 0.0;
   out_5105792754762297630[227] = 0.0;
   out_5105792754762297630[228] = 1.0;
   out_5105792754762297630[229] = 0.0;
   out_5105792754762297630[230] = 0.0;
   out_5105792754762297630[231] = 0.0;
   out_5105792754762297630[232] = 0.0;
   out_5105792754762297630[233] = 0.0;
   out_5105792754762297630[234] = 0.0;
   out_5105792754762297630[235] = 0.0;
   out_5105792754762297630[236] = 0.0;
   out_5105792754762297630[237] = 0.0;
   out_5105792754762297630[238] = 0.0;
   out_5105792754762297630[239] = 0.0;
   out_5105792754762297630[240] = 0.0;
   out_5105792754762297630[241] = 0.0;
   out_5105792754762297630[242] = 0.0;
   out_5105792754762297630[243] = 0.0;
   out_5105792754762297630[244] = 0.0;
   out_5105792754762297630[245] = 0.0;
   out_5105792754762297630[246] = 0.0;
   out_5105792754762297630[247] = 1.0;
   out_5105792754762297630[248] = 0.0;
   out_5105792754762297630[249] = 0.0;
   out_5105792754762297630[250] = 0.0;
   out_5105792754762297630[251] = 0.0;
   out_5105792754762297630[252] = 0.0;
   out_5105792754762297630[253] = 0.0;
   out_5105792754762297630[254] = 0.0;
   out_5105792754762297630[255] = 0.0;
   out_5105792754762297630[256] = 0.0;
   out_5105792754762297630[257] = 0.0;
   out_5105792754762297630[258] = 0.0;
   out_5105792754762297630[259] = 0.0;
   out_5105792754762297630[260] = 0.0;
   out_5105792754762297630[261] = 0.0;
   out_5105792754762297630[262] = 0.0;
   out_5105792754762297630[263] = 0.0;
   out_5105792754762297630[264] = 0.0;
   out_5105792754762297630[265] = 0.0;
   out_5105792754762297630[266] = 1.0;
   out_5105792754762297630[267] = 0.0;
   out_5105792754762297630[268] = 0.0;
   out_5105792754762297630[269] = 0.0;
   out_5105792754762297630[270] = 0.0;
   out_5105792754762297630[271] = 0.0;
   out_5105792754762297630[272] = 0.0;
   out_5105792754762297630[273] = 0.0;
   out_5105792754762297630[274] = 0.0;
   out_5105792754762297630[275] = 0.0;
   out_5105792754762297630[276] = 0.0;
   out_5105792754762297630[277] = 0.0;
   out_5105792754762297630[278] = 0.0;
   out_5105792754762297630[279] = 0.0;
   out_5105792754762297630[280] = 0.0;
   out_5105792754762297630[281] = 0.0;
   out_5105792754762297630[282] = 0.0;
   out_5105792754762297630[283] = 0.0;
   out_5105792754762297630[284] = 0.0;
   out_5105792754762297630[285] = 1.0;
   out_5105792754762297630[286] = 0.0;
   out_5105792754762297630[287] = 0.0;
   out_5105792754762297630[288] = 0.0;
   out_5105792754762297630[289] = 0.0;
   out_5105792754762297630[290] = 0.0;
   out_5105792754762297630[291] = 0.0;
   out_5105792754762297630[292] = 0.0;
   out_5105792754762297630[293] = 0.0;
   out_5105792754762297630[294] = 0.0;
   out_5105792754762297630[295] = 0.0;
   out_5105792754762297630[296] = 0.0;
   out_5105792754762297630[297] = 0.0;
   out_5105792754762297630[298] = 0.0;
   out_5105792754762297630[299] = 0.0;
   out_5105792754762297630[300] = 0.0;
   out_5105792754762297630[301] = 0.0;
   out_5105792754762297630[302] = 0.0;
   out_5105792754762297630[303] = 0.0;
   out_5105792754762297630[304] = 1.0;
   out_5105792754762297630[305] = 0.0;
   out_5105792754762297630[306] = 0.0;
   out_5105792754762297630[307] = 0.0;
   out_5105792754762297630[308] = 0.0;
   out_5105792754762297630[309] = 0.0;
   out_5105792754762297630[310] = 0.0;
   out_5105792754762297630[311] = 0.0;
   out_5105792754762297630[312] = 0.0;
   out_5105792754762297630[313] = 0.0;
   out_5105792754762297630[314] = 0.0;
   out_5105792754762297630[315] = 0.0;
   out_5105792754762297630[316] = 0.0;
   out_5105792754762297630[317] = 0.0;
   out_5105792754762297630[318] = 0.0;
   out_5105792754762297630[319] = 0.0;
   out_5105792754762297630[320] = 0.0;
   out_5105792754762297630[321] = 0.0;
   out_5105792754762297630[322] = 0.0;
   out_5105792754762297630[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_493911584477072580) {
   out_493911584477072580[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_493911584477072580[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_493911584477072580[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_493911584477072580[3] = dt*state[12] + state[3];
   out_493911584477072580[4] = dt*state[13] + state[4];
   out_493911584477072580[5] = dt*state[14] + state[5];
   out_493911584477072580[6] = state[6];
   out_493911584477072580[7] = state[7];
   out_493911584477072580[8] = state[8];
   out_493911584477072580[9] = state[9];
   out_493911584477072580[10] = state[10];
   out_493911584477072580[11] = state[11];
   out_493911584477072580[12] = state[12];
   out_493911584477072580[13] = state[13];
   out_493911584477072580[14] = state[14];
   out_493911584477072580[15] = state[15];
   out_493911584477072580[16] = state[16];
   out_493911584477072580[17] = state[17];
}
void F_fun(double *state, double dt, double *out_5423918991428774343) {
   out_5423918991428774343[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_5423918991428774343[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_5423918991428774343[2] = 0;
   out_5423918991428774343[3] = 0;
   out_5423918991428774343[4] = 0;
   out_5423918991428774343[5] = 0;
   out_5423918991428774343[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_5423918991428774343[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_5423918991428774343[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_5423918991428774343[9] = 0;
   out_5423918991428774343[10] = 0;
   out_5423918991428774343[11] = 0;
   out_5423918991428774343[12] = 0;
   out_5423918991428774343[13] = 0;
   out_5423918991428774343[14] = 0;
   out_5423918991428774343[15] = 0;
   out_5423918991428774343[16] = 0;
   out_5423918991428774343[17] = 0;
   out_5423918991428774343[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_5423918991428774343[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_5423918991428774343[20] = 0;
   out_5423918991428774343[21] = 0;
   out_5423918991428774343[22] = 0;
   out_5423918991428774343[23] = 0;
   out_5423918991428774343[24] = 0;
   out_5423918991428774343[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_5423918991428774343[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_5423918991428774343[27] = 0;
   out_5423918991428774343[28] = 0;
   out_5423918991428774343[29] = 0;
   out_5423918991428774343[30] = 0;
   out_5423918991428774343[31] = 0;
   out_5423918991428774343[32] = 0;
   out_5423918991428774343[33] = 0;
   out_5423918991428774343[34] = 0;
   out_5423918991428774343[35] = 0;
   out_5423918991428774343[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_5423918991428774343[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_5423918991428774343[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_5423918991428774343[39] = 0;
   out_5423918991428774343[40] = 0;
   out_5423918991428774343[41] = 0;
   out_5423918991428774343[42] = 0;
   out_5423918991428774343[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_5423918991428774343[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_5423918991428774343[45] = 0;
   out_5423918991428774343[46] = 0;
   out_5423918991428774343[47] = 0;
   out_5423918991428774343[48] = 0;
   out_5423918991428774343[49] = 0;
   out_5423918991428774343[50] = 0;
   out_5423918991428774343[51] = 0;
   out_5423918991428774343[52] = 0;
   out_5423918991428774343[53] = 0;
   out_5423918991428774343[54] = 0;
   out_5423918991428774343[55] = 0;
   out_5423918991428774343[56] = 0;
   out_5423918991428774343[57] = 1;
   out_5423918991428774343[58] = 0;
   out_5423918991428774343[59] = 0;
   out_5423918991428774343[60] = 0;
   out_5423918991428774343[61] = 0;
   out_5423918991428774343[62] = 0;
   out_5423918991428774343[63] = 0;
   out_5423918991428774343[64] = 0;
   out_5423918991428774343[65] = 0;
   out_5423918991428774343[66] = dt;
   out_5423918991428774343[67] = 0;
   out_5423918991428774343[68] = 0;
   out_5423918991428774343[69] = 0;
   out_5423918991428774343[70] = 0;
   out_5423918991428774343[71] = 0;
   out_5423918991428774343[72] = 0;
   out_5423918991428774343[73] = 0;
   out_5423918991428774343[74] = 0;
   out_5423918991428774343[75] = 0;
   out_5423918991428774343[76] = 1;
   out_5423918991428774343[77] = 0;
   out_5423918991428774343[78] = 0;
   out_5423918991428774343[79] = 0;
   out_5423918991428774343[80] = 0;
   out_5423918991428774343[81] = 0;
   out_5423918991428774343[82] = 0;
   out_5423918991428774343[83] = 0;
   out_5423918991428774343[84] = 0;
   out_5423918991428774343[85] = dt;
   out_5423918991428774343[86] = 0;
   out_5423918991428774343[87] = 0;
   out_5423918991428774343[88] = 0;
   out_5423918991428774343[89] = 0;
   out_5423918991428774343[90] = 0;
   out_5423918991428774343[91] = 0;
   out_5423918991428774343[92] = 0;
   out_5423918991428774343[93] = 0;
   out_5423918991428774343[94] = 0;
   out_5423918991428774343[95] = 1;
   out_5423918991428774343[96] = 0;
   out_5423918991428774343[97] = 0;
   out_5423918991428774343[98] = 0;
   out_5423918991428774343[99] = 0;
   out_5423918991428774343[100] = 0;
   out_5423918991428774343[101] = 0;
   out_5423918991428774343[102] = 0;
   out_5423918991428774343[103] = 0;
   out_5423918991428774343[104] = dt;
   out_5423918991428774343[105] = 0;
   out_5423918991428774343[106] = 0;
   out_5423918991428774343[107] = 0;
   out_5423918991428774343[108] = 0;
   out_5423918991428774343[109] = 0;
   out_5423918991428774343[110] = 0;
   out_5423918991428774343[111] = 0;
   out_5423918991428774343[112] = 0;
   out_5423918991428774343[113] = 0;
   out_5423918991428774343[114] = 1;
   out_5423918991428774343[115] = 0;
   out_5423918991428774343[116] = 0;
   out_5423918991428774343[117] = 0;
   out_5423918991428774343[118] = 0;
   out_5423918991428774343[119] = 0;
   out_5423918991428774343[120] = 0;
   out_5423918991428774343[121] = 0;
   out_5423918991428774343[122] = 0;
   out_5423918991428774343[123] = 0;
   out_5423918991428774343[124] = 0;
   out_5423918991428774343[125] = 0;
   out_5423918991428774343[126] = 0;
   out_5423918991428774343[127] = 0;
   out_5423918991428774343[128] = 0;
   out_5423918991428774343[129] = 0;
   out_5423918991428774343[130] = 0;
   out_5423918991428774343[131] = 0;
   out_5423918991428774343[132] = 0;
   out_5423918991428774343[133] = 1;
   out_5423918991428774343[134] = 0;
   out_5423918991428774343[135] = 0;
   out_5423918991428774343[136] = 0;
   out_5423918991428774343[137] = 0;
   out_5423918991428774343[138] = 0;
   out_5423918991428774343[139] = 0;
   out_5423918991428774343[140] = 0;
   out_5423918991428774343[141] = 0;
   out_5423918991428774343[142] = 0;
   out_5423918991428774343[143] = 0;
   out_5423918991428774343[144] = 0;
   out_5423918991428774343[145] = 0;
   out_5423918991428774343[146] = 0;
   out_5423918991428774343[147] = 0;
   out_5423918991428774343[148] = 0;
   out_5423918991428774343[149] = 0;
   out_5423918991428774343[150] = 0;
   out_5423918991428774343[151] = 0;
   out_5423918991428774343[152] = 1;
   out_5423918991428774343[153] = 0;
   out_5423918991428774343[154] = 0;
   out_5423918991428774343[155] = 0;
   out_5423918991428774343[156] = 0;
   out_5423918991428774343[157] = 0;
   out_5423918991428774343[158] = 0;
   out_5423918991428774343[159] = 0;
   out_5423918991428774343[160] = 0;
   out_5423918991428774343[161] = 0;
   out_5423918991428774343[162] = 0;
   out_5423918991428774343[163] = 0;
   out_5423918991428774343[164] = 0;
   out_5423918991428774343[165] = 0;
   out_5423918991428774343[166] = 0;
   out_5423918991428774343[167] = 0;
   out_5423918991428774343[168] = 0;
   out_5423918991428774343[169] = 0;
   out_5423918991428774343[170] = 0;
   out_5423918991428774343[171] = 1;
   out_5423918991428774343[172] = 0;
   out_5423918991428774343[173] = 0;
   out_5423918991428774343[174] = 0;
   out_5423918991428774343[175] = 0;
   out_5423918991428774343[176] = 0;
   out_5423918991428774343[177] = 0;
   out_5423918991428774343[178] = 0;
   out_5423918991428774343[179] = 0;
   out_5423918991428774343[180] = 0;
   out_5423918991428774343[181] = 0;
   out_5423918991428774343[182] = 0;
   out_5423918991428774343[183] = 0;
   out_5423918991428774343[184] = 0;
   out_5423918991428774343[185] = 0;
   out_5423918991428774343[186] = 0;
   out_5423918991428774343[187] = 0;
   out_5423918991428774343[188] = 0;
   out_5423918991428774343[189] = 0;
   out_5423918991428774343[190] = 1;
   out_5423918991428774343[191] = 0;
   out_5423918991428774343[192] = 0;
   out_5423918991428774343[193] = 0;
   out_5423918991428774343[194] = 0;
   out_5423918991428774343[195] = 0;
   out_5423918991428774343[196] = 0;
   out_5423918991428774343[197] = 0;
   out_5423918991428774343[198] = 0;
   out_5423918991428774343[199] = 0;
   out_5423918991428774343[200] = 0;
   out_5423918991428774343[201] = 0;
   out_5423918991428774343[202] = 0;
   out_5423918991428774343[203] = 0;
   out_5423918991428774343[204] = 0;
   out_5423918991428774343[205] = 0;
   out_5423918991428774343[206] = 0;
   out_5423918991428774343[207] = 0;
   out_5423918991428774343[208] = 0;
   out_5423918991428774343[209] = 1;
   out_5423918991428774343[210] = 0;
   out_5423918991428774343[211] = 0;
   out_5423918991428774343[212] = 0;
   out_5423918991428774343[213] = 0;
   out_5423918991428774343[214] = 0;
   out_5423918991428774343[215] = 0;
   out_5423918991428774343[216] = 0;
   out_5423918991428774343[217] = 0;
   out_5423918991428774343[218] = 0;
   out_5423918991428774343[219] = 0;
   out_5423918991428774343[220] = 0;
   out_5423918991428774343[221] = 0;
   out_5423918991428774343[222] = 0;
   out_5423918991428774343[223] = 0;
   out_5423918991428774343[224] = 0;
   out_5423918991428774343[225] = 0;
   out_5423918991428774343[226] = 0;
   out_5423918991428774343[227] = 0;
   out_5423918991428774343[228] = 1;
   out_5423918991428774343[229] = 0;
   out_5423918991428774343[230] = 0;
   out_5423918991428774343[231] = 0;
   out_5423918991428774343[232] = 0;
   out_5423918991428774343[233] = 0;
   out_5423918991428774343[234] = 0;
   out_5423918991428774343[235] = 0;
   out_5423918991428774343[236] = 0;
   out_5423918991428774343[237] = 0;
   out_5423918991428774343[238] = 0;
   out_5423918991428774343[239] = 0;
   out_5423918991428774343[240] = 0;
   out_5423918991428774343[241] = 0;
   out_5423918991428774343[242] = 0;
   out_5423918991428774343[243] = 0;
   out_5423918991428774343[244] = 0;
   out_5423918991428774343[245] = 0;
   out_5423918991428774343[246] = 0;
   out_5423918991428774343[247] = 1;
   out_5423918991428774343[248] = 0;
   out_5423918991428774343[249] = 0;
   out_5423918991428774343[250] = 0;
   out_5423918991428774343[251] = 0;
   out_5423918991428774343[252] = 0;
   out_5423918991428774343[253] = 0;
   out_5423918991428774343[254] = 0;
   out_5423918991428774343[255] = 0;
   out_5423918991428774343[256] = 0;
   out_5423918991428774343[257] = 0;
   out_5423918991428774343[258] = 0;
   out_5423918991428774343[259] = 0;
   out_5423918991428774343[260] = 0;
   out_5423918991428774343[261] = 0;
   out_5423918991428774343[262] = 0;
   out_5423918991428774343[263] = 0;
   out_5423918991428774343[264] = 0;
   out_5423918991428774343[265] = 0;
   out_5423918991428774343[266] = 1;
   out_5423918991428774343[267] = 0;
   out_5423918991428774343[268] = 0;
   out_5423918991428774343[269] = 0;
   out_5423918991428774343[270] = 0;
   out_5423918991428774343[271] = 0;
   out_5423918991428774343[272] = 0;
   out_5423918991428774343[273] = 0;
   out_5423918991428774343[274] = 0;
   out_5423918991428774343[275] = 0;
   out_5423918991428774343[276] = 0;
   out_5423918991428774343[277] = 0;
   out_5423918991428774343[278] = 0;
   out_5423918991428774343[279] = 0;
   out_5423918991428774343[280] = 0;
   out_5423918991428774343[281] = 0;
   out_5423918991428774343[282] = 0;
   out_5423918991428774343[283] = 0;
   out_5423918991428774343[284] = 0;
   out_5423918991428774343[285] = 1;
   out_5423918991428774343[286] = 0;
   out_5423918991428774343[287] = 0;
   out_5423918991428774343[288] = 0;
   out_5423918991428774343[289] = 0;
   out_5423918991428774343[290] = 0;
   out_5423918991428774343[291] = 0;
   out_5423918991428774343[292] = 0;
   out_5423918991428774343[293] = 0;
   out_5423918991428774343[294] = 0;
   out_5423918991428774343[295] = 0;
   out_5423918991428774343[296] = 0;
   out_5423918991428774343[297] = 0;
   out_5423918991428774343[298] = 0;
   out_5423918991428774343[299] = 0;
   out_5423918991428774343[300] = 0;
   out_5423918991428774343[301] = 0;
   out_5423918991428774343[302] = 0;
   out_5423918991428774343[303] = 0;
   out_5423918991428774343[304] = 1;
   out_5423918991428774343[305] = 0;
   out_5423918991428774343[306] = 0;
   out_5423918991428774343[307] = 0;
   out_5423918991428774343[308] = 0;
   out_5423918991428774343[309] = 0;
   out_5423918991428774343[310] = 0;
   out_5423918991428774343[311] = 0;
   out_5423918991428774343[312] = 0;
   out_5423918991428774343[313] = 0;
   out_5423918991428774343[314] = 0;
   out_5423918991428774343[315] = 0;
   out_5423918991428774343[316] = 0;
   out_5423918991428774343[317] = 0;
   out_5423918991428774343[318] = 0;
   out_5423918991428774343[319] = 0;
   out_5423918991428774343[320] = 0;
   out_5423918991428774343[321] = 0;
   out_5423918991428774343[322] = 0;
   out_5423918991428774343[323] = 1;
}
void h_4(double *state, double *unused, double *out_5685577583451539137) {
   out_5685577583451539137[0] = state[6] + state[9];
   out_5685577583451539137[1] = state[7] + state[10];
   out_5685577583451539137[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_3094512129210927137) {
   out_3094512129210927137[0] = 0;
   out_3094512129210927137[1] = 0;
   out_3094512129210927137[2] = 0;
   out_3094512129210927137[3] = 0;
   out_3094512129210927137[4] = 0;
   out_3094512129210927137[5] = 0;
   out_3094512129210927137[6] = 1;
   out_3094512129210927137[7] = 0;
   out_3094512129210927137[8] = 0;
   out_3094512129210927137[9] = 1;
   out_3094512129210927137[10] = 0;
   out_3094512129210927137[11] = 0;
   out_3094512129210927137[12] = 0;
   out_3094512129210927137[13] = 0;
   out_3094512129210927137[14] = 0;
   out_3094512129210927137[15] = 0;
   out_3094512129210927137[16] = 0;
   out_3094512129210927137[17] = 0;
   out_3094512129210927137[18] = 0;
   out_3094512129210927137[19] = 0;
   out_3094512129210927137[20] = 0;
   out_3094512129210927137[21] = 0;
   out_3094512129210927137[22] = 0;
   out_3094512129210927137[23] = 0;
   out_3094512129210927137[24] = 0;
   out_3094512129210927137[25] = 1;
   out_3094512129210927137[26] = 0;
   out_3094512129210927137[27] = 0;
   out_3094512129210927137[28] = 1;
   out_3094512129210927137[29] = 0;
   out_3094512129210927137[30] = 0;
   out_3094512129210927137[31] = 0;
   out_3094512129210927137[32] = 0;
   out_3094512129210927137[33] = 0;
   out_3094512129210927137[34] = 0;
   out_3094512129210927137[35] = 0;
   out_3094512129210927137[36] = 0;
   out_3094512129210927137[37] = 0;
   out_3094512129210927137[38] = 0;
   out_3094512129210927137[39] = 0;
   out_3094512129210927137[40] = 0;
   out_3094512129210927137[41] = 0;
   out_3094512129210927137[42] = 0;
   out_3094512129210927137[43] = 0;
   out_3094512129210927137[44] = 1;
   out_3094512129210927137[45] = 0;
   out_3094512129210927137[46] = 0;
   out_3094512129210927137[47] = 1;
   out_3094512129210927137[48] = 0;
   out_3094512129210927137[49] = 0;
   out_3094512129210927137[50] = 0;
   out_3094512129210927137[51] = 0;
   out_3094512129210927137[52] = 0;
   out_3094512129210927137[53] = 0;
}
void h_10(double *state, double *unused, double *out_6387867301137382468) {
   out_6387867301137382468[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_6387867301137382468[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_6387867301137382468[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_9062747238086022329) {
   out_9062747238086022329[0] = 0;
   out_9062747238086022329[1] = 9.8100000000000005*cos(state[1]);
   out_9062747238086022329[2] = 0;
   out_9062747238086022329[3] = 0;
   out_9062747238086022329[4] = -state[8];
   out_9062747238086022329[5] = state[7];
   out_9062747238086022329[6] = 0;
   out_9062747238086022329[7] = state[5];
   out_9062747238086022329[8] = -state[4];
   out_9062747238086022329[9] = 0;
   out_9062747238086022329[10] = 0;
   out_9062747238086022329[11] = 0;
   out_9062747238086022329[12] = 1;
   out_9062747238086022329[13] = 0;
   out_9062747238086022329[14] = 0;
   out_9062747238086022329[15] = 1;
   out_9062747238086022329[16] = 0;
   out_9062747238086022329[17] = 0;
   out_9062747238086022329[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_9062747238086022329[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_9062747238086022329[20] = 0;
   out_9062747238086022329[21] = state[8];
   out_9062747238086022329[22] = 0;
   out_9062747238086022329[23] = -state[6];
   out_9062747238086022329[24] = -state[5];
   out_9062747238086022329[25] = 0;
   out_9062747238086022329[26] = state[3];
   out_9062747238086022329[27] = 0;
   out_9062747238086022329[28] = 0;
   out_9062747238086022329[29] = 0;
   out_9062747238086022329[30] = 0;
   out_9062747238086022329[31] = 1;
   out_9062747238086022329[32] = 0;
   out_9062747238086022329[33] = 0;
   out_9062747238086022329[34] = 1;
   out_9062747238086022329[35] = 0;
   out_9062747238086022329[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_9062747238086022329[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_9062747238086022329[38] = 0;
   out_9062747238086022329[39] = -state[7];
   out_9062747238086022329[40] = state[6];
   out_9062747238086022329[41] = 0;
   out_9062747238086022329[42] = state[4];
   out_9062747238086022329[43] = -state[3];
   out_9062747238086022329[44] = 0;
   out_9062747238086022329[45] = 0;
   out_9062747238086022329[46] = 0;
   out_9062747238086022329[47] = 0;
   out_9062747238086022329[48] = 0;
   out_9062747238086022329[49] = 0;
   out_9062747238086022329[50] = 1;
   out_9062747238086022329[51] = 0;
   out_9062747238086022329[52] = 0;
   out_9062747238086022329[53] = 1;
}
void h_13(double *state, double *unused, double *out_1513057542253796280) {
   out_1513057542253796280[0] = state[3];
   out_1513057542253796280[1] = state[4];
   out_1513057542253796280[2] = state[5];
}
void H_13(double *state, double *unused, double *out_7741600736181923550) {
   out_7741600736181923550[0] = 0;
   out_7741600736181923550[1] = 0;
   out_7741600736181923550[2] = 0;
   out_7741600736181923550[3] = 1;
   out_7741600736181923550[4] = 0;
   out_7741600736181923550[5] = 0;
   out_7741600736181923550[6] = 0;
   out_7741600736181923550[7] = 0;
   out_7741600736181923550[8] = 0;
   out_7741600736181923550[9] = 0;
   out_7741600736181923550[10] = 0;
   out_7741600736181923550[11] = 0;
   out_7741600736181923550[12] = 0;
   out_7741600736181923550[13] = 0;
   out_7741600736181923550[14] = 0;
   out_7741600736181923550[15] = 0;
   out_7741600736181923550[16] = 0;
   out_7741600736181923550[17] = 0;
   out_7741600736181923550[18] = 0;
   out_7741600736181923550[19] = 0;
   out_7741600736181923550[20] = 0;
   out_7741600736181923550[21] = 0;
   out_7741600736181923550[22] = 1;
   out_7741600736181923550[23] = 0;
   out_7741600736181923550[24] = 0;
   out_7741600736181923550[25] = 0;
   out_7741600736181923550[26] = 0;
   out_7741600736181923550[27] = 0;
   out_7741600736181923550[28] = 0;
   out_7741600736181923550[29] = 0;
   out_7741600736181923550[30] = 0;
   out_7741600736181923550[31] = 0;
   out_7741600736181923550[32] = 0;
   out_7741600736181923550[33] = 0;
   out_7741600736181923550[34] = 0;
   out_7741600736181923550[35] = 0;
   out_7741600736181923550[36] = 0;
   out_7741600736181923550[37] = 0;
   out_7741600736181923550[38] = 0;
   out_7741600736181923550[39] = 0;
   out_7741600736181923550[40] = 0;
   out_7741600736181923550[41] = 1;
   out_7741600736181923550[42] = 0;
   out_7741600736181923550[43] = 0;
   out_7741600736181923550[44] = 0;
   out_7741600736181923550[45] = 0;
   out_7741600736181923550[46] = 0;
   out_7741600736181923550[47] = 0;
   out_7741600736181923550[48] = 0;
   out_7741600736181923550[49] = 0;
   out_7741600736181923550[50] = 0;
   out_7741600736181923550[51] = 0;
   out_7741600736181923550[52] = 0;
   out_7741600736181923550[53] = 0;
}
void h_14(double *state, double *unused, double *out_5536059940238760705) {
   out_5536059940238760705[0] = state[6];
   out_5536059940238760705[1] = state[7];
   out_5536059940238760705[2] = state[8];
}
void H_14(double *state, double *unused, double *out_7057752985550411666) {
   out_7057752985550411666[0] = 0;
   out_7057752985550411666[1] = 0;
   out_7057752985550411666[2] = 0;
   out_7057752985550411666[3] = 0;
   out_7057752985550411666[4] = 0;
   out_7057752985550411666[5] = 0;
   out_7057752985550411666[6] = 1;
   out_7057752985550411666[7] = 0;
   out_7057752985550411666[8] = 0;
   out_7057752985550411666[9] = 0;
   out_7057752985550411666[10] = 0;
   out_7057752985550411666[11] = 0;
   out_7057752985550411666[12] = 0;
   out_7057752985550411666[13] = 0;
   out_7057752985550411666[14] = 0;
   out_7057752985550411666[15] = 0;
   out_7057752985550411666[16] = 0;
   out_7057752985550411666[17] = 0;
   out_7057752985550411666[18] = 0;
   out_7057752985550411666[19] = 0;
   out_7057752985550411666[20] = 0;
   out_7057752985550411666[21] = 0;
   out_7057752985550411666[22] = 0;
   out_7057752985550411666[23] = 0;
   out_7057752985550411666[24] = 0;
   out_7057752985550411666[25] = 1;
   out_7057752985550411666[26] = 0;
   out_7057752985550411666[27] = 0;
   out_7057752985550411666[28] = 0;
   out_7057752985550411666[29] = 0;
   out_7057752985550411666[30] = 0;
   out_7057752985550411666[31] = 0;
   out_7057752985550411666[32] = 0;
   out_7057752985550411666[33] = 0;
   out_7057752985550411666[34] = 0;
   out_7057752985550411666[35] = 0;
   out_7057752985550411666[36] = 0;
   out_7057752985550411666[37] = 0;
   out_7057752985550411666[38] = 0;
   out_7057752985550411666[39] = 0;
   out_7057752985550411666[40] = 0;
   out_7057752985550411666[41] = 0;
   out_7057752985550411666[42] = 0;
   out_7057752985550411666[43] = 0;
   out_7057752985550411666[44] = 1;
   out_7057752985550411666[45] = 0;
   out_7057752985550411666[46] = 0;
   out_7057752985550411666[47] = 0;
   out_7057752985550411666[48] = 0;
   out_7057752985550411666[49] = 0;
   out_7057752985550411666[50] = 0;
   out_7057752985550411666[51] = 0;
   out_7057752985550411666[52] = 0;
   out_7057752985550411666[53] = 0;
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
void pose_err_fun(double *nom_x, double *delta_x, double *out_1111971644641323647) {
  err_fun(nom_x, delta_x, out_1111971644641323647);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_2323187048211860073) {
  inv_err_fun(nom_x, true_x, out_2323187048211860073);
}
void pose_H_mod_fun(double *state, double *out_5105792754762297630) {
  H_mod_fun(state, out_5105792754762297630);
}
void pose_f_fun(double *state, double dt, double *out_493911584477072580) {
  f_fun(state,  dt, out_493911584477072580);
}
void pose_F_fun(double *state, double dt, double *out_5423918991428774343) {
  F_fun(state,  dt, out_5423918991428774343);
}
void pose_h_4(double *state, double *unused, double *out_5685577583451539137) {
  h_4(state, unused, out_5685577583451539137);
}
void pose_H_4(double *state, double *unused, double *out_3094512129210927137) {
  H_4(state, unused, out_3094512129210927137);
}
void pose_h_10(double *state, double *unused, double *out_6387867301137382468) {
  h_10(state, unused, out_6387867301137382468);
}
void pose_H_10(double *state, double *unused, double *out_9062747238086022329) {
  H_10(state, unused, out_9062747238086022329);
}
void pose_h_13(double *state, double *unused, double *out_1513057542253796280) {
  h_13(state, unused, out_1513057542253796280);
}
void pose_H_13(double *state, double *unused, double *out_7741600736181923550) {
  H_13(state, unused, out_7741600736181923550);
}
void pose_h_14(double *state, double *unused, double *out_5536059940238760705) {
  h_14(state, unused, out_5536059940238760705);
}
void pose_H_14(double *state, double *unused, double *out_7057752985550411666) {
  H_14(state, unused, out_7057752985550411666);
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
