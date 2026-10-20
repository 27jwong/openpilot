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
void err_fun(double *nom_x, double *delta_x, double *out_5451371327471294677) {
   out_5451371327471294677[0] = delta_x[0] + nom_x[0];
   out_5451371327471294677[1] = delta_x[1] + nom_x[1];
   out_5451371327471294677[2] = delta_x[2] + nom_x[2];
   out_5451371327471294677[3] = delta_x[3] + nom_x[3];
   out_5451371327471294677[4] = delta_x[4] + nom_x[4];
   out_5451371327471294677[5] = delta_x[5] + nom_x[5];
   out_5451371327471294677[6] = delta_x[6] + nom_x[6];
   out_5451371327471294677[7] = delta_x[7] + nom_x[7];
   out_5451371327471294677[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_8522218724742075239) {
   out_8522218724742075239[0] = -nom_x[0] + true_x[0];
   out_8522218724742075239[1] = -nom_x[1] + true_x[1];
   out_8522218724742075239[2] = -nom_x[2] + true_x[2];
   out_8522218724742075239[3] = -nom_x[3] + true_x[3];
   out_8522218724742075239[4] = -nom_x[4] + true_x[4];
   out_8522218724742075239[5] = -nom_x[5] + true_x[5];
   out_8522218724742075239[6] = -nom_x[6] + true_x[6];
   out_8522218724742075239[7] = -nom_x[7] + true_x[7];
   out_8522218724742075239[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_8466627282619758941) {
   out_8466627282619758941[0] = 1.0;
   out_8466627282619758941[1] = 0.0;
   out_8466627282619758941[2] = 0.0;
   out_8466627282619758941[3] = 0.0;
   out_8466627282619758941[4] = 0.0;
   out_8466627282619758941[5] = 0.0;
   out_8466627282619758941[6] = 0.0;
   out_8466627282619758941[7] = 0.0;
   out_8466627282619758941[8] = 0.0;
   out_8466627282619758941[9] = 0.0;
   out_8466627282619758941[10] = 1.0;
   out_8466627282619758941[11] = 0.0;
   out_8466627282619758941[12] = 0.0;
   out_8466627282619758941[13] = 0.0;
   out_8466627282619758941[14] = 0.0;
   out_8466627282619758941[15] = 0.0;
   out_8466627282619758941[16] = 0.0;
   out_8466627282619758941[17] = 0.0;
   out_8466627282619758941[18] = 0.0;
   out_8466627282619758941[19] = 0.0;
   out_8466627282619758941[20] = 1.0;
   out_8466627282619758941[21] = 0.0;
   out_8466627282619758941[22] = 0.0;
   out_8466627282619758941[23] = 0.0;
   out_8466627282619758941[24] = 0.0;
   out_8466627282619758941[25] = 0.0;
   out_8466627282619758941[26] = 0.0;
   out_8466627282619758941[27] = 0.0;
   out_8466627282619758941[28] = 0.0;
   out_8466627282619758941[29] = 0.0;
   out_8466627282619758941[30] = 1.0;
   out_8466627282619758941[31] = 0.0;
   out_8466627282619758941[32] = 0.0;
   out_8466627282619758941[33] = 0.0;
   out_8466627282619758941[34] = 0.0;
   out_8466627282619758941[35] = 0.0;
   out_8466627282619758941[36] = 0.0;
   out_8466627282619758941[37] = 0.0;
   out_8466627282619758941[38] = 0.0;
   out_8466627282619758941[39] = 0.0;
   out_8466627282619758941[40] = 1.0;
   out_8466627282619758941[41] = 0.0;
   out_8466627282619758941[42] = 0.0;
   out_8466627282619758941[43] = 0.0;
   out_8466627282619758941[44] = 0.0;
   out_8466627282619758941[45] = 0.0;
   out_8466627282619758941[46] = 0.0;
   out_8466627282619758941[47] = 0.0;
   out_8466627282619758941[48] = 0.0;
   out_8466627282619758941[49] = 0.0;
   out_8466627282619758941[50] = 1.0;
   out_8466627282619758941[51] = 0.0;
   out_8466627282619758941[52] = 0.0;
   out_8466627282619758941[53] = 0.0;
   out_8466627282619758941[54] = 0.0;
   out_8466627282619758941[55] = 0.0;
   out_8466627282619758941[56] = 0.0;
   out_8466627282619758941[57] = 0.0;
   out_8466627282619758941[58] = 0.0;
   out_8466627282619758941[59] = 0.0;
   out_8466627282619758941[60] = 1.0;
   out_8466627282619758941[61] = 0.0;
   out_8466627282619758941[62] = 0.0;
   out_8466627282619758941[63] = 0.0;
   out_8466627282619758941[64] = 0.0;
   out_8466627282619758941[65] = 0.0;
   out_8466627282619758941[66] = 0.0;
   out_8466627282619758941[67] = 0.0;
   out_8466627282619758941[68] = 0.0;
   out_8466627282619758941[69] = 0.0;
   out_8466627282619758941[70] = 1.0;
   out_8466627282619758941[71] = 0.0;
   out_8466627282619758941[72] = 0.0;
   out_8466627282619758941[73] = 0.0;
   out_8466627282619758941[74] = 0.0;
   out_8466627282619758941[75] = 0.0;
   out_8466627282619758941[76] = 0.0;
   out_8466627282619758941[77] = 0.0;
   out_8466627282619758941[78] = 0.0;
   out_8466627282619758941[79] = 0.0;
   out_8466627282619758941[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_2258902101046555831) {
   out_2258902101046555831[0] = state[0];
   out_2258902101046555831[1] = state[1];
   out_2258902101046555831[2] = state[2];
   out_2258902101046555831[3] = state[3];
   out_2258902101046555831[4] = state[4];
   out_2258902101046555831[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8100000000000005*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_2258902101046555831[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_2258902101046555831[7] = state[7];
   out_2258902101046555831[8] = state[8];
}
void F_fun(double *state, double dt, double *out_6156202546407203168) {
   out_6156202546407203168[0] = 1;
   out_6156202546407203168[1] = 0;
   out_6156202546407203168[2] = 0;
   out_6156202546407203168[3] = 0;
   out_6156202546407203168[4] = 0;
   out_6156202546407203168[5] = 0;
   out_6156202546407203168[6] = 0;
   out_6156202546407203168[7] = 0;
   out_6156202546407203168[8] = 0;
   out_6156202546407203168[9] = 0;
   out_6156202546407203168[10] = 1;
   out_6156202546407203168[11] = 0;
   out_6156202546407203168[12] = 0;
   out_6156202546407203168[13] = 0;
   out_6156202546407203168[14] = 0;
   out_6156202546407203168[15] = 0;
   out_6156202546407203168[16] = 0;
   out_6156202546407203168[17] = 0;
   out_6156202546407203168[18] = 0;
   out_6156202546407203168[19] = 0;
   out_6156202546407203168[20] = 1;
   out_6156202546407203168[21] = 0;
   out_6156202546407203168[22] = 0;
   out_6156202546407203168[23] = 0;
   out_6156202546407203168[24] = 0;
   out_6156202546407203168[25] = 0;
   out_6156202546407203168[26] = 0;
   out_6156202546407203168[27] = 0;
   out_6156202546407203168[28] = 0;
   out_6156202546407203168[29] = 0;
   out_6156202546407203168[30] = 1;
   out_6156202546407203168[31] = 0;
   out_6156202546407203168[32] = 0;
   out_6156202546407203168[33] = 0;
   out_6156202546407203168[34] = 0;
   out_6156202546407203168[35] = 0;
   out_6156202546407203168[36] = 0;
   out_6156202546407203168[37] = 0;
   out_6156202546407203168[38] = 0;
   out_6156202546407203168[39] = 0;
   out_6156202546407203168[40] = 1;
   out_6156202546407203168[41] = 0;
   out_6156202546407203168[42] = 0;
   out_6156202546407203168[43] = 0;
   out_6156202546407203168[44] = 0;
   out_6156202546407203168[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_6156202546407203168[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_6156202546407203168[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_6156202546407203168[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_6156202546407203168[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_6156202546407203168[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_6156202546407203168[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_6156202546407203168[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_6156202546407203168[53] = -9.8100000000000005*dt;
   out_6156202546407203168[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_6156202546407203168[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_6156202546407203168[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_6156202546407203168[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_6156202546407203168[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_6156202546407203168[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_6156202546407203168[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_6156202546407203168[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_6156202546407203168[62] = 0;
   out_6156202546407203168[63] = 0;
   out_6156202546407203168[64] = 0;
   out_6156202546407203168[65] = 0;
   out_6156202546407203168[66] = 0;
   out_6156202546407203168[67] = 0;
   out_6156202546407203168[68] = 0;
   out_6156202546407203168[69] = 0;
   out_6156202546407203168[70] = 1;
   out_6156202546407203168[71] = 0;
   out_6156202546407203168[72] = 0;
   out_6156202546407203168[73] = 0;
   out_6156202546407203168[74] = 0;
   out_6156202546407203168[75] = 0;
   out_6156202546407203168[76] = 0;
   out_6156202546407203168[77] = 0;
   out_6156202546407203168[78] = 0;
   out_6156202546407203168[79] = 0;
   out_6156202546407203168[80] = 1;
}
void h_25(double *state, double *unused, double *out_653631558110655492) {
   out_653631558110655492[0] = state[6];
}
void H_25(double *state, double *unused, double *out_5076424965905127835) {
   out_5076424965905127835[0] = 0;
   out_5076424965905127835[1] = 0;
   out_5076424965905127835[2] = 0;
   out_5076424965905127835[3] = 0;
   out_5076424965905127835[4] = 0;
   out_5076424965905127835[5] = 0;
   out_5076424965905127835[6] = 1;
   out_5076424965905127835[7] = 0;
   out_5076424965905127835[8] = 0;
}
void h_24(double *state, double *unused, double *out_3887391113902346836) {
   out_3887391113902346836[0] = state[4];
   out_3887391113902346836[1] = state[5];
}
void H_24(double *state, double *unused, double *out_2899210542297977862) {
   out_2899210542297977862[0] = 0;
   out_2899210542297977862[1] = 0;
   out_2899210542297977862[2] = 0;
   out_2899210542297977862[3] = 0;
   out_2899210542297977862[4] = 1;
   out_2899210542297977862[5] = 0;
   out_2899210542297977862[6] = 0;
   out_2899210542297977862[7] = 0;
   out_2899210542297977862[8] = 0;
   out_2899210542297977862[9] = 0;
   out_2899210542297977862[10] = 0;
   out_2899210542297977862[11] = 0;
   out_2899210542297977862[12] = 0;
   out_2899210542297977862[13] = 0;
   out_2899210542297977862[14] = 1;
   out_2899210542297977862[15] = 0;
   out_2899210542297977862[16] = 0;
   out_2899210542297977862[17] = 0;
}
void h_30(double *state, double *unused, double *out_378437495826149603) {
   out_378437495826149603[0] = state[4];
}
void H_30(double *state, double *unused, double *out_2558092007397879208) {
   out_2558092007397879208[0] = 0;
   out_2558092007397879208[1] = 0;
   out_2558092007397879208[2] = 0;
   out_2558092007397879208[3] = 0;
   out_2558092007397879208[4] = 1;
   out_2558092007397879208[5] = 0;
   out_2558092007397879208[6] = 0;
   out_2558092007397879208[7] = 0;
   out_2558092007397879208[8] = 0;
}
void h_26(double *state, double *unused, double *out_6480956931559343993) {
   out_6480956931559343993[0] = state[7];
}
void H_26(double *state, double *unused, double *out_8817928284779184059) {
   out_8817928284779184059[0] = 0;
   out_8817928284779184059[1] = 0;
   out_8817928284779184059[2] = 0;
   out_8817928284779184059[3] = 0;
   out_8817928284779184059[4] = 0;
   out_8817928284779184059[5] = 0;
   out_8817928284779184059[6] = 0;
   out_8817928284779184059[7] = 1;
   out_8817928284779184059[8] = 0;
}
void h_27(double *state, double *unused, double *out_50315673434177975) {
   out_50315673434177975[0] = state[3];
}
void H_27(double *state, double *unused, double *out_4732855319198304119) {
   out_4732855319198304119[0] = 0;
   out_4732855319198304119[1] = 0;
   out_4732855319198304119[2] = 0;
   out_4732855319198304119[3] = 1;
   out_4732855319198304119[4] = 0;
   out_4732855319198304119[5] = 0;
   out_4732855319198304119[6] = 0;
   out_4732855319198304119[7] = 0;
   out_4732855319198304119[8] = 0;
}
void h_29(double *state, double *unused, double *out_7813580930254850416) {
   out_7813580930254850416[0] = state[1];
}
void H_29(double *state, double *unused, double *out_2047860663083487024) {
   out_2047860663083487024[0] = 0;
   out_2047860663083487024[1] = 1;
   out_2047860663083487024[2] = 0;
   out_2047860663083487024[3] = 0;
   out_2047860663083487024[4] = 0;
   out_2047860663083487024[5] = 0;
   out_2047860663083487024[6] = 0;
   out_2047860663083487024[7] = 0;
   out_2047860663083487024[8] = 0;
}
void h_28(double *state, double *unused, double *out_5175591542401146972) {
   out_5175591542401146972[0] = state[0];
}
void H_28(double *state, double *unused, double *out_7130259680153017598) {
   out_7130259680153017598[0] = 1;
   out_7130259680153017598[1] = 0;
   out_7130259680153017598[2] = 0;
   out_7130259680153017598[3] = 0;
   out_7130259680153017598[4] = 0;
   out_7130259680153017598[5] = 0;
   out_7130259680153017598[6] = 0;
   out_7130259680153017598[7] = 0;
   out_7130259680153017598[8] = 0;
}
void h_31(double *state, double *unused, double *out_4291081086364677842) {
   out_4291081086364677842[0] = state[8];
}
void H_31(double *state, double *unused, double *out_9002607686697016081) {
   out_9002607686697016081[0] = 0;
   out_9002607686697016081[1] = 0;
   out_9002607686697016081[2] = 0;
   out_9002607686697016081[3] = 0;
   out_9002607686697016081[4] = 0;
   out_9002607686697016081[5] = 0;
   out_9002607686697016081[6] = 0;
   out_9002607686697016081[7] = 0;
   out_9002607686697016081[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_5451371327471294677) {
  err_fun(nom_x, delta_x, out_5451371327471294677);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_8522218724742075239) {
  inv_err_fun(nom_x, true_x, out_8522218724742075239);
}
void car_H_mod_fun(double *state, double *out_8466627282619758941) {
  H_mod_fun(state, out_8466627282619758941);
}
void car_f_fun(double *state, double dt, double *out_2258902101046555831) {
  f_fun(state,  dt, out_2258902101046555831);
}
void car_F_fun(double *state, double dt, double *out_6156202546407203168) {
  F_fun(state,  dt, out_6156202546407203168);
}
void car_h_25(double *state, double *unused, double *out_653631558110655492) {
  h_25(state, unused, out_653631558110655492);
}
void car_H_25(double *state, double *unused, double *out_5076424965905127835) {
  H_25(state, unused, out_5076424965905127835);
}
void car_h_24(double *state, double *unused, double *out_3887391113902346836) {
  h_24(state, unused, out_3887391113902346836);
}
void car_H_24(double *state, double *unused, double *out_2899210542297977862) {
  H_24(state, unused, out_2899210542297977862);
}
void car_h_30(double *state, double *unused, double *out_378437495826149603) {
  h_30(state, unused, out_378437495826149603);
}
void car_H_30(double *state, double *unused, double *out_2558092007397879208) {
  H_30(state, unused, out_2558092007397879208);
}
void car_h_26(double *state, double *unused, double *out_6480956931559343993) {
  h_26(state, unused, out_6480956931559343993);
}
void car_H_26(double *state, double *unused, double *out_8817928284779184059) {
  H_26(state, unused, out_8817928284779184059);
}
void car_h_27(double *state, double *unused, double *out_50315673434177975) {
  h_27(state, unused, out_50315673434177975);
}
void car_H_27(double *state, double *unused, double *out_4732855319198304119) {
  H_27(state, unused, out_4732855319198304119);
}
void car_h_29(double *state, double *unused, double *out_7813580930254850416) {
  h_29(state, unused, out_7813580930254850416);
}
void car_H_29(double *state, double *unused, double *out_2047860663083487024) {
  H_29(state, unused, out_2047860663083487024);
}
void car_h_28(double *state, double *unused, double *out_5175591542401146972) {
  h_28(state, unused, out_5175591542401146972);
}
void car_H_28(double *state, double *unused, double *out_7130259680153017598) {
  H_28(state, unused, out_7130259680153017598);
}
void car_h_31(double *state, double *unused, double *out_4291081086364677842) {
  h_31(state, unused, out_4291081086364677842);
}
void car_H_31(double *state, double *unused, double *out_9002607686697016081) {
  H_31(state, unused, out_9002607686697016081);
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
