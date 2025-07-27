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
void err_fun(double *nom_x, double *delta_x, double *out_5056961877498531206) {
   out_5056961877498531206[0] = delta_x[0] + nom_x[0];
   out_5056961877498531206[1] = delta_x[1] + nom_x[1];
   out_5056961877498531206[2] = delta_x[2] + nom_x[2];
   out_5056961877498531206[3] = delta_x[3] + nom_x[3];
   out_5056961877498531206[4] = delta_x[4] + nom_x[4];
   out_5056961877498531206[5] = delta_x[5] + nom_x[5];
   out_5056961877498531206[6] = delta_x[6] + nom_x[6];
   out_5056961877498531206[7] = delta_x[7] + nom_x[7];
   out_5056961877498531206[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_2373746904672379421) {
   out_2373746904672379421[0] = -nom_x[0] + true_x[0];
   out_2373746904672379421[1] = -nom_x[1] + true_x[1];
   out_2373746904672379421[2] = -nom_x[2] + true_x[2];
   out_2373746904672379421[3] = -nom_x[3] + true_x[3];
   out_2373746904672379421[4] = -nom_x[4] + true_x[4];
   out_2373746904672379421[5] = -nom_x[5] + true_x[5];
   out_2373746904672379421[6] = -nom_x[6] + true_x[6];
   out_2373746904672379421[7] = -nom_x[7] + true_x[7];
   out_2373746904672379421[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_6070728261649413941) {
   out_6070728261649413941[0] = 1.0;
   out_6070728261649413941[1] = 0;
   out_6070728261649413941[2] = 0;
   out_6070728261649413941[3] = 0;
   out_6070728261649413941[4] = 0;
   out_6070728261649413941[5] = 0;
   out_6070728261649413941[6] = 0;
   out_6070728261649413941[7] = 0;
   out_6070728261649413941[8] = 0;
   out_6070728261649413941[9] = 0;
   out_6070728261649413941[10] = 1.0;
   out_6070728261649413941[11] = 0;
   out_6070728261649413941[12] = 0;
   out_6070728261649413941[13] = 0;
   out_6070728261649413941[14] = 0;
   out_6070728261649413941[15] = 0;
   out_6070728261649413941[16] = 0;
   out_6070728261649413941[17] = 0;
   out_6070728261649413941[18] = 0;
   out_6070728261649413941[19] = 0;
   out_6070728261649413941[20] = 1.0;
   out_6070728261649413941[21] = 0;
   out_6070728261649413941[22] = 0;
   out_6070728261649413941[23] = 0;
   out_6070728261649413941[24] = 0;
   out_6070728261649413941[25] = 0;
   out_6070728261649413941[26] = 0;
   out_6070728261649413941[27] = 0;
   out_6070728261649413941[28] = 0;
   out_6070728261649413941[29] = 0;
   out_6070728261649413941[30] = 1.0;
   out_6070728261649413941[31] = 0;
   out_6070728261649413941[32] = 0;
   out_6070728261649413941[33] = 0;
   out_6070728261649413941[34] = 0;
   out_6070728261649413941[35] = 0;
   out_6070728261649413941[36] = 0;
   out_6070728261649413941[37] = 0;
   out_6070728261649413941[38] = 0;
   out_6070728261649413941[39] = 0;
   out_6070728261649413941[40] = 1.0;
   out_6070728261649413941[41] = 0;
   out_6070728261649413941[42] = 0;
   out_6070728261649413941[43] = 0;
   out_6070728261649413941[44] = 0;
   out_6070728261649413941[45] = 0;
   out_6070728261649413941[46] = 0;
   out_6070728261649413941[47] = 0;
   out_6070728261649413941[48] = 0;
   out_6070728261649413941[49] = 0;
   out_6070728261649413941[50] = 1.0;
   out_6070728261649413941[51] = 0;
   out_6070728261649413941[52] = 0;
   out_6070728261649413941[53] = 0;
   out_6070728261649413941[54] = 0;
   out_6070728261649413941[55] = 0;
   out_6070728261649413941[56] = 0;
   out_6070728261649413941[57] = 0;
   out_6070728261649413941[58] = 0;
   out_6070728261649413941[59] = 0;
   out_6070728261649413941[60] = 1.0;
   out_6070728261649413941[61] = 0;
   out_6070728261649413941[62] = 0;
   out_6070728261649413941[63] = 0;
   out_6070728261649413941[64] = 0;
   out_6070728261649413941[65] = 0;
   out_6070728261649413941[66] = 0;
   out_6070728261649413941[67] = 0;
   out_6070728261649413941[68] = 0;
   out_6070728261649413941[69] = 0;
   out_6070728261649413941[70] = 1.0;
   out_6070728261649413941[71] = 0;
   out_6070728261649413941[72] = 0;
   out_6070728261649413941[73] = 0;
   out_6070728261649413941[74] = 0;
   out_6070728261649413941[75] = 0;
   out_6070728261649413941[76] = 0;
   out_6070728261649413941[77] = 0;
   out_6070728261649413941[78] = 0;
   out_6070728261649413941[79] = 0;
   out_6070728261649413941[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_4949972851619277991) {
   out_4949972851619277991[0] = state[0];
   out_4949972851619277991[1] = state[1];
   out_4949972851619277991[2] = state[2];
   out_4949972851619277991[3] = state[3];
   out_4949972851619277991[4] = state[4];
   out_4949972851619277991[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8000000000000007*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_4949972851619277991[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_4949972851619277991[7] = state[7];
   out_4949972851619277991[8] = state[8];
}
void F_fun(double *state, double dt, double *out_5415151718457381463) {
   out_5415151718457381463[0] = 1;
   out_5415151718457381463[1] = 0;
   out_5415151718457381463[2] = 0;
   out_5415151718457381463[3] = 0;
   out_5415151718457381463[4] = 0;
   out_5415151718457381463[5] = 0;
   out_5415151718457381463[6] = 0;
   out_5415151718457381463[7] = 0;
   out_5415151718457381463[8] = 0;
   out_5415151718457381463[9] = 0;
   out_5415151718457381463[10] = 1;
   out_5415151718457381463[11] = 0;
   out_5415151718457381463[12] = 0;
   out_5415151718457381463[13] = 0;
   out_5415151718457381463[14] = 0;
   out_5415151718457381463[15] = 0;
   out_5415151718457381463[16] = 0;
   out_5415151718457381463[17] = 0;
   out_5415151718457381463[18] = 0;
   out_5415151718457381463[19] = 0;
   out_5415151718457381463[20] = 1;
   out_5415151718457381463[21] = 0;
   out_5415151718457381463[22] = 0;
   out_5415151718457381463[23] = 0;
   out_5415151718457381463[24] = 0;
   out_5415151718457381463[25] = 0;
   out_5415151718457381463[26] = 0;
   out_5415151718457381463[27] = 0;
   out_5415151718457381463[28] = 0;
   out_5415151718457381463[29] = 0;
   out_5415151718457381463[30] = 1;
   out_5415151718457381463[31] = 0;
   out_5415151718457381463[32] = 0;
   out_5415151718457381463[33] = 0;
   out_5415151718457381463[34] = 0;
   out_5415151718457381463[35] = 0;
   out_5415151718457381463[36] = 0;
   out_5415151718457381463[37] = 0;
   out_5415151718457381463[38] = 0;
   out_5415151718457381463[39] = 0;
   out_5415151718457381463[40] = 1;
   out_5415151718457381463[41] = 0;
   out_5415151718457381463[42] = 0;
   out_5415151718457381463[43] = 0;
   out_5415151718457381463[44] = 0;
   out_5415151718457381463[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_5415151718457381463[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_5415151718457381463[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_5415151718457381463[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_5415151718457381463[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_5415151718457381463[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_5415151718457381463[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_5415151718457381463[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_5415151718457381463[53] = -9.8000000000000007*dt;
   out_5415151718457381463[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_5415151718457381463[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_5415151718457381463[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5415151718457381463[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5415151718457381463[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_5415151718457381463[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_5415151718457381463[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_5415151718457381463[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5415151718457381463[62] = 0;
   out_5415151718457381463[63] = 0;
   out_5415151718457381463[64] = 0;
   out_5415151718457381463[65] = 0;
   out_5415151718457381463[66] = 0;
   out_5415151718457381463[67] = 0;
   out_5415151718457381463[68] = 0;
   out_5415151718457381463[69] = 0;
   out_5415151718457381463[70] = 1;
   out_5415151718457381463[71] = 0;
   out_5415151718457381463[72] = 0;
   out_5415151718457381463[73] = 0;
   out_5415151718457381463[74] = 0;
   out_5415151718457381463[75] = 0;
   out_5415151718457381463[76] = 0;
   out_5415151718457381463[77] = 0;
   out_5415151718457381463[78] = 0;
   out_5415151718457381463[79] = 0;
   out_5415151718457381463[80] = 1;
}
void h_25(double *state, double *unused, double *out_7549992356003039594) {
   out_7549992356003039594[0] = state[6];
}
void H_25(double *state, double *unused, double *out_8227472358946516549) {
   out_8227472358946516549[0] = 0;
   out_8227472358946516549[1] = 0;
   out_8227472358946516549[2] = 0;
   out_8227472358946516549[3] = 0;
   out_8227472358946516549[4] = 0;
   out_8227472358946516549[5] = 0;
   out_8227472358946516549[6] = 1;
   out_8227472358946516549[7] = 0;
   out_8227472358946516549[8] = 0;
}
void h_24(double *state, double *unused, double *out_7967005173366859492) {
   out_7967005173366859492[0] = state[4];
   out_7967005173366859492[1] = state[5];
}
void H_24(double *state, double *unused, double *out_4018239034911873641) {
   out_4018239034911873641[0] = 0;
   out_4018239034911873641[1] = 0;
   out_4018239034911873641[2] = 0;
   out_4018239034911873641[3] = 0;
   out_4018239034911873641[4] = 1;
   out_4018239034911873641[5] = 0;
   out_4018239034911873641[6] = 0;
   out_4018239034911873641[7] = 0;
   out_4018239034911873641[8] = 0;
   out_4018239034911873641[9] = 0;
   out_4018239034911873641[10] = 0;
   out_4018239034911873641[11] = 0;
   out_4018239034911873641[12] = 0;
   out_4018239034911873641[13] = 0;
   out_4018239034911873641[14] = 1;
   out_4018239034911873641[15] = 0;
   out_4018239034911873641[16] = 0;
   out_4018239034911873641[17] = 0;
}
void h_30(double *state, double *unused, double *out_8805063721657517665) {
   out_8805063721657517665[0] = state[4];
}
void H_30(double *state, double *unused, double *out_5691575384635426869) {
   out_5691575384635426869[0] = 0;
   out_5691575384635426869[1] = 0;
   out_5691575384635426869[2] = 0;
   out_5691575384635426869[3] = 0;
   out_5691575384635426869[4] = 1;
   out_5691575384635426869[5] = 0;
   out_5691575384635426869[6] = 0;
   out_5691575384635426869[7] = 0;
   out_5691575384635426869[8] = 0;
}
void h_26(double *state, double *unused, double *out_3803390453996672020) {
   out_3803390453996672020[0] = state[7];
}
void H_26(double *state, double *unused, double *out_6477768395888978843) {
   out_6477768395888978843[0] = 0;
   out_6477768395888978843[1] = 0;
   out_6477768395888978843[2] = 0;
   out_6477768395888978843[3] = 0;
   out_6477768395888978843[4] = 0;
   out_6477768395888978843[5] = 0;
   out_6477768395888978843[6] = 0;
   out_6477768395888978843[7] = 1;
   out_6477768395888978843[8] = 0;
}
void h_27(double *state, double *unused, double *out_7586359806471349341) {
   out_7586359806471349341[0] = state[3];
}
void H_27(double *state, double *unused, double *out_3516812072835001958) {
   out_3516812072835001958[0] = 0;
   out_3516812072835001958[1] = 0;
   out_3516812072835001958[2] = 0;
   out_3516812072835001958[3] = 1;
   out_3516812072835001958[4] = 0;
   out_3516812072835001958[5] = 0;
   out_3516812072835001958[6] = 0;
   out_3516812072835001958[7] = 0;
   out_3516812072835001958[8] = 0;
}
void h_29(double *state, double *unused, double *out_8359393676721164528) {
   out_8359393676721164528[0] = state[1];
}
void H_29(double *state, double *unused, double *out_6201806728949819053) {
   out_6201806728949819053[0] = 0;
   out_6201806728949819053[1] = 1;
   out_6201806728949819053[2] = 0;
   out_6201806728949819053[3] = 0;
   out_6201806728949819053[4] = 0;
   out_6201806728949819053[5] = 0;
   out_6201806728949819053[6] = 0;
   out_6201806728949819053[7] = 0;
   out_6201806728949819053[8] = 0;
}
void h_28(double *state, double *unused, double *out_4981954485662651690) {
   out_4981954485662651690[0] = state[0];
}
void H_28(double *state, double *unused, double *out_1119407711880288479) {
   out_1119407711880288479[0] = 1;
   out_1119407711880288479[1] = 0;
   out_1119407711880288479[2] = 0;
   out_1119407711880288479[3] = 0;
   out_1119407711880288479[4] = 0;
   out_1119407711880288479[5] = 0;
   out_1119407711880288479[6] = 0;
   out_1119407711880288479[7] = 0;
   out_1119407711880288479[8] = 0;
}
void h_31(double *state, double *unused, double *out_2598656411706830949) {
   out_2598656411706830949[0] = state[8];
}
void H_31(double *state, double *unused, double *out_5851560293655627367) {
   out_5851560293655627367[0] = 0;
   out_5851560293655627367[1] = 0;
   out_5851560293655627367[2] = 0;
   out_5851560293655627367[3] = 0;
   out_5851560293655627367[4] = 0;
   out_5851560293655627367[5] = 0;
   out_5851560293655627367[6] = 0;
   out_5851560293655627367[7] = 0;
   out_5851560293655627367[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_5056961877498531206) {
  err_fun(nom_x, delta_x, out_5056961877498531206);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_2373746904672379421) {
  inv_err_fun(nom_x, true_x, out_2373746904672379421);
}
void car_H_mod_fun(double *state, double *out_6070728261649413941) {
  H_mod_fun(state, out_6070728261649413941);
}
void car_f_fun(double *state, double dt, double *out_4949972851619277991) {
  f_fun(state,  dt, out_4949972851619277991);
}
void car_F_fun(double *state, double dt, double *out_5415151718457381463) {
  F_fun(state,  dt, out_5415151718457381463);
}
void car_h_25(double *state, double *unused, double *out_7549992356003039594) {
  h_25(state, unused, out_7549992356003039594);
}
void car_H_25(double *state, double *unused, double *out_8227472358946516549) {
  H_25(state, unused, out_8227472358946516549);
}
void car_h_24(double *state, double *unused, double *out_7967005173366859492) {
  h_24(state, unused, out_7967005173366859492);
}
void car_H_24(double *state, double *unused, double *out_4018239034911873641) {
  H_24(state, unused, out_4018239034911873641);
}
void car_h_30(double *state, double *unused, double *out_8805063721657517665) {
  h_30(state, unused, out_8805063721657517665);
}
void car_H_30(double *state, double *unused, double *out_5691575384635426869) {
  H_30(state, unused, out_5691575384635426869);
}
void car_h_26(double *state, double *unused, double *out_3803390453996672020) {
  h_26(state, unused, out_3803390453996672020);
}
void car_H_26(double *state, double *unused, double *out_6477768395888978843) {
  H_26(state, unused, out_6477768395888978843);
}
void car_h_27(double *state, double *unused, double *out_7586359806471349341) {
  h_27(state, unused, out_7586359806471349341);
}
void car_H_27(double *state, double *unused, double *out_3516812072835001958) {
  H_27(state, unused, out_3516812072835001958);
}
void car_h_29(double *state, double *unused, double *out_8359393676721164528) {
  h_29(state, unused, out_8359393676721164528);
}
void car_H_29(double *state, double *unused, double *out_6201806728949819053) {
  H_29(state, unused, out_6201806728949819053);
}
void car_h_28(double *state, double *unused, double *out_4981954485662651690) {
  h_28(state, unused, out_4981954485662651690);
}
void car_H_28(double *state, double *unused, double *out_1119407711880288479) {
  H_28(state, unused, out_1119407711880288479);
}
void car_h_31(double *state, double *unused, double *out_2598656411706830949) {
  h_31(state, unused, out_2598656411706830949);
}
void car_H_31(double *state, double *unused, double *out_5851560293655627367) {
  H_31(state, unused, out_5851560293655627367);
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
