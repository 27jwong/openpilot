#include "car.h"

namespace {
#define DIM 9
#define EDIM 9
#define MEDIM 9
typedef void (*Hfun)(double *, double *, double *);

double mass;

void set_mass(double x){ mass = x;}

double rotational_inertia;

void set_rotational_inertia(double x){ rotational_inertia = x;}

double center_to_front;

void set_center_to_front(double x){ center_to_front = x;}

double center_to_rear;

void set_center_to_rear(double x){ center_to_rear = x;}

double stiffness_front;

void set_stiffness_front(double x){ stiffness_front = x;}

double stiffness_rear;

void set_stiffness_rear(double x){ stiffness_rear = x;}
const static double MAHA_THRESH_25 = 3.8414588206941227;
const static double MAHA_THRESH_24 = 5.991464547107981;
const static double MAHA_THRESH_30 = 3.8414588206941227;
const static double MAHA_THRESH_26 = 3.8414588206941227;
const static double MAHA_THRESH_27 = 3.8414588206941227;
const static double MAHA_THRESH_29 = 3.8414588206941227;
const static double MAHA_THRESH_28 = 3.8414588206941227;
const static double MAHA_THRESH_31 = 3.8414588206941227;

/******************************************************************************
 *                      Code generated with SymPy 1.14.0                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_1207077157841363728) {
   out_1207077157841363728[0] = delta_x[0] + nom_x[0];
   out_1207077157841363728[1] = delta_x[1] + nom_x[1];
   out_1207077157841363728[2] = delta_x[2] + nom_x[2];
   out_1207077157841363728[3] = delta_x[3] + nom_x[3];
   out_1207077157841363728[4] = delta_x[4] + nom_x[4];
   out_1207077157841363728[5] = delta_x[5] + nom_x[5];
   out_1207077157841363728[6] = delta_x[6] + nom_x[6];
   out_1207077157841363728[7] = delta_x[7] + nom_x[7];
   out_1207077157841363728[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_5167403064917713258) {
   out_5167403064917713258[0] = -nom_x[0] + true_x[0];
   out_5167403064917713258[1] = -nom_x[1] + true_x[1];
   out_5167403064917713258[2] = -nom_x[2] + true_x[2];
   out_5167403064917713258[3] = -nom_x[3] + true_x[3];
   out_5167403064917713258[4] = -nom_x[4] + true_x[4];
   out_5167403064917713258[5] = -nom_x[5] + true_x[5];
   out_5167403064917713258[6] = -nom_x[6] + true_x[6];
   out_5167403064917713258[7] = -nom_x[7] + true_x[7];
   out_5167403064917713258[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_5732271557167151234) {
   out_5732271557167151234[0] = 1.0;
   out_5732271557167151234[1] = 0.0;
   out_5732271557167151234[2] = 0.0;
   out_5732271557167151234[3] = 0.0;
   out_5732271557167151234[4] = 0.0;
   out_5732271557167151234[5] = 0.0;
   out_5732271557167151234[6] = 0.0;
   out_5732271557167151234[7] = 0.0;
   out_5732271557167151234[8] = 0.0;
   out_5732271557167151234[9] = 0.0;
   out_5732271557167151234[10] = 1.0;
   out_5732271557167151234[11] = 0.0;
   out_5732271557167151234[12] = 0.0;
   out_5732271557167151234[13] = 0.0;
   out_5732271557167151234[14] = 0.0;
   out_5732271557167151234[15] = 0.0;
   out_5732271557167151234[16] = 0.0;
   out_5732271557167151234[17] = 0.0;
   out_5732271557167151234[18] = 0.0;
   out_5732271557167151234[19] = 0.0;
   out_5732271557167151234[20] = 1.0;
   out_5732271557167151234[21] = 0.0;
   out_5732271557167151234[22] = 0.0;
   out_5732271557167151234[23] = 0.0;
   out_5732271557167151234[24] = 0.0;
   out_5732271557167151234[25] = 0.0;
   out_5732271557167151234[26] = 0.0;
   out_5732271557167151234[27] = 0.0;
   out_5732271557167151234[28] = 0.0;
   out_5732271557167151234[29] = 0.0;
   out_5732271557167151234[30] = 1.0;
   out_5732271557167151234[31] = 0.0;
   out_5732271557167151234[32] = 0.0;
   out_5732271557167151234[33] = 0.0;
   out_5732271557167151234[34] = 0.0;
   out_5732271557167151234[35] = 0.0;
   out_5732271557167151234[36] = 0.0;
   out_5732271557167151234[37] = 0.0;
   out_5732271557167151234[38] = 0.0;
   out_5732271557167151234[39] = 0.0;
   out_5732271557167151234[40] = 1.0;
   out_5732271557167151234[41] = 0.0;
   out_5732271557167151234[42] = 0.0;
   out_5732271557167151234[43] = 0.0;
   out_5732271557167151234[44] = 0.0;
   out_5732271557167151234[45] = 0.0;
   out_5732271557167151234[46] = 0.0;
   out_5732271557167151234[47] = 0.0;
   out_5732271557167151234[48] = 0.0;
   out_5732271557167151234[49] = 0.0;
   out_5732271557167151234[50] = 1.0;
   out_5732271557167151234[51] = 0.0;
   out_5732271557167151234[52] = 0.0;
   out_5732271557167151234[53] = 0.0;
   out_5732271557167151234[54] = 0.0;
   out_5732271557167151234[55] = 0.0;
   out_5732271557167151234[56] = 0.0;
   out_5732271557167151234[57] = 0.0;
   out_5732271557167151234[58] = 0.0;
   out_5732271557167151234[59] = 0.0;
   out_5732271557167151234[60] = 1.0;
   out_5732271557167151234[61] = 0.0;
   out_5732271557167151234[62] = 0.0;
   out_5732271557167151234[63] = 0.0;
   out_5732271557167151234[64] = 0.0;
   out_5732271557167151234[65] = 0.0;
   out_5732271557167151234[66] = 0.0;
   out_5732271557167151234[67] = 0.0;
   out_5732271557167151234[68] = 0.0;
   out_5732271557167151234[69] = 0.0;
   out_5732271557167151234[70] = 1.0;
   out_5732271557167151234[71] = 0.0;
   out_5732271557167151234[72] = 0.0;
   out_5732271557167151234[73] = 0.0;
   out_5732271557167151234[74] = 0.0;
   out_5732271557167151234[75] = 0.0;
   out_5732271557167151234[76] = 0.0;
   out_5732271557167151234[77] = 0.0;
   out_5732271557167151234[78] = 0.0;
   out_5732271557167151234[79] = 0.0;
   out_5732271557167151234[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_4596155381636715650) {
   out_4596155381636715650[0] = state[0];
   out_4596155381636715650[1] = state[1];
   out_4596155381636715650[2] = state[2];
   out_4596155381636715650[3] = state[3];
   out_4596155381636715650[4] = state[4];
   out_4596155381636715650[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8100000000000005*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_4596155381636715650[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_4596155381636715650[7] = state[7];
   out_4596155381636715650[8] = state[8];
}
void F_fun(double *state, double dt, double *out_8271283846288930916) {
   out_8271283846288930916[0] = 1;
   out_8271283846288930916[1] = 0;
   out_8271283846288930916[2] = 0;
   out_8271283846288930916[3] = 0;
   out_8271283846288930916[4] = 0;
   out_8271283846288930916[5] = 0;
   out_8271283846288930916[6] = 0;
   out_8271283846288930916[7] = 0;
   out_8271283846288930916[8] = 0;
   out_8271283846288930916[9] = 0;
   out_8271283846288930916[10] = 1;
   out_8271283846288930916[11] = 0;
   out_8271283846288930916[12] = 0;
   out_8271283846288930916[13] = 0;
   out_8271283846288930916[14] = 0;
   out_8271283846288930916[15] = 0;
   out_8271283846288930916[16] = 0;
   out_8271283846288930916[17] = 0;
   out_8271283846288930916[18] = 0;
   out_8271283846288930916[19] = 0;
   out_8271283846288930916[20] = 1;
   out_8271283846288930916[21] = 0;
   out_8271283846288930916[22] = 0;
   out_8271283846288930916[23] = 0;
   out_8271283846288930916[24] = 0;
   out_8271283846288930916[25] = 0;
   out_8271283846288930916[26] = 0;
   out_8271283846288930916[27] = 0;
   out_8271283846288930916[28] = 0;
   out_8271283846288930916[29] = 0;
   out_8271283846288930916[30] = 1;
   out_8271283846288930916[31] = 0;
   out_8271283846288930916[32] = 0;
   out_8271283846288930916[33] = 0;
   out_8271283846288930916[34] = 0;
   out_8271283846288930916[35] = 0;
   out_8271283846288930916[36] = 0;
   out_8271283846288930916[37] = 0;
   out_8271283846288930916[38] = 0;
   out_8271283846288930916[39] = 0;
   out_8271283846288930916[40] = 1;
   out_8271283846288930916[41] = 0;
   out_8271283846288930916[42] = 0;
   out_8271283846288930916[43] = 0;
   out_8271283846288930916[44] = 0;
   out_8271283846288930916[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_8271283846288930916[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_8271283846288930916[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_8271283846288930916[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_8271283846288930916[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_8271283846288930916[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_8271283846288930916[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_8271283846288930916[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_8271283846288930916[53] = -9.8100000000000005*dt;
   out_8271283846288930916[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_8271283846288930916[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_8271283846288930916[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8271283846288930916[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8271283846288930916[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_8271283846288930916[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_8271283846288930916[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_8271283846288930916[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8271283846288930916[62] = 0;
   out_8271283846288930916[63] = 0;
   out_8271283846288930916[64] = 0;
   out_8271283846288930916[65] = 0;
   out_8271283846288930916[66] = 0;
   out_8271283846288930916[67] = 0;
   out_8271283846288930916[68] = 0;
   out_8271283846288930916[69] = 0;
   out_8271283846288930916[70] = 1;
   out_8271283846288930916[71] = 0;
   out_8271283846288930916[72] = 0;
   out_8271283846288930916[73] = 0;
   out_8271283846288930916[74] = 0;
   out_8271283846288930916[75] = 0;
   out_8271283846288930916[76] = 0;
   out_8271283846288930916[77] = 0;
   out_8271283846288930916[78] = 0;
   out_8271283846288930916[79] = 0;
   out_8271283846288930916[80] = 1;
}
void h_25(double *state, double *unused, double *out_1071579631746279652) {
   out_1071579631746279652[0] = state[6];
}
void H_25(double *state, double *unused, double *out_947424562669153677) {
   out_947424562669153677[0] = 0;
   out_947424562669153677[1] = 0;
   out_947424562669153677[2] = 0;
   out_947424562669153677[3] = 0;
   out_947424562669153677[4] = 0;
   out_947424562669153677[5] = 0;
   out_947424562669153677[6] = 1;
   out_947424562669153677[7] = 0;
   out_947424562669153677[8] = 0;
}
void h_24(double *state, double *unused, double *out_3648611834353910669) {
   out_3648611834353910669[0] = state[4];
   out_3648611834353910669[1] = state[5];
}
void H_24(double *state, double *unused, double *out_1225225036336345889) {
   out_1225225036336345889[0] = 0;
   out_1225225036336345889[1] = 0;
   out_1225225036336345889[2] = 0;
   out_1225225036336345889[3] = 0;
   out_1225225036336345889[4] = 1;
   out_1225225036336345889[5] = 0;
   out_1225225036336345889[6] = 0;
   out_1225225036336345889[7] = 0;
   out_1225225036336345889[8] = 0;
   out_1225225036336345889[9] = 0;
   out_1225225036336345889[10] = 0;
   out_1225225036336345889[11] = 0;
   out_1225225036336345889[12] = 0;
   out_1225225036336345889[13] = 0;
   out_1225225036336345889[14] = 1;
   out_1225225036336345889[15] = 0;
   out_1225225036336345889[16] = 0;
   out_1225225036336345889[17] = 0;
}
void h_30(double *state, double *unused, double *out_8237447157279420782) {
   out_8237447157279420782[0] = state[4];
}
void H_30(double *state, double *unused, double *out_3465757521176402304) {
   out_3465757521176402304[0] = 0;
   out_3465757521176402304[1] = 0;
   out_3465757521176402304[2] = 0;
   out_3465757521176402304[3] = 0;
   out_3465757521176402304[4] = 1;
   out_3465757521176402304[5] = 0;
   out_3465757521176402304[6] = 0;
   out_3465757521176402304[7] = 0;
   out_3465757521176402304[8] = 0;
}
void h_26(double *state, double *unused, double *out_5927733075186470326) {
   out_5927733075186470326[0] = state[7];
}
void H_26(double *state, double *unused, double *out_4251950532429954278) {
   out_4251950532429954278[0] = 0;
   out_4251950532429954278[1] = 0;
   out_4251950532429954278[2] = 0;
   out_4251950532429954278[3] = 0;
   out_4251950532429954278[4] = 0;
   out_4251950532429954278[5] = 0;
   out_4251950532429954278[6] = 0;
   out_4251950532429954278[7] = 1;
   out_4251950532429954278[8] = 0;
}
void h_27(double *state, double *unused, double *out_3930093635914565993) {
   out_3930093635914565993[0] = state[3];
}
void H_27(double *state, double *unused, double *out_1290994209375977393) {
   out_1290994209375977393[0] = 0;
   out_1290994209375977393[1] = 0;
   out_1290994209375977393[2] = 0;
   out_1290994209375977393[3] = 1;
   out_1290994209375977393[4] = 0;
   out_1290994209375977393[5] = 0;
   out_1290994209375977393[6] = 0;
   out_1290994209375977393[7] = 0;
   out_1290994209375977393[8] = 0;
}
void h_29(double *state, double *unused, double *out_2479462243285682826) {
   out_2479462243285682826[0] = state[1];
}
void H_29(double *state, double *unused, double *out_3975988865490794488) {
   out_3975988865490794488[0] = 0;
   out_3975988865490794488[1] = 1;
   out_3975988865490794488[2] = 0;
   out_3975988865490794488[3] = 0;
   out_3975988865490794488[4] = 0;
   out_3975988865490794488[5] = 0;
   out_3975988865490794488[6] = 0;
   out_3975988865490794488[7] = 0;
   out_3975988865490794488[8] = 0;
}
void h_28(double *state, double *unused, double *out_6296653325891602595) {
   out_6296653325891602595[0] = state[0];
}
void H_28(double *state, double *unused, double *out_1106410151578736086) {
   out_1106410151578736086[0] = 1;
   out_1106410151578736086[1] = 0;
   out_1106410151578736086[2] = 0;
   out_1106410151578736086[3] = 0;
   out_1106410151578736086[4] = 0;
   out_1106410151578736086[5] = 0;
   out_1106410151578736086[6] = 0;
   out_1106410151578736086[7] = 0;
   out_1106410151578736086[8] = 0;
}
void h_31(double *state, double *unused, double *out_4454477804804743461) {
   out_4454477804804743461[0] = state[8];
}
void H_31(double *state, double *unused, double *out_3625742430196602802) {
   out_3625742430196602802[0] = 0;
   out_3625742430196602802[1] = 0;
   out_3625742430196602802[2] = 0;
   out_3625742430196602802[3] = 0;
   out_3625742430196602802[4] = 0;
   out_3625742430196602802[5] = 0;
   out_3625742430196602802[6] = 0;
   out_3625742430196602802[7] = 0;
   out_3625742430196602802[8] = 1;
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

void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_25, H_25, NULL, in_z, in_R, in_ea, MAHA_THRESH_25);
}
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<2, 3, 0>(in_x, in_P, h_24, H_24, NULL, in_z, in_R, in_ea, MAHA_THRESH_24);
}
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_30, H_30, NULL, in_z, in_R, in_ea, MAHA_THRESH_30);
}
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_26, H_26, NULL, in_z, in_R, in_ea, MAHA_THRESH_26);
}
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_27, H_27, NULL, in_z, in_R, in_ea, MAHA_THRESH_27);
}
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_29, H_29, NULL, in_z, in_R, in_ea, MAHA_THRESH_29);
}
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_28, H_28, NULL, in_z, in_R, in_ea, MAHA_THRESH_28);
}
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_31, H_31, NULL, in_z, in_R, in_ea, MAHA_THRESH_31);
}
void car_err_fun(double *nom_x, double *delta_x, double *out_1207077157841363728) {
  err_fun(nom_x, delta_x, out_1207077157841363728);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_5167403064917713258) {
  inv_err_fun(nom_x, true_x, out_5167403064917713258);
}
void car_H_mod_fun(double *state, double *out_5732271557167151234) {
  H_mod_fun(state, out_5732271557167151234);
}
void car_f_fun(double *state, double dt, double *out_4596155381636715650) {
  f_fun(state,  dt, out_4596155381636715650);
}
void car_F_fun(double *state, double dt, double *out_8271283846288930916) {
  F_fun(state,  dt, out_8271283846288930916);
}
void car_h_25(double *state, double *unused, double *out_1071579631746279652) {
  h_25(state, unused, out_1071579631746279652);
}
void car_H_25(double *state, double *unused, double *out_947424562669153677) {
  H_25(state, unused, out_947424562669153677);
}
void car_h_24(double *state, double *unused, double *out_3648611834353910669) {
  h_24(state, unused, out_3648611834353910669);
}
void car_H_24(double *state, double *unused, double *out_1225225036336345889) {
  H_24(state, unused, out_1225225036336345889);
}
void car_h_30(double *state, double *unused, double *out_8237447157279420782) {
  h_30(state, unused, out_8237447157279420782);
}
void car_H_30(double *state, double *unused, double *out_3465757521176402304) {
  H_30(state, unused, out_3465757521176402304);
}
void car_h_26(double *state, double *unused, double *out_5927733075186470326) {
  h_26(state, unused, out_5927733075186470326);
}
void car_H_26(double *state, double *unused, double *out_4251950532429954278) {
  H_26(state, unused, out_4251950532429954278);
}
void car_h_27(double *state, double *unused, double *out_3930093635914565993) {
  h_27(state, unused, out_3930093635914565993);
}
void car_H_27(double *state, double *unused, double *out_1290994209375977393) {
  H_27(state, unused, out_1290994209375977393);
}
void car_h_29(double *state, double *unused, double *out_2479462243285682826) {
  h_29(state, unused, out_2479462243285682826);
}
void car_H_29(double *state, double *unused, double *out_3975988865490794488) {
  H_29(state, unused, out_3975988865490794488);
}
void car_h_28(double *state, double *unused, double *out_6296653325891602595) {
  h_28(state, unused, out_6296653325891602595);
}
void car_H_28(double *state, double *unused, double *out_1106410151578736086) {
  H_28(state, unused, out_1106410151578736086);
}
void car_h_31(double *state, double *unused, double *out_4454477804804743461) {
  h_31(state, unused, out_4454477804804743461);
}
void car_H_31(double *state, double *unused, double *out_3625742430196602802) {
  H_31(state, unused, out_3625742430196602802);
}
void car_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
void car_set_mass(double x) {
  set_mass(x);
}
void car_set_rotational_inertia(double x) {
  set_rotational_inertia(x);
}
void car_set_center_to_front(double x) {
  set_center_to_front(x);
}
void car_set_center_to_rear(double x) {
  set_center_to_rear(x);
}
void car_set_stiffness_front(double x) {
  set_stiffness_front(x);
}
void car_set_stiffness_rear(double x) {
  set_stiffness_rear(x);
}
}

const EKF car = {
  .name = "car",
  .kinds = { 25, 24, 30, 26, 27, 29, 28, 31 },
  .feature_kinds = {  },
  .f_fun = car_f_fun,
  .F_fun = car_F_fun,
  .err_fun = car_err_fun,
  .inv_err_fun = car_inv_err_fun,
  .H_mod_fun = car_H_mod_fun,
  .predict = car_predict,
  .hs = {
    { 25, car_h_25 },
    { 24, car_h_24 },
    { 30, car_h_30 },
    { 26, car_h_26 },
    { 27, car_h_27 },
    { 29, car_h_29 },
    { 28, car_h_28 },
    { 31, car_h_31 },
  },
  .Hs = {
    { 25, car_H_25 },
    { 24, car_H_24 },
    { 30, car_H_30 },
    { 26, car_H_26 },
    { 27, car_H_27 },
    { 29, car_H_29 },
    { 28, car_H_28 },
    { 31, car_H_31 },
  },
  .updates = {
    { 25, car_update_25 },
    { 24, car_update_24 },
    { 30, car_update_30 },
    { 26, car_update_26 },
    { 27, car_update_27 },
    { 29, car_update_29 },
    { 28, car_update_28 },
    { 31, car_update_31 },
  },
  .Hes = {
  },
  .sets = {
    { "mass", car_set_mass },
    { "rotational_inertia", car_set_rotational_inertia },
    { "center_to_front", car_set_center_to_front },
    { "center_to_rear", car_set_center_to_rear },
    { "stiffness_front", car_set_stiffness_front },
    { "stiffness_rear", car_set_stiffness_rear },
  },
  .extra_routines = {
  },
};

ekf_lib_init(car)
