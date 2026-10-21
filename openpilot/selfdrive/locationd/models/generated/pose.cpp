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
void err_fun(double *nom_x, double *delta_x, double *out_2136964252019170465) {
   out_2136964252019170465[0] = delta_x[0] + nom_x[0];
   out_2136964252019170465[1] = delta_x[1] + nom_x[1];
   out_2136964252019170465[2] = delta_x[2] + nom_x[2];
   out_2136964252019170465[3] = delta_x[3] + nom_x[3];
   out_2136964252019170465[4] = delta_x[4] + nom_x[4];
   out_2136964252019170465[5] = delta_x[5] + nom_x[5];
   out_2136964252019170465[6] = delta_x[6] + nom_x[6];
   out_2136964252019170465[7] = delta_x[7] + nom_x[7];
   out_2136964252019170465[8] = delta_x[8] + nom_x[8];
   out_2136964252019170465[9] = delta_x[9] + nom_x[9];
   out_2136964252019170465[10] = delta_x[10] + nom_x[10];
   out_2136964252019170465[11] = delta_x[11] + nom_x[11];
   out_2136964252019170465[12] = delta_x[12] + nom_x[12];
   out_2136964252019170465[13] = delta_x[13] + nom_x[13];
   out_2136964252019170465[14] = delta_x[14] + nom_x[14];
   out_2136964252019170465[15] = delta_x[15] + nom_x[15];
   out_2136964252019170465[16] = delta_x[16] + nom_x[16];
   out_2136964252019170465[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_1227563649279098162) {
   out_1227563649279098162[0] = -nom_x[0] + true_x[0];
   out_1227563649279098162[1] = -nom_x[1] + true_x[1];
   out_1227563649279098162[2] = -nom_x[2] + true_x[2];
   out_1227563649279098162[3] = -nom_x[3] + true_x[3];
   out_1227563649279098162[4] = -nom_x[4] + true_x[4];
   out_1227563649279098162[5] = -nom_x[5] + true_x[5];
   out_1227563649279098162[6] = -nom_x[6] + true_x[6];
   out_1227563649279098162[7] = -nom_x[7] + true_x[7];
   out_1227563649279098162[8] = -nom_x[8] + true_x[8];
   out_1227563649279098162[9] = -nom_x[9] + true_x[9];
   out_1227563649279098162[10] = -nom_x[10] + true_x[10];
   out_1227563649279098162[11] = -nom_x[11] + true_x[11];
   out_1227563649279098162[12] = -nom_x[12] + true_x[12];
   out_1227563649279098162[13] = -nom_x[13] + true_x[13];
   out_1227563649279098162[14] = -nom_x[14] + true_x[14];
   out_1227563649279098162[15] = -nom_x[15] + true_x[15];
   out_1227563649279098162[16] = -nom_x[16] + true_x[16];
   out_1227563649279098162[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_497819547195898390) {
   out_497819547195898390[0] = 1.0;
   out_497819547195898390[1] = 0.0;
   out_497819547195898390[2] = 0.0;
   out_497819547195898390[3] = 0.0;
   out_497819547195898390[4] = 0.0;
   out_497819547195898390[5] = 0.0;
   out_497819547195898390[6] = 0.0;
   out_497819547195898390[7] = 0.0;
   out_497819547195898390[8] = 0.0;
   out_497819547195898390[9] = 0.0;
   out_497819547195898390[10] = 0.0;
   out_497819547195898390[11] = 0.0;
   out_497819547195898390[12] = 0.0;
   out_497819547195898390[13] = 0.0;
   out_497819547195898390[14] = 0.0;
   out_497819547195898390[15] = 0.0;
   out_497819547195898390[16] = 0.0;
   out_497819547195898390[17] = 0.0;
   out_497819547195898390[18] = 0.0;
   out_497819547195898390[19] = 1.0;
   out_497819547195898390[20] = 0.0;
   out_497819547195898390[21] = 0.0;
   out_497819547195898390[22] = 0.0;
   out_497819547195898390[23] = 0.0;
   out_497819547195898390[24] = 0.0;
   out_497819547195898390[25] = 0.0;
   out_497819547195898390[26] = 0.0;
   out_497819547195898390[27] = 0.0;
   out_497819547195898390[28] = 0.0;
   out_497819547195898390[29] = 0.0;
   out_497819547195898390[30] = 0.0;
   out_497819547195898390[31] = 0.0;
   out_497819547195898390[32] = 0.0;
   out_497819547195898390[33] = 0.0;
   out_497819547195898390[34] = 0.0;
   out_497819547195898390[35] = 0.0;
   out_497819547195898390[36] = 0.0;
   out_497819547195898390[37] = 0.0;
   out_497819547195898390[38] = 1.0;
   out_497819547195898390[39] = 0.0;
   out_497819547195898390[40] = 0.0;
   out_497819547195898390[41] = 0.0;
   out_497819547195898390[42] = 0.0;
   out_497819547195898390[43] = 0.0;
   out_497819547195898390[44] = 0.0;
   out_497819547195898390[45] = 0.0;
   out_497819547195898390[46] = 0.0;
   out_497819547195898390[47] = 0.0;
   out_497819547195898390[48] = 0.0;
   out_497819547195898390[49] = 0.0;
   out_497819547195898390[50] = 0.0;
   out_497819547195898390[51] = 0.0;
   out_497819547195898390[52] = 0.0;
   out_497819547195898390[53] = 0.0;
   out_497819547195898390[54] = 0.0;
   out_497819547195898390[55] = 0.0;
   out_497819547195898390[56] = 0.0;
   out_497819547195898390[57] = 1.0;
   out_497819547195898390[58] = 0.0;
   out_497819547195898390[59] = 0.0;
   out_497819547195898390[60] = 0.0;
   out_497819547195898390[61] = 0.0;
   out_497819547195898390[62] = 0.0;
   out_497819547195898390[63] = 0.0;
   out_497819547195898390[64] = 0.0;
   out_497819547195898390[65] = 0.0;
   out_497819547195898390[66] = 0.0;
   out_497819547195898390[67] = 0.0;
   out_497819547195898390[68] = 0.0;
   out_497819547195898390[69] = 0.0;
   out_497819547195898390[70] = 0.0;
   out_497819547195898390[71] = 0.0;
   out_497819547195898390[72] = 0.0;
   out_497819547195898390[73] = 0.0;
   out_497819547195898390[74] = 0.0;
   out_497819547195898390[75] = 0.0;
   out_497819547195898390[76] = 1.0;
   out_497819547195898390[77] = 0.0;
   out_497819547195898390[78] = 0.0;
   out_497819547195898390[79] = 0.0;
   out_497819547195898390[80] = 0.0;
   out_497819547195898390[81] = 0.0;
   out_497819547195898390[82] = 0.0;
   out_497819547195898390[83] = 0.0;
   out_497819547195898390[84] = 0.0;
   out_497819547195898390[85] = 0.0;
   out_497819547195898390[86] = 0.0;
   out_497819547195898390[87] = 0.0;
   out_497819547195898390[88] = 0.0;
   out_497819547195898390[89] = 0.0;
   out_497819547195898390[90] = 0.0;
   out_497819547195898390[91] = 0.0;
   out_497819547195898390[92] = 0.0;
   out_497819547195898390[93] = 0.0;
   out_497819547195898390[94] = 0.0;
   out_497819547195898390[95] = 1.0;
   out_497819547195898390[96] = 0.0;
   out_497819547195898390[97] = 0.0;
   out_497819547195898390[98] = 0.0;
   out_497819547195898390[99] = 0.0;
   out_497819547195898390[100] = 0.0;
   out_497819547195898390[101] = 0.0;
   out_497819547195898390[102] = 0.0;
   out_497819547195898390[103] = 0.0;
   out_497819547195898390[104] = 0.0;
   out_497819547195898390[105] = 0.0;
   out_497819547195898390[106] = 0.0;
   out_497819547195898390[107] = 0.0;
   out_497819547195898390[108] = 0.0;
   out_497819547195898390[109] = 0.0;
   out_497819547195898390[110] = 0.0;
   out_497819547195898390[111] = 0.0;
   out_497819547195898390[112] = 0.0;
   out_497819547195898390[113] = 0.0;
   out_497819547195898390[114] = 1.0;
   out_497819547195898390[115] = 0.0;
   out_497819547195898390[116] = 0.0;
   out_497819547195898390[117] = 0.0;
   out_497819547195898390[118] = 0.0;
   out_497819547195898390[119] = 0.0;
   out_497819547195898390[120] = 0.0;
   out_497819547195898390[121] = 0.0;
   out_497819547195898390[122] = 0.0;
   out_497819547195898390[123] = 0.0;
   out_497819547195898390[124] = 0.0;
   out_497819547195898390[125] = 0.0;
   out_497819547195898390[126] = 0.0;
   out_497819547195898390[127] = 0.0;
   out_497819547195898390[128] = 0.0;
   out_497819547195898390[129] = 0.0;
   out_497819547195898390[130] = 0.0;
   out_497819547195898390[131] = 0.0;
   out_497819547195898390[132] = 0.0;
   out_497819547195898390[133] = 1.0;
   out_497819547195898390[134] = 0.0;
   out_497819547195898390[135] = 0.0;
   out_497819547195898390[136] = 0.0;
   out_497819547195898390[137] = 0.0;
   out_497819547195898390[138] = 0.0;
   out_497819547195898390[139] = 0.0;
   out_497819547195898390[140] = 0.0;
   out_497819547195898390[141] = 0.0;
   out_497819547195898390[142] = 0.0;
   out_497819547195898390[143] = 0.0;
   out_497819547195898390[144] = 0.0;
   out_497819547195898390[145] = 0.0;
   out_497819547195898390[146] = 0.0;
   out_497819547195898390[147] = 0.0;
   out_497819547195898390[148] = 0.0;
   out_497819547195898390[149] = 0.0;
   out_497819547195898390[150] = 0.0;
   out_497819547195898390[151] = 0.0;
   out_497819547195898390[152] = 1.0;
   out_497819547195898390[153] = 0.0;
   out_497819547195898390[154] = 0.0;
   out_497819547195898390[155] = 0.0;
   out_497819547195898390[156] = 0.0;
   out_497819547195898390[157] = 0.0;
   out_497819547195898390[158] = 0.0;
   out_497819547195898390[159] = 0.0;
   out_497819547195898390[160] = 0.0;
   out_497819547195898390[161] = 0.0;
   out_497819547195898390[162] = 0.0;
   out_497819547195898390[163] = 0.0;
   out_497819547195898390[164] = 0.0;
   out_497819547195898390[165] = 0.0;
   out_497819547195898390[166] = 0.0;
   out_497819547195898390[167] = 0.0;
   out_497819547195898390[168] = 0.0;
   out_497819547195898390[169] = 0.0;
   out_497819547195898390[170] = 0.0;
   out_497819547195898390[171] = 1.0;
   out_497819547195898390[172] = 0.0;
   out_497819547195898390[173] = 0.0;
   out_497819547195898390[174] = 0.0;
   out_497819547195898390[175] = 0.0;
   out_497819547195898390[176] = 0.0;
   out_497819547195898390[177] = 0.0;
   out_497819547195898390[178] = 0.0;
   out_497819547195898390[179] = 0.0;
   out_497819547195898390[180] = 0.0;
   out_497819547195898390[181] = 0.0;
   out_497819547195898390[182] = 0.0;
   out_497819547195898390[183] = 0.0;
   out_497819547195898390[184] = 0.0;
   out_497819547195898390[185] = 0.0;
   out_497819547195898390[186] = 0.0;
   out_497819547195898390[187] = 0.0;
   out_497819547195898390[188] = 0.0;
   out_497819547195898390[189] = 0.0;
   out_497819547195898390[190] = 1.0;
   out_497819547195898390[191] = 0.0;
   out_497819547195898390[192] = 0.0;
   out_497819547195898390[193] = 0.0;
   out_497819547195898390[194] = 0.0;
   out_497819547195898390[195] = 0.0;
   out_497819547195898390[196] = 0.0;
   out_497819547195898390[197] = 0.0;
   out_497819547195898390[198] = 0.0;
   out_497819547195898390[199] = 0.0;
   out_497819547195898390[200] = 0.0;
   out_497819547195898390[201] = 0.0;
   out_497819547195898390[202] = 0.0;
   out_497819547195898390[203] = 0.0;
   out_497819547195898390[204] = 0.0;
   out_497819547195898390[205] = 0.0;
   out_497819547195898390[206] = 0.0;
   out_497819547195898390[207] = 0.0;
   out_497819547195898390[208] = 0.0;
   out_497819547195898390[209] = 1.0;
   out_497819547195898390[210] = 0.0;
   out_497819547195898390[211] = 0.0;
   out_497819547195898390[212] = 0.0;
   out_497819547195898390[213] = 0.0;
   out_497819547195898390[214] = 0.0;
   out_497819547195898390[215] = 0.0;
   out_497819547195898390[216] = 0.0;
   out_497819547195898390[217] = 0.0;
   out_497819547195898390[218] = 0.0;
   out_497819547195898390[219] = 0.0;
   out_497819547195898390[220] = 0.0;
   out_497819547195898390[221] = 0.0;
   out_497819547195898390[222] = 0.0;
   out_497819547195898390[223] = 0.0;
   out_497819547195898390[224] = 0.0;
   out_497819547195898390[225] = 0.0;
   out_497819547195898390[226] = 0.0;
   out_497819547195898390[227] = 0.0;
   out_497819547195898390[228] = 1.0;
   out_497819547195898390[229] = 0.0;
   out_497819547195898390[230] = 0.0;
   out_497819547195898390[231] = 0.0;
   out_497819547195898390[232] = 0.0;
   out_497819547195898390[233] = 0.0;
   out_497819547195898390[234] = 0.0;
   out_497819547195898390[235] = 0.0;
   out_497819547195898390[236] = 0.0;
   out_497819547195898390[237] = 0.0;
   out_497819547195898390[238] = 0.0;
   out_497819547195898390[239] = 0.0;
   out_497819547195898390[240] = 0.0;
   out_497819547195898390[241] = 0.0;
   out_497819547195898390[242] = 0.0;
   out_497819547195898390[243] = 0.0;
   out_497819547195898390[244] = 0.0;
   out_497819547195898390[245] = 0.0;
   out_497819547195898390[246] = 0.0;
   out_497819547195898390[247] = 1.0;
   out_497819547195898390[248] = 0.0;
   out_497819547195898390[249] = 0.0;
   out_497819547195898390[250] = 0.0;
   out_497819547195898390[251] = 0.0;
   out_497819547195898390[252] = 0.0;
   out_497819547195898390[253] = 0.0;
   out_497819547195898390[254] = 0.0;
   out_497819547195898390[255] = 0.0;
   out_497819547195898390[256] = 0.0;
   out_497819547195898390[257] = 0.0;
   out_497819547195898390[258] = 0.0;
   out_497819547195898390[259] = 0.0;
   out_497819547195898390[260] = 0.0;
   out_497819547195898390[261] = 0.0;
   out_497819547195898390[262] = 0.0;
   out_497819547195898390[263] = 0.0;
   out_497819547195898390[264] = 0.0;
   out_497819547195898390[265] = 0.0;
   out_497819547195898390[266] = 1.0;
   out_497819547195898390[267] = 0.0;
   out_497819547195898390[268] = 0.0;
   out_497819547195898390[269] = 0.0;
   out_497819547195898390[270] = 0.0;
   out_497819547195898390[271] = 0.0;
   out_497819547195898390[272] = 0.0;
   out_497819547195898390[273] = 0.0;
   out_497819547195898390[274] = 0.0;
   out_497819547195898390[275] = 0.0;
   out_497819547195898390[276] = 0.0;
   out_497819547195898390[277] = 0.0;
   out_497819547195898390[278] = 0.0;
   out_497819547195898390[279] = 0.0;
   out_497819547195898390[280] = 0.0;
   out_497819547195898390[281] = 0.0;
   out_497819547195898390[282] = 0.0;
   out_497819547195898390[283] = 0.0;
   out_497819547195898390[284] = 0.0;
   out_497819547195898390[285] = 1.0;
   out_497819547195898390[286] = 0.0;
   out_497819547195898390[287] = 0.0;
   out_497819547195898390[288] = 0.0;
   out_497819547195898390[289] = 0.0;
   out_497819547195898390[290] = 0.0;
   out_497819547195898390[291] = 0.0;
   out_497819547195898390[292] = 0.0;
   out_497819547195898390[293] = 0.0;
   out_497819547195898390[294] = 0.0;
   out_497819547195898390[295] = 0.0;
   out_497819547195898390[296] = 0.0;
   out_497819547195898390[297] = 0.0;
   out_497819547195898390[298] = 0.0;
   out_497819547195898390[299] = 0.0;
   out_497819547195898390[300] = 0.0;
   out_497819547195898390[301] = 0.0;
   out_497819547195898390[302] = 0.0;
   out_497819547195898390[303] = 0.0;
   out_497819547195898390[304] = 1.0;
   out_497819547195898390[305] = 0.0;
   out_497819547195898390[306] = 0.0;
   out_497819547195898390[307] = 0.0;
   out_497819547195898390[308] = 0.0;
   out_497819547195898390[309] = 0.0;
   out_497819547195898390[310] = 0.0;
   out_497819547195898390[311] = 0.0;
   out_497819547195898390[312] = 0.0;
   out_497819547195898390[313] = 0.0;
   out_497819547195898390[314] = 0.0;
   out_497819547195898390[315] = 0.0;
   out_497819547195898390[316] = 0.0;
   out_497819547195898390[317] = 0.0;
   out_497819547195898390[318] = 0.0;
   out_497819547195898390[319] = 0.0;
   out_497819547195898390[320] = 0.0;
   out_497819547195898390[321] = 0.0;
   out_497819547195898390[322] = 0.0;
   out_497819547195898390[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_4587333856526582363) {
   out_4587333856526582363[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_4587333856526582363[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_4587333856526582363[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_4587333856526582363[3] = dt*state[12] + state[3];
   out_4587333856526582363[4] = dt*state[13] + state[4];
   out_4587333856526582363[5] = dt*state[14] + state[5];
   out_4587333856526582363[6] = state[6];
   out_4587333856526582363[7] = state[7];
   out_4587333856526582363[8] = state[8];
   out_4587333856526582363[9] = state[9];
   out_4587333856526582363[10] = state[10];
   out_4587333856526582363[11] = state[11];
   out_4587333856526582363[12] = state[12];
   out_4587333856526582363[13] = state[13];
   out_4587333856526582363[14] = state[14];
   out_4587333856526582363[15] = state[15];
   out_4587333856526582363[16] = state[16];
   out_4587333856526582363[17] = state[17];
}
void F_fun(double *state, double dt, double *out_494582711598413239) {
   out_494582711598413239[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_494582711598413239[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_494582711598413239[2] = 0;
   out_494582711598413239[3] = 0;
   out_494582711598413239[4] = 0;
   out_494582711598413239[5] = 0;
   out_494582711598413239[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_494582711598413239[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_494582711598413239[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_494582711598413239[9] = 0;
   out_494582711598413239[10] = 0;
   out_494582711598413239[11] = 0;
   out_494582711598413239[12] = 0;
   out_494582711598413239[13] = 0;
   out_494582711598413239[14] = 0;
   out_494582711598413239[15] = 0;
   out_494582711598413239[16] = 0;
   out_494582711598413239[17] = 0;
   out_494582711598413239[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_494582711598413239[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_494582711598413239[20] = 0;
   out_494582711598413239[21] = 0;
   out_494582711598413239[22] = 0;
   out_494582711598413239[23] = 0;
   out_494582711598413239[24] = 0;
   out_494582711598413239[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_494582711598413239[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_494582711598413239[27] = 0;
   out_494582711598413239[28] = 0;
   out_494582711598413239[29] = 0;
   out_494582711598413239[30] = 0;
   out_494582711598413239[31] = 0;
   out_494582711598413239[32] = 0;
   out_494582711598413239[33] = 0;
   out_494582711598413239[34] = 0;
   out_494582711598413239[35] = 0;
   out_494582711598413239[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_494582711598413239[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_494582711598413239[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_494582711598413239[39] = 0;
   out_494582711598413239[40] = 0;
   out_494582711598413239[41] = 0;
   out_494582711598413239[42] = 0;
   out_494582711598413239[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_494582711598413239[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_494582711598413239[45] = 0;
   out_494582711598413239[46] = 0;
   out_494582711598413239[47] = 0;
   out_494582711598413239[48] = 0;
   out_494582711598413239[49] = 0;
   out_494582711598413239[50] = 0;
   out_494582711598413239[51] = 0;
   out_494582711598413239[52] = 0;
   out_494582711598413239[53] = 0;
   out_494582711598413239[54] = 0;
   out_494582711598413239[55] = 0;
   out_494582711598413239[56] = 0;
   out_494582711598413239[57] = 1;
   out_494582711598413239[58] = 0;
   out_494582711598413239[59] = 0;
   out_494582711598413239[60] = 0;
   out_494582711598413239[61] = 0;
   out_494582711598413239[62] = 0;
   out_494582711598413239[63] = 0;
   out_494582711598413239[64] = 0;
   out_494582711598413239[65] = 0;
   out_494582711598413239[66] = dt;
   out_494582711598413239[67] = 0;
   out_494582711598413239[68] = 0;
   out_494582711598413239[69] = 0;
   out_494582711598413239[70] = 0;
   out_494582711598413239[71] = 0;
   out_494582711598413239[72] = 0;
   out_494582711598413239[73] = 0;
   out_494582711598413239[74] = 0;
   out_494582711598413239[75] = 0;
   out_494582711598413239[76] = 1;
   out_494582711598413239[77] = 0;
   out_494582711598413239[78] = 0;
   out_494582711598413239[79] = 0;
   out_494582711598413239[80] = 0;
   out_494582711598413239[81] = 0;
   out_494582711598413239[82] = 0;
   out_494582711598413239[83] = 0;
   out_494582711598413239[84] = 0;
   out_494582711598413239[85] = dt;
   out_494582711598413239[86] = 0;
   out_494582711598413239[87] = 0;
   out_494582711598413239[88] = 0;
   out_494582711598413239[89] = 0;
   out_494582711598413239[90] = 0;
   out_494582711598413239[91] = 0;
   out_494582711598413239[92] = 0;
   out_494582711598413239[93] = 0;
   out_494582711598413239[94] = 0;
   out_494582711598413239[95] = 1;
   out_494582711598413239[96] = 0;
   out_494582711598413239[97] = 0;
   out_494582711598413239[98] = 0;
   out_494582711598413239[99] = 0;
   out_494582711598413239[100] = 0;
   out_494582711598413239[101] = 0;
   out_494582711598413239[102] = 0;
   out_494582711598413239[103] = 0;
   out_494582711598413239[104] = dt;
   out_494582711598413239[105] = 0;
   out_494582711598413239[106] = 0;
   out_494582711598413239[107] = 0;
   out_494582711598413239[108] = 0;
   out_494582711598413239[109] = 0;
   out_494582711598413239[110] = 0;
   out_494582711598413239[111] = 0;
   out_494582711598413239[112] = 0;
   out_494582711598413239[113] = 0;
   out_494582711598413239[114] = 1;
   out_494582711598413239[115] = 0;
   out_494582711598413239[116] = 0;
   out_494582711598413239[117] = 0;
   out_494582711598413239[118] = 0;
   out_494582711598413239[119] = 0;
   out_494582711598413239[120] = 0;
   out_494582711598413239[121] = 0;
   out_494582711598413239[122] = 0;
   out_494582711598413239[123] = 0;
   out_494582711598413239[124] = 0;
   out_494582711598413239[125] = 0;
   out_494582711598413239[126] = 0;
   out_494582711598413239[127] = 0;
   out_494582711598413239[128] = 0;
   out_494582711598413239[129] = 0;
   out_494582711598413239[130] = 0;
   out_494582711598413239[131] = 0;
   out_494582711598413239[132] = 0;
   out_494582711598413239[133] = 1;
   out_494582711598413239[134] = 0;
   out_494582711598413239[135] = 0;
   out_494582711598413239[136] = 0;
   out_494582711598413239[137] = 0;
   out_494582711598413239[138] = 0;
   out_494582711598413239[139] = 0;
   out_494582711598413239[140] = 0;
   out_494582711598413239[141] = 0;
   out_494582711598413239[142] = 0;
   out_494582711598413239[143] = 0;
   out_494582711598413239[144] = 0;
   out_494582711598413239[145] = 0;
   out_494582711598413239[146] = 0;
   out_494582711598413239[147] = 0;
   out_494582711598413239[148] = 0;
   out_494582711598413239[149] = 0;
   out_494582711598413239[150] = 0;
   out_494582711598413239[151] = 0;
   out_494582711598413239[152] = 1;
   out_494582711598413239[153] = 0;
   out_494582711598413239[154] = 0;
   out_494582711598413239[155] = 0;
   out_494582711598413239[156] = 0;
   out_494582711598413239[157] = 0;
   out_494582711598413239[158] = 0;
   out_494582711598413239[159] = 0;
   out_494582711598413239[160] = 0;
   out_494582711598413239[161] = 0;
   out_494582711598413239[162] = 0;
   out_494582711598413239[163] = 0;
   out_494582711598413239[164] = 0;
   out_494582711598413239[165] = 0;
   out_494582711598413239[166] = 0;
   out_494582711598413239[167] = 0;
   out_494582711598413239[168] = 0;
   out_494582711598413239[169] = 0;
   out_494582711598413239[170] = 0;
   out_494582711598413239[171] = 1;
   out_494582711598413239[172] = 0;
   out_494582711598413239[173] = 0;
   out_494582711598413239[174] = 0;
   out_494582711598413239[175] = 0;
   out_494582711598413239[176] = 0;
   out_494582711598413239[177] = 0;
   out_494582711598413239[178] = 0;
   out_494582711598413239[179] = 0;
   out_494582711598413239[180] = 0;
   out_494582711598413239[181] = 0;
   out_494582711598413239[182] = 0;
   out_494582711598413239[183] = 0;
   out_494582711598413239[184] = 0;
   out_494582711598413239[185] = 0;
   out_494582711598413239[186] = 0;
   out_494582711598413239[187] = 0;
   out_494582711598413239[188] = 0;
   out_494582711598413239[189] = 0;
   out_494582711598413239[190] = 1;
   out_494582711598413239[191] = 0;
   out_494582711598413239[192] = 0;
   out_494582711598413239[193] = 0;
   out_494582711598413239[194] = 0;
   out_494582711598413239[195] = 0;
   out_494582711598413239[196] = 0;
   out_494582711598413239[197] = 0;
   out_494582711598413239[198] = 0;
   out_494582711598413239[199] = 0;
   out_494582711598413239[200] = 0;
   out_494582711598413239[201] = 0;
   out_494582711598413239[202] = 0;
   out_494582711598413239[203] = 0;
   out_494582711598413239[204] = 0;
   out_494582711598413239[205] = 0;
   out_494582711598413239[206] = 0;
   out_494582711598413239[207] = 0;
   out_494582711598413239[208] = 0;
   out_494582711598413239[209] = 1;
   out_494582711598413239[210] = 0;
   out_494582711598413239[211] = 0;
   out_494582711598413239[212] = 0;
   out_494582711598413239[213] = 0;
   out_494582711598413239[214] = 0;
   out_494582711598413239[215] = 0;
   out_494582711598413239[216] = 0;
   out_494582711598413239[217] = 0;
   out_494582711598413239[218] = 0;
   out_494582711598413239[219] = 0;
   out_494582711598413239[220] = 0;
   out_494582711598413239[221] = 0;
   out_494582711598413239[222] = 0;
   out_494582711598413239[223] = 0;
   out_494582711598413239[224] = 0;
   out_494582711598413239[225] = 0;
   out_494582711598413239[226] = 0;
   out_494582711598413239[227] = 0;
   out_494582711598413239[228] = 1;
   out_494582711598413239[229] = 0;
   out_494582711598413239[230] = 0;
   out_494582711598413239[231] = 0;
   out_494582711598413239[232] = 0;
   out_494582711598413239[233] = 0;
   out_494582711598413239[234] = 0;
   out_494582711598413239[235] = 0;
   out_494582711598413239[236] = 0;
   out_494582711598413239[237] = 0;
   out_494582711598413239[238] = 0;
   out_494582711598413239[239] = 0;
   out_494582711598413239[240] = 0;
   out_494582711598413239[241] = 0;
   out_494582711598413239[242] = 0;
   out_494582711598413239[243] = 0;
   out_494582711598413239[244] = 0;
   out_494582711598413239[245] = 0;
   out_494582711598413239[246] = 0;
   out_494582711598413239[247] = 1;
   out_494582711598413239[248] = 0;
   out_494582711598413239[249] = 0;
   out_494582711598413239[250] = 0;
   out_494582711598413239[251] = 0;
   out_494582711598413239[252] = 0;
   out_494582711598413239[253] = 0;
   out_494582711598413239[254] = 0;
   out_494582711598413239[255] = 0;
   out_494582711598413239[256] = 0;
   out_494582711598413239[257] = 0;
   out_494582711598413239[258] = 0;
   out_494582711598413239[259] = 0;
   out_494582711598413239[260] = 0;
   out_494582711598413239[261] = 0;
   out_494582711598413239[262] = 0;
   out_494582711598413239[263] = 0;
   out_494582711598413239[264] = 0;
   out_494582711598413239[265] = 0;
   out_494582711598413239[266] = 1;
   out_494582711598413239[267] = 0;
   out_494582711598413239[268] = 0;
   out_494582711598413239[269] = 0;
   out_494582711598413239[270] = 0;
   out_494582711598413239[271] = 0;
   out_494582711598413239[272] = 0;
   out_494582711598413239[273] = 0;
   out_494582711598413239[274] = 0;
   out_494582711598413239[275] = 0;
   out_494582711598413239[276] = 0;
   out_494582711598413239[277] = 0;
   out_494582711598413239[278] = 0;
   out_494582711598413239[279] = 0;
   out_494582711598413239[280] = 0;
   out_494582711598413239[281] = 0;
   out_494582711598413239[282] = 0;
   out_494582711598413239[283] = 0;
   out_494582711598413239[284] = 0;
   out_494582711598413239[285] = 1;
   out_494582711598413239[286] = 0;
   out_494582711598413239[287] = 0;
   out_494582711598413239[288] = 0;
   out_494582711598413239[289] = 0;
   out_494582711598413239[290] = 0;
   out_494582711598413239[291] = 0;
   out_494582711598413239[292] = 0;
   out_494582711598413239[293] = 0;
   out_494582711598413239[294] = 0;
   out_494582711598413239[295] = 0;
   out_494582711598413239[296] = 0;
   out_494582711598413239[297] = 0;
   out_494582711598413239[298] = 0;
   out_494582711598413239[299] = 0;
   out_494582711598413239[300] = 0;
   out_494582711598413239[301] = 0;
   out_494582711598413239[302] = 0;
   out_494582711598413239[303] = 0;
   out_494582711598413239[304] = 1;
   out_494582711598413239[305] = 0;
   out_494582711598413239[306] = 0;
   out_494582711598413239[307] = 0;
   out_494582711598413239[308] = 0;
   out_494582711598413239[309] = 0;
   out_494582711598413239[310] = 0;
   out_494582711598413239[311] = 0;
   out_494582711598413239[312] = 0;
   out_494582711598413239[313] = 0;
   out_494582711598413239[314] = 0;
   out_494582711598413239[315] = 0;
   out_494582711598413239[316] = 0;
   out_494582711598413239[317] = 0;
   out_494582711598413239[318] = 0;
   out_494582711598413239[319] = 0;
   out_494582711598413239[320] = 0;
   out_494582711598413239[321] = 0;
   out_494582711598413239[322] = 0;
   out_494582711598413239[323] = 1;
}
void h_4(double *state, double *unused, double *out_916469630118270672) {
   out_916469630118270672[0] = state[6] + state[9];
   out_916469630118270672[1] = state[7] + state[10];
   out_916469630118270672[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_1695767029078796494) {
   out_1695767029078796494[0] = 0;
   out_1695767029078796494[1] = 0;
   out_1695767029078796494[2] = 0;
   out_1695767029078796494[3] = 0;
   out_1695767029078796494[4] = 0;
   out_1695767029078796494[5] = 0;
   out_1695767029078796494[6] = 1;
   out_1695767029078796494[7] = 0;
   out_1695767029078796494[8] = 0;
   out_1695767029078796494[9] = 1;
   out_1695767029078796494[10] = 0;
   out_1695767029078796494[11] = 0;
   out_1695767029078796494[12] = 0;
   out_1695767029078796494[13] = 0;
   out_1695767029078796494[14] = 0;
   out_1695767029078796494[15] = 0;
   out_1695767029078796494[16] = 0;
   out_1695767029078796494[17] = 0;
   out_1695767029078796494[18] = 0;
   out_1695767029078796494[19] = 0;
   out_1695767029078796494[20] = 0;
   out_1695767029078796494[21] = 0;
   out_1695767029078796494[22] = 0;
   out_1695767029078796494[23] = 0;
   out_1695767029078796494[24] = 0;
   out_1695767029078796494[25] = 1;
   out_1695767029078796494[26] = 0;
   out_1695767029078796494[27] = 0;
   out_1695767029078796494[28] = 1;
   out_1695767029078796494[29] = 0;
   out_1695767029078796494[30] = 0;
   out_1695767029078796494[31] = 0;
   out_1695767029078796494[32] = 0;
   out_1695767029078796494[33] = 0;
   out_1695767029078796494[34] = 0;
   out_1695767029078796494[35] = 0;
   out_1695767029078796494[36] = 0;
   out_1695767029078796494[37] = 0;
   out_1695767029078796494[38] = 0;
   out_1695767029078796494[39] = 0;
   out_1695767029078796494[40] = 0;
   out_1695767029078796494[41] = 0;
   out_1695767029078796494[42] = 0;
   out_1695767029078796494[43] = 0;
   out_1695767029078796494[44] = 1;
   out_1695767029078796494[45] = 0;
   out_1695767029078796494[46] = 0;
   out_1695767029078796494[47] = 1;
   out_1695767029078796494[48] = 0;
   out_1695767029078796494[49] = 0;
   out_1695767029078796494[50] = 0;
   out_1695767029078796494[51] = 0;
   out_1695767029078796494[52] = 0;
   out_1695767029078796494[53] = 0;
}
void h_10(double *state, double *unused, double *out_8613013218873323766) {
   out_8613013218873323766[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_8613013218873323766[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_8613013218873323766[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_4891296382714452251) {
   out_4891296382714452251[0] = 0;
   out_4891296382714452251[1] = 9.8100000000000005*cos(state[1]);
   out_4891296382714452251[2] = 0;
   out_4891296382714452251[3] = 0;
   out_4891296382714452251[4] = -state[8];
   out_4891296382714452251[5] = state[7];
   out_4891296382714452251[6] = 0;
   out_4891296382714452251[7] = state[5];
   out_4891296382714452251[8] = -state[4];
   out_4891296382714452251[9] = 0;
   out_4891296382714452251[10] = 0;
   out_4891296382714452251[11] = 0;
   out_4891296382714452251[12] = 1;
   out_4891296382714452251[13] = 0;
   out_4891296382714452251[14] = 0;
   out_4891296382714452251[15] = 1;
   out_4891296382714452251[16] = 0;
   out_4891296382714452251[17] = 0;
   out_4891296382714452251[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_4891296382714452251[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_4891296382714452251[20] = 0;
   out_4891296382714452251[21] = state[8];
   out_4891296382714452251[22] = 0;
   out_4891296382714452251[23] = -state[6];
   out_4891296382714452251[24] = -state[5];
   out_4891296382714452251[25] = 0;
   out_4891296382714452251[26] = state[3];
   out_4891296382714452251[27] = 0;
   out_4891296382714452251[28] = 0;
   out_4891296382714452251[29] = 0;
   out_4891296382714452251[30] = 0;
   out_4891296382714452251[31] = 1;
   out_4891296382714452251[32] = 0;
   out_4891296382714452251[33] = 0;
   out_4891296382714452251[34] = 1;
   out_4891296382714452251[35] = 0;
   out_4891296382714452251[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_4891296382714452251[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_4891296382714452251[38] = 0;
   out_4891296382714452251[39] = -state[7];
   out_4891296382714452251[40] = state[6];
   out_4891296382714452251[41] = 0;
   out_4891296382714452251[42] = state[4];
   out_4891296382714452251[43] = -state[3];
   out_4891296382714452251[44] = 0;
   out_4891296382714452251[45] = 0;
   out_4891296382714452251[46] = 0;
   out_4891296382714452251[47] = 0;
   out_4891296382714452251[48] = 0;
   out_4891296382714452251[49] = 0;
   out_4891296382714452251[50] = 1;
   out_4891296382714452251[51] = 0;
   out_4891296382714452251[52] = 0;
   out_4891296382714452251[53] = 1;
}
void h_13(double *state, double *unused, double *out_1875641267677564388) {
   out_1875641267677564388[0] = state[3];
   out_1875641267677564388[1] = state[4];
   out_1875641267677564388[2] = state[5];
}
void H_13(double *state, double *unused, double *out_4908040854411129295) {
   out_4908040854411129295[0] = 0;
   out_4908040854411129295[1] = 0;
   out_4908040854411129295[2] = 0;
   out_4908040854411129295[3] = 1;
   out_4908040854411129295[4] = 0;
   out_4908040854411129295[5] = 0;
   out_4908040854411129295[6] = 0;
   out_4908040854411129295[7] = 0;
   out_4908040854411129295[8] = 0;
   out_4908040854411129295[9] = 0;
   out_4908040854411129295[10] = 0;
   out_4908040854411129295[11] = 0;
   out_4908040854411129295[12] = 0;
   out_4908040854411129295[13] = 0;
   out_4908040854411129295[14] = 0;
   out_4908040854411129295[15] = 0;
   out_4908040854411129295[16] = 0;
   out_4908040854411129295[17] = 0;
   out_4908040854411129295[18] = 0;
   out_4908040854411129295[19] = 0;
   out_4908040854411129295[20] = 0;
   out_4908040854411129295[21] = 0;
   out_4908040854411129295[22] = 1;
   out_4908040854411129295[23] = 0;
   out_4908040854411129295[24] = 0;
   out_4908040854411129295[25] = 0;
   out_4908040854411129295[26] = 0;
   out_4908040854411129295[27] = 0;
   out_4908040854411129295[28] = 0;
   out_4908040854411129295[29] = 0;
   out_4908040854411129295[30] = 0;
   out_4908040854411129295[31] = 0;
   out_4908040854411129295[32] = 0;
   out_4908040854411129295[33] = 0;
   out_4908040854411129295[34] = 0;
   out_4908040854411129295[35] = 0;
   out_4908040854411129295[36] = 0;
   out_4908040854411129295[37] = 0;
   out_4908040854411129295[38] = 0;
   out_4908040854411129295[39] = 0;
   out_4908040854411129295[40] = 0;
   out_4908040854411129295[41] = 1;
   out_4908040854411129295[42] = 0;
   out_4908040854411129295[43] = 0;
   out_4908040854411129295[44] = 0;
   out_4908040854411129295[45] = 0;
   out_4908040854411129295[46] = 0;
   out_4908040854411129295[47] = 0;
   out_4908040854411129295[48] = 0;
   out_4908040854411129295[49] = 0;
   out_4908040854411129295[50] = 0;
   out_4908040854411129295[51] = 0;
   out_4908040854411129295[52] = 0;
   out_4908040854411129295[53] = 0;
}
void h_14(double *state, double *unused, double *out_8565313325299850314) {
   out_8565313325299850314[0] = state[6];
   out_8565313325299850314[1] = state[7];
   out_8565313325299850314[2] = state[8];
}
void H_14(double *state, double *unused, double *out_1387021403216575802) {
   out_1387021403216575802[0] = 0;
   out_1387021403216575802[1] = 0;
   out_1387021403216575802[2] = 0;
   out_1387021403216575802[3] = 0;
   out_1387021403216575802[4] = 0;
   out_1387021403216575802[5] = 0;
   out_1387021403216575802[6] = 1;
   out_1387021403216575802[7] = 0;
   out_1387021403216575802[8] = 0;
   out_1387021403216575802[9] = 0;
   out_1387021403216575802[10] = 0;
   out_1387021403216575802[11] = 0;
   out_1387021403216575802[12] = 0;
   out_1387021403216575802[13] = 0;
   out_1387021403216575802[14] = 0;
   out_1387021403216575802[15] = 0;
   out_1387021403216575802[16] = 0;
   out_1387021403216575802[17] = 0;
   out_1387021403216575802[18] = 0;
   out_1387021403216575802[19] = 0;
   out_1387021403216575802[20] = 0;
   out_1387021403216575802[21] = 0;
   out_1387021403216575802[22] = 0;
   out_1387021403216575802[23] = 0;
   out_1387021403216575802[24] = 0;
   out_1387021403216575802[25] = 1;
   out_1387021403216575802[26] = 0;
   out_1387021403216575802[27] = 0;
   out_1387021403216575802[28] = 0;
   out_1387021403216575802[29] = 0;
   out_1387021403216575802[30] = 0;
   out_1387021403216575802[31] = 0;
   out_1387021403216575802[32] = 0;
   out_1387021403216575802[33] = 0;
   out_1387021403216575802[34] = 0;
   out_1387021403216575802[35] = 0;
   out_1387021403216575802[36] = 0;
   out_1387021403216575802[37] = 0;
   out_1387021403216575802[38] = 0;
   out_1387021403216575802[39] = 0;
   out_1387021403216575802[40] = 0;
   out_1387021403216575802[41] = 0;
   out_1387021403216575802[42] = 0;
   out_1387021403216575802[43] = 0;
   out_1387021403216575802[44] = 1;
   out_1387021403216575802[45] = 0;
   out_1387021403216575802[46] = 0;
   out_1387021403216575802[47] = 0;
   out_1387021403216575802[48] = 0;
   out_1387021403216575802[49] = 0;
   out_1387021403216575802[50] = 0;
   out_1387021403216575802[51] = 0;
   out_1387021403216575802[52] = 0;
   out_1387021403216575802[53] = 0;
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
void pose_err_fun(double *nom_x, double *delta_x, double *out_2136964252019170465) {
  err_fun(nom_x, delta_x, out_2136964252019170465);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_1227563649279098162) {
  inv_err_fun(nom_x, true_x, out_1227563649279098162);
}
void pose_H_mod_fun(double *state, double *out_497819547195898390) {
  H_mod_fun(state, out_497819547195898390);
}
void pose_f_fun(double *state, double dt, double *out_4587333856526582363) {
  f_fun(state,  dt, out_4587333856526582363);
}
void pose_F_fun(double *state, double dt, double *out_494582711598413239) {
  F_fun(state,  dt, out_494582711598413239);
}
void pose_h_4(double *state, double *unused, double *out_916469630118270672) {
  h_4(state, unused, out_916469630118270672);
}
void pose_H_4(double *state, double *unused, double *out_1695767029078796494) {
  H_4(state, unused, out_1695767029078796494);
}
void pose_h_10(double *state, double *unused, double *out_8613013218873323766) {
  h_10(state, unused, out_8613013218873323766);
}
void pose_H_10(double *state, double *unused, double *out_4891296382714452251) {
  H_10(state, unused, out_4891296382714452251);
}
void pose_h_13(double *state, double *unused, double *out_1875641267677564388) {
  h_13(state, unused, out_1875641267677564388);
}
void pose_H_13(double *state, double *unused, double *out_4908040854411129295) {
  H_13(state, unused, out_4908040854411129295);
}
void pose_h_14(double *state, double *unused, double *out_8565313325299850314) {
  h_14(state, unused, out_8565313325299850314);
}
void pose_H_14(double *state, double *unused, double *out_1387021403216575802) {
  H_14(state, unused, out_1387021403216575802);
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
