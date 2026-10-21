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
void err_fun(double *nom_x, double *delta_x, double *out_3628762341002155077) {
   out_3628762341002155077[0] = delta_x[0] + nom_x[0];
   out_3628762341002155077[1] = delta_x[1] + nom_x[1];
   out_3628762341002155077[2] = delta_x[2] + nom_x[2];
   out_3628762341002155077[3] = delta_x[3] + nom_x[3];
   out_3628762341002155077[4] = delta_x[4] + nom_x[4];
   out_3628762341002155077[5] = delta_x[5] + nom_x[5];
   out_3628762341002155077[6] = delta_x[6] + nom_x[6];
   out_3628762341002155077[7] = delta_x[7] + nom_x[7];
   out_3628762341002155077[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_8326913521565900806) {
   out_8326913521565900806[0] = -nom_x[0] + true_x[0];
   out_8326913521565900806[1] = -nom_x[1] + true_x[1];
   out_8326913521565900806[2] = -nom_x[2] + true_x[2];
   out_8326913521565900806[3] = -nom_x[3] + true_x[3];
   out_8326913521565900806[4] = -nom_x[4] + true_x[4];
   out_8326913521565900806[5] = -nom_x[5] + true_x[5];
   out_8326913521565900806[6] = -nom_x[6] + true_x[6];
   out_8326913521565900806[7] = -nom_x[7] + true_x[7];
   out_8326913521565900806[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_1936405662837462559) {
   out_1936405662837462559[0] = 1.0;
   out_1936405662837462559[1] = 0.0;
   out_1936405662837462559[2] = 0.0;
   out_1936405662837462559[3] = 0.0;
   out_1936405662837462559[4] = 0.0;
   out_1936405662837462559[5] = 0.0;
   out_1936405662837462559[6] = 0.0;
   out_1936405662837462559[7] = 0.0;
   out_1936405662837462559[8] = 0.0;
   out_1936405662837462559[9] = 0.0;
   out_1936405662837462559[10] = 1.0;
   out_1936405662837462559[11] = 0.0;
   out_1936405662837462559[12] = 0.0;
   out_1936405662837462559[13] = 0.0;
   out_1936405662837462559[14] = 0.0;
   out_1936405662837462559[15] = 0.0;
   out_1936405662837462559[16] = 0.0;
   out_1936405662837462559[17] = 0.0;
   out_1936405662837462559[18] = 0.0;
   out_1936405662837462559[19] = 0.0;
   out_1936405662837462559[20] = 1.0;
   out_1936405662837462559[21] = 0.0;
   out_1936405662837462559[22] = 0.0;
   out_1936405662837462559[23] = 0.0;
   out_1936405662837462559[24] = 0.0;
   out_1936405662837462559[25] = 0.0;
   out_1936405662837462559[26] = 0.0;
   out_1936405662837462559[27] = 0.0;
   out_1936405662837462559[28] = 0.0;
   out_1936405662837462559[29] = 0.0;
   out_1936405662837462559[30] = 1.0;
   out_1936405662837462559[31] = 0.0;
   out_1936405662837462559[32] = 0.0;
   out_1936405662837462559[33] = 0.0;
   out_1936405662837462559[34] = 0.0;
   out_1936405662837462559[35] = 0.0;
   out_1936405662837462559[36] = 0.0;
   out_1936405662837462559[37] = 0.0;
   out_1936405662837462559[38] = 0.0;
   out_1936405662837462559[39] = 0.0;
   out_1936405662837462559[40] = 1.0;
   out_1936405662837462559[41] = 0.0;
   out_1936405662837462559[42] = 0.0;
   out_1936405662837462559[43] = 0.0;
   out_1936405662837462559[44] = 0.0;
   out_1936405662837462559[45] = 0.0;
   out_1936405662837462559[46] = 0.0;
   out_1936405662837462559[47] = 0.0;
   out_1936405662837462559[48] = 0.0;
   out_1936405662837462559[49] = 0.0;
   out_1936405662837462559[50] = 1.0;
   out_1936405662837462559[51] = 0.0;
   out_1936405662837462559[52] = 0.0;
   out_1936405662837462559[53] = 0.0;
   out_1936405662837462559[54] = 0.0;
   out_1936405662837462559[55] = 0.0;
   out_1936405662837462559[56] = 0.0;
   out_1936405662837462559[57] = 0.0;
   out_1936405662837462559[58] = 0.0;
   out_1936405662837462559[59] = 0.0;
   out_1936405662837462559[60] = 1.0;
   out_1936405662837462559[61] = 0.0;
   out_1936405662837462559[62] = 0.0;
   out_1936405662837462559[63] = 0.0;
   out_1936405662837462559[64] = 0.0;
   out_1936405662837462559[65] = 0.0;
   out_1936405662837462559[66] = 0.0;
   out_1936405662837462559[67] = 0.0;
   out_1936405662837462559[68] = 0.0;
   out_1936405662837462559[69] = 0.0;
   out_1936405662837462559[70] = 1.0;
   out_1936405662837462559[71] = 0.0;
   out_1936405662837462559[72] = 0.0;
   out_1936405662837462559[73] = 0.0;
   out_1936405662837462559[74] = 0.0;
   out_1936405662837462559[75] = 0.0;
   out_1936405662837462559[76] = 0.0;
   out_1936405662837462559[77] = 0.0;
   out_1936405662837462559[78] = 0.0;
   out_1936405662837462559[79] = 0.0;
   out_1936405662837462559[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_5241560014263660552) {
   out_5241560014263660552[0] = state[0];
   out_5241560014263660552[1] = state[1];
   out_5241560014263660552[2] = state[2];
   out_5241560014263660552[3] = state[3];
   out_5241560014263660552[4] = state[4];
   out_5241560014263660552[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8100000000000005*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_5241560014263660552[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_5241560014263660552[7] = state[7];
   out_5241560014263660552[8] = state[8];
}
void F_fun(double *state, double dt, double *out_8799266228203575332) {
   out_8799266228203575332[0] = 1;
   out_8799266228203575332[1] = 0;
   out_8799266228203575332[2] = 0;
   out_8799266228203575332[3] = 0;
   out_8799266228203575332[4] = 0;
   out_8799266228203575332[5] = 0;
   out_8799266228203575332[6] = 0;
   out_8799266228203575332[7] = 0;
   out_8799266228203575332[8] = 0;
   out_8799266228203575332[9] = 0;
   out_8799266228203575332[10] = 1;
   out_8799266228203575332[11] = 0;
   out_8799266228203575332[12] = 0;
   out_8799266228203575332[13] = 0;
   out_8799266228203575332[14] = 0;
   out_8799266228203575332[15] = 0;
   out_8799266228203575332[16] = 0;
   out_8799266228203575332[17] = 0;
   out_8799266228203575332[18] = 0;
   out_8799266228203575332[19] = 0;
   out_8799266228203575332[20] = 1;
   out_8799266228203575332[21] = 0;
   out_8799266228203575332[22] = 0;
   out_8799266228203575332[23] = 0;
   out_8799266228203575332[24] = 0;
   out_8799266228203575332[25] = 0;
   out_8799266228203575332[26] = 0;
   out_8799266228203575332[27] = 0;
   out_8799266228203575332[28] = 0;
   out_8799266228203575332[29] = 0;
   out_8799266228203575332[30] = 1;
   out_8799266228203575332[31] = 0;
   out_8799266228203575332[32] = 0;
   out_8799266228203575332[33] = 0;
   out_8799266228203575332[34] = 0;
   out_8799266228203575332[35] = 0;
   out_8799266228203575332[36] = 0;
   out_8799266228203575332[37] = 0;
   out_8799266228203575332[38] = 0;
   out_8799266228203575332[39] = 0;
   out_8799266228203575332[40] = 1;
   out_8799266228203575332[41] = 0;
   out_8799266228203575332[42] = 0;
   out_8799266228203575332[43] = 0;
   out_8799266228203575332[44] = 0;
   out_8799266228203575332[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_8799266228203575332[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_8799266228203575332[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_8799266228203575332[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_8799266228203575332[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_8799266228203575332[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_8799266228203575332[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_8799266228203575332[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_8799266228203575332[53] = -9.8100000000000005*dt;
   out_8799266228203575332[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_8799266228203575332[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_8799266228203575332[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8799266228203575332[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8799266228203575332[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_8799266228203575332[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_8799266228203575332[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_8799266228203575332[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8799266228203575332[62] = 0;
   out_8799266228203575332[63] = 0;
   out_8799266228203575332[64] = 0;
   out_8799266228203575332[65] = 0;
   out_8799266228203575332[66] = 0;
   out_8799266228203575332[67] = 0;
   out_8799266228203575332[68] = 0;
   out_8799266228203575332[69] = 0;
   out_8799266228203575332[70] = 1;
   out_8799266228203575332[71] = 0;
   out_8799266228203575332[72] = 0;
   out_8799266228203575332[73] = 0;
   out_8799266228203575332[74] = 0;
   out_8799266228203575332[75] = 0;
   out_8799266228203575332[76] = 0;
   out_8799266228203575332[77] = 0;
   out_8799266228203575332[78] = 0;
   out_8799266228203575332[79] = 0;
   out_8799266228203575332[80] = 1;
}
void h_25(double *state, double *unused, double *out_1070812002986692230) {
   out_1070812002986692230[0] = state[6];
}
void H_25(double *state, double *unused, double *out_7317150184959851820) {
   out_7317150184959851820[0] = 0;
   out_7317150184959851820[1] = 0;
   out_7317150184959851820[2] = 0;
   out_7317150184959851820[3] = 0;
   out_7317150184959851820[4] = 0;
   out_7317150184959851820[5] = 0;
   out_7317150184959851820[6] = 1;
   out_7317150184959851820[7] = 0;
   out_7317150184959851820[8] = 0;
}
void h_24(double *state, double *unused, double *out_8298197210527653023) {
   out_8298197210527653023[0] = state[4];
   out_8298197210527653023[1] = state[5];
}
void H_24(double *state, double *unused, double *out_5144500585954352254) {
   out_5144500585954352254[0] = 0;
   out_5144500585954352254[1] = 0;
   out_5144500585954352254[2] = 0;
   out_5144500585954352254[3] = 0;
   out_5144500585954352254[4] = 1;
   out_5144500585954352254[5] = 0;
   out_5144500585954352254[6] = 0;
   out_5144500585954352254[7] = 0;
   out_5144500585954352254[8] = 0;
   out_5144500585954352254[9] = 0;
   out_5144500585954352254[10] = 0;
   out_5144500585954352254[11] = 0;
   out_5144500585954352254[12] = 0;
   out_5144500585954352254[13] = 0;
   out_5144500585954352254[14] = 1;
   out_5144500585954352254[15] = 0;
   out_5144500585954352254[16] = 0;
   out_5144500585954352254[17] = 0;
}
void h_30(double *state, double *unused, double *out_5700023223363658706) {
   out_5700023223363658706[0] = state[4];
}
void H_30(double *state, double *unused, double *out_8611260930242451169) {
   out_8611260930242451169[0] = 0;
   out_8611260930242451169[1] = 0;
   out_8611260930242451169[2] = 0;
   out_8611260930242451169[3] = 0;
   out_8611260930242451169[4] = 1;
   out_8611260930242451169[5] = 0;
   out_8611260930242451169[6] = 0;
   out_8611260930242451169[7] = 0;
   out_8611260930242451169[8] = 0;
}
void h_26(double *state, double *unused, double *out_8348768769064263898) {
   out_8348768769064263898[0] = state[7];
}
void H_26(double *state, double *unused, double *out_7825067918988899195) {
   out_7825067918988899195[0] = 0;
   out_7825067918988899195[1] = 0;
   out_7825067918988899195[2] = 0;
   out_7825067918988899195[3] = 0;
   out_7825067918988899195[4] = 0;
   out_7825067918988899195[5] = 0;
   out_7825067918988899195[6] = 0;
   out_7825067918988899195[7] = 1;
   out_7825067918988899195[8] = 0;
}
void h_27(double *state, double *unused, double *out_6986442756720554566) {
   out_6986442756720554566[0] = state[3];
}
void H_27(double *state, double *unused, double *out_6387666859058507952) {
   out_6387666859058507952[0] = 0;
   out_6387666859058507952[1] = 0;
   out_6387666859058507952[2] = 0;
   out_6387666859058507952[3] = 1;
   out_6387666859058507952[4] = 0;
   out_6387666859058507952[5] = 0;
   out_6387666859058507952[6] = 0;
   out_6387666859058507952[7] = 0;
   out_6387666859058507952[8] = 0;
}
void h_29(double *state, double *unused, double *out_546427834096514762) {
   out_546427834096514762[0] = state[1];
}
void H_29(double *state, double *unused, double *out_8101029585928058985) {
   out_8101029585928058985[0] = 0;
   out_8101029585928058985[1] = 1;
   out_8101029585928058985[2] = 0;
   out_8101029585928058985[3] = 0;
   out_8101029585928058985[4] = 0;
   out_8101029585928058985[5] = 0;
   out_8101029585928058985[6] = 0;
   out_8101029585928058985[7] = 0;
   out_8101029585928058985[8] = 0;
}
void h_28(double *state, double *unused, double *out_5128975417745875440) {
   out_5128975417745875440[0] = state[0];
}
void H_28(double *state, double *unused, double *out_5263315470711962057) {
   out_5263315470711962057[0] = 1;
   out_5263315470711962057[1] = 0;
   out_5263315470711962057[2] = 0;
   out_5263315470711962057[3] = 0;
   out_5263315470711962057[4] = 0;
   out_5263315470711962057[5] = 0;
   out_5263315470711962057[6] = 0;
   out_5263315470711962057[7] = 0;
   out_5263315470711962057[8] = 0;
}
void h_31(double *state, double *unused, double *out_2521443395615575397) {
   out_2521443395615575397[0] = state[8];
}
void H_31(double *state, double *unused, double *out_8451276021222250671) {
   out_8451276021222250671[0] = 0;
   out_8451276021222250671[1] = 0;
   out_8451276021222250671[2] = 0;
   out_8451276021222250671[3] = 0;
   out_8451276021222250671[4] = 0;
   out_8451276021222250671[5] = 0;
   out_8451276021222250671[6] = 0;
   out_8451276021222250671[7] = 0;
   out_8451276021222250671[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_3628762341002155077) {
  err_fun(nom_x, delta_x, out_3628762341002155077);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_8326913521565900806) {
  inv_err_fun(nom_x, true_x, out_8326913521565900806);
}
void car_H_mod_fun(double *state, double *out_1936405662837462559) {
  H_mod_fun(state, out_1936405662837462559);
}
void car_f_fun(double *state, double dt, double *out_5241560014263660552) {
  f_fun(state,  dt, out_5241560014263660552);
}
void car_F_fun(double *state, double dt, double *out_8799266228203575332) {
  F_fun(state,  dt, out_8799266228203575332);
}
void car_h_25(double *state, double *unused, double *out_1070812002986692230) {
  h_25(state, unused, out_1070812002986692230);
}
void car_H_25(double *state, double *unused, double *out_7317150184959851820) {
  H_25(state, unused, out_7317150184959851820);
}
void car_h_24(double *state, double *unused, double *out_8298197210527653023) {
  h_24(state, unused, out_8298197210527653023);
}
void car_H_24(double *state, double *unused, double *out_5144500585954352254) {
  H_24(state, unused, out_5144500585954352254);
}
void car_h_30(double *state, double *unused, double *out_5700023223363658706) {
  h_30(state, unused, out_5700023223363658706);
}
void car_H_30(double *state, double *unused, double *out_8611260930242451169) {
  H_30(state, unused, out_8611260930242451169);
}
void car_h_26(double *state, double *unused, double *out_8348768769064263898) {
  h_26(state, unused, out_8348768769064263898);
}
void car_H_26(double *state, double *unused, double *out_7825067918988899195) {
  H_26(state, unused, out_7825067918988899195);
}
void car_h_27(double *state, double *unused, double *out_6986442756720554566) {
  h_27(state, unused, out_6986442756720554566);
}
void car_H_27(double *state, double *unused, double *out_6387666859058507952) {
  H_27(state, unused, out_6387666859058507952);
}
void car_h_29(double *state, double *unused, double *out_546427834096514762) {
  h_29(state, unused, out_546427834096514762);
}
void car_H_29(double *state, double *unused, double *out_8101029585928058985) {
  H_29(state, unused, out_8101029585928058985);
}
void car_h_28(double *state, double *unused, double *out_5128975417745875440) {
  h_28(state, unused, out_5128975417745875440);
}
void car_H_28(double *state, double *unused, double *out_5263315470711962057) {
  H_28(state, unused, out_5263315470711962057);
}
void car_h_31(double *state, double *unused, double *out_2521443395615575397) {
  h_31(state, unused, out_2521443395615575397);
}
void car_H_31(double *state, double *unused, double *out_8451276021222250671) {
  H_31(state, unused, out_8451276021222250671);
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
