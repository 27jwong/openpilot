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
void err_fun(double *nom_x, double *delta_x, double *out_8852768432473250938) {
   out_8852768432473250938[0] = delta_x[0] + nom_x[0];
   out_8852768432473250938[1] = delta_x[1] + nom_x[1];
   out_8852768432473250938[2] = delta_x[2] + nom_x[2];
   out_8852768432473250938[3] = delta_x[3] + nom_x[3];
   out_8852768432473250938[4] = delta_x[4] + nom_x[4];
   out_8852768432473250938[5] = delta_x[5] + nom_x[5];
   out_8852768432473250938[6] = delta_x[6] + nom_x[6];
   out_8852768432473250938[7] = delta_x[7] + nom_x[7];
   out_8852768432473250938[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_5554712225475198881) {
   out_5554712225475198881[0] = -nom_x[0] + true_x[0];
   out_5554712225475198881[1] = -nom_x[1] + true_x[1];
   out_5554712225475198881[2] = -nom_x[2] + true_x[2];
   out_5554712225475198881[3] = -nom_x[3] + true_x[3];
   out_5554712225475198881[4] = -nom_x[4] + true_x[4];
   out_5554712225475198881[5] = -nom_x[5] + true_x[5];
   out_5554712225475198881[6] = -nom_x[6] + true_x[6];
   out_5554712225475198881[7] = -nom_x[7] + true_x[7];
   out_5554712225475198881[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_3442117041647752880) {
   out_3442117041647752880[0] = 1.0;
   out_3442117041647752880[1] = 0;
   out_3442117041647752880[2] = 0;
   out_3442117041647752880[3] = 0;
   out_3442117041647752880[4] = 0;
   out_3442117041647752880[5] = 0;
   out_3442117041647752880[6] = 0;
   out_3442117041647752880[7] = 0;
   out_3442117041647752880[8] = 0;
   out_3442117041647752880[9] = 0;
   out_3442117041647752880[10] = 1.0;
   out_3442117041647752880[11] = 0;
   out_3442117041647752880[12] = 0;
   out_3442117041647752880[13] = 0;
   out_3442117041647752880[14] = 0;
   out_3442117041647752880[15] = 0;
   out_3442117041647752880[16] = 0;
   out_3442117041647752880[17] = 0;
   out_3442117041647752880[18] = 0;
   out_3442117041647752880[19] = 0;
   out_3442117041647752880[20] = 1.0;
   out_3442117041647752880[21] = 0;
   out_3442117041647752880[22] = 0;
   out_3442117041647752880[23] = 0;
   out_3442117041647752880[24] = 0;
   out_3442117041647752880[25] = 0;
   out_3442117041647752880[26] = 0;
   out_3442117041647752880[27] = 0;
   out_3442117041647752880[28] = 0;
   out_3442117041647752880[29] = 0;
   out_3442117041647752880[30] = 1.0;
   out_3442117041647752880[31] = 0;
   out_3442117041647752880[32] = 0;
   out_3442117041647752880[33] = 0;
   out_3442117041647752880[34] = 0;
   out_3442117041647752880[35] = 0;
   out_3442117041647752880[36] = 0;
   out_3442117041647752880[37] = 0;
   out_3442117041647752880[38] = 0;
   out_3442117041647752880[39] = 0;
   out_3442117041647752880[40] = 1.0;
   out_3442117041647752880[41] = 0;
   out_3442117041647752880[42] = 0;
   out_3442117041647752880[43] = 0;
   out_3442117041647752880[44] = 0;
   out_3442117041647752880[45] = 0;
   out_3442117041647752880[46] = 0;
   out_3442117041647752880[47] = 0;
   out_3442117041647752880[48] = 0;
   out_3442117041647752880[49] = 0;
   out_3442117041647752880[50] = 1.0;
   out_3442117041647752880[51] = 0;
   out_3442117041647752880[52] = 0;
   out_3442117041647752880[53] = 0;
   out_3442117041647752880[54] = 0;
   out_3442117041647752880[55] = 0;
   out_3442117041647752880[56] = 0;
   out_3442117041647752880[57] = 0;
   out_3442117041647752880[58] = 0;
   out_3442117041647752880[59] = 0;
   out_3442117041647752880[60] = 1.0;
   out_3442117041647752880[61] = 0;
   out_3442117041647752880[62] = 0;
   out_3442117041647752880[63] = 0;
   out_3442117041647752880[64] = 0;
   out_3442117041647752880[65] = 0;
   out_3442117041647752880[66] = 0;
   out_3442117041647752880[67] = 0;
   out_3442117041647752880[68] = 0;
   out_3442117041647752880[69] = 0;
   out_3442117041647752880[70] = 1.0;
   out_3442117041647752880[71] = 0;
   out_3442117041647752880[72] = 0;
   out_3442117041647752880[73] = 0;
   out_3442117041647752880[74] = 0;
   out_3442117041647752880[75] = 0;
   out_3442117041647752880[76] = 0;
   out_3442117041647752880[77] = 0;
   out_3442117041647752880[78] = 0;
   out_3442117041647752880[79] = 0;
   out_3442117041647752880[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_8924317162494420698) {
   out_8924317162494420698[0] = state[0];
   out_8924317162494420698[1] = state[1];
   out_8924317162494420698[2] = state[2];
   out_8924317162494420698[3] = state[3];
   out_8924317162494420698[4] = state[4];
   out_8924317162494420698[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8000000000000007*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_8924317162494420698[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_8924317162494420698[7] = state[7];
   out_8924317162494420698[8] = state[8];
}
void F_fun(double *state, double dt, double *out_4892913796386743771) {
   out_4892913796386743771[0] = 1;
   out_4892913796386743771[1] = 0;
   out_4892913796386743771[2] = 0;
   out_4892913796386743771[3] = 0;
   out_4892913796386743771[4] = 0;
   out_4892913796386743771[5] = 0;
   out_4892913796386743771[6] = 0;
   out_4892913796386743771[7] = 0;
   out_4892913796386743771[8] = 0;
   out_4892913796386743771[9] = 0;
   out_4892913796386743771[10] = 1;
   out_4892913796386743771[11] = 0;
   out_4892913796386743771[12] = 0;
   out_4892913796386743771[13] = 0;
   out_4892913796386743771[14] = 0;
   out_4892913796386743771[15] = 0;
   out_4892913796386743771[16] = 0;
   out_4892913796386743771[17] = 0;
   out_4892913796386743771[18] = 0;
   out_4892913796386743771[19] = 0;
   out_4892913796386743771[20] = 1;
   out_4892913796386743771[21] = 0;
   out_4892913796386743771[22] = 0;
   out_4892913796386743771[23] = 0;
   out_4892913796386743771[24] = 0;
   out_4892913796386743771[25] = 0;
   out_4892913796386743771[26] = 0;
   out_4892913796386743771[27] = 0;
   out_4892913796386743771[28] = 0;
   out_4892913796386743771[29] = 0;
   out_4892913796386743771[30] = 1;
   out_4892913796386743771[31] = 0;
   out_4892913796386743771[32] = 0;
   out_4892913796386743771[33] = 0;
   out_4892913796386743771[34] = 0;
   out_4892913796386743771[35] = 0;
   out_4892913796386743771[36] = 0;
   out_4892913796386743771[37] = 0;
   out_4892913796386743771[38] = 0;
   out_4892913796386743771[39] = 0;
   out_4892913796386743771[40] = 1;
   out_4892913796386743771[41] = 0;
   out_4892913796386743771[42] = 0;
   out_4892913796386743771[43] = 0;
   out_4892913796386743771[44] = 0;
   out_4892913796386743771[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_4892913796386743771[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_4892913796386743771[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_4892913796386743771[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_4892913796386743771[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_4892913796386743771[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_4892913796386743771[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_4892913796386743771[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_4892913796386743771[53] = -9.8000000000000007*dt;
   out_4892913796386743771[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_4892913796386743771[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_4892913796386743771[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_4892913796386743771[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_4892913796386743771[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_4892913796386743771[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_4892913796386743771[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_4892913796386743771[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_4892913796386743771[62] = 0;
   out_4892913796386743771[63] = 0;
   out_4892913796386743771[64] = 0;
   out_4892913796386743771[65] = 0;
   out_4892913796386743771[66] = 0;
   out_4892913796386743771[67] = 0;
   out_4892913796386743771[68] = 0;
   out_4892913796386743771[69] = 0;
   out_4892913796386743771[70] = 1;
   out_4892913796386743771[71] = 0;
   out_4892913796386743771[72] = 0;
   out_4892913796386743771[73] = 0;
   out_4892913796386743771[74] = 0;
   out_4892913796386743771[75] = 0;
   out_4892913796386743771[76] = 0;
   out_4892913796386743771[77] = 0;
   out_4892913796386743771[78] = 0;
   out_4892913796386743771[79] = 0;
   out_4892913796386743771[80] = 1;
}
void h_25(double *state, double *unused, double *out_1210797927955610947) {
   out_1210797927955610947[0] = state[6];
}
void H_25(double *state, double *unused, double *out_6460553776064276833) {
   out_6460553776064276833[0] = 0;
   out_6460553776064276833[1] = 0;
   out_6460553776064276833[2] = 0;
   out_6460553776064276833[3] = 0;
   out_6460553776064276833[4] = 0;
   out_6460553776064276833[5] = 0;
   out_6460553776064276833[6] = 1;
   out_6460553776064276833[7] = 0;
   out_6460553776064276833[8] = 0;
}
void h_24(double *state, double *unused, double *out_4722325592217899045) {
   out_4722325592217899045[0] = state[4];
   out_4722325592217899045[1] = state[5];
}
void H_24(double *state, double *unused, double *out_8633203375069776399) {
   out_8633203375069776399[0] = 0;
   out_8633203375069776399[1] = 0;
   out_8633203375069776399[2] = 0;
   out_8633203375069776399[3] = 0;
   out_8633203375069776399[4] = 1;
   out_8633203375069776399[5] = 0;
   out_8633203375069776399[6] = 0;
   out_8633203375069776399[7] = 0;
   out_8633203375069776399[8] = 0;
   out_8633203375069776399[9] = 0;
   out_8633203375069776399[10] = 0;
   out_8633203375069776399[11] = 0;
   out_8633203375069776399[12] = 0;
   out_8633203375069776399[13] = 0;
   out_8633203375069776399[14] = 1;
   out_8633203375069776399[15] = 0;
   out_8633203375069776399[16] = 0;
   out_8633203375069776399[17] = 0;
}
void h_30(double *state, double *unused, double *out_8974063184776283388) {
   out_8974063184776283388[0] = state[4];
}
void H_30(double *state, double *unused, double *out_3942220817557028206) {
   out_3942220817557028206[0] = 0;
   out_3942220817557028206[1] = 0;
   out_3942220817557028206[2] = 0;
   out_3942220817557028206[3] = 0;
   out_3942220817557028206[4] = 1;
   out_3942220817557028206[5] = 0;
   out_3942220817557028206[6] = 0;
   out_3942220817557028206[7] = 0;
   out_3942220817557028206[8] = 0;
}
void h_26(double *state, double *unused, double *out_5047319973635036695) {
   out_5047319973635036695[0] = state[7];
}
void H_26(double *state, double *unused, double *out_8244686978771218559) {
   out_8244686978771218559[0] = 0;
   out_8244686978771218559[1] = 0;
   out_8244686978771218559[2] = 0;
   out_8244686978771218559[3] = 0;
   out_8244686978771218559[4] = 0;
   out_8244686978771218559[5] = 0;
   out_8244686978771218559[6] = 0;
   out_8244686978771218559[7] = 1;
   out_8244686978771218559[8] = 0;
}
void h_27(double *state, double *unused, double *out_6968782841949348903) {
   out_6968782841949348903[0] = state[3];
}
void H_27(double *state, double *unused, double *out_6116984129357453117) {
   out_6116984129357453117[0] = 0;
   out_6116984129357453117[1] = 0;
   out_6116984129357453117[2] = 0;
   out_6116984129357453117[3] = 1;
   out_6116984129357453117[4] = 0;
   out_6116984129357453117[5] = 0;
   out_6116984129357453117[6] = 0;
   out_6116984129357453117[7] = 0;
   out_6116984129357453117[8] = 0;
}
void h_29(double *state, double *unused, double *out_3972389917115437743) {
   out_3972389917115437743[0] = state[1];
}
void H_29(double *state, double *unused, double *out_3431989473242636022) {
   out_3431989473242636022[0] = 0;
   out_3431989473242636022[1] = 1;
   out_3431989473242636022[2] = 0;
   out_3431989473242636022[3] = 0;
   out_3431989473242636022[4] = 0;
   out_3431989473242636022[5] = 0;
   out_3431989473242636022[6] = 0;
   out_3431989473242636022[7] = 0;
   out_3431989473242636022[8] = 0;
}
void h_28(double *state, double *unused, double *out_4693254024336296769) {
   out_4693254024336296769[0] = state[0];
}
void H_28(double *state, double *unused, double *out_8514388490312166596) {
   out_8514388490312166596[0] = 1;
   out_8514388490312166596[1] = 0;
   out_8514388490312166596[2] = 0;
   out_8514388490312166596[3] = 0;
   out_8514388490312166596[4] = 0;
   out_8514388490312166596[5] = 0;
   out_8514388490312166596[6] = 0;
   out_8514388490312166596[7] = 0;
   out_8514388490312166596[8] = 0;
}
void h_31(double *state, double *unused, double *out_5560037298394739989) {
   out_5560037298394739989[0] = state[8];
}
void H_31(double *state, double *unused, double *out_7618478876537867083) {
   out_7618478876537867083[0] = 0;
   out_7618478876537867083[1] = 0;
   out_7618478876537867083[2] = 0;
   out_7618478876537867083[3] = 0;
   out_7618478876537867083[4] = 0;
   out_7618478876537867083[5] = 0;
   out_7618478876537867083[6] = 0;
   out_7618478876537867083[7] = 0;
   out_7618478876537867083[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_8852768432473250938) {
  err_fun(nom_x, delta_x, out_8852768432473250938);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_5554712225475198881) {
  inv_err_fun(nom_x, true_x, out_5554712225475198881);
}
void car_H_mod_fun(double *state, double *out_3442117041647752880) {
  H_mod_fun(state, out_3442117041647752880);
}
void car_f_fun(double *state, double dt, double *out_8924317162494420698) {
  f_fun(state,  dt, out_8924317162494420698);
}
void car_F_fun(double *state, double dt, double *out_4892913796386743771) {
  F_fun(state,  dt, out_4892913796386743771);
}
void car_h_25(double *state, double *unused, double *out_1210797927955610947) {
  h_25(state, unused, out_1210797927955610947);
}
void car_H_25(double *state, double *unused, double *out_6460553776064276833) {
  H_25(state, unused, out_6460553776064276833);
}
void car_h_24(double *state, double *unused, double *out_4722325592217899045) {
  h_24(state, unused, out_4722325592217899045);
}
void car_H_24(double *state, double *unused, double *out_8633203375069776399) {
  H_24(state, unused, out_8633203375069776399);
}
void car_h_30(double *state, double *unused, double *out_8974063184776283388) {
  h_30(state, unused, out_8974063184776283388);
}
void car_H_30(double *state, double *unused, double *out_3942220817557028206) {
  H_30(state, unused, out_3942220817557028206);
}
void car_h_26(double *state, double *unused, double *out_5047319973635036695) {
  h_26(state, unused, out_5047319973635036695);
}
void car_H_26(double *state, double *unused, double *out_8244686978771218559) {
  H_26(state, unused, out_8244686978771218559);
}
void car_h_27(double *state, double *unused, double *out_6968782841949348903) {
  h_27(state, unused, out_6968782841949348903);
}
void car_H_27(double *state, double *unused, double *out_6116984129357453117) {
  H_27(state, unused, out_6116984129357453117);
}
void car_h_29(double *state, double *unused, double *out_3972389917115437743) {
  h_29(state, unused, out_3972389917115437743);
}
void car_H_29(double *state, double *unused, double *out_3431989473242636022) {
  H_29(state, unused, out_3431989473242636022);
}
void car_h_28(double *state, double *unused, double *out_4693254024336296769) {
  h_28(state, unused, out_4693254024336296769);
}
void car_H_28(double *state, double *unused, double *out_8514388490312166596) {
  H_28(state, unused, out_8514388490312166596);
}
void car_h_31(double *state, double *unused, double *out_5560037298394739989) {
  h_31(state, unused, out_5560037298394739989);
}
void car_H_31(double *state, double *unused, double *out_7618478876537867083) {
  H_31(state, unused, out_7618478876537867083);
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
