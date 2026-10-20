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
void err_fun(double *nom_x, double *delta_x, double *out_1557287137713055710) {
   out_1557287137713055710[0] = delta_x[0] + nom_x[0];
   out_1557287137713055710[1] = delta_x[1] + nom_x[1];
   out_1557287137713055710[2] = delta_x[2] + nom_x[2];
   out_1557287137713055710[3] = delta_x[3] + nom_x[3];
   out_1557287137713055710[4] = delta_x[4] + nom_x[4];
   out_1557287137713055710[5] = delta_x[5] + nom_x[5];
   out_1557287137713055710[6] = delta_x[6] + nom_x[6];
   out_1557287137713055710[7] = delta_x[7] + nom_x[7];
   out_1557287137713055710[8] = delta_x[8] + nom_x[8];
   out_1557287137713055710[9] = delta_x[9] + nom_x[9];
   out_1557287137713055710[10] = delta_x[10] + nom_x[10];
   out_1557287137713055710[11] = delta_x[11] + nom_x[11];
   out_1557287137713055710[12] = delta_x[12] + nom_x[12];
   out_1557287137713055710[13] = delta_x[13] + nom_x[13];
   out_1557287137713055710[14] = delta_x[14] + nom_x[14];
   out_1557287137713055710[15] = delta_x[15] + nom_x[15];
   out_1557287137713055710[16] = delta_x[16] + nom_x[16];
   out_1557287137713055710[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_6312955019060494268) {
   out_6312955019060494268[0] = -nom_x[0] + true_x[0];
   out_6312955019060494268[1] = -nom_x[1] + true_x[1];
   out_6312955019060494268[2] = -nom_x[2] + true_x[2];
   out_6312955019060494268[3] = -nom_x[3] + true_x[3];
   out_6312955019060494268[4] = -nom_x[4] + true_x[4];
   out_6312955019060494268[5] = -nom_x[5] + true_x[5];
   out_6312955019060494268[6] = -nom_x[6] + true_x[6];
   out_6312955019060494268[7] = -nom_x[7] + true_x[7];
   out_6312955019060494268[8] = -nom_x[8] + true_x[8];
   out_6312955019060494268[9] = -nom_x[9] + true_x[9];
   out_6312955019060494268[10] = -nom_x[10] + true_x[10];
   out_6312955019060494268[11] = -nom_x[11] + true_x[11];
   out_6312955019060494268[12] = -nom_x[12] + true_x[12];
   out_6312955019060494268[13] = -nom_x[13] + true_x[13];
   out_6312955019060494268[14] = -nom_x[14] + true_x[14];
   out_6312955019060494268[15] = -nom_x[15] + true_x[15];
   out_6312955019060494268[16] = -nom_x[16] + true_x[16];
   out_6312955019060494268[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_2002785547247954198) {
   out_2002785547247954198[0] = 1.0;
   out_2002785547247954198[1] = 0.0;
   out_2002785547247954198[2] = 0.0;
   out_2002785547247954198[3] = 0.0;
   out_2002785547247954198[4] = 0.0;
   out_2002785547247954198[5] = 0.0;
   out_2002785547247954198[6] = 0.0;
   out_2002785547247954198[7] = 0.0;
   out_2002785547247954198[8] = 0.0;
   out_2002785547247954198[9] = 0.0;
   out_2002785547247954198[10] = 0.0;
   out_2002785547247954198[11] = 0.0;
   out_2002785547247954198[12] = 0.0;
   out_2002785547247954198[13] = 0.0;
   out_2002785547247954198[14] = 0.0;
   out_2002785547247954198[15] = 0.0;
   out_2002785547247954198[16] = 0.0;
   out_2002785547247954198[17] = 0.0;
   out_2002785547247954198[18] = 0.0;
   out_2002785547247954198[19] = 1.0;
   out_2002785547247954198[20] = 0.0;
   out_2002785547247954198[21] = 0.0;
   out_2002785547247954198[22] = 0.0;
   out_2002785547247954198[23] = 0.0;
   out_2002785547247954198[24] = 0.0;
   out_2002785547247954198[25] = 0.0;
   out_2002785547247954198[26] = 0.0;
   out_2002785547247954198[27] = 0.0;
   out_2002785547247954198[28] = 0.0;
   out_2002785547247954198[29] = 0.0;
   out_2002785547247954198[30] = 0.0;
   out_2002785547247954198[31] = 0.0;
   out_2002785547247954198[32] = 0.0;
   out_2002785547247954198[33] = 0.0;
   out_2002785547247954198[34] = 0.0;
   out_2002785547247954198[35] = 0.0;
   out_2002785547247954198[36] = 0.0;
   out_2002785547247954198[37] = 0.0;
   out_2002785547247954198[38] = 1.0;
   out_2002785547247954198[39] = 0.0;
   out_2002785547247954198[40] = 0.0;
   out_2002785547247954198[41] = 0.0;
   out_2002785547247954198[42] = 0.0;
   out_2002785547247954198[43] = 0.0;
   out_2002785547247954198[44] = 0.0;
   out_2002785547247954198[45] = 0.0;
   out_2002785547247954198[46] = 0.0;
   out_2002785547247954198[47] = 0.0;
   out_2002785547247954198[48] = 0.0;
   out_2002785547247954198[49] = 0.0;
   out_2002785547247954198[50] = 0.0;
   out_2002785547247954198[51] = 0.0;
   out_2002785547247954198[52] = 0.0;
   out_2002785547247954198[53] = 0.0;
   out_2002785547247954198[54] = 0.0;
   out_2002785547247954198[55] = 0.0;
   out_2002785547247954198[56] = 0.0;
   out_2002785547247954198[57] = 1.0;
   out_2002785547247954198[58] = 0.0;
   out_2002785547247954198[59] = 0.0;
   out_2002785547247954198[60] = 0.0;
   out_2002785547247954198[61] = 0.0;
   out_2002785547247954198[62] = 0.0;
   out_2002785547247954198[63] = 0.0;
   out_2002785547247954198[64] = 0.0;
   out_2002785547247954198[65] = 0.0;
   out_2002785547247954198[66] = 0.0;
   out_2002785547247954198[67] = 0.0;
   out_2002785547247954198[68] = 0.0;
   out_2002785547247954198[69] = 0.0;
   out_2002785547247954198[70] = 0.0;
   out_2002785547247954198[71] = 0.0;
   out_2002785547247954198[72] = 0.0;
   out_2002785547247954198[73] = 0.0;
   out_2002785547247954198[74] = 0.0;
   out_2002785547247954198[75] = 0.0;
   out_2002785547247954198[76] = 1.0;
   out_2002785547247954198[77] = 0.0;
   out_2002785547247954198[78] = 0.0;
   out_2002785547247954198[79] = 0.0;
   out_2002785547247954198[80] = 0.0;
   out_2002785547247954198[81] = 0.0;
   out_2002785547247954198[82] = 0.0;
   out_2002785547247954198[83] = 0.0;
   out_2002785547247954198[84] = 0.0;
   out_2002785547247954198[85] = 0.0;
   out_2002785547247954198[86] = 0.0;
   out_2002785547247954198[87] = 0.0;
   out_2002785547247954198[88] = 0.0;
   out_2002785547247954198[89] = 0.0;
   out_2002785547247954198[90] = 0.0;
   out_2002785547247954198[91] = 0.0;
   out_2002785547247954198[92] = 0.0;
   out_2002785547247954198[93] = 0.0;
   out_2002785547247954198[94] = 0.0;
   out_2002785547247954198[95] = 1.0;
   out_2002785547247954198[96] = 0.0;
   out_2002785547247954198[97] = 0.0;
   out_2002785547247954198[98] = 0.0;
   out_2002785547247954198[99] = 0.0;
   out_2002785547247954198[100] = 0.0;
   out_2002785547247954198[101] = 0.0;
   out_2002785547247954198[102] = 0.0;
   out_2002785547247954198[103] = 0.0;
   out_2002785547247954198[104] = 0.0;
   out_2002785547247954198[105] = 0.0;
   out_2002785547247954198[106] = 0.0;
   out_2002785547247954198[107] = 0.0;
   out_2002785547247954198[108] = 0.0;
   out_2002785547247954198[109] = 0.0;
   out_2002785547247954198[110] = 0.0;
   out_2002785547247954198[111] = 0.0;
   out_2002785547247954198[112] = 0.0;
   out_2002785547247954198[113] = 0.0;
   out_2002785547247954198[114] = 1.0;
   out_2002785547247954198[115] = 0.0;
   out_2002785547247954198[116] = 0.0;
   out_2002785547247954198[117] = 0.0;
   out_2002785547247954198[118] = 0.0;
   out_2002785547247954198[119] = 0.0;
   out_2002785547247954198[120] = 0.0;
   out_2002785547247954198[121] = 0.0;
   out_2002785547247954198[122] = 0.0;
   out_2002785547247954198[123] = 0.0;
   out_2002785547247954198[124] = 0.0;
   out_2002785547247954198[125] = 0.0;
   out_2002785547247954198[126] = 0.0;
   out_2002785547247954198[127] = 0.0;
   out_2002785547247954198[128] = 0.0;
   out_2002785547247954198[129] = 0.0;
   out_2002785547247954198[130] = 0.0;
   out_2002785547247954198[131] = 0.0;
   out_2002785547247954198[132] = 0.0;
   out_2002785547247954198[133] = 1.0;
   out_2002785547247954198[134] = 0.0;
   out_2002785547247954198[135] = 0.0;
   out_2002785547247954198[136] = 0.0;
   out_2002785547247954198[137] = 0.0;
   out_2002785547247954198[138] = 0.0;
   out_2002785547247954198[139] = 0.0;
   out_2002785547247954198[140] = 0.0;
   out_2002785547247954198[141] = 0.0;
   out_2002785547247954198[142] = 0.0;
   out_2002785547247954198[143] = 0.0;
   out_2002785547247954198[144] = 0.0;
   out_2002785547247954198[145] = 0.0;
   out_2002785547247954198[146] = 0.0;
   out_2002785547247954198[147] = 0.0;
   out_2002785547247954198[148] = 0.0;
   out_2002785547247954198[149] = 0.0;
   out_2002785547247954198[150] = 0.0;
   out_2002785547247954198[151] = 0.0;
   out_2002785547247954198[152] = 1.0;
   out_2002785547247954198[153] = 0.0;
   out_2002785547247954198[154] = 0.0;
   out_2002785547247954198[155] = 0.0;
   out_2002785547247954198[156] = 0.0;
   out_2002785547247954198[157] = 0.0;
   out_2002785547247954198[158] = 0.0;
   out_2002785547247954198[159] = 0.0;
   out_2002785547247954198[160] = 0.0;
   out_2002785547247954198[161] = 0.0;
   out_2002785547247954198[162] = 0.0;
   out_2002785547247954198[163] = 0.0;
   out_2002785547247954198[164] = 0.0;
   out_2002785547247954198[165] = 0.0;
   out_2002785547247954198[166] = 0.0;
   out_2002785547247954198[167] = 0.0;
   out_2002785547247954198[168] = 0.0;
   out_2002785547247954198[169] = 0.0;
   out_2002785547247954198[170] = 0.0;
   out_2002785547247954198[171] = 1.0;
   out_2002785547247954198[172] = 0.0;
   out_2002785547247954198[173] = 0.0;
   out_2002785547247954198[174] = 0.0;
   out_2002785547247954198[175] = 0.0;
   out_2002785547247954198[176] = 0.0;
   out_2002785547247954198[177] = 0.0;
   out_2002785547247954198[178] = 0.0;
   out_2002785547247954198[179] = 0.0;
   out_2002785547247954198[180] = 0.0;
   out_2002785547247954198[181] = 0.0;
   out_2002785547247954198[182] = 0.0;
   out_2002785547247954198[183] = 0.0;
   out_2002785547247954198[184] = 0.0;
   out_2002785547247954198[185] = 0.0;
   out_2002785547247954198[186] = 0.0;
   out_2002785547247954198[187] = 0.0;
   out_2002785547247954198[188] = 0.0;
   out_2002785547247954198[189] = 0.0;
   out_2002785547247954198[190] = 1.0;
   out_2002785547247954198[191] = 0.0;
   out_2002785547247954198[192] = 0.0;
   out_2002785547247954198[193] = 0.0;
   out_2002785547247954198[194] = 0.0;
   out_2002785547247954198[195] = 0.0;
   out_2002785547247954198[196] = 0.0;
   out_2002785547247954198[197] = 0.0;
   out_2002785547247954198[198] = 0.0;
   out_2002785547247954198[199] = 0.0;
   out_2002785547247954198[200] = 0.0;
   out_2002785547247954198[201] = 0.0;
   out_2002785547247954198[202] = 0.0;
   out_2002785547247954198[203] = 0.0;
   out_2002785547247954198[204] = 0.0;
   out_2002785547247954198[205] = 0.0;
   out_2002785547247954198[206] = 0.0;
   out_2002785547247954198[207] = 0.0;
   out_2002785547247954198[208] = 0.0;
   out_2002785547247954198[209] = 1.0;
   out_2002785547247954198[210] = 0.0;
   out_2002785547247954198[211] = 0.0;
   out_2002785547247954198[212] = 0.0;
   out_2002785547247954198[213] = 0.0;
   out_2002785547247954198[214] = 0.0;
   out_2002785547247954198[215] = 0.0;
   out_2002785547247954198[216] = 0.0;
   out_2002785547247954198[217] = 0.0;
   out_2002785547247954198[218] = 0.0;
   out_2002785547247954198[219] = 0.0;
   out_2002785547247954198[220] = 0.0;
   out_2002785547247954198[221] = 0.0;
   out_2002785547247954198[222] = 0.0;
   out_2002785547247954198[223] = 0.0;
   out_2002785547247954198[224] = 0.0;
   out_2002785547247954198[225] = 0.0;
   out_2002785547247954198[226] = 0.0;
   out_2002785547247954198[227] = 0.0;
   out_2002785547247954198[228] = 1.0;
   out_2002785547247954198[229] = 0.0;
   out_2002785547247954198[230] = 0.0;
   out_2002785547247954198[231] = 0.0;
   out_2002785547247954198[232] = 0.0;
   out_2002785547247954198[233] = 0.0;
   out_2002785547247954198[234] = 0.0;
   out_2002785547247954198[235] = 0.0;
   out_2002785547247954198[236] = 0.0;
   out_2002785547247954198[237] = 0.0;
   out_2002785547247954198[238] = 0.0;
   out_2002785547247954198[239] = 0.0;
   out_2002785547247954198[240] = 0.0;
   out_2002785547247954198[241] = 0.0;
   out_2002785547247954198[242] = 0.0;
   out_2002785547247954198[243] = 0.0;
   out_2002785547247954198[244] = 0.0;
   out_2002785547247954198[245] = 0.0;
   out_2002785547247954198[246] = 0.0;
   out_2002785547247954198[247] = 1.0;
   out_2002785547247954198[248] = 0.0;
   out_2002785547247954198[249] = 0.0;
   out_2002785547247954198[250] = 0.0;
   out_2002785547247954198[251] = 0.0;
   out_2002785547247954198[252] = 0.0;
   out_2002785547247954198[253] = 0.0;
   out_2002785547247954198[254] = 0.0;
   out_2002785547247954198[255] = 0.0;
   out_2002785547247954198[256] = 0.0;
   out_2002785547247954198[257] = 0.0;
   out_2002785547247954198[258] = 0.0;
   out_2002785547247954198[259] = 0.0;
   out_2002785547247954198[260] = 0.0;
   out_2002785547247954198[261] = 0.0;
   out_2002785547247954198[262] = 0.0;
   out_2002785547247954198[263] = 0.0;
   out_2002785547247954198[264] = 0.0;
   out_2002785547247954198[265] = 0.0;
   out_2002785547247954198[266] = 1.0;
   out_2002785547247954198[267] = 0.0;
   out_2002785547247954198[268] = 0.0;
   out_2002785547247954198[269] = 0.0;
   out_2002785547247954198[270] = 0.0;
   out_2002785547247954198[271] = 0.0;
   out_2002785547247954198[272] = 0.0;
   out_2002785547247954198[273] = 0.0;
   out_2002785547247954198[274] = 0.0;
   out_2002785547247954198[275] = 0.0;
   out_2002785547247954198[276] = 0.0;
   out_2002785547247954198[277] = 0.0;
   out_2002785547247954198[278] = 0.0;
   out_2002785547247954198[279] = 0.0;
   out_2002785547247954198[280] = 0.0;
   out_2002785547247954198[281] = 0.0;
   out_2002785547247954198[282] = 0.0;
   out_2002785547247954198[283] = 0.0;
   out_2002785547247954198[284] = 0.0;
   out_2002785547247954198[285] = 1.0;
   out_2002785547247954198[286] = 0.0;
   out_2002785547247954198[287] = 0.0;
   out_2002785547247954198[288] = 0.0;
   out_2002785547247954198[289] = 0.0;
   out_2002785547247954198[290] = 0.0;
   out_2002785547247954198[291] = 0.0;
   out_2002785547247954198[292] = 0.0;
   out_2002785547247954198[293] = 0.0;
   out_2002785547247954198[294] = 0.0;
   out_2002785547247954198[295] = 0.0;
   out_2002785547247954198[296] = 0.0;
   out_2002785547247954198[297] = 0.0;
   out_2002785547247954198[298] = 0.0;
   out_2002785547247954198[299] = 0.0;
   out_2002785547247954198[300] = 0.0;
   out_2002785547247954198[301] = 0.0;
   out_2002785547247954198[302] = 0.0;
   out_2002785547247954198[303] = 0.0;
   out_2002785547247954198[304] = 1.0;
   out_2002785547247954198[305] = 0.0;
   out_2002785547247954198[306] = 0.0;
   out_2002785547247954198[307] = 0.0;
   out_2002785547247954198[308] = 0.0;
   out_2002785547247954198[309] = 0.0;
   out_2002785547247954198[310] = 0.0;
   out_2002785547247954198[311] = 0.0;
   out_2002785547247954198[312] = 0.0;
   out_2002785547247954198[313] = 0.0;
   out_2002785547247954198[314] = 0.0;
   out_2002785547247954198[315] = 0.0;
   out_2002785547247954198[316] = 0.0;
   out_2002785547247954198[317] = 0.0;
   out_2002785547247954198[318] = 0.0;
   out_2002785547247954198[319] = 0.0;
   out_2002785547247954198[320] = 0.0;
   out_2002785547247954198[321] = 0.0;
   out_2002785547247954198[322] = 0.0;
   out_2002785547247954198[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_7460364811199515324) {
   out_7460364811199515324[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_7460364811199515324[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_7460364811199515324[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_7460364811199515324[3] = dt*state[12] + state[3];
   out_7460364811199515324[4] = dt*state[13] + state[4];
   out_7460364811199515324[5] = dt*state[14] + state[5];
   out_7460364811199515324[6] = state[6];
   out_7460364811199515324[7] = state[7];
   out_7460364811199515324[8] = state[8];
   out_7460364811199515324[9] = state[9];
   out_7460364811199515324[10] = state[10];
   out_7460364811199515324[11] = state[11];
   out_7460364811199515324[12] = state[12];
   out_7460364811199515324[13] = state[13];
   out_7460364811199515324[14] = state[14];
   out_7460364811199515324[15] = state[15];
   out_7460364811199515324[16] = state[16];
   out_7460364811199515324[17] = state[17];
}
void F_fun(double *state, double dt, double *out_3737951720851394665) {
   out_3737951720851394665[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_3737951720851394665[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_3737951720851394665[2] = 0;
   out_3737951720851394665[3] = 0;
   out_3737951720851394665[4] = 0;
   out_3737951720851394665[5] = 0;
   out_3737951720851394665[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_3737951720851394665[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_3737951720851394665[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_3737951720851394665[9] = 0;
   out_3737951720851394665[10] = 0;
   out_3737951720851394665[11] = 0;
   out_3737951720851394665[12] = 0;
   out_3737951720851394665[13] = 0;
   out_3737951720851394665[14] = 0;
   out_3737951720851394665[15] = 0;
   out_3737951720851394665[16] = 0;
   out_3737951720851394665[17] = 0;
   out_3737951720851394665[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_3737951720851394665[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_3737951720851394665[20] = 0;
   out_3737951720851394665[21] = 0;
   out_3737951720851394665[22] = 0;
   out_3737951720851394665[23] = 0;
   out_3737951720851394665[24] = 0;
   out_3737951720851394665[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_3737951720851394665[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_3737951720851394665[27] = 0;
   out_3737951720851394665[28] = 0;
   out_3737951720851394665[29] = 0;
   out_3737951720851394665[30] = 0;
   out_3737951720851394665[31] = 0;
   out_3737951720851394665[32] = 0;
   out_3737951720851394665[33] = 0;
   out_3737951720851394665[34] = 0;
   out_3737951720851394665[35] = 0;
   out_3737951720851394665[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_3737951720851394665[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_3737951720851394665[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_3737951720851394665[39] = 0;
   out_3737951720851394665[40] = 0;
   out_3737951720851394665[41] = 0;
   out_3737951720851394665[42] = 0;
   out_3737951720851394665[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_3737951720851394665[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_3737951720851394665[45] = 0;
   out_3737951720851394665[46] = 0;
   out_3737951720851394665[47] = 0;
   out_3737951720851394665[48] = 0;
   out_3737951720851394665[49] = 0;
   out_3737951720851394665[50] = 0;
   out_3737951720851394665[51] = 0;
   out_3737951720851394665[52] = 0;
   out_3737951720851394665[53] = 0;
   out_3737951720851394665[54] = 0;
   out_3737951720851394665[55] = 0;
   out_3737951720851394665[56] = 0;
   out_3737951720851394665[57] = 1;
   out_3737951720851394665[58] = 0;
   out_3737951720851394665[59] = 0;
   out_3737951720851394665[60] = 0;
   out_3737951720851394665[61] = 0;
   out_3737951720851394665[62] = 0;
   out_3737951720851394665[63] = 0;
   out_3737951720851394665[64] = 0;
   out_3737951720851394665[65] = 0;
   out_3737951720851394665[66] = dt;
   out_3737951720851394665[67] = 0;
   out_3737951720851394665[68] = 0;
   out_3737951720851394665[69] = 0;
   out_3737951720851394665[70] = 0;
   out_3737951720851394665[71] = 0;
   out_3737951720851394665[72] = 0;
   out_3737951720851394665[73] = 0;
   out_3737951720851394665[74] = 0;
   out_3737951720851394665[75] = 0;
   out_3737951720851394665[76] = 1;
   out_3737951720851394665[77] = 0;
   out_3737951720851394665[78] = 0;
   out_3737951720851394665[79] = 0;
   out_3737951720851394665[80] = 0;
   out_3737951720851394665[81] = 0;
   out_3737951720851394665[82] = 0;
   out_3737951720851394665[83] = 0;
   out_3737951720851394665[84] = 0;
   out_3737951720851394665[85] = dt;
   out_3737951720851394665[86] = 0;
   out_3737951720851394665[87] = 0;
   out_3737951720851394665[88] = 0;
   out_3737951720851394665[89] = 0;
   out_3737951720851394665[90] = 0;
   out_3737951720851394665[91] = 0;
   out_3737951720851394665[92] = 0;
   out_3737951720851394665[93] = 0;
   out_3737951720851394665[94] = 0;
   out_3737951720851394665[95] = 1;
   out_3737951720851394665[96] = 0;
   out_3737951720851394665[97] = 0;
   out_3737951720851394665[98] = 0;
   out_3737951720851394665[99] = 0;
   out_3737951720851394665[100] = 0;
   out_3737951720851394665[101] = 0;
   out_3737951720851394665[102] = 0;
   out_3737951720851394665[103] = 0;
   out_3737951720851394665[104] = dt;
   out_3737951720851394665[105] = 0;
   out_3737951720851394665[106] = 0;
   out_3737951720851394665[107] = 0;
   out_3737951720851394665[108] = 0;
   out_3737951720851394665[109] = 0;
   out_3737951720851394665[110] = 0;
   out_3737951720851394665[111] = 0;
   out_3737951720851394665[112] = 0;
   out_3737951720851394665[113] = 0;
   out_3737951720851394665[114] = 1;
   out_3737951720851394665[115] = 0;
   out_3737951720851394665[116] = 0;
   out_3737951720851394665[117] = 0;
   out_3737951720851394665[118] = 0;
   out_3737951720851394665[119] = 0;
   out_3737951720851394665[120] = 0;
   out_3737951720851394665[121] = 0;
   out_3737951720851394665[122] = 0;
   out_3737951720851394665[123] = 0;
   out_3737951720851394665[124] = 0;
   out_3737951720851394665[125] = 0;
   out_3737951720851394665[126] = 0;
   out_3737951720851394665[127] = 0;
   out_3737951720851394665[128] = 0;
   out_3737951720851394665[129] = 0;
   out_3737951720851394665[130] = 0;
   out_3737951720851394665[131] = 0;
   out_3737951720851394665[132] = 0;
   out_3737951720851394665[133] = 1;
   out_3737951720851394665[134] = 0;
   out_3737951720851394665[135] = 0;
   out_3737951720851394665[136] = 0;
   out_3737951720851394665[137] = 0;
   out_3737951720851394665[138] = 0;
   out_3737951720851394665[139] = 0;
   out_3737951720851394665[140] = 0;
   out_3737951720851394665[141] = 0;
   out_3737951720851394665[142] = 0;
   out_3737951720851394665[143] = 0;
   out_3737951720851394665[144] = 0;
   out_3737951720851394665[145] = 0;
   out_3737951720851394665[146] = 0;
   out_3737951720851394665[147] = 0;
   out_3737951720851394665[148] = 0;
   out_3737951720851394665[149] = 0;
   out_3737951720851394665[150] = 0;
   out_3737951720851394665[151] = 0;
   out_3737951720851394665[152] = 1;
   out_3737951720851394665[153] = 0;
   out_3737951720851394665[154] = 0;
   out_3737951720851394665[155] = 0;
   out_3737951720851394665[156] = 0;
   out_3737951720851394665[157] = 0;
   out_3737951720851394665[158] = 0;
   out_3737951720851394665[159] = 0;
   out_3737951720851394665[160] = 0;
   out_3737951720851394665[161] = 0;
   out_3737951720851394665[162] = 0;
   out_3737951720851394665[163] = 0;
   out_3737951720851394665[164] = 0;
   out_3737951720851394665[165] = 0;
   out_3737951720851394665[166] = 0;
   out_3737951720851394665[167] = 0;
   out_3737951720851394665[168] = 0;
   out_3737951720851394665[169] = 0;
   out_3737951720851394665[170] = 0;
   out_3737951720851394665[171] = 1;
   out_3737951720851394665[172] = 0;
   out_3737951720851394665[173] = 0;
   out_3737951720851394665[174] = 0;
   out_3737951720851394665[175] = 0;
   out_3737951720851394665[176] = 0;
   out_3737951720851394665[177] = 0;
   out_3737951720851394665[178] = 0;
   out_3737951720851394665[179] = 0;
   out_3737951720851394665[180] = 0;
   out_3737951720851394665[181] = 0;
   out_3737951720851394665[182] = 0;
   out_3737951720851394665[183] = 0;
   out_3737951720851394665[184] = 0;
   out_3737951720851394665[185] = 0;
   out_3737951720851394665[186] = 0;
   out_3737951720851394665[187] = 0;
   out_3737951720851394665[188] = 0;
   out_3737951720851394665[189] = 0;
   out_3737951720851394665[190] = 1;
   out_3737951720851394665[191] = 0;
   out_3737951720851394665[192] = 0;
   out_3737951720851394665[193] = 0;
   out_3737951720851394665[194] = 0;
   out_3737951720851394665[195] = 0;
   out_3737951720851394665[196] = 0;
   out_3737951720851394665[197] = 0;
   out_3737951720851394665[198] = 0;
   out_3737951720851394665[199] = 0;
   out_3737951720851394665[200] = 0;
   out_3737951720851394665[201] = 0;
   out_3737951720851394665[202] = 0;
   out_3737951720851394665[203] = 0;
   out_3737951720851394665[204] = 0;
   out_3737951720851394665[205] = 0;
   out_3737951720851394665[206] = 0;
   out_3737951720851394665[207] = 0;
   out_3737951720851394665[208] = 0;
   out_3737951720851394665[209] = 1;
   out_3737951720851394665[210] = 0;
   out_3737951720851394665[211] = 0;
   out_3737951720851394665[212] = 0;
   out_3737951720851394665[213] = 0;
   out_3737951720851394665[214] = 0;
   out_3737951720851394665[215] = 0;
   out_3737951720851394665[216] = 0;
   out_3737951720851394665[217] = 0;
   out_3737951720851394665[218] = 0;
   out_3737951720851394665[219] = 0;
   out_3737951720851394665[220] = 0;
   out_3737951720851394665[221] = 0;
   out_3737951720851394665[222] = 0;
   out_3737951720851394665[223] = 0;
   out_3737951720851394665[224] = 0;
   out_3737951720851394665[225] = 0;
   out_3737951720851394665[226] = 0;
   out_3737951720851394665[227] = 0;
   out_3737951720851394665[228] = 1;
   out_3737951720851394665[229] = 0;
   out_3737951720851394665[230] = 0;
   out_3737951720851394665[231] = 0;
   out_3737951720851394665[232] = 0;
   out_3737951720851394665[233] = 0;
   out_3737951720851394665[234] = 0;
   out_3737951720851394665[235] = 0;
   out_3737951720851394665[236] = 0;
   out_3737951720851394665[237] = 0;
   out_3737951720851394665[238] = 0;
   out_3737951720851394665[239] = 0;
   out_3737951720851394665[240] = 0;
   out_3737951720851394665[241] = 0;
   out_3737951720851394665[242] = 0;
   out_3737951720851394665[243] = 0;
   out_3737951720851394665[244] = 0;
   out_3737951720851394665[245] = 0;
   out_3737951720851394665[246] = 0;
   out_3737951720851394665[247] = 1;
   out_3737951720851394665[248] = 0;
   out_3737951720851394665[249] = 0;
   out_3737951720851394665[250] = 0;
   out_3737951720851394665[251] = 0;
   out_3737951720851394665[252] = 0;
   out_3737951720851394665[253] = 0;
   out_3737951720851394665[254] = 0;
   out_3737951720851394665[255] = 0;
   out_3737951720851394665[256] = 0;
   out_3737951720851394665[257] = 0;
   out_3737951720851394665[258] = 0;
   out_3737951720851394665[259] = 0;
   out_3737951720851394665[260] = 0;
   out_3737951720851394665[261] = 0;
   out_3737951720851394665[262] = 0;
   out_3737951720851394665[263] = 0;
   out_3737951720851394665[264] = 0;
   out_3737951720851394665[265] = 0;
   out_3737951720851394665[266] = 1;
   out_3737951720851394665[267] = 0;
   out_3737951720851394665[268] = 0;
   out_3737951720851394665[269] = 0;
   out_3737951720851394665[270] = 0;
   out_3737951720851394665[271] = 0;
   out_3737951720851394665[272] = 0;
   out_3737951720851394665[273] = 0;
   out_3737951720851394665[274] = 0;
   out_3737951720851394665[275] = 0;
   out_3737951720851394665[276] = 0;
   out_3737951720851394665[277] = 0;
   out_3737951720851394665[278] = 0;
   out_3737951720851394665[279] = 0;
   out_3737951720851394665[280] = 0;
   out_3737951720851394665[281] = 0;
   out_3737951720851394665[282] = 0;
   out_3737951720851394665[283] = 0;
   out_3737951720851394665[284] = 0;
   out_3737951720851394665[285] = 1;
   out_3737951720851394665[286] = 0;
   out_3737951720851394665[287] = 0;
   out_3737951720851394665[288] = 0;
   out_3737951720851394665[289] = 0;
   out_3737951720851394665[290] = 0;
   out_3737951720851394665[291] = 0;
   out_3737951720851394665[292] = 0;
   out_3737951720851394665[293] = 0;
   out_3737951720851394665[294] = 0;
   out_3737951720851394665[295] = 0;
   out_3737951720851394665[296] = 0;
   out_3737951720851394665[297] = 0;
   out_3737951720851394665[298] = 0;
   out_3737951720851394665[299] = 0;
   out_3737951720851394665[300] = 0;
   out_3737951720851394665[301] = 0;
   out_3737951720851394665[302] = 0;
   out_3737951720851394665[303] = 0;
   out_3737951720851394665[304] = 1;
   out_3737951720851394665[305] = 0;
   out_3737951720851394665[306] = 0;
   out_3737951720851394665[307] = 0;
   out_3737951720851394665[308] = 0;
   out_3737951720851394665[309] = 0;
   out_3737951720851394665[310] = 0;
   out_3737951720851394665[311] = 0;
   out_3737951720851394665[312] = 0;
   out_3737951720851394665[313] = 0;
   out_3737951720851394665[314] = 0;
   out_3737951720851394665[315] = 0;
   out_3737951720851394665[316] = 0;
   out_3737951720851394665[317] = 0;
   out_3737951720851394665[318] = 0;
   out_3737951720851394665[319] = 0;
   out_3737951720851394665[320] = 0;
   out_3737951720851394665[321] = 0;
   out_3737951720851394665[322] = 0;
   out_3737951720851394665[323] = 1;
}
void h_4(double *state, double *unused, double *out_8295372454373326831) {
   out_8295372454373326831[0] = state[6] + state[9];
   out_8295372454373326831[1] = state[7] + state[10];
   out_8295372454373326831[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_183086821341657337) {
   out_183086821341657337[0] = 0;
   out_183086821341657337[1] = 0;
   out_183086821341657337[2] = 0;
   out_183086821341657337[3] = 0;
   out_183086821341657337[4] = 0;
   out_183086821341657337[5] = 0;
   out_183086821341657337[6] = 1;
   out_183086821341657337[7] = 0;
   out_183086821341657337[8] = 0;
   out_183086821341657337[9] = 1;
   out_183086821341657337[10] = 0;
   out_183086821341657337[11] = 0;
   out_183086821341657337[12] = 0;
   out_183086821341657337[13] = 0;
   out_183086821341657337[14] = 0;
   out_183086821341657337[15] = 0;
   out_183086821341657337[16] = 0;
   out_183086821341657337[17] = 0;
   out_183086821341657337[18] = 0;
   out_183086821341657337[19] = 0;
   out_183086821341657337[20] = 0;
   out_183086821341657337[21] = 0;
   out_183086821341657337[22] = 0;
   out_183086821341657337[23] = 0;
   out_183086821341657337[24] = 0;
   out_183086821341657337[25] = 1;
   out_183086821341657337[26] = 0;
   out_183086821341657337[27] = 0;
   out_183086821341657337[28] = 1;
   out_183086821341657337[29] = 0;
   out_183086821341657337[30] = 0;
   out_183086821341657337[31] = 0;
   out_183086821341657337[32] = 0;
   out_183086821341657337[33] = 0;
   out_183086821341657337[34] = 0;
   out_183086821341657337[35] = 0;
   out_183086821341657337[36] = 0;
   out_183086821341657337[37] = 0;
   out_183086821341657337[38] = 0;
   out_183086821341657337[39] = 0;
   out_183086821341657337[40] = 0;
   out_183086821341657337[41] = 0;
   out_183086821341657337[42] = 0;
   out_183086821341657337[43] = 0;
   out_183086821341657337[44] = 1;
   out_183086821341657337[45] = 0;
   out_183086821341657337[46] = 0;
   out_183086821341657337[47] = 1;
   out_183086821341657337[48] = 0;
   out_183086821341657337[49] = 0;
   out_183086821341657337[50] = 0;
   out_183086821341657337[51] = 0;
   out_183086821341657337[52] = 0;
   out_183086821341657337[53] = 0;
}
void h_10(double *state, double *unused, double *out_7644256498264067236) {
   out_7644256498264067236[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_7644256498264067236[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_7644256498264067236[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_9222592977772692444) {
   out_9222592977772692444[0] = 0;
   out_9222592977772692444[1] = 9.8100000000000005*cos(state[1]);
   out_9222592977772692444[2] = 0;
   out_9222592977772692444[3] = 0;
   out_9222592977772692444[4] = -state[8];
   out_9222592977772692444[5] = state[7];
   out_9222592977772692444[6] = 0;
   out_9222592977772692444[7] = state[5];
   out_9222592977772692444[8] = -state[4];
   out_9222592977772692444[9] = 0;
   out_9222592977772692444[10] = 0;
   out_9222592977772692444[11] = 0;
   out_9222592977772692444[12] = 1;
   out_9222592977772692444[13] = 0;
   out_9222592977772692444[14] = 0;
   out_9222592977772692444[15] = 1;
   out_9222592977772692444[16] = 0;
   out_9222592977772692444[17] = 0;
   out_9222592977772692444[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_9222592977772692444[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_9222592977772692444[20] = 0;
   out_9222592977772692444[21] = state[8];
   out_9222592977772692444[22] = 0;
   out_9222592977772692444[23] = -state[6];
   out_9222592977772692444[24] = -state[5];
   out_9222592977772692444[25] = 0;
   out_9222592977772692444[26] = state[3];
   out_9222592977772692444[27] = 0;
   out_9222592977772692444[28] = 0;
   out_9222592977772692444[29] = 0;
   out_9222592977772692444[30] = 0;
   out_9222592977772692444[31] = 1;
   out_9222592977772692444[32] = 0;
   out_9222592977772692444[33] = 0;
   out_9222592977772692444[34] = 1;
   out_9222592977772692444[35] = 0;
   out_9222592977772692444[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_9222592977772692444[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_9222592977772692444[38] = 0;
   out_9222592977772692444[39] = -state[7];
   out_9222592977772692444[40] = state[6];
   out_9222592977772692444[41] = 0;
   out_9222592977772692444[42] = state[4];
   out_9222592977772692444[43] = -state[3];
   out_9222592977772692444[44] = 0;
   out_9222592977772692444[45] = 0;
   out_9222592977772692444[46] = 0;
   out_9222592977772692444[47] = 0;
   out_9222592977772692444[48] = 0;
   out_9222592977772692444[49] = 0;
   out_9222592977772692444[50] = 1;
   out_9222592977772692444[51] = 0;
   out_9222592977772692444[52] = 0;
   out_9222592977772692444[53] = 1;
}
void h_13(double *state, double *unused, double *out_2771420024160106737) {
   out_2771420024160106737[0] = state[3];
   out_2771420024160106737[1] = state[4];
   out_2771420024160106737[2] = state[5];
}
void H_13(double *state, double *unused, double *out_3650668641960866687) {
   out_3650668641960866687[0] = 0;
   out_3650668641960866687[1] = 0;
   out_3650668641960866687[2] = 0;
   out_3650668641960866687[3] = 1;
   out_3650668641960866687[4] = 0;
   out_3650668641960866687[5] = 0;
   out_3650668641960866687[6] = 0;
   out_3650668641960866687[7] = 0;
   out_3650668641960866687[8] = 0;
   out_3650668641960866687[9] = 0;
   out_3650668641960866687[10] = 0;
   out_3650668641960866687[11] = 0;
   out_3650668641960866687[12] = 0;
   out_3650668641960866687[13] = 0;
   out_3650668641960866687[14] = 0;
   out_3650668641960866687[15] = 0;
   out_3650668641960866687[16] = 0;
   out_3650668641960866687[17] = 0;
   out_3650668641960866687[18] = 0;
   out_3650668641960866687[19] = 0;
   out_3650668641960866687[20] = 0;
   out_3650668641960866687[21] = 0;
   out_3650668641960866687[22] = 1;
   out_3650668641960866687[23] = 0;
   out_3650668641960866687[24] = 0;
   out_3650668641960866687[25] = 0;
   out_3650668641960866687[26] = 0;
   out_3650668641960866687[27] = 0;
   out_3650668641960866687[28] = 0;
   out_3650668641960866687[29] = 0;
   out_3650668641960866687[30] = 0;
   out_3650668641960866687[31] = 0;
   out_3650668641960866687[32] = 0;
   out_3650668641960866687[33] = 0;
   out_3650668641960866687[34] = 0;
   out_3650668641960866687[35] = 0;
   out_3650668641960866687[36] = 0;
   out_3650668641960866687[37] = 0;
   out_3650668641960866687[38] = 0;
   out_3650668641960866687[39] = 0;
   out_3650668641960866687[40] = 0;
   out_3650668641960866687[41] = 1;
   out_3650668641960866687[42] = 0;
   out_3650668641960866687[43] = 0;
   out_3650668641960866687[44] = 0;
   out_3650668641960866687[45] = 0;
   out_3650668641960866687[46] = 0;
   out_3650668641960866687[47] = 0;
   out_3650668641960866687[48] = 0;
   out_3650668641960866687[49] = 0;
   out_3650668641960866687[50] = 0;
   out_3650668641960866687[51] = 0;
   out_3650668641960866687[52] = 0;
   out_3650668641960866687[53] = 0;
}
void h_14(double *state, double *unused, double *out_352453830606757886) {
   out_352453830606757886[0] = state[6];
   out_352453830606757886[1] = state[7];
   out_352453830606757886[2] = state[8];
}
void H_14(double *state, double *unused, double *out_2899701610953714959) {
   out_2899701610953714959[0] = 0;
   out_2899701610953714959[1] = 0;
   out_2899701610953714959[2] = 0;
   out_2899701610953714959[3] = 0;
   out_2899701610953714959[4] = 0;
   out_2899701610953714959[5] = 0;
   out_2899701610953714959[6] = 1;
   out_2899701610953714959[7] = 0;
   out_2899701610953714959[8] = 0;
   out_2899701610953714959[9] = 0;
   out_2899701610953714959[10] = 0;
   out_2899701610953714959[11] = 0;
   out_2899701610953714959[12] = 0;
   out_2899701610953714959[13] = 0;
   out_2899701610953714959[14] = 0;
   out_2899701610953714959[15] = 0;
   out_2899701610953714959[16] = 0;
   out_2899701610953714959[17] = 0;
   out_2899701610953714959[18] = 0;
   out_2899701610953714959[19] = 0;
   out_2899701610953714959[20] = 0;
   out_2899701610953714959[21] = 0;
   out_2899701610953714959[22] = 0;
   out_2899701610953714959[23] = 0;
   out_2899701610953714959[24] = 0;
   out_2899701610953714959[25] = 1;
   out_2899701610953714959[26] = 0;
   out_2899701610953714959[27] = 0;
   out_2899701610953714959[28] = 0;
   out_2899701610953714959[29] = 0;
   out_2899701610953714959[30] = 0;
   out_2899701610953714959[31] = 0;
   out_2899701610953714959[32] = 0;
   out_2899701610953714959[33] = 0;
   out_2899701610953714959[34] = 0;
   out_2899701610953714959[35] = 0;
   out_2899701610953714959[36] = 0;
   out_2899701610953714959[37] = 0;
   out_2899701610953714959[38] = 0;
   out_2899701610953714959[39] = 0;
   out_2899701610953714959[40] = 0;
   out_2899701610953714959[41] = 0;
   out_2899701610953714959[42] = 0;
   out_2899701610953714959[43] = 0;
   out_2899701610953714959[44] = 1;
   out_2899701610953714959[45] = 0;
   out_2899701610953714959[46] = 0;
   out_2899701610953714959[47] = 0;
   out_2899701610953714959[48] = 0;
   out_2899701610953714959[49] = 0;
   out_2899701610953714959[50] = 0;
   out_2899701610953714959[51] = 0;
   out_2899701610953714959[52] = 0;
   out_2899701610953714959[53] = 0;
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
void pose_err_fun(double *nom_x, double *delta_x, double *out_1557287137713055710) {
  err_fun(nom_x, delta_x, out_1557287137713055710);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_6312955019060494268) {
  inv_err_fun(nom_x, true_x, out_6312955019060494268);
}
void pose_H_mod_fun(double *state, double *out_2002785547247954198) {
  H_mod_fun(state, out_2002785547247954198);
}
void pose_f_fun(double *state, double dt, double *out_7460364811199515324) {
  f_fun(state,  dt, out_7460364811199515324);
}
void pose_F_fun(double *state, double dt, double *out_3737951720851394665) {
  F_fun(state,  dt, out_3737951720851394665);
}
void pose_h_4(double *state, double *unused, double *out_8295372454373326831) {
  h_4(state, unused, out_8295372454373326831);
}
void pose_H_4(double *state, double *unused, double *out_183086821341657337) {
  H_4(state, unused, out_183086821341657337);
}
void pose_h_10(double *state, double *unused, double *out_7644256498264067236) {
  h_10(state, unused, out_7644256498264067236);
}
void pose_H_10(double *state, double *unused, double *out_9222592977772692444) {
  H_10(state, unused, out_9222592977772692444);
}
void pose_h_13(double *state, double *unused, double *out_2771420024160106737) {
  h_13(state, unused, out_2771420024160106737);
}
void pose_H_13(double *state, double *unused, double *out_3650668641960866687) {
  H_13(state, unused, out_3650668641960866687);
}
void pose_h_14(double *state, double *unused, double *out_352453830606757886) {
  h_14(state, unused, out_352453830606757886);
}
void pose_H_14(double *state, double *unused, double *out_2899701610953714959) {
  H_14(state, unused, out_2899701610953714959);
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
