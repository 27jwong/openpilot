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
void err_fun(double *nom_x, double *delta_x, double *out_6734524038688205591) {
   out_6734524038688205591[0] = delta_x[0] + nom_x[0];
   out_6734524038688205591[1] = delta_x[1] + nom_x[1];
   out_6734524038688205591[2] = delta_x[2] + nom_x[2];
   out_6734524038688205591[3] = delta_x[3] + nom_x[3];
   out_6734524038688205591[4] = delta_x[4] + nom_x[4];
   out_6734524038688205591[5] = delta_x[5] + nom_x[5];
   out_6734524038688205591[6] = delta_x[6] + nom_x[6];
   out_6734524038688205591[7] = delta_x[7] + nom_x[7];
   out_6734524038688205591[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_6126652950877688806) {
   out_6126652950877688806[0] = -nom_x[0] + true_x[0];
   out_6126652950877688806[1] = -nom_x[1] + true_x[1];
   out_6126652950877688806[2] = -nom_x[2] + true_x[2];
   out_6126652950877688806[3] = -nom_x[3] + true_x[3];
   out_6126652950877688806[4] = -nom_x[4] + true_x[4];
   out_6126652950877688806[5] = -nom_x[5] + true_x[5];
   out_6126652950877688806[6] = -nom_x[6] + true_x[6];
   out_6126652950877688806[7] = -nom_x[7] + true_x[7];
   out_6126652950877688806[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_2786804655155788198) {
   out_2786804655155788198[0] = 1.0;
   out_2786804655155788198[1] = 0.0;
   out_2786804655155788198[2] = 0.0;
   out_2786804655155788198[3] = 0.0;
   out_2786804655155788198[4] = 0.0;
   out_2786804655155788198[5] = 0.0;
   out_2786804655155788198[6] = 0.0;
   out_2786804655155788198[7] = 0.0;
   out_2786804655155788198[8] = 0.0;
   out_2786804655155788198[9] = 0.0;
   out_2786804655155788198[10] = 1.0;
   out_2786804655155788198[11] = 0.0;
   out_2786804655155788198[12] = 0.0;
   out_2786804655155788198[13] = 0.0;
   out_2786804655155788198[14] = 0.0;
   out_2786804655155788198[15] = 0.0;
   out_2786804655155788198[16] = 0.0;
   out_2786804655155788198[17] = 0.0;
   out_2786804655155788198[18] = 0.0;
   out_2786804655155788198[19] = 0.0;
   out_2786804655155788198[20] = 1.0;
   out_2786804655155788198[21] = 0.0;
   out_2786804655155788198[22] = 0.0;
   out_2786804655155788198[23] = 0.0;
   out_2786804655155788198[24] = 0.0;
   out_2786804655155788198[25] = 0.0;
   out_2786804655155788198[26] = 0.0;
   out_2786804655155788198[27] = 0.0;
   out_2786804655155788198[28] = 0.0;
   out_2786804655155788198[29] = 0.0;
   out_2786804655155788198[30] = 1.0;
   out_2786804655155788198[31] = 0.0;
   out_2786804655155788198[32] = 0.0;
   out_2786804655155788198[33] = 0.0;
   out_2786804655155788198[34] = 0.0;
   out_2786804655155788198[35] = 0.0;
   out_2786804655155788198[36] = 0.0;
   out_2786804655155788198[37] = 0.0;
   out_2786804655155788198[38] = 0.0;
   out_2786804655155788198[39] = 0.0;
   out_2786804655155788198[40] = 1.0;
   out_2786804655155788198[41] = 0.0;
   out_2786804655155788198[42] = 0.0;
   out_2786804655155788198[43] = 0.0;
   out_2786804655155788198[44] = 0.0;
   out_2786804655155788198[45] = 0.0;
   out_2786804655155788198[46] = 0.0;
   out_2786804655155788198[47] = 0.0;
   out_2786804655155788198[48] = 0.0;
   out_2786804655155788198[49] = 0.0;
   out_2786804655155788198[50] = 1.0;
   out_2786804655155788198[51] = 0.0;
   out_2786804655155788198[52] = 0.0;
   out_2786804655155788198[53] = 0.0;
   out_2786804655155788198[54] = 0.0;
   out_2786804655155788198[55] = 0.0;
   out_2786804655155788198[56] = 0.0;
   out_2786804655155788198[57] = 0.0;
   out_2786804655155788198[58] = 0.0;
   out_2786804655155788198[59] = 0.0;
   out_2786804655155788198[60] = 1.0;
   out_2786804655155788198[61] = 0.0;
   out_2786804655155788198[62] = 0.0;
   out_2786804655155788198[63] = 0.0;
   out_2786804655155788198[64] = 0.0;
   out_2786804655155788198[65] = 0.0;
   out_2786804655155788198[66] = 0.0;
   out_2786804655155788198[67] = 0.0;
   out_2786804655155788198[68] = 0.0;
   out_2786804655155788198[69] = 0.0;
   out_2786804655155788198[70] = 1.0;
   out_2786804655155788198[71] = 0.0;
   out_2786804655155788198[72] = 0.0;
   out_2786804655155788198[73] = 0.0;
   out_2786804655155788198[74] = 0.0;
   out_2786804655155788198[75] = 0.0;
   out_2786804655155788198[76] = 0.0;
   out_2786804655155788198[77] = 0.0;
   out_2786804655155788198[78] = 0.0;
   out_2786804655155788198[79] = 0.0;
   out_2786804655155788198[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_2423327080634798879) {
   out_2423327080634798879[0] = state[0];
   out_2423327080634798879[1] = state[1];
   out_2423327080634798879[2] = state[2];
   out_2423327080634798879[3] = state[3];
   out_2423327080634798879[4] = state[4];
   out_2423327080634798879[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8100000000000005*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_2423327080634798879[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_2423327080634798879[7] = state[7];
   out_2423327080634798879[8] = state[8];
}
void F_fun(double *state, double dt, double *out_3740104839002625692) {
   out_3740104839002625692[0] = 1;
   out_3740104839002625692[1] = 0;
   out_3740104839002625692[2] = 0;
   out_3740104839002625692[3] = 0;
   out_3740104839002625692[4] = 0;
   out_3740104839002625692[5] = 0;
   out_3740104839002625692[6] = 0;
   out_3740104839002625692[7] = 0;
   out_3740104839002625692[8] = 0;
   out_3740104839002625692[9] = 0;
   out_3740104839002625692[10] = 1;
   out_3740104839002625692[11] = 0;
   out_3740104839002625692[12] = 0;
   out_3740104839002625692[13] = 0;
   out_3740104839002625692[14] = 0;
   out_3740104839002625692[15] = 0;
   out_3740104839002625692[16] = 0;
   out_3740104839002625692[17] = 0;
   out_3740104839002625692[18] = 0;
   out_3740104839002625692[19] = 0;
   out_3740104839002625692[20] = 1;
   out_3740104839002625692[21] = 0;
   out_3740104839002625692[22] = 0;
   out_3740104839002625692[23] = 0;
   out_3740104839002625692[24] = 0;
   out_3740104839002625692[25] = 0;
   out_3740104839002625692[26] = 0;
   out_3740104839002625692[27] = 0;
   out_3740104839002625692[28] = 0;
   out_3740104839002625692[29] = 0;
   out_3740104839002625692[30] = 1;
   out_3740104839002625692[31] = 0;
   out_3740104839002625692[32] = 0;
   out_3740104839002625692[33] = 0;
   out_3740104839002625692[34] = 0;
   out_3740104839002625692[35] = 0;
   out_3740104839002625692[36] = 0;
   out_3740104839002625692[37] = 0;
   out_3740104839002625692[38] = 0;
   out_3740104839002625692[39] = 0;
   out_3740104839002625692[40] = 1;
   out_3740104839002625692[41] = 0;
   out_3740104839002625692[42] = 0;
   out_3740104839002625692[43] = 0;
   out_3740104839002625692[44] = 0;
   out_3740104839002625692[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_3740104839002625692[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_3740104839002625692[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_3740104839002625692[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_3740104839002625692[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_3740104839002625692[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_3740104839002625692[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_3740104839002625692[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_3740104839002625692[53] = -9.8100000000000005*dt;
   out_3740104839002625692[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_3740104839002625692[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_3740104839002625692[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3740104839002625692[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3740104839002625692[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_3740104839002625692[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_3740104839002625692[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_3740104839002625692[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3740104839002625692[62] = 0;
   out_3740104839002625692[63] = 0;
   out_3740104839002625692[64] = 0;
   out_3740104839002625692[65] = 0;
   out_3740104839002625692[66] = 0;
   out_3740104839002625692[67] = 0;
   out_3740104839002625692[68] = 0;
   out_3740104839002625692[69] = 0;
   out_3740104839002625692[70] = 1;
   out_3740104839002625692[71] = 0;
   out_3740104839002625692[72] = 0;
   out_3740104839002625692[73] = 0;
   out_3740104839002625692[74] = 0;
   out_3740104839002625692[75] = 0;
   out_3740104839002625692[76] = 0;
   out_3740104839002625692[77] = 0;
   out_3740104839002625692[78] = 0;
   out_3740104839002625692[79] = 0;
   out_3740104839002625692[80] = 1;
}
void h_25(double *state, double *unused, double *out_709584225704138086) {
   out_709584225704138086[0] = state[6];
}
void H_25(double *state, double *unused, double *out_5134533377818071607) {
   out_5134533377818071607[0] = 0;
   out_5134533377818071607[1] = 0;
   out_5134533377818071607[2] = 0;
   out_5134533377818071607[3] = 0;
   out_5134533377818071607[4] = 0;
   out_5134533377818071607[5] = 0;
   out_5134533377818071607[6] = 1;
   out_5134533377818071607[7] = 0;
   out_5134533377818071607[8] = 0;
}
void h_24(double *state, double *unused, double *out_8657280578141947059) {
   out_8657280578141947059[0] = state[4];
   out_8657280578141947059[1] = state[5];
}
void H_24(double *state, double *unused, double *out_8692458976620394743) {
   out_8692458976620394743[0] = 0;
   out_8692458976620394743[1] = 0;
   out_8692458976620394743[2] = 0;
   out_8692458976620394743[3] = 0;
   out_8692458976620394743[4] = 1;
   out_8692458976620394743[5] = 0;
   out_8692458976620394743[6] = 0;
   out_8692458976620394743[7] = 0;
   out_8692458976620394743[8] = 0;
   out_8692458976620394743[9] = 0;
   out_8692458976620394743[10] = 0;
   out_8692458976620394743[11] = 0;
   out_8692458976620394743[12] = 0;
   out_8692458976620394743[13] = 0;
   out_8692458976620394743[14] = 1;
   out_8692458976620394743[15] = 0;
   out_8692458976620394743[16] = 0;
   out_8692458976620394743[17] = 0;
}
void h_30(double *state, double *unused, double *out_2927865302549884264) {
   out_2927865302549884264[0] = state[4];
}
void H_30(double *state, double *unused, double *out_7652866336325320234) {
   out_7652866336325320234[0] = 0;
   out_7652866336325320234[1] = 0;
   out_7652866336325320234[2] = 0;
   out_7652866336325320234[3] = 0;
   out_7652866336325320234[4] = 1;
   out_7652866336325320234[5] = 0;
   out_7652866336325320234[6] = 0;
   out_7652866336325320234[7] = 0;
   out_7652866336325320234[8] = 0;
}
void h_26(double *state, double *unused, double *out_4095609103626026001) {
   out_4095609103626026001[0] = state[7];
}
void H_26(double *state, double *unused, double *out_1393030058944015383) {
   out_1393030058944015383[0] = 0;
   out_1393030058944015383[1] = 0;
   out_1393030058944015383[2] = 0;
   out_1393030058944015383[3] = 0;
   out_1393030058944015383[4] = 0;
   out_1393030058944015383[5] = 0;
   out_1393030058944015383[6] = 0;
   out_1393030058944015383[7] = 1;
   out_1393030058944015383[8] = 0;
}
void h_27(double *state, double *unused, double *out_272499867631160026) {
   out_272499867631160026[0] = state[3];
}
void H_27(double *state, double *unused, double *out_8570283666200288165) {
   out_8570283666200288165[0] = 0;
   out_8570283666200288165[1] = 0;
   out_8570283666200288165[2] = 0;
   out_8570283666200288165[3] = 1;
   out_8570283666200288165[4] = 0;
   out_8570283666200288165[5] = 0;
   out_8570283666200288165[6] = 0;
   out_8570283666200288165[7] = 0;
   out_8570283666200288165[8] = 0;
}
void h_29(double *state, double *unused, double *out_8153772518385209211) {
   out_8153772518385209211[0] = state[1];
}
void H_29(double *state, double *unused, double *out_8163097680639712418) {
   out_8163097680639712418[0] = 0;
   out_8163097680639712418[1] = 1;
   out_8163097680639712418[2] = 0;
   out_8163097680639712418[3] = 0;
   out_8163097680639712418[4] = 0;
   out_8163097680639712418[5] = 0;
   out_8163097680639712418[6] = 0;
   out_8163097680639712418[7] = 0;
   out_8163097680639712418[8] = 0;
}
void h_28(double *state, double *unused, double *out_5274173135292005671) {
   out_5274173135292005671[0] = state[0];
}
void H_28(double *state, double *unused, double *out_3080698663570181844) {
   out_3080698663570181844[0] = 1;
   out_3080698663570181844[1] = 0;
   out_3080698663570181844[2] = 0;
   out_3080698663570181844[3] = 0;
   out_3080698663570181844[4] = 0;
   out_3080698663570181844[5] = 0;
   out_3080698663570181844[6] = 0;
   out_3080698663570181844[7] = 0;
   out_3080698663570181844[8] = 0;
}
void h_31(double *state, double *unused, double *out_984778287988643975) {
   out_984778287988643975[0] = state[8];
}
void H_31(double *state, double *unused, double *out_766821956710663907) {
   out_766821956710663907[0] = 0;
   out_766821956710663907[1] = 0;
   out_766821956710663907[2] = 0;
   out_766821956710663907[3] = 0;
   out_766821956710663907[4] = 0;
   out_766821956710663907[5] = 0;
   out_766821956710663907[6] = 0;
   out_766821956710663907[7] = 0;
   out_766821956710663907[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_6734524038688205591) {
  err_fun(nom_x, delta_x, out_6734524038688205591);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_6126652950877688806) {
  inv_err_fun(nom_x, true_x, out_6126652950877688806);
}
void car_H_mod_fun(double *state, double *out_2786804655155788198) {
  H_mod_fun(state, out_2786804655155788198);
}
void car_f_fun(double *state, double dt, double *out_2423327080634798879) {
  f_fun(state,  dt, out_2423327080634798879);
}
void car_F_fun(double *state, double dt, double *out_3740104839002625692) {
  F_fun(state,  dt, out_3740104839002625692);
}
void car_h_25(double *state, double *unused, double *out_709584225704138086) {
  h_25(state, unused, out_709584225704138086);
}
void car_H_25(double *state, double *unused, double *out_5134533377818071607) {
  H_25(state, unused, out_5134533377818071607);
}
void car_h_24(double *state, double *unused, double *out_8657280578141947059) {
  h_24(state, unused, out_8657280578141947059);
}
void car_H_24(double *state, double *unused, double *out_8692458976620394743) {
  H_24(state, unused, out_8692458976620394743);
}
void car_h_30(double *state, double *unused, double *out_2927865302549884264) {
  h_30(state, unused, out_2927865302549884264);
}
void car_H_30(double *state, double *unused, double *out_7652866336325320234) {
  H_30(state, unused, out_7652866336325320234);
}
void car_h_26(double *state, double *unused, double *out_4095609103626026001) {
  h_26(state, unused, out_4095609103626026001);
}
void car_H_26(double *state, double *unused, double *out_1393030058944015383) {
  H_26(state, unused, out_1393030058944015383);
}
void car_h_27(double *state, double *unused, double *out_272499867631160026) {
  h_27(state, unused, out_272499867631160026);
}
void car_H_27(double *state, double *unused, double *out_8570283666200288165) {
  H_27(state, unused, out_8570283666200288165);
}
void car_h_29(double *state, double *unused, double *out_8153772518385209211) {
  h_29(state, unused, out_8153772518385209211);
}
void car_H_29(double *state, double *unused, double *out_8163097680639712418) {
  H_29(state, unused, out_8163097680639712418);
}
void car_h_28(double *state, double *unused, double *out_5274173135292005671) {
  h_28(state, unused, out_5274173135292005671);
}
void car_H_28(double *state, double *unused, double *out_3080698663570181844) {
  H_28(state, unused, out_3080698663570181844);
}
void car_h_31(double *state, double *unused, double *out_984778287988643975) {
  h_31(state, unused, out_984778287988643975);
}
void car_H_31(double *state, double *unused, double *out_766821956710663907) {
  H_31(state, unused, out_766821956710663907);
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
