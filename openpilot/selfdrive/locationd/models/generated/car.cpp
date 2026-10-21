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
void err_fun(double *nom_x, double *delta_x, double *out_8940518425730742921) {
   out_8940518425730742921[0] = delta_x[0] + nom_x[0];
   out_8940518425730742921[1] = delta_x[1] + nom_x[1];
   out_8940518425730742921[2] = delta_x[2] + nom_x[2];
   out_8940518425730742921[3] = delta_x[3] + nom_x[3];
   out_8940518425730742921[4] = delta_x[4] + nom_x[4];
   out_8940518425730742921[5] = delta_x[5] + nom_x[5];
   out_8940518425730742921[6] = delta_x[6] + nom_x[6];
   out_8940518425730742921[7] = delta_x[7] + nom_x[7];
   out_8940518425730742921[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_928144761037053321) {
   out_928144761037053321[0] = -nom_x[0] + true_x[0];
   out_928144761037053321[1] = -nom_x[1] + true_x[1];
   out_928144761037053321[2] = -nom_x[2] + true_x[2];
   out_928144761037053321[3] = -nom_x[3] + true_x[3];
   out_928144761037053321[4] = -nom_x[4] + true_x[4];
   out_928144761037053321[5] = -nom_x[5] + true_x[5];
   out_928144761037053321[6] = -nom_x[6] + true_x[6];
   out_928144761037053321[7] = -nom_x[7] + true_x[7];
   out_928144761037053321[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_9149626453786373432) {
   out_9149626453786373432[0] = 1.0;
   out_9149626453786373432[1] = 0.0;
   out_9149626453786373432[2] = 0.0;
   out_9149626453786373432[3] = 0.0;
   out_9149626453786373432[4] = 0.0;
   out_9149626453786373432[5] = 0.0;
   out_9149626453786373432[6] = 0.0;
   out_9149626453786373432[7] = 0.0;
   out_9149626453786373432[8] = 0.0;
   out_9149626453786373432[9] = 0.0;
   out_9149626453786373432[10] = 1.0;
   out_9149626453786373432[11] = 0.0;
   out_9149626453786373432[12] = 0.0;
   out_9149626453786373432[13] = 0.0;
   out_9149626453786373432[14] = 0.0;
   out_9149626453786373432[15] = 0.0;
   out_9149626453786373432[16] = 0.0;
   out_9149626453786373432[17] = 0.0;
   out_9149626453786373432[18] = 0.0;
   out_9149626453786373432[19] = 0.0;
   out_9149626453786373432[20] = 1.0;
   out_9149626453786373432[21] = 0.0;
   out_9149626453786373432[22] = 0.0;
   out_9149626453786373432[23] = 0.0;
   out_9149626453786373432[24] = 0.0;
   out_9149626453786373432[25] = 0.0;
   out_9149626453786373432[26] = 0.0;
   out_9149626453786373432[27] = 0.0;
   out_9149626453786373432[28] = 0.0;
   out_9149626453786373432[29] = 0.0;
   out_9149626453786373432[30] = 1.0;
   out_9149626453786373432[31] = 0.0;
   out_9149626453786373432[32] = 0.0;
   out_9149626453786373432[33] = 0.0;
   out_9149626453786373432[34] = 0.0;
   out_9149626453786373432[35] = 0.0;
   out_9149626453786373432[36] = 0.0;
   out_9149626453786373432[37] = 0.0;
   out_9149626453786373432[38] = 0.0;
   out_9149626453786373432[39] = 0.0;
   out_9149626453786373432[40] = 1.0;
   out_9149626453786373432[41] = 0.0;
   out_9149626453786373432[42] = 0.0;
   out_9149626453786373432[43] = 0.0;
   out_9149626453786373432[44] = 0.0;
   out_9149626453786373432[45] = 0.0;
   out_9149626453786373432[46] = 0.0;
   out_9149626453786373432[47] = 0.0;
   out_9149626453786373432[48] = 0.0;
   out_9149626453786373432[49] = 0.0;
   out_9149626453786373432[50] = 1.0;
   out_9149626453786373432[51] = 0.0;
   out_9149626453786373432[52] = 0.0;
   out_9149626453786373432[53] = 0.0;
   out_9149626453786373432[54] = 0.0;
   out_9149626453786373432[55] = 0.0;
   out_9149626453786373432[56] = 0.0;
   out_9149626453786373432[57] = 0.0;
   out_9149626453786373432[58] = 0.0;
   out_9149626453786373432[59] = 0.0;
   out_9149626453786373432[60] = 1.0;
   out_9149626453786373432[61] = 0.0;
   out_9149626453786373432[62] = 0.0;
   out_9149626453786373432[63] = 0.0;
   out_9149626453786373432[64] = 0.0;
   out_9149626453786373432[65] = 0.0;
   out_9149626453786373432[66] = 0.0;
   out_9149626453786373432[67] = 0.0;
   out_9149626453786373432[68] = 0.0;
   out_9149626453786373432[69] = 0.0;
   out_9149626453786373432[70] = 1.0;
   out_9149626453786373432[71] = 0.0;
   out_9149626453786373432[72] = 0.0;
   out_9149626453786373432[73] = 0.0;
   out_9149626453786373432[74] = 0.0;
   out_9149626453786373432[75] = 0.0;
   out_9149626453786373432[76] = 0.0;
   out_9149626453786373432[77] = 0.0;
   out_9149626453786373432[78] = 0.0;
   out_9149626453786373432[79] = 0.0;
   out_9149626453786373432[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_8450363153625730423) {
   out_8450363153625730423[0] = state[0];
   out_8450363153625730423[1] = state[1];
   out_8450363153625730423[2] = state[2];
   out_8450363153625730423[3] = state[3];
   out_8450363153625730423[4] = state[4];
   out_8450363153625730423[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8100000000000005*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_8450363153625730423[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_8450363153625730423[7] = state[7];
   out_8450363153625730423[8] = state[8];
}
void F_fun(double *state, double dt, double *out_1973945452823698766) {
   out_1973945452823698766[0] = 1;
   out_1973945452823698766[1] = 0;
   out_1973945452823698766[2] = 0;
   out_1973945452823698766[3] = 0;
   out_1973945452823698766[4] = 0;
   out_1973945452823698766[5] = 0;
   out_1973945452823698766[6] = 0;
   out_1973945452823698766[7] = 0;
   out_1973945452823698766[8] = 0;
   out_1973945452823698766[9] = 0;
   out_1973945452823698766[10] = 1;
   out_1973945452823698766[11] = 0;
   out_1973945452823698766[12] = 0;
   out_1973945452823698766[13] = 0;
   out_1973945452823698766[14] = 0;
   out_1973945452823698766[15] = 0;
   out_1973945452823698766[16] = 0;
   out_1973945452823698766[17] = 0;
   out_1973945452823698766[18] = 0;
   out_1973945452823698766[19] = 0;
   out_1973945452823698766[20] = 1;
   out_1973945452823698766[21] = 0;
   out_1973945452823698766[22] = 0;
   out_1973945452823698766[23] = 0;
   out_1973945452823698766[24] = 0;
   out_1973945452823698766[25] = 0;
   out_1973945452823698766[26] = 0;
   out_1973945452823698766[27] = 0;
   out_1973945452823698766[28] = 0;
   out_1973945452823698766[29] = 0;
   out_1973945452823698766[30] = 1;
   out_1973945452823698766[31] = 0;
   out_1973945452823698766[32] = 0;
   out_1973945452823698766[33] = 0;
   out_1973945452823698766[34] = 0;
   out_1973945452823698766[35] = 0;
   out_1973945452823698766[36] = 0;
   out_1973945452823698766[37] = 0;
   out_1973945452823698766[38] = 0;
   out_1973945452823698766[39] = 0;
   out_1973945452823698766[40] = 1;
   out_1973945452823698766[41] = 0;
   out_1973945452823698766[42] = 0;
   out_1973945452823698766[43] = 0;
   out_1973945452823698766[44] = 0;
   out_1973945452823698766[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_1973945452823698766[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_1973945452823698766[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_1973945452823698766[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_1973945452823698766[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_1973945452823698766[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_1973945452823698766[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_1973945452823698766[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_1973945452823698766[53] = -9.8100000000000005*dt;
   out_1973945452823698766[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_1973945452823698766[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_1973945452823698766[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_1973945452823698766[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_1973945452823698766[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_1973945452823698766[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_1973945452823698766[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_1973945452823698766[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_1973945452823698766[62] = 0;
   out_1973945452823698766[63] = 0;
   out_1973945452823698766[64] = 0;
   out_1973945452823698766[65] = 0;
   out_1973945452823698766[66] = 0;
   out_1973945452823698766[67] = 0;
   out_1973945452823698766[68] = 0;
   out_1973945452823698766[69] = 0;
   out_1973945452823698766[70] = 1;
   out_1973945452823698766[71] = 0;
   out_1973945452823698766[72] = 0;
   out_1973945452823698766[73] = 0;
   out_1973945452823698766[74] = 0;
   out_1973945452823698766[75] = 0;
   out_1973945452823698766[76] = 0;
   out_1973945452823698766[77] = 0;
   out_1973945452823698766[78] = 0;
   out_1973945452823698766[79] = 0;
   out_1973945452823698766[80] = 1;
}
void h_25(double *state, double *unused, double *out_483576256057780874) {
   out_483576256057780874[0] = state[6];
}
void H_25(double *state, double *unused, double *out_6311960329387577407) {
   out_6311960329387577407[0] = 0;
   out_6311960329387577407[1] = 0;
   out_6311960329387577407[2] = 0;
   out_6311960329387577407[3] = 0;
   out_6311960329387577407[4] = 0;
   out_6311960329387577407[5] = 0;
   out_6311960329387577407[6] = 1;
   out_6311960329387577407[7] = 0;
   out_6311960329387577407[8] = 0;
}
void h_24(double *state, double *unused, double *out_4979738999913224721) {
   out_4979738999913224721[0] = state[4];
   out_4979738999913224721[1] = state[5];
}
void H_24(double *state, double *unused, double *out_2329637177989809509) {
   out_2329637177989809509[0] = 0;
   out_2329637177989809509[1] = 0;
   out_2329637177989809509[2] = 0;
   out_2329637177989809509[3] = 0;
   out_2329637177989809509[4] = 1;
   out_2329637177989809509[5] = 0;
   out_2329637177989809509[6] = 0;
   out_2329637177989809509[7] = 0;
   out_2329637177989809509[8] = 0;
   out_2329637177989809509[9] = 0;
   out_2329637177989809509[10] = 0;
   out_2329637177989809509[11] = 0;
   out_2329637177989809509[12] = 0;
   out_2329637177989809509[13] = 0;
   out_2329637177989809509[14] = 1;
   out_2329637177989809509[15] = 0;
   out_2329637177989809509[16] = 0;
   out_2329637177989809509[17] = 0;
}
void h_30(double *state, double *unused, double *out_8246841512878453315) {
   out_8246841512878453315[0] = state[4];
}
void H_30(double *state, double *unused, double *out_8830293287894826034) {
   out_8830293287894826034[0] = 0;
   out_8830293287894826034[1] = 0;
   out_8830293287894826034[2] = 0;
   out_8830293287894826034[3] = 0;
   out_8830293287894826034[4] = 1;
   out_8830293287894826034[5] = 0;
   out_8830293287894826034[6] = 0;
   out_8830293287894826034[7] = 0;
   out_8830293287894826034[8] = 0;
}
void h_26(double *state, double *unused, double *out_1244078809085387536) {
   out_1244078809085387536[0] = state[7];
}
void H_26(double *state, double *unused, double *out_2570457010513521183) {
   out_2570457010513521183[0] = 0;
   out_2570457010513521183[1] = 0;
   out_2570457010513521183[2] = 0;
   out_2570457010513521183[3] = 0;
   out_2570457010513521183[4] = 0;
   out_2570457010513521183[5] = 0;
   out_2570457010513521183[6] = 0;
   out_2570457010513521183[7] = 1;
   out_2570457010513521183[8] = 0;
}
void h_27(double *state, double *unused, double *out_3520362307502113559) {
   out_3520362307502113559[0] = state[3];
}
void H_27(double *state, double *unused, double *out_7392856714630782365) {
   out_7392856714630782365[0] = 0;
   out_7392856714630782365[1] = 0;
   out_7392856714630782365[2] = 0;
   out_7392856714630782365[3] = 1;
   out_7392856714630782365[4] = 0;
   out_7392856714630782365[5] = 0;
   out_7392856714630782365[6] = 0;
   out_7392856714630782365[7] = 0;
   out_7392856714630782365[8] = 0;
}
void h_29(double *state, double *unused, double *out_3245168245217607670) {
   out_3245168245217607670[0] = state[1];
}
void H_29(double *state, double *unused, double *out_9106219441500333398) {
   out_9106219441500333398[0] = 0;
   out_9106219441500333398[1] = 1;
   out_9106219441500333398[2] = 0;
   out_9106219441500333398[3] = 0;
   out_9106219441500333398[4] = 0;
   out_9106219441500333398[5] = 0;
   out_9106219441500333398[6] = 0;
   out_9106219441500333398[7] = 0;
   out_9106219441500333398[8] = 0;
}
void h_28(double *state, double *unused, double *out_4463872160403775994) {
   out_4463872160403775994[0] = state[0];
}
void H_28(double *state, double *unused, double *out_4258125615139687644) {
   out_4258125615139687644[0] = 1;
   out_4258125615139687644[1] = 0;
   out_4258125615139687644[2] = 0;
   out_4258125615139687644[3] = 0;
   out_4258125615139687644[4] = 0;
   out_4258125615139687644[5] = 0;
   out_4258125615139687644[6] = 0;
   out_4258125615139687644[7] = 0;
   out_4258125615139687644[8] = 0;
}
void h_31(double *state, double *unused, double *out_7071404182534076037) {
   out_7071404182534076037[0] = state[8];
}
void H_31(double *state, double *unused, double *out_1944248908280169707) {
   out_1944248908280169707[0] = 0;
   out_1944248908280169707[1] = 0;
   out_1944248908280169707[2] = 0;
   out_1944248908280169707[3] = 0;
   out_1944248908280169707[4] = 0;
   out_1944248908280169707[5] = 0;
   out_1944248908280169707[6] = 0;
   out_1944248908280169707[7] = 0;
   out_1944248908280169707[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_8940518425730742921) {
  err_fun(nom_x, delta_x, out_8940518425730742921);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_928144761037053321) {
  inv_err_fun(nom_x, true_x, out_928144761037053321);
}
void car_H_mod_fun(double *state, double *out_9149626453786373432) {
  H_mod_fun(state, out_9149626453786373432);
}
void car_f_fun(double *state, double dt, double *out_8450363153625730423) {
  f_fun(state,  dt, out_8450363153625730423);
}
void car_F_fun(double *state, double dt, double *out_1973945452823698766) {
  F_fun(state,  dt, out_1973945452823698766);
}
void car_h_25(double *state, double *unused, double *out_483576256057780874) {
  h_25(state, unused, out_483576256057780874);
}
void car_H_25(double *state, double *unused, double *out_6311960329387577407) {
  H_25(state, unused, out_6311960329387577407);
}
void car_h_24(double *state, double *unused, double *out_4979738999913224721) {
  h_24(state, unused, out_4979738999913224721);
}
void car_H_24(double *state, double *unused, double *out_2329637177989809509) {
  H_24(state, unused, out_2329637177989809509);
}
void car_h_30(double *state, double *unused, double *out_8246841512878453315) {
  h_30(state, unused, out_8246841512878453315);
}
void car_H_30(double *state, double *unused, double *out_8830293287894826034) {
  H_30(state, unused, out_8830293287894826034);
}
void car_h_26(double *state, double *unused, double *out_1244078809085387536) {
  h_26(state, unused, out_1244078809085387536);
}
void car_H_26(double *state, double *unused, double *out_2570457010513521183) {
  H_26(state, unused, out_2570457010513521183);
}
void car_h_27(double *state, double *unused, double *out_3520362307502113559) {
  h_27(state, unused, out_3520362307502113559);
}
void car_H_27(double *state, double *unused, double *out_7392856714630782365) {
  H_27(state, unused, out_7392856714630782365);
}
void car_h_29(double *state, double *unused, double *out_3245168245217607670) {
  h_29(state, unused, out_3245168245217607670);
}
void car_H_29(double *state, double *unused, double *out_9106219441500333398) {
  H_29(state, unused, out_9106219441500333398);
}
void car_h_28(double *state, double *unused, double *out_4463872160403775994) {
  h_28(state, unused, out_4463872160403775994);
}
void car_H_28(double *state, double *unused, double *out_4258125615139687644) {
  H_28(state, unused, out_4258125615139687644);
}
void car_h_31(double *state, double *unused, double *out_7071404182534076037) {
  h_31(state, unused, out_7071404182534076037);
}
void car_H_31(double *state, double *unused, double *out_1944248908280169707) {
  H_31(state, unused, out_1944248908280169707);
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
