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
 *                       Code generated with SymPy 1.12                       *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_2634779008644068589) {
   out_2634779008644068589[0] = delta_x[0] + nom_x[0];
   out_2634779008644068589[1] = delta_x[1] + nom_x[1];
   out_2634779008644068589[2] = delta_x[2] + nom_x[2];
   out_2634779008644068589[3] = delta_x[3] + nom_x[3];
   out_2634779008644068589[4] = delta_x[4] + nom_x[4];
   out_2634779008644068589[5] = delta_x[5] + nom_x[5];
   out_2634779008644068589[6] = delta_x[6] + nom_x[6];
   out_2634779008644068589[7] = delta_x[7] + nom_x[7];
   out_2634779008644068589[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_1696869583105116695) {
   out_1696869583105116695[0] = -nom_x[0] + true_x[0];
   out_1696869583105116695[1] = -nom_x[1] + true_x[1];
   out_1696869583105116695[2] = -nom_x[2] + true_x[2];
   out_1696869583105116695[3] = -nom_x[3] + true_x[3];
   out_1696869583105116695[4] = -nom_x[4] + true_x[4];
   out_1696869583105116695[5] = -nom_x[5] + true_x[5];
   out_1696869583105116695[6] = -nom_x[6] + true_x[6];
   out_1696869583105116695[7] = -nom_x[7] + true_x[7];
   out_1696869583105116695[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_1075205047521838002) {
   out_1075205047521838002[0] = 1.0;
   out_1075205047521838002[1] = 0;
   out_1075205047521838002[2] = 0;
   out_1075205047521838002[3] = 0;
   out_1075205047521838002[4] = 0;
   out_1075205047521838002[5] = 0;
   out_1075205047521838002[6] = 0;
   out_1075205047521838002[7] = 0;
   out_1075205047521838002[8] = 0;
   out_1075205047521838002[9] = 0;
   out_1075205047521838002[10] = 1.0;
   out_1075205047521838002[11] = 0;
   out_1075205047521838002[12] = 0;
   out_1075205047521838002[13] = 0;
   out_1075205047521838002[14] = 0;
   out_1075205047521838002[15] = 0;
   out_1075205047521838002[16] = 0;
   out_1075205047521838002[17] = 0;
   out_1075205047521838002[18] = 0;
   out_1075205047521838002[19] = 0;
   out_1075205047521838002[20] = 1.0;
   out_1075205047521838002[21] = 0;
   out_1075205047521838002[22] = 0;
   out_1075205047521838002[23] = 0;
   out_1075205047521838002[24] = 0;
   out_1075205047521838002[25] = 0;
   out_1075205047521838002[26] = 0;
   out_1075205047521838002[27] = 0;
   out_1075205047521838002[28] = 0;
   out_1075205047521838002[29] = 0;
   out_1075205047521838002[30] = 1.0;
   out_1075205047521838002[31] = 0;
   out_1075205047521838002[32] = 0;
   out_1075205047521838002[33] = 0;
   out_1075205047521838002[34] = 0;
   out_1075205047521838002[35] = 0;
   out_1075205047521838002[36] = 0;
   out_1075205047521838002[37] = 0;
   out_1075205047521838002[38] = 0;
   out_1075205047521838002[39] = 0;
   out_1075205047521838002[40] = 1.0;
   out_1075205047521838002[41] = 0;
   out_1075205047521838002[42] = 0;
   out_1075205047521838002[43] = 0;
   out_1075205047521838002[44] = 0;
   out_1075205047521838002[45] = 0;
   out_1075205047521838002[46] = 0;
   out_1075205047521838002[47] = 0;
   out_1075205047521838002[48] = 0;
   out_1075205047521838002[49] = 0;
   out_1075205047521838002[50] = 1.0;
   out_1075205047521838002[51] = 0;
   out_1075205047521838002[52] = 0;
   out_1075205047521838002[53] = 0;
   out_1075205047521838002[54] = 0;
   out_1075205047521838002[55] = 0;
   out_1075205047521838002[56] = 0;
   out_1075205047521838002[57] = 0;
   out_1075205047521838002[58] = 0;
   out_1075205047521838002[59] = 0;
   out_1075205047521838002[60] = 1.0;
   out_1075205047521838002[61] = 0;
   out_1075205047521838002[62] = 0;
   out_1075205047521838002[63] = 0;
   out_1075205047521838002[64] = 0;
   out_1075205047521838002[65] = 0;
   out_1075205047521838002[66] = 0;
   out_1075205047521838002[67] = 0;
   out_1075205047521838002[68] = 0;
   out_1075205047521838002[69] = 0;
   out_1075205047521838002[70] = 1.0;
   out_1075205047521838002[71] = 0;
   out_1075205047521838002[72] = 0;
   out_1075205047521838002[73] = 0;
   out_1075205047521838002[74] = 0;
   out_1075205047521838002[75] = 0;
   out_1075205047521838002[76] = 0;
   out_1075205047521838002[77] = 0;
   out_1075205047521838002[78] = 0;
   out_1075205047521838002[79] = 0;
   out_1075205047521838002[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_855625704405736627) {
   out_855625704405736627[0] = state[0];
   out_855625704405736627[1] = state[1];
   out_855625704405736627[2] = state[2];
   out_855625704405736627[3] = state[3];
   out_855625704405736627[4] = state[4];
   out_855625704405736627[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8000000000000007*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_855625704405736627[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_855625704405736627[7] = state[7];
   out_855625704405736627[8] = state[8];
}
void F_fun(double *state, double dt, double *out_2678194347730504025) {
   out_2678194347730504025[0] = 1;
   out_2678194347730504025[1] = 0;
   out_2678194347730504025[2] = 0;
   out_2678194347730504025[3] = 0;
   out_2678194347730504025[4] = 0;
   out_2678194347730504025[5] = 0;
   out_2678194347730504025[6] = 0;
   out_2678194347730504025[7] = 0;
   out_2678194347730504025[8] = 0;
   out_2678194347730504025[9] = 0;
   out_2678194347730504025[10] = 1;
   out_2678194347730504025[11] = 0;
   out_2678194347730504025[12] = 0;
   out_2678194347730504025[13] = 0;
   out_2678194347730504025[14] = 0;
   out_2678194347730504025[15] = 0;
   out_2678194347730504025[16] = 0;
   out_2678194347730504025[17] = 0;
   out_2678194347730504025[18] = 0;
   out_2678194347730504025[19] = 0;
   out_2678194347730504025[20] = 1;
   out_2678194347730504025[21] = 0;
   out_2678194347730504025[22] = 0;
   out_2678194347730504025[23] = 0;
   out_2678194347730504025[24] = 0;
   out_2678194347730504025[25] = 0;
   out_2678194347730504025[26] = 0;
   out_2678194347730504025[27] = 0;
   out_2678194347730504025[28] = 0;
   out_2678194347730504025[29] = 0;
   out_2678194347730504025[30] = 1;
   out_2678194347730504025[31] = 0;
   out_2678194347730504025[32] = 0;
   out_2678194347730504025[33] = 0;
   out_2678194347730504025[34] = 0;
   out_2678194347730504025[35] = 0;
   out_2678194347730504025[36] = 0;
   out_2678194347730504025[37] = 0;
   out_2678194347730504025[38] = 0;
   out_2678194347730504025[39] = 0;
   out_2678194347730504025[40] = 1;
   out_2678194347730504025[41] = 0;
   out_2678194347730504025[42] = 0;
   out_2678194347730504025[43] = 0;
   out_2678194347730504025[44] = 0;
   out_2678194347730504025[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_2678194347730504025[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_2678194347730504025[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_2678194347730504025[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_2678194347730504025[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_2678194347730504025[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_2678194347730504025[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_2678194347730504025[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_2678194347730504025[53] = -9.8000000000000007*dt;
   out_2678194347730504025[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_2678194347730504025[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_2678194347730504025[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_2678194347730504025[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_2678194347730504025[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_2678194347730504025[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_2678194347730504025[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_2678194347730504025[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_2678194347730504025[62] = 0;
   out_2678194347730504025[63] = 0;
   out_2678194347730504025[64] = 0;
   out_2678194347730504025[65] = 0;
   out_2678194347730504025[66] = 0;
   out_2678194347730504025[67] = 0;
   out_2678194347730504025[68] = 0;
   out_2678194347730504025[69] = 0;
   out_2678194347730504025[70] = 1;
   out_2678194347730504025[71] = 0;
   out_2678194347730504025[72] = 0;
   out_2678194347730504025[73] = 0;
   out_2678194347730504025[74] = 0;
   out_2678194347730504025[75] = 0;
   out_2678194347730504025[76] = 0;
   out_2678194347730504025[77] = 0;
   out_2678194347730504025[78] = 0;
   out_2678194347730504025[79] = 0;
   out_2678194347730504025[80] = 1;
}
void h_25(double *state, double *unused, double *out_5096647656520908803) {
   out_5096647656520908803[0] = state[6];
}
void H_25(double *state, double *unused, double *out_2441479718001600494) {
   out_2441479718001600494[0] = 0;
   out_2441479718001600494[1] = 0;
   out_2441479718001600494[2] = 0;
   out_2441479718001600494[3] = 0;
   out_2441479718001600494[4] = 0;
   out_2441479718001600494[5] = 0;
   out_2441479718001600494[6] = 1;
   out_2441479718001600494[7] = 0;
   out_2441479718001600494[8] = 0;
}
void h_24(double *state, double *unused, double *out_6879438719061737652) {
   out_6879438719061737652[0] = state[4];
   out_6879438719061737652[1] = state[5];
}
void H_24(double *state, double *unused, double *out_1971022235958261770) {
   out_1971022235958261770[0] = 0;
   out_1971022235958261770[1] = 0;
   out_1971022235958261770[2] = 0;
   out_1971022235958261770[3] = 0;
   out_1971022235958261770[4] = 1;
   out_1971022235958261770[5] = 0;
   out_1971022235958261770[6] = 0;
   out_1971022235958261770[7] = 0;
   out_1971022235958261770[8] = 0;
   out_1971022235958261770[9] = 0;
   out_1971022235958261770[10] = 0;
   out_1971022235958261770[11] = 0;
   out_1971022235958261770[12] = 0;
   out_1971022235958261770[13] = 0;
   out_1971022235958261770[14] = 1;
   out_1971022235958261770[15] = 0;
   out_1971022235958261770[16] = 0;
   out_1971022235958261770[17] = 0;
}
void h_30(double *state, double *unused, double *out_1070466383450225500) {
   out_1070466383450225500[0] = state[4];
}
void H_30(double *state, double *unused, double *out_2086216612126007704) {
   out_2086216612126007704[0] = 0;
   out_2086216612126007704[1] = 0;
   out_2086216612126007704[2] = 0;
   out_2086216612126007704[3] = 0;
   out_2086216612126007704[4] = 1;
   out_2086216612126007704[5] = 0;
   out_2086216612126007704[6] = 0;
   out_2086216612126007704[7] = 0;
   out_2086216612126007704[8] = 0;
}
void h_26(double *state, double *unused, double *out_6819077632945089670) {
   out_6819077632945089670[0] = state[7];
}
void H_26(double *state, double *unused, double *out_1300023600872455730) {
   out_1300023600872455730[0] = 0;
   out_1300023600872455730[1] = 0;
   out_1300023600872455730[2] = 0;
   out_1300023600872455730[3] = 0;
   out_1300023600872455730[4] = 0;
   out_1300023600872455730[5] = 0;
   out_1300023600872455730[6] = 0;
   out_1300023600872455730[7] = 1;
   out_1300023600872455730[8] = 0;
}
void h_27(double *state, double *unused, double *out_3036108280470412349) {
   out_3036108280470412349[0] = state[3];
}
void H_27(double *state, double *unused, double *out_137377459057935513) {
   out_137377459057935513[0] = 0;
   out_137377459057935513[1] = 0;
   out_137377459057935513[2] = 0;
   out_137377459057935513[3] = 1;
   out_137377459057935513[4] = 0;
   out_137377459057935513[5] = 0;
   out_137377459057935513[6] = 0;
   out_137377459057935513[7] = 0;
   out_137377459057935513[8] = 0;
}
void h_29(double *state, double *unused, double *out_8073229087243291279) {
   out_8073229087243291279[0] = state[1];
}
void H_29(double *state, double *unused, double *out_1575985267811615520) {
   out_1575985267811615520[0] = 0;
   out_1575985267811615520[1] = 1;
   out_1575985267811615520[2] = 0;
   out_1575985267811615520[3] = 0;
   out_1575985267811615520[4] = 0;
   out_1575985267811615520[5] = 0;
   out_1575985267811615520[6] = 0;
   out_1575985267811615520[7] = 0;
   out_1575985267811615520[8] = 0;
}
void h_28(double *state, double *unused, double *out_4613236892840921993) {
   out_4613236892840921993[0] = state[0];
}
void H_28(double *state, double *unused, double *out_6658384284881146094) {
   out_6658384284881146094[0] = 1;
   out_6658384284881146094[1] = 0;
   out_6658384284881146094[2] = 0;
   out_6658384284881146094[3] = 0;
   out_6658384284881146094[4] = 0;
   out_6658384284881146094[5] = 0;
   out_6658384284881146094[6] = 0;
   out_6658384284881146094[7] = 0;
   out_6658384284881146094[8] = 0;
}
void h_31(double *state, double *unused, double *out_6547279049149791970) {
   out_6547279049149791970[0] = state[8];
}
void H_31(double *state, double *unused, double *out_2472125679878560922) {
   out_2472125679878560922[0] = 0;
   out_2472125679878560922[1] = 0;
   out_2472125679878560922[2] = 0;
   out_2472125679878560922[3] = 0;
   out_2472125679878560922[4] = 0;
   out_2472125679878560922[5] = 0;
   out_2472125679878560922[6] = 0;
   out_2472125679878560922[7] = 0;
   out_2472125679878560922[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_2634779008644068589) {
  err_fun(nom_x, delta_x, out_2634779008644068589);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_1696869583105116695) {
  inv_err_fun(nom_x, true_x, out_1696869583105116695);
}
void car_H_mod_fun(double *state, double *out_1075205047521838002) {
  H_mod_fun(state, out_1075205047521838002);
}
void car_f_fun(double *state, double dt, double *out_855625704405736627) {
  f_fun(state,  dt, out_855625704405736627);
}
void car_F_fun(double *state, double dt, double *out_2678194347730504025) {
  F_fun(state,  dt, out_2678194347730504025);
}
void car_h_25(double *state, double *unused, double *out_5096647656520908803) {
  h_25(state, unused, out_5096647656520908803);
}
void car_H_25(double *state, double *unused, double *out_2441479718001600494) {
  H_25(state, unused, out_2441479718001600494);
}
void car_h_24(double *state, double *unused, double *out_6879438719061737652) {
  h_24(state, unused, out_6879438719061737652);
}
void car_H_24(double *state, double *unused, double *out_1971022235958261770) {
  H_24(state, unused, out_1971022235958261770);
}
void car_h_30(double *state, double *unused, double *out_1070466383450225500) {
  h_30(state, unused, out_1070466383450225500);
}
void car_H_30(double *state, double *unused, double *out_2086216612126007704) {
  H_30(state, unused, out_2086216612126007704);
}
void car_h_26(double *state, double *unused, double *out_6819077632945089670) {
  h_26(state, unused, out_6819077632945089670);
}
void car_H_26(double *state, double *unused, double *out_1300023600872455730) {
  H_26(state, unused, out_1300023600872455730);
}
void car_h_27(double *state, double *unused, double *out_3036108280470412349) {
  h_27(state, unused, out_3036108280470412349);
}
void car_H_27(double *state, double *unused, double *out_137377459057935513) {
  H_27(state, unused, out_137377459057935513);
}
void car_h_29(double *state, double *unused, double *out_8073229087243291279) {
  h_29(state, unused, out_8073229087243291279);
}
void car_H_29(double *state, double *unused, double *out_1575985267811615520) {
  H_29(state, unused, out_1575985267811615520);
}
void car_h_28(double *state, double *unused, double *out_4613236892840921993) {
  h_28(state, unused, out_4613236892840921993);
}
void car_H_28(double *state, double *unused, double *out_6658384284881146094) {
  H_28(state, unused, out_6658384284881146094);
}
void car_h_31(double *state, double *unused, double *out_6547279049149791970) {
  h_31(state, unused, out_6547279049149791970);
}
void car_H_31(double *state, double *unused, double *out_2472125679878560922) {
  H_31(state, unused, out_2472125679878560922);
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
