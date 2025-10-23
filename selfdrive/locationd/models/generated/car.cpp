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
void err_fun(double *nom_x, double *delta_x, double *out_345657473849428930) {
   out_345657473849428930[0] = delta_x[0] + nom_x[0];
   out_345657473849428930[1] = delta_x[1] + nom_x[1];
   out_345657473849428930[2] = delta_x[2] + nom_x[2];
   out_345657473849428930[3] = delta_x[3] + nom_x[3];
   out_345657473849428930[4] = delta_x[4] + nom_x[4];
   out_345657473849428930[5] = delta_x[5] + nom_x[5];
   out_345657473849428930[6] = delta_x[6] + nom_x[6];
   out_345657473849428930[7] = delta_x[7] + nom_x[7];
   out_345657473849428930[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_7904680352313940648) {
   out_7904680352313940648[0] = -nom_x[0] + true_x[0];
   out_7904680352313940648[1] = -nom_x[1] + true_x[1];
   out_7904680352313940648[2] = -nom_x[2] + true_x[2];
   out_7904680352313940648[3] = -nom_x[3] + true_x[3];
   out_7904680352313940648[4] = -nom_x[4] + true_x[4];
   out_7904680352313940648[5] = -nom_x[5] + true_x[5];
   out_7904680352313940648[6] = -nom_x[6] + true_x[6];
   out_7904680352313940648[7] = -nom_x[7] + true_x[7];
   out_7904680352313940648[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_3477656150477313209) {
   out_3477656150477313209[0] = 1.0;
   out_3477656150477313209[1] = 0;
   out_3477656150477313209[2] = 0;
   out_3477656150477313209[3] = 0;
   out_3477656150477313209[4] = 0;
   out_3477656150477313209[5] = 0;
   out_3477656150477313209[6] = 0;
   out_3477656150477313209[7] = 0;
   out_3477656150477313209[8] = 0;
   out_3477656150477313209[9] = 0;
   out_3477656150477313209[10] = 1.0;
   out_3477656150477313209[11] = 0;
   out_3477656150477313209[12] = 0;
   out_3477656150477313209[13] = 0;
   out_3477656150477313209[14] = 0;
   out_3477656150477313209[15] = 0;
   out_3477656150477313209[16] = 0;
   out_3477656150477313209[17] = 0;
   out_3477656150477313209[18] = 0;
   out_3477656150477313209[19] = 0;
   out_3477656150477313209[20] = 1.0;
   out_3477656150477313209[21] = 0;
   out_3477656150477313209[22] = 0;
   out_3477656150477313209[23] = 0;
   out_3477656150477313209[24] = 0;
   out_3477656150477313209[25] = 0;
   out_3477656150477313209[26] = 0;
   out_3477656150477313209[27] = 0;
   out_3477656150477313209[28] = 0;
   out_3477656150477313209[29] = 0;
   out_3477656150477313209[30] = 1.0;
   out_3477656150477313209[31] = 0;
   out_3477656150477313209[32] = 0;
   out_3477656150477313209[33] = 0;
   out_3477656150477313209[34] = 0;
   out_3477656150477313209[35] = 0;
   out_3477656150477313209[36] = 0;
   out_3477656150477313209[37] = 0;
   out_3477656150477313209[38] = 0;
   out_3477656150477313209[39] = 0;
   out_3477656150477313209[40] = 1.0;
   out_3477656150477313209[41] = 0;
   out_3477656150477313209[42] = 0;
   out_3477656150477313209[43] = 0;
   out_3477656150477313209[44] = 0;
   out_3477656150477313209[45] = 0;
   out_3477656150477313209[46] = 0;
   out_3477656150477313209[47] = 0;
   out_3477656150477313209[48] = 0;
   out_3477656150477313209[49] = 0;
   out_3477656150477313209[50] = 1.0;
   out_3477656150477313209[51] = 0;
   out_3477656150477313209[52] = 0;
   out_3477656150477313209[53] = 0;
   out_3477656150477313209[54] = 0;
   out_3477656150477313209[55] = 0;
   out_3477656150477313209[56] = 0;
   out_3477656150477313209[57] = 0;
   out_3477656150477313209[58] = 0;
   out_3477656150477313209[59] = 0;
   out_3477656150477313209[60] = 1.0;
   out_3477656150477313209[61] = 0;
   out_3477656150477313209[62] = 0;
   out_3477656150477313209[63] = 0;
   out_3477656150477313209[64] = 0;
   out_3477656150477313209[65] = 0;
   out_3477656150477313209[66] = 0;
   out_3477656150477313209[67] = 0;
   out_3477656150477313209[68] = 0;
   out_3477656150477313209[69] = 0;
   out_3477656150477313209[70] = 1.0;
   out_3477656150477313209[71] = 0;
   out_3477656150477313209[72] = 0;
   out_3477656150477313209[73] = 0;
   out_3477656150477313209[74] = 0;
   out_3477656150477313209[75] = 0;
   out_3477656150477313209[76] = 0;
   out_3477656150477313209[77] = 0;
   out_3477656150477313209[78] = 0;
   out_3477656150477313209[79] = 0;
   out_3477656150477313209[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_5247221098079988295) {
   out_5247221098079988295[0] = state[0];
   out_5247221098079988295[1] = state[1];
   out_5247221098079988295[2] = state[2];
   out_5247221098079988295[3] = state[3];
   out_5247221098079988295[4] = state[4];
   out_5247221098079988295[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8000000000000007*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_5247221098079988295[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_5247221098079988295[7] = state[7];
   out_5247221098079988295[8] = state[8];
}
void F_fun(double *state, double dt, double *out_3843490184204667838) {
   out_3843490184204667838[0] = 1;
   out_3843490184204667838[1] = 0;
   out_3843490184204667838[2] = 0;
   out_3843490184204667838[3] = 0;
   out_3843490184204667838[4] = 0;
   out_3843490184204667838[5] = 0;
   out_3843490184204667838[6] = 0;
   out_3843490184204667838[7] = 0;
   out_3843490184204667838[8] = 0;
   out_3843490184204667838[9] = 0;
   out_3843490184204667838[10] = 1;
   out_3843490184204667838[11] = 0;
   out_3843490184204667838[12] = 0;
   out_3843490184204667838[13] = 0;
   out_3843490184204667838[14] = 0;
   out_3843490184204667838[15] = 0;
   out_3843490184204667838[16] = 0;
   out_3843490184204667838[17] = 0;
   out_3843490184204667838[18] = 0;
   out_3843490184204667838[19] = 0;
   out_3843490184204667838[20] = 1;
   out_3843490184204667838[21] = 0;
   out_3843490184204667838[22] = 0;
   out_3843490184204667838[23] = 0;
   out_3843490184204667838[24] = 0;
   out_3843490184204667838[25] = 0;
   out_3843490184204667838[26] = 0;
   out_3843490184204667838[27] = 0;
   out_3843490184204667838[28] = 0;
   out_3843490184204667838[29] = 0;
   out_3843490184204667838[30] = 1;
   out_3843490184204667838[31] = 0;
   out_3843490184204667838[32] = 0;
   out_3843490184204667838[33] = 0;
   out_3843490184204667838[34] = 0;
   out_3843490184204667838[35] = 0;
   out_3843490184204667838[36] = 0;
   out_3843490184204667838[37] = 0;
   out_3843490184204667838[38] = 0;
   out_3843490184204667838[39] = 0;
   out_3843490184204667838[40] = 1;
   out_3843490184204667838[41] = 0;
   out_3843490184204667838[42] = 0;
   out_3843490184204667838[43] = 0;
   out_3843490184204667838[44] = 0;
   out_3843490184204667838[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_3843490184204667838[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_3843490184204667838[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_3843490184204667838[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_3843490184204667838[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_3843490184204667838[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_3843490184204667838[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_3843490184204667838[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_3843490184204667838[53] = -9.8000000000000007*dt;
   out_3843490184204667838[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_3843490184204667838[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_3843490184204667838[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3843490184204667838[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3843490184204667838[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_3843490184204667838[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_3843490184204667838[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_3843490184204667838[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3843490184204667838[62] = 0;
   out_3843490184204667838[63] = 0;
   out_3843490184204667838[64] = 0;
   out_3843490184204667838[65] = 0;
   out_3843490184204667838[66] = 0;
   out_3843490184204667838[67] = 0;
   out_3843490184204667838[68] = 0;
   out_3843490184204667838[69] = 0;
   out_3843490184204667838[70] = 1;
   out_3843490184204667838[71] = 0;
   out_3843490184204667838[72] = 0;
   out_3843490184204667838[73] = 0;
   out_3843490184204667838[74] = 0;
   out_3843490184204667838[75] = 0;
   out_3843490184204667838[76] = 0;
   out_3843490184204667838[77] = 0;
   out_3843490184204667838[78] = 0;
   out_3843490184204667838[79] = 0;
   out_3843490184204667838[80] = 1;
}
void h_25(double *state, double *unused, double *out_9212583378466553495) {
   out_9212583378466553495[0] = state[6];
}
void H_25(double *state, double *unused, double *out_4666770324178786990) {
   out_4666770324178786990[0] = 0;
   out_4666770324178786990[1] = 0;
   out_4666770324178786990[2] = 0;
   out_4666770324178786990[3] = 0;
   out_4666770324178786990[4] = 0;
   out_4666770324178786990[5] = 0;
   out_4666770324178786990[6] = 1;
   out_4666770324178786990[7] = 0;
   out_4666770324178786990[8] = 0;
}
void h_24(double *state, double *unused, double *out_8338416765466038827) {
   out_8338416765466038827[0] = state[4];
   out_8338416765466038827[1] = state[5];
}
void H_24(double *state, double *unused, double *out_6892478108157655552) {
   out_6892478108157655552[0] = 0;
   out_6892478108157655552[1] = 0;
   out_6892478108157655552[2] = 0;
   out_6892478108157655552[3] = 0;
   out_6892478108157655552[4] = 1;
   out_6892478108157655552[5] = 0;
   out_6892478108157655552[6] = 0;
   out_6892478108157655552[7] = 0;
   out_6892478108157655552[8] = 0;
   out_6892478108157655552[9] = 0;
   out_6892478108157655552[10] = 0;
   out_6892478108157655552[11] = 0;
   out_6892478108157655552[12] = 0;
   out_6892478108157655552[13] = 0;
   out_6892478108157655552[14] = 1;
   out_6892478108157655552[15] = 0;
   out_6892478108157655552[16] = 0;
   out_6892478108157655552[17] = 0;
}
void h_30(double *state, double *unused, double *out_6665202913546333755) {
   out_6665202913546333755[0] = state[4];
}
void H_30(double *state, double *unused, double *out_6863283408039147871) {
   out_6863283408039147871[0] = 0;
   out_6863283408039147871[1] = 0;
   out_6863283408039147871[2] = 0;
   out_6863283408039147871[3] = 0;
   out_6863283408039147871[4] = 1;
   out_6863283408039147871[5] = 0;
   out_6863283408039147871[6] = 0;
   out_6863283408039147871[7] = 0;
   out_6863283408039147871[8] = 0;
}
void h_26(double *state, double *unused, double *out_4132195986851883519) {
   out_4132195986851883519[0] = state[7];
}
void H_26(double *state, double *unused, double *out_925267005304730766) {
   out_925267005304730766[0] = 0;
   out_925267005304730766[1] = 0;
   out_925267005304730766[2] = 0;
   out_925267005304730766[3] = 0;
   out_925267005304730766[4] = 0;
   out_925267005304730766[5] = 0;
   out_925267005304730766[6] = 0;
   out_925267005304730766[7] = 1;
   out_925267005304730766[8] = 0;
}
void h_27(double *state, double *unused, double *out_2956758656507506241) {
   out_2956758656507506241[0] = state[3];
}
void H_27(double *state, double *unused, double *out_2362668065235122009) {
   out_2362668065235122009[0] = 0;
   out_2362668065235122009[1] = 0;
   out_2362668065235122009[2] = 0;
   out_2362668065235122009[3] = 1;
   out_2362668065235122009[4] = 0;
   out_2362668065235122009[5] = 0;
   out_2362668065235122009[6] = 0;
   out_2362668065235122009[7] = 0;
   out_2362668065235122009[8] = 0;
}
void h_29(double *state, double *unused, double *out_5683118820211035643) {
   out_5683118820211035643[0] = state[1];
}
void H_29(double *state, double *unused, double *out_5047662721349939104) {
   out_5047662721349939104[0] = 0;
   out_5047662721349939104[1] = 1;
   out_5047662721349939104[2] = 0;
   out_5047662721349939104[3] = 0;
   out_5047662721349939104[4] = 0;
   out_5047662721349939104[5] = 0;
   out_5047662721349939104[6] = 0;
   out_5047662721349939104[7] = 0;
   out_5047662721349939104[8] = 0;
}
void h_28(double *state, double *unused, double *out_7958431924168351886) {
   out_7958431924168351886[0] = state[0];
}
void H_28(double *state, double *unused, double *out_7011292992915265355) {
   out_7011292992915265355[0] = 1;
   out_7011292992915265355[1] = 0;
   out_7011292992915265355[2] = 0;
   out_7011292992915265355[3] = 0;
   out_7011292992915265355[4] = 0;
   out_7011292992915265355[5] = 0;
   out_7011292992915265355[6] = 0;
   out_7011292992915265355[7] = 0;
   out_7011292992915265355[8] = 0;
}
void h_31(double *state, double *unused, double *out_5575133850212531145) {
   out_5575133850212531145[0] = state[8];
}
void H_31(double *state, double *unused, double *out_4697416286055747418) {
   out_4697416286055747418[0] = 0;
   out_4697416286055747418[1] = 0;
   out_4697416286055747418[2] = 0;
   out_4697416286055747418[3] = 0;
   out_4697416286055747418[4] = 0;
   out_4697416286055747418[5] = 0;
   out_4697416286055747418[6] = 0;
   out_4697416286055747418[7] = 0;
   out_4697416286055747418[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_345657473849428930) {
  err_fun(nom_x, delta_x, out_345657473849428930);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_7904680352313940648) {
  inv_err_fun(nom_x, true_x, out_7904680352313940648);
}
void car_H_mod_fun(double *state, double *out_3477656150477313209) {
  H_mod_fun(state, out_3477656150477313209);
}
void car_f_fun(double *state, double dt, double *out_5247221098079988295) {
  f_fun(state,  dt, out_5247221098079988295);
}
void car_F_fun(double *state, double dt, double *out_3843490184204667838) {
  F_fun(state,  dt, out_3843490184204667838);
}
void car_h_25(double *state, double *unused, double *out_9212583378466553495) {
  h_25(state, unused, out_9212583378466553495);
}
void car_H_25(double *state, double *unused, double *out_4666770324178786990) {
  H_25(state, unused, out_4666770324178786990);
}
void car_h_24(double *state, double *unused, double *out_8338416765466038827) {
  h_24(state, unused, out_8338416765466038827);
}
void car_H_24(double *state, double *unused, double *out_6892478108157655552) {
  H_24(state, unused, out_6892478108157655552);
}
void car_h_30(double *state, double *unused, double *out_6665202913546333755) {
  h_30(state, unused, out_6665202913546333755);
}
void car_H_30(double *state, double *unused, double *out_6863283408039147871) {
  H_30(state, unused, out_6863283408039147871);
}
void car_h_26(double *state, double *unused, double *out_4132195986851883519) {
  h_26(state, unused, out_4132195986851883519);
}
void car_H_26(double *state, double *unused, double *out_925267005304730766) {
  H_26(state, unused, out_925267005304730766);
}
void car_h_27(double *state, double *unused, double *out_2956758656507506241) {
  h_27(state, unused, out_2956758656507506241);
}
void car_H_27(double *state, double *unused, double *out_2362668065235122009) {
  H_27(state, unused, out_2362668065235122009);
}
void car_h_29(double *state, double *unused, double *out_5683118820211035643) {
  h_29(state, unused, out_5683118820211035643);
}
void car_H_29(double *state, double *unused, double *out_5047662721349939104) {
  H_29(state, unused, out_5047662721349939104);
}
void car_h_28(double *state, double *unused, double *out_7958431924168351886) {
  h_28(state, unused, out_7958431924168351886);
}
void car_H_28(double *state, double *unused, double *out_7011292992915265355) {
  H_28(state, unused, out_7011292992915265355);
}
void car_h_31(double *state, double *unused, double *out_5575133850212531145) {
  h_31(state, unused, out_5575133850212531145);
}
void car_H_31(double *state, double *unused, double *out_4697416286055747418) {
  H_31(state, unused, out_4697416286055747418);
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
