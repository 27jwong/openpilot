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
void err_fun(double *nom_x, double *delta_x, double *out_8532074412831680005) {
   out_8532074412831680005[0] = delta_x[0] + nom_x[0];
   out_8532074412831680005[1] = delta_x[1] + nom_x[1];
   out_8532074412831680005[2] = delta_x[2] + nom_x[2];
   out_8532074412831680005[3] = delta_x[3] + nom_x[3];
   out_8532074412831680005[4] = delta_x[4] + nom_x[4];
   out_8532074412831680005[5] = delta_x[5] + nom_x[5];
   out_8532074412831680005[6] = delta_x[6] + nom_x[6];
   out_8532074412831680005[7] = delta_x[7] + nom_x[7];
   out_8532074412831680005[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_8887769849182574222) {
   out_8887769849182574222[0] = -nom_x[0] + true_x[0];
   out_8887769849182574222[1] = -nom_x[1] + true_x[1];
   out_8887769849182574222[2] = -nom_x[2] + true_x[2];
   out_8887769849182574222[3] = -nom_x[3] + true_x[3];
   out_8887769849182574222[4] = -nom_x[4] + true_x[4];
   out_8887769849182574222[5] = -nom_x[5] + true_x[5];
   out_8887769849182574222[6] = -nom_x[6] + true_x[6];
   out_8887769849182574222[7] = -nom_x[7] + true_x[7];
   out_8887769849182574222[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_2434822093009109592) {
   out_2434822093009109592[0] = 1.0;
   out_2434822093009109592[1] = 0.0;
   out_2434822093009109592[2] = 0.0;
   out_2434822093009109592[3] = 0.0;
   out_2434822093009109592[4] = 0.0;
   out_2434822093009109592[5] = 0.0;
   out_2434822093009109592[6] = 0.0;
   out_2434822093009109592[7] = 0.0;
   out_2434822093009109592[8] = 0.0;
   out_2434822093009109592[9] = 0.0;
   out_2434822093009109592[10] = 1.0;
   out_2434822093009109592[11] = 0.0;
   out_2434822093009109592[12] = 0.0;
   out_2434822093009109592[13] = 0.0;
   out_2434822093009109592[14] = 0.0;
   out_2434822093009109592[15] = 0.0;
   out_2434822093009109592[16] = 0.0;
   out_2434822093009109592[17] = 0.0;
   out_2434822093009109592[18] = 0.0;
   out_2434822093009109592[19] = 0.0;
   out_2434822093009109592[20] = 1.0;
   out_2434822093009109592[21] = 0.0;
   out_2434822093009109592[22] = 0.0;
   out_2434822093009109592[23] = 0.0;
   out_2434822093009109592[24] = 0.0;
   out_2434822093009109592[25] = 0.0;
   out_2434822093009109592[26] = 0.0;
   out_2434822093009109592[27] = 0.0;
   out_2434822093009109592[28] = 0.0;
   out_2434822093009109592[29] = 0.0;
   out_2434822093009109592[30] = 1.0;
   out_2434822093009109592[31] = 0.0;
   out_2434822093009109592[32] = 0.0;
   out_2434822093009109592[33] = 0.0;
   out_2434822093009109592[34] = 0.0;
   out_2434822093009109592[35] = 0.0;
   out_2434822093009109592[36] = 0.0;
   out_2434822093009109592[37] = 0.0;
   out_2434822093009109592[38] = 0.0;
   out_2434822093009109592[39] = 0.0;
   out_2434822093009109592[40] = 1.0;
   out_2434822093009109592[41] = 0.0;
   out_2434822093009109592[42] = 0.0;
   out_2434822093009109592[43] = 0.0;
   out_2434822093009109592[44] = 0.0;
   out_2434822093009109592[45] = 0.0;
   out_2434822093009109592[46] = 0.0;
   out_2434822093009109592[47] = 0.0;
   out_2434822093009109592[48] = 0.0;
   out_2434822093009109592[49] = 0.0;
   out_2434822093009109592[50] = 1.0;
   out_2434822093009109592[51] = 0.0;
   out_2434822093009109592[52] = 0.0;
   out_2434822093009109592[53] = 0.0;
   out_2434822093009109592[54] = 0.0;
   out_2434822093009109592[55] = 0.0;
   out_2434822093009109592[56] = 0.0;
   out_2434822093009109592[57] = 0.0;
   out_2434822093009109592[58] = 0.0;
   out_2434822093009109592[59] = 0.0;
   out_2434822093009109592[60] = 1.0;
   out_2434822093009109592[61] = 0.0;
   out_2434822093009109592[62] = 0.0;
   out_2434822093009109592[63] = 0.0;
   out_2434822093009109592[64] = 0.0;
   out_2434822093009109592[65] = 0.0;
   out_2434822093009109592[66] = 0.0;
   out_2434822093009109592[67] = 0.0;
   out_2434822093009109592[68] = 0.0;
   out_2434822093009109592[69] = 0.0;
   out_2434822093009109592[70] = 1.0;
   out_2434822093009109592[71] = 0.0;
   out_2434822093009109592[72] = 0.0;
   out_2434822093009109592[73] = 0.0;
   out_2434822093009109592[74] = 0.0;
   out_2434822093009109592[75] = 0.0;
   out_2434822093009109592[76] = 0.0;
   out_2434822093009109592[77] = 0.0;
   out_2434822093009109592[78] = 0.0;
   out_2434822093009109592[79] = 0.0;
   out_2434822093009109592[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_7489300736878445586) {
   out_7489300736878445586[0] = state[0];
   out_7489300736878445586[1] = state[1];
   out_7489300736878445586[2] = state[2];
   out_7489300736878445586[3] = state[3];
   out_7489300736878445586[4] = state[4];
   out_7489300736878445586[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8100000000000005*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_7489300736878445586[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_7489300736878445586[7] = state[7];
   out_7489300736878445586[8] = state[8];
}
void F_fun(double *state, double dt, double *out_5688594807383263913) {
   out_5688594807383263913[0] = 1;
   out_5688594807383263913[1] = 0;
   out_5688594807383263913[2] = 0;
   out_5688594807383263913[3] = 0;
   out_5688594807383263913[4] = 0;
   out_5688594807383263913[5] = 0;
   out_5688594807383263913[6] = 0;
   out_5688594807383263913[7] = 0;
   out_5688594807383263913[8] = 0;
   out_5688594807383263913[9] = 0;
   out_5688594807383263913[10] = 1;
   out_5688594807383263913[11] = 0;
   out_5688594807383263913[12] = 0;
   out_5688594807383263913[13] = 0;
   out_5688594807383263913[14] = 0;
   out_5688594807383263913[15] = 0;
   out_5688594807383263913[16] = 0;
   out_5688594807383263913[17] = 0;
   out_5688594807383263913[18] = 0;
   out_5688594807383263913[19] = 0;
   out_5688594807383263913[20] = 1;
   out_5688594807383263913[21] = 0;
   out_5688594807383263913[22] = 0;
   out_5688594807383263913[23] = 0;
   out_5688594807383263913[24] = 0;
   out_5688594807383263913[25] = 0;
   out_5688594807383263913[26] = 0;
   out_5688594807383263913[27] = 0;
   out_5688594807383263913[28] = 0;
   out_5688594807383263913[29] = 0;
   out_5688594807383263913[30] = 1;
   out_5688594807383263913[31] = 0;
   out_5688594807383263913[32] = 0;
   out_5688594807383263913[33] = 0;
   out_5688594807383263913[34] = 0;
   out_5688594807383263913[35] = 0;
   out_5688594807383263913[36] = 0;
   out_5688594807383263913[37] = 0;
   out_5688594807383263913[38] = 0;
   out_5688594807383263913[39] = 0;
   out_5688594807383263913[40] = 1;
   out_5688594807383263913[41] = 0;
   out_5688594807383263913[42] = 0;
   out_5688594807383263913[43] = 0;
   out_5688594807383263913[44] = 0;
   out_5688594807383263913[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_5688594807383263913[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_5688594807383263913[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_5688594807383263913[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_5688594807383263913[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_5688594807383263913[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_5688594807383263913[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_5688594807383263913[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_5688594807383263913[53] = -9.8100000000000005*dt;
   out_5688594807383263913[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_5688594807383263913[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_5688594807383263913[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5688594807383263913[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5688594807383263913[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_5688594807383263913[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_5688594807383263913[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_5688594807383263913[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5688594807383263913[62] = 0;
   out_5688594807383263913[63] = 0;
   out_5688594807383263913[64] = 0;
   out_5688594807383263913[65] = 0;
   out_5688594807383263913[66] = 0;
   out_5688594807383263913[67] = 0;
   out_5688594807383263913[68] = 0;
   out_5688594807383263913[69] = 0;
   out_5688594807383263913[70] = 1;
   out_5688594807383263913[71] = 0;
   out_5688594807383263913[72] = 0;
   out_5688594807383263913[73] = 0;
   out_5688594807383263913[74] = 0;
   out_5688594807383263913[75] = 0;
   out_5688594807383263913[76] = 0;
   out_5688594807383263913[77] = 0;
   out_5688594807383263913[78] = 0;
   out_5688594807383263913[79] = 0;
   out_5688594807383263913[80] = 1;
}
void h_25(double *state, double *unused, double *out_4593071790268116288) {
   out_4593071790268116288[0] = state[6];
}
void H_25(double *state, double *unused, double *out_3739161203201081212) {
   out_3739161203201081212[0] = 0;
   out_3739161203201081212[1] = 0;
   out_3739161203201081212[2] = 0;
   out_3739161203201081212[3] = 0;
   out_3739161203201081212[4] = 0;
   out_3739161203201081212[5] = 0;
   out_3739161203201081212[6] = 1;
   out_3739161203201081212[7] = 0;
   out_3739161203201081212[8] = 0;
}
void h_24(double *state, double *unused, double *out_8003921618768497979) {
   out_8003921618768497979[0] = state[4];
   out_8003921618768497979[1] = state[5];
}
void H_24(double *state, double *unused, double *out_4646084155782705221) {
   out_4646084155782705221[0] = 0;
   out_4646084155782705221[1] = 0;
   out_4646084155782705221[2] = 0;
   out_4646084155782705221[3] = 0;
   out_4646084155782705221[4] = 1;
   out_4646084155782705221[5] = 0;
   out_4646084155782705221[6] = 0;
   out_4646084155782705221[7] = 0;
   out_4646084155782705221[8] = 0;
   out_4646084155782705221[9] = 0;
   out_4646084155782705221[10] = 0;
   out_4646084155782705221[11] = 0;
   out_4646084155782705221[12] = 0;
   out_4646084155782705221[13] = 0;
   out_4646084155782705221[14] = 1;
   out_4646084155782705221[15] = 0;
   out_4646084155782705221[16] = 0;
   out_4646084155782705221[17] = 0;
}
void h_30(double *state, double *unused, double *out_7633295100594421359) {
   out_7633295100594421359[0] = state[4];
}
void H_30(double *state, double *unused, double *out_3177529138290535543) {
   out_3177529138290535543[0] = 0;
   out_3177529138290535543[1] = 0;
   out_3177529138290535543[2] = 0;
   out_3177529138290535543[3] = 0;
   out_3177529138290535543[4] = 1;
   out_3177529138290535543[5] = 0;
   out_3177529138290535543[6] = 0;
   out_3177529138290535543[7] = 0;
   out_3177529138290535543[8] = 0;
}
void h_26(double *state, double *unused, double *out_5811775705454284612) {
   out_5811775705454284612[0] = state[7];
}
void H_26(double *state, double *unused, double *out_7480664522075137436) {
   out_7480664522075137436[0] = 0;
   out_7480664522075137436[1] = 0;
   out_7480664522075137436[2] = 0;
   out_7480664522075137436[3] = 0;
   out_7480664522075137436[4] = 0;
   out_7480664522075137436[5] = 0;
   out_7480664522075137436[6] = 0;
   out_7480664522075137436[7] = 1;
   out_7480664522075137436[8] = 0;
}
void h_27(double *state, double *unused, double *out_8851999015780589683) {
   out_8851999015780589683[0] = state[3];
}
void H_27(double *state, double *unused, double *out_6043263462144746193) {
   out_6043263462144746193[0] = 0;
   out_6043263462144746193[1] = 0;
   out_6043263462144746193[2] = 0;
   out_6043263462144746193[3] = 1;
   out_6043263462144746193[4] = 0;
   out_6043263462144746193[5] = 0;
   out_6043263462144746193[6] = 0;
   out_6043263462144746193[7] = 0;
   out_6043263462144746193[8] = 0;
}
void h_29(double *state, double *unused, double *out_1831479801108289492) {
   out_1831479801108289492[0] = state[1];
}
void H_29(double *state, double *unused, double *out_7756626189014297226) {
   out_7756626189014297226[0] = 0;
   out_7756626189014297226[1] = 1;
   out_7756626189014297226[2] = 0;
   out_7756626189014297226[3] = 0;
   out_7756626189014297226[4] = 0;
   out_7756626189014297226[5] = 0;
   out_7756626189014297226[6] = 0;
   out_7756626189014297226[7] = 0;
   out_7756626189014297226[8] = 0;
}
void h_28(double *state, double *unused, double *out_6557959006484629248) {
   out_6557959006484629248[0] = state[0];
}
void H_28(double *state, double *unused, double *out_5792995917448970975) {
   out_5792995917448970975[0] = 1;
   out_5792995917448970975[1] = 0;
   out_5792995917448970975[2] = 0;
   out_5792995917448970975[3] = 0;
   out_5792995917448970975[4] = 0;
   out_5792995917448970975[5] = 0;
   out_5792995917448970975[6] = 0;
   out_5792995917448970975[7] = 0;
   out_5792995917448970975[8] = 0;
}
void h_31(double *state, double *unused, double *out_5357011602177695336) {
   out_5357011602177695336[0] = state[8];
}
void H_31(double *state, double *unused, double *out_3708515241324120784) {
   out_3708515241324120784[0] = 0;
   out_3708515241324120784[1] = 0;
   out_3708515241324120784[2] = 0;
   out_3708515241324120784[3] = 0;
   out_3708515241324120784[4] = 0;
   out_3708515241324120784[5] = 0;
   out_3708515241324120784[6] = 0;
   out_3708515241324120784[7] = 0;
   out_3708515241324120784[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_8532074412831680005) {
  err_fun(nom_x, delta_x, out_8532074412831680005);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_8887769849182574222) {
  inv_err_fun(nom_x, true_x, out_8887769849182574222);
}
void car_H_mod_fun(double *state, double *out_2434822093009109592) {
  H_mod_fun(state, out_2434822093009109592);
}
void car_f_fun(double *state, double dt, double *out_7489300736878445586) {
  f_fun(state,  dt, out_7489300736878445586);
}
void car_F_fun(double *state, double dt, double *out_5688594807383263913) {
  F_fun(state,  dt, out_5688594807383263913);
}
void car_h_25(double *state, double *unused, double *out_4593071790268116288) {
  h_25(state, unused, out_4593071790268116288);
}
void car_H_25(double *state, double *unused, double *out_3739161203201081212) {
  H_25(state, unused, out_3739161203201081212);
}
void car_h_24(double *state, double *unused, double *out_8003921618768497979) {
  h_24(state, unused, out_8003921618768497979);
}
void car_H_24(double *state, double *unused, double *out_4646084155782705221) {
  H_24(state, unused, out_4646084155782705221);
}
void car_h_30(double *state, double *unused, double *out_7633295100594421359) {
  h_30(state, unused, out_7633295100594421359);
}
void car_H_30(double *state, double *unused, double *out_3177529138290535543) {
  H_30(state, unused, out_3177529138290535543);
}
void car_h_26(double *state, double *unused, double *out_5811775705454284612) {
  h_26(state, unused, out_5811775705454284612);
}
void car_H_26(double *state, double *unused, double *out_7480664522075137436) {
  H_26(state, unused, out_7480664522075137436);
}
void car_h_27(double *state, double *unused, double *out_8851999015780589683) {
  h_27(state, unused, out_8851999015780589683);
}
void car_H_27(double *state, double *unused, double *out_6043263462144746193) {
  H_27(state, unused, out_6043263462144746193);
}
void car_h_29(double *state, double *unused, double *out_1831479801108289492) {
  h_29(state, unused, out_1831479801108289492);
}
void car_H_29(double *state, double *unused, double *out_7756626189014297226) {
  H_29(state, unused, out_7756626189014297226);
}
void car_h_28(double *state, double *unused, double *out_6557959006484629248) {
  h_28(state, unused, out_6557959006484629248);
}
void car_H_28(double *state, double *unused, double *out_5792995917448970975) {
  H_28(state, unused, out_5792995917448970975);
}
void car_h_31(double *state, double *unused, double *out_5357011602177695336) {
  h_31(state, unused, out_5357011602177695336);
}
void car_H_31(double *state, double *unused, double *out_3708515241324120784) {
  H_31(state, unused, out_3708515241324120784);
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
