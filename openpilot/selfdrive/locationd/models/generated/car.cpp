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
void err_fun(double *nom_x, double *delta_x, double *out_4603381077007055755) {
   out_4603381077007055755[0] = delta_x[0] + nom_x[0];
   out_4603381077007055755[1] = delta_x[1] + nom_x[1];
   out_4603381077007055755[2] = delta_x[2] + nom_x[2];
   out_4603381077007055755[3] = delta_x[3] + nom_x[3];
   out_4603381077007055755[4] = delta_x[4] + nom_x[4];
   out_4603381077007055755[5] = delta_x[5] + nom_x[5];
   out_4603381077007055755[6] = delta_x[6] + nom_x[6];
   out_4603381077007055755[7] = delta_x[7] + nom_x[7];
   out_4603381077007055755[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_5273785100438362063) {
   out_5273785100438362063[0] = -nom_x[0] + true_x[0];
   out_5273785100438362063[1] = -nom_x[1] + true_x[1];
   out_5273785100438362063[2] = -nom_x[2] + true_x[2];
   out_5273785100438362063[3] = -nom_x[3] + true_x[3];
   out_5273785100438362063[4] = -nom_x[4] + true_x[4];
   out_5273785100438362063[5] = -nom_x[5] + true_x[5];
   out_5273785100438362063[6] = -nom_x[6] + true_x[6];
   out_5273785100438362063[7] = -nom_x[7] + true_x[7];
   out_5273785100438362063[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_8414806225118962866) {
   out_8414806225118962866[0] = 1.0;
   out_8414806225118962866[1] = 0.0;
   out_8414806225118962866[2] = 0.0;
   out_8414806225118962866[3] = 0.0;
   out_8414806225118962866[4] = 0.0;
   out_8414806225118962866[5] = 0.0;
   out_8414806225118962866[6] = 0.0;
   out_8414806225118962866[7] = 0.0;
   out_8414806225118962866[8] = 0.0;
   out_8414806225118962866[9] = 0.0;
   out_8414806225118962866[10] = 1.0;
   out_8414806225118962866[11] = 0.0;
   out_8414806225118962866[12] = 0.0;
   out_8414806225118962866[13] = 0.0;
   out_8414806225118962866[14] = 0.0;
   out_8414806225118962866[15] = 0.0;
   out_8414806225118962866[16] = 0.0;
   out_8414806225118962866[17] = 0.0;
   out_8414806225118962866[18] = 0.0;
   out_8414806225118962866[19] = 0.0;
   out_8414806225118962866[20] = 1.0;
   out_8414806225118962866[21] = 0.0;
   out_8414806225118962866[22] = 0.0;
   out_8414806225118962866[23] = 0.0;
   out_8414806225118962866[24] = 0.0;
   out_8414806225118962866[25] = 0.0;
   out_8414806225118962866[26] = 0.0;
   out_8414806225118962866[27] = 0.0;
   out_8414806225118962866[28] = 0.0;
   out_8414806225118962866[29] = 0.0;
   out_8414806225118962866[30] = 1.0;
   out_8414806225118962866[31] = 0.0;
   out_8414806225118962866[32] = 0.0;
   out_8414806225118962866[33] = 0.0;
   out_8414806225118962866[34] = 0.0;
   out_8414806225118962866[35] = 0.0;
   out_8414806225118962866[36] = 0.0;
   out_8414806225118962866[37] = 0.0;
   out_8414806225118962866[38] = 0.0;
   out_8414806225118962866[39] = 0.0;
   out_8414806225118962866[40] = 1.0;
   out_8414806225118962866[41] = 0.0;
   out_8414806225118962866[42] = 0.0;
   out_8414806225118962866[43] = 0.0;
   out_8414806225118962866[44] = 0.0;
   out_8414806225118962866[45] = 0.0;
   out_8414806225118962866[46] = 0.0;
   out_8414806225118962866[47] = 0.0;
   out_8414806225118962866[48] = 0.0;
   out_8414806225118962866[49] = 0.0;
   out_8414806225118962866[50] = 1.0;
   out_8414806225118962866[51] = 0.0;
   out_8414806225118962866[52] = 0.0;
   out_8414806225118962866[53] = 0.0;
   out_8414806225118962866[54] = 0.0;
   out_8414806225118962866[55] = 0.0;
   out_8414806225118962866[56] = 0.0;
   out_8414806225118962866[57] = 0.0;
   out_8414806225118962866[58] = 0.0;
   out_8414806225118962866[59] = 0.0;
   out_8414806225118962866[60] = 1.0;
   out_8414806225118962866[61] = 0.0;
   out_8414806225118962866[62] = 0.0;
   out_8414806225118962866[63] = 0.0;
   out_8414806225118962866[64] = 0.0;
   out_8414806225118962866[65] = 0.0;
   out_8414806225118962866[66] = 0.0;
   out_8414806225118962866[67] = 0.0;
   out_8414806225118962866[68] = 0.0;
   out_8414806225118962866[69] = 0.0;
   out_8414806225118962866[70] = 1.0;
   out_8414806225118962866[71] = 0.0;
   out_8414806225118962866[72] = 0.0;
   out_8414806225118962866[73] = 0.0;
   out_8414806225118962866[74] = 0.0;
   out_8414806225118962866[75] = 0.0;
   out_8414806225118962866[76] = 0.0;
   out_8414806225118962866[77] = 0.0;
   out_8414806225118962866[78] = 0.0;
   out_8414806225118962866[79] = 0.0;
   out_8414806225118962866[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_8599373732669577078) {
   out_8599373732669577078[0] = state[0];
   out_8599373732669577078[1] = state[1];
   out_8599373732669577078[2] = state[2];
   out_8599373732669577078[3] = state[3];
   out_8599373732669577078[4] = state[4];
   out_8599373732669577078[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8100000000000005*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_8599373732669577078[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_8599373732669577078[7] = state[7];
   out_8599373732669577078[8] = state[8];
}
void F_fun(double *state, double dt, double *out_5796812835580827553) {
   out_5796812835580827553[0] = 1;
   out_5796812835580827553[1] = 0;
   out_5796812835580827553[2] = 0;
   out_5796812835580827553[3] = 0;
   out_5796812835580827553[4] = 0;
   out_5796812835580827553[5] = 0;
   out_5796812835580827553[6] = 0;
   out_5796812835580827553[7] = 0;
   out_5796812835580827553[8] = 0;
   out_5796812835580827553[9] = 0;
   out_5796812835580827553[10] = 1;
   out_5796812835580827553[11] = 0;
   out_5796812835580827553[12] = 0;
   out_5796812835580827553[13] = 0;
   out_5796812835580827553[14] = 0;
   out_5796812835580827553[15] = 0;
   out_5796812835580827553[16] = 0;
   out_5796812835580827553[17] = 0;
   out_5796812835580827553[18] = 0;
   out_5796812835580827553[19] = 0;
   out_5796812835580827553[20] = 1;
   out_5796812835580827553[21] = 0;
   out_5796812835580827553[22] = 0;
   out_5796812835580827553[23] = 0;
   out_5796812835580827553[24] = 0;
   out_5796812835580827553[25] = 0;
   out_5796812835580827553[26] = 0;
   out_5796812835580827553[27] = 0;
   out_5796812835580827553[28] = 0;
   out_5796812835580827553[29] = 0;
   out_5796812835580827553[30] = 1;
   out_5796812835580827553[31] = 0;
   out_5796812835580827553[32] = 0;
   out_5796812835580827553[33] = 0;
   out_5796812835580827553[34] = 0;
   out_5796812835580827553[35] = 0;
   out_5796812835580827553[36] = 0;
   out_5796812835580827553[37] = 0;
   out_5796812835580827553[38] = 0;
   out_5796812835580827553[39] = 0;
   out_5796812835580827553[40] = 1;
   out_5796812835580827553[41] = 0;
   out_5796812835580827553[42] = 0;
   out_5796812835580827553[43] = 0;
   out_5796812835580827553[44] = 0;
   out_5796812835580827553[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_5796812835580827553[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_5796812835580827553[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_5796812835580827553[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_5796812835580827553[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_5796812835580827553[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_5796812835580827553[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_5796812835580827553[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_5796812835580827553[53] = -9.8100000000000005*dt;
   out_5796812835580827553[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_5796812835580827553[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_5796812835580827553[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5796812835580827553[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5796812835580827553[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_5796812835580827553[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_5796812835580827553[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_5796812835580827553[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5796812835580827553[62] = 0;
   out_5796812835580827553[63] = 0;
   out_5796812835580827553[64] = 0;
   out_5796812835580827553[65] = 0;
   out_5796812835580827553[66] = 0;
   out_5796812835580827553[67] = 0;
   out_5796812835580827553[68] = 0;
   out_5796812835580827553[69] = 0;
   out_5796812835580827553[70] = 1;
   out_5796812835580827553[71] = 0;
   out_5796812835580827553[72] = 0;
   out_5796812835580827553[73] = 0;
   out_5796812835580827553[74] = 0;
   out_5796812835580827553[75] = 0;
   out_5796812835580827553[76] = 0;
   out_5796812835580827553[77] = 0;
   out_5796812835580827553[78] = 0;
   out_5796812835580827553[79] = 0;
   out_5796812835580827553[80] = 1;
}
void h_25(double *state, double *unused, double *out_5539409521994919118) {
   out_5539409521994919118[0] = state[6];
}
void H_25(double *state, double *unused, double *out_838749622678351513) {
   out_838749622678351513[0] = 0;
   out_838749622678351513[1] = 0;
   out_838749622678351513[2] = 0;
   out_838749622678351513[3] = 0;
   out_838749622678351513[4] = 0;
   out_838749622678351513[5] = 0;
   out_838749622678351513[6] = 1;
   out_838749622678351513[7] = 0;
   out_838749622678351513[8] = 0;
}
void h_24(double *state, double *unused, double *out_8112980868095491689) {
   out_8112980868095491689[0] = state[4];
   out_8112980868095491689[1] = state[5];
}
void H_24(double *state, double *unused, double *out_8379929264962004878) {
   out_8379929264962004878[0] = 0;
   out_8379929264962004878[1] = 0;
   out_8379929264962004878[2] = 0;
   out_8379929264962004878[3] = 0;
   out_8379929264962004878[4] = 1;
   out_8379929264962004878[5] = 0;
   out_8379929264962004878[6] = 0;
   out_8379929264962004878[7] = 0;
   out_8379929264962004878[8] = 0;
   out_8379929264962004878[9] = 0;
   out_8379929264962004878[10] = 0;
   out_8379929264962004878[11] = 0;
   out_8379929264962004878[12] = 0;
   out_8379929264962004878[13] = 0;
   out_8379929264962004878[14] = 1;
   out_8379929264962004878[15] = 0;
   out_8379929264962004878[16] = 0;
   out_8379929264962004878[17] = 0;
}
void h_30(double *state, double *unused, double *out_5264215459710413229) {
   out_5264215459710413229[0] = state[4];
}
void H_30(double *state, double *unused, double *out_3688946707449256685) {
   out_3688946707449256685[0] = 0;
   out_3688946707449256685[1] = 0;
   out_3688946707449256685[2] = 0;
   out_3688946707449256685[3] = 0;
   out_3688946707449256685[4] = 1;
   out_3688946707449256685[5] = 0;
   out_3688946707449256685[6] = 0;
   out_3688946707449256685[7] = 0;
   out_3688946707449256685[8] = 0;
}
void h_26(double *state, double *unused, double *out_2659810138901715578) {
   out_2659810138901715578[0] = state[7];
}
void H_26(double *state, double *unused, double *out_2902753696195704711) {
   out_2902753696195704711[0] = 0;
   out_2902753696195704711[1] = 0;
   out_2902753696195704711[2] = 0;
   out_2902753696195704711[3] = 0;
   out_2902753696195704711[4] = 0;
   out_2902753696195704711[5] = 0;
   out_2902753696195704711[6] = 0;
   out_2902753696195704711[7] = 1;
   out_2902753696195704711[8] = 0;
}
void h_27(double *state, double *unused, double *out_2109935651316415224) {
   out_2109935651316415224[0] = state[3];
}
void H_27(double *state, double *unused, double *out_1465352636265313468) {
   out_1465352636265313468[0] = 0;
   out_1465352636265313468[1] = 0;
   out_1465352636265313468[2] = 0;
   out_1465352636265313468[3] = 1;
   out_1465352636265313468[4] = 0;
   out_1465352636265313468[5] = 0;
   out_1465352636265313468[6] = 0;
   out_1465352636265313468[7] = 0;
   out_1465352636265313468[8] = 0;
}
void h_29(double *state, double *unused, double *out_1988367646962956640) {
   out_1988367646962956640[0] = state[1];
}
void H_29(double *state, double *unused, double *out_3178715363134864501) {
   out_3178715363134864501[0] = 0;
   out_3178715363134864501[1] = 1;
   out_3178715363134864501[2] = 0;
   out_3178715363134864501[3] = 0;
   out_3178715363134864501[4] = 0;
   out_3178715363134864501[5] = 0;
   out_3178715363134864501[6] = 0;
   out_3178715363134864501[7] = 0;
   out_3178715363134864501[8] = 0;
}
void h_28(double *state, double *unused, double *out_1166425798414752789) {
   out_1166425798414752789[0] = state[0];
}
void H_28(double *state, double *unused, double *out_8261114380204395075) {
   out_8261114380204395075[0] = 1;
   out_8261114380204395075[1] = 0;
   out_8261114380204395075[2] = 0;
   out_8261114380204395075[3] = 0;
   out_8261114380204395075[4] = 0;
   out_8261114380204395075[5] = 0;
   out_8261114380204395075[6] = 0;
   out_8261114380204395075[7] = 0;
   out_8261114380204395075[8] = 0;
}
void h_31(double *state, double *unused, double *out_9176859050248941468) {
   out_9176859050248941468[0] = state[8];
}
void H_31(double *state, double *unused, double *out_869395584555311941) {
   out_869395584555311941[0] = 0;
   out_869395584555311941[1] = 0;
   out_869395584555311941[2] = 0;
   out_869395584555311941[3] = 0;
   out_869395584555311941[4] = 0;
   out_869395584555311941[5] = 0;
   out_869395584555311941[6] = 0;
   out_869395584555311941[7] = 0;
   out_869395584555311941[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_4603381077007055755) {
  err_fun(nom_x, delta_x, out_4603381077007055755);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_5273785100438362063) {
  inv_err_fun(nom_x, true_x, out_5273785100438362063);
}
void car_H_mod_fun(double *state, double *out_8414806225118962866) {
  H_mod_fun(state, out_8414806225118962866);
}
void car_f_fun(double *state, double dt, double *out_8599373732669577078) {
  f_fun(state,  dt, out_8599373732669577078);
}
void car_F_fun(double *state, double dt, double *out_5796812835580827553) {
  F_fun(state,  dt, out_5796812835580827553);
}
void car_h_25(double *state, double *unused, double *out_5539409521994919118) {
  h_25(state, unused, out_5539409521994919118);
}
void car_H_25(double *state, double *unused, double *out_838749622678351513) {
  H_25(state, unused, out_838749622678351513);
}
void car_h_24(double *state, double *unused, double *out_8112980868095491689) {
  h_24(state, unused, out_8112980868095491689);
}
void car_H_24(double *state, double *unused, double *out_8379929264962004878) {
  H_24(state, unused, out_8379929264962004878);
}
void car_h_30(double *state, double *unused, double *out_5264215459710413229) {
  h_30(state, unused, out_5264215459710413229);
}
void car_H_30(double *state, double *unused, double *out_3688946707449256685) {
  H_30(state, unused, out_3688946707449256685);
}
void car_h_26(double *state, double *unused, double *out_2659810138901715578) {
  h_26(state, unused, out_2659810138901715578);
}
void car_H_26(double *state, double *unused, double *out_2902753696195704711) {
  H_26(state, unused, out_2902753696195704711);
}
void car_h_27(double *state, double *unused, double *out_2109935651316415224) {
  h_27(state, unused, out_2109935651316415224);
}
void car_H_27(double *state, double *unused, double *out_1465352636265313468) {
  H_27(state, unused, out_1465352636265313468);
}
void car_h_29(double *state, double *unused, double *out_1988367646962956640) {
  h_29(state, unused, out_1988367646962956640);
}
void car_H_29(double *state, double *unused, double *out_3178715363134864501) {
  H_29(state, unused, out_3178715363134864501);
}
void car_h_28(double *state, double *unused, double *out_1166425798414752789) {
  h_28(state, unused, out_1166425798414752789);
}
void car_H_28(double *state, double *unused, double *out_8261114380204395075) {
  H_28(state, unused, out_8261114380204395075);
}
void car_h_31(double *state, double *unused, double *out_9176859050248941468) {
  h_31(state, unused, out_9176859050248941468);
}
void car_H_31(double *state, double *unused, double *out_869395584555311941) {
  H_31(state, unused, out_869395584555311941);
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
