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
void err_fun(double *nom_x, double *delta_x, double *out_9051704423920016590) {
   out_9051704423920016590[0] = delta_x[0] + nom_x[0];
   out_9051704423920016590[1] = delta_x[1] + nom_x[1];
   out_9051704423920016590[2] = delta_x[2] + nom_x[2];
   out_9051704423920016590[3] = delta_x[3] + nom_x[3];
   out_9051704423920016590[4] = delta_x[4] + nom_x[4];
   out_9051704423920016590[5] = delta_x[5] + nom_x[5];
   out_9051704423920016590[6] = delta_x[6] + nom_x[6];
   out_9051704423920016590[7] = delta_x[7] + nom_x[7];
   out_9051704423920016590[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_8262520073968428420) {
   out_8262520073968428420[0] = -nom_x[0] + true_x[0];
   out_8262520073968428420[1] = -nom_x[1] + true_x[1];
   out_8262520073968428420[2] = -nom_x[2] + true_x[2];
   out_8262520073968428420[3] = -nom_x[3] + true_x[3];
   out_8262520073968428420[4] = -nom_x[4] + true_x[4];
   out_8262520073968428420[5] = -nom_x[5] + true_x[5];
   out_8262520073968428420[6] = -nom_x[6] + true_x[6];
   out_8262520073968428420[7] = -nom_x[7] + true_x[7];
   out_8262520073968428420[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_8032114232992966708) {
   out_8032114232992966708[0] = 1.0;
   out_8032114232992966708[1] = 0.0;
   out_8032114232992966708[2] = 0.0;
   out_8032114232992966708[3] = 0.0;
   out_8032114232992966708[4] = 0.0;
   out_8032114232992966708[5] = 0.0;
   out_8032114232992966708[6] = 0.0;
   out_8032114232992966708[7] = 0.0;
   out_8032114232992966708[8] = 0.0;
   out_8032114232992966708[9] = 0.0;
   out_8032114232992966708[10] = 1.0;
   out_8032114232992966708[11] = 0.0;
   out_8032114232992966708[12] = 0.0;
   out_8032114232992966708[13] = 0.0;
   out_8032114232992966708[14] = 0.0;
   out_8032114232992966708[15] = 0.0;
   out_8032114232992966708[16] = 0.0;
   out_8032114232992966708[17] = 0.0;
   out_8032114232992966708[18] = 0.0;
   out_8032114232992966708[19] = 0.0;
   out_8032114232992966708[20] = 1.0;
   out_8032114232992966708[21] = 0.0;
   out_8032114232992966708[22] = 0.0;
   out_8032114232992966708[23] = 0.0;
   out_8032114232992966708[24] = 0.0;
   out_8032114232992966708[25] = 0.0;
   out_8032114232992966708[26] = 0.0;
   out_8032114232992966708[27] = 0.0;
   out_8032114232992966708[28] = 0.0;
   out_8032114232992966708[29] = 0.0;
   out_8032114232992966708[30] = 1.0;
   out_8032114232992966708[31] = 0.0;
   out_8032114232992966708[32] = 0.0;
   out_8032114232992966708[33] = 0.0;
   out_8032114232992966708[34] = 0.0;
   out_8032114232992966708[35] = 0.0;
   out_8032114232992966708[36] = 0.0;
   out_8032114232992966708[37] = 0.0;
   out_8032114232992966708[38] = 0.0;
   out_8032114232992966708[39] = 0.0;
   out_8032114232992966708[40] = 1.0;
   out_8032114232992966708[41] = 0.0;
   out_8032114232992966708[42] = 0.0;
   out_8032114232992966708[43] = 0.0;
   out_8032114232992966708[44] = 0.0;
   out_8032114232992966708[45] = 0.0;
   out_8032114232992966708[46] = 0.0;
   out_8032114232992966708[47] = 0.0;
   out_8032114232992966708[48] = 0.0;
   out_8032114232992966708[49] = 0.0;
   out_8032114232992966708[50] = 1.0;
   out_8032114232992966708[51] = 0.0;
   out_8032114232992966708[52] = 0.0;
   out_8032114232992966708[53] = 0.0;
   out_8032114232992966708[54] = 0.0;
   out_8032114232992966708[55] = 0.0;
   out_8032114232992966708[56] = 0.0;
   out_8032114232992966708[57] = 0.0;
   out_8032114232992966708[58] = 0.0;
   out_8032114232992966708[59] = 0.0;
   out_8032114232992966708[60] = 1.0;
   out_8032114232992966708[61] = 0.0;
   out_8032114232992966708[62] = 0.0;
   out_8032114232992966708[63] = 0.0;
   out_8032114232992966708[64] = 0.0;
   out_8032114232992966708[65] = 0.0;
   out_8032114232992966708[66] = 0.0;
   out_8032114232992966708[67] = 0.0;
   out_8032114232992966708[68] = 0.0;
   out_8032114232992966708[69] = 0.0;
   out_8032114232992966708[70] = 1.0;
   out_8032114232992966708[71] = 0.0;
   out_8032114232992966708[72] = 0.0;
   out_8032114232992966708[73] = 0.0;
   out_8032114232992966708[74] = 0.0;
   out_8032114232992966708[75] = 0.0;
   out_8032114232992966708[76] = 0.0;
   out_8032114232992966708[77] = 0.0;
   out_8032114232992966708[78] = 0.0;
   out_8032114232992966708[79] = 0.0;
   out_8032114232992966708[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_4567942705157615163) {
   out_4567942705157615163[0] = state[0];
   out_4567942705157615163[1] = state[1];
   out_4567942705157615163[2] = state[2];
   out_4567942705157615163[3] = state[3];
   out_4567942705157615163[4] = state[4];
   out_4567942705157615163[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8100000000000005*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_4567942705157615163[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_4567942705157615163[7] = state[7];
   out_4567942705157615163[8] = state[8];
}
void F_fun(double *state, double dt, double *out_3687136313485263185) {
   out_3687136313485263185[0] = 1;
   out_3687136313485263185[1] = 0;
   out_3687136313485263185[2] = 0;
   out_3687136313485263185[3] = 0;
   out_3687136313485263185[4] = 0;
   out_3687136313485263185[5] = 0;
   out_3687136313485263185[6] = 0;
   out_3687136313485263185[7] = 0;
   out_3687136313485263185[8] = 0;
   out_3687136313485263185[9] = 0;
   out_3687136313485263185[10] = 1;
   out_3687136313485263185[11] = 0;
   out_3687136313485263185[12] = 0;
   out_3687136313485263185[13] = 0;
   out_3687136313485263185[14] = 0;
   out_3687136313485263185[15] = 0;
   out_3687136313485263185[16] = 0;
   out_3687136313485263185[17] = 0;
   out_3687136313485263185[18] = 0;
   out_3687136313485263185[19] = 0;
   out_3687136313485263185[20] = 1;
   out_3687136313485263185[21] = 0;
   out_3687136313485263185[22] = 0;
   out_3687136313485263185[23] = 0;
   out_3687136313485263185[24] = 0;
   out_3687136313485263185[25] = 0;
   out_3687136313485263185[26] = 0;
   out_3687136313485263185[27] = 0;
   out_3687136313485263185[28] = 0;
   out_3687136313485263185[29] = 0;
   out_3687136313485263185[30] = 1;
   out_3687136313485263185[31] = 0;
   out_3687136313485263185[32] = 0;
   out_3687136313485263185[33] = 0;
   out_3687136313485263185[34] = 0;
   out_3687136313485263185[35] = 0;
   out_3687136313485263185[36] = 0;
   out_3687136313485263185[37] = 0;
   out_3687136313485263185[38] = 0;
   out_3687136313485263185[39] = 0;
   out_3687136313485263185[40] = 1;
   out_3687136313485263185[41] = 0;
   out_3687136313485263185[42] = 0;
   out_3687136313485263185[43] = 0;
   out_3687136313485263185[44] = 0;
   out_3687136313485263185[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_3687136313485263185[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_3687136313485263185[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_3687136313485263185[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_3687136313485263185[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_3687136313485263185[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_3687136313485263185[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_3687136313485263185[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_3687136313485263185[53] = -9.8100000000000005*dt;
   out_3687136313485263185[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_3687136313485263185[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_3687136313485263185[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3687136313485263185[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3687136313485263185[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_3687136313485263185[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_3687136313485263185[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_3687136313485263185[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3687136313485263185[62] = 0;
   out_3687136313485263185[63] = 0;
   out_3687136313485263185[64] = 0;
   out_3687136313485263185[65] = 0;
   out_3687136313485263185[66] = 0;
   out_3687136313485263185[67] = 0;
   out_3687136313485263185[68] = 0;
   out_3687136313485263185[69] = 0;
   out_3687136313485263185[70] = 1;
   out_3687136313485263185[71] = 0;
   out_3687136313485263185[72] = 0;
   out_3687136313485263185[73] = 0;
   out_3687136313485263185[74] = 0;
   out_3687136313485263185[75] = 0;
   out_3687136313485263185[76] = 0;
   out_3687136313485263185[77] = 0;
   out_3687136313485263185[78] = 0;
   out_3687136313485263185[79] = 0;
   out_3687136313485263185[80] = 1;
}
void h_25(double *state, double *unused, double *out_2888373885214709447) {
   out_2888373885214709447[0] = state[6];
}
void H_25(double *state, double *unused, double *out_1161073992919270529) {
   out_1161073992919270529[0] = 0;
   out_1161073992919270529[1] = 0;
   out_1161073992919270529[2] = 0;
   out_1161073992919270529[3] = 0;
   out_1161073992919270529[4] = 0;
   out_1161073992919270529[5] = 0;
   out_1161073992919270529[6] = 1;
   out_1161073992919270529[7] = 0;
   out_1161073992919270529[8] = 0;
}
void h_24(double *state, double *unused, double *out_3830852376846248192) {
   out_3830852376846248192[0] = state[4];
   out_3830852376846248192[1] = state[5];
}
void H_24(double *state, double *unused, double *out_5907583319885779563) {
   out_5907583319885779563[0] = 0;
   out_5907583319885779563[1] = 0;
   out_5907583319885779563[2] = 0;
   out_5907583319885779563[3] = 0;
   out_5907583319885779563[4] = 1;
   out_5907583319885779563[5] = 0;
   out_5907583319885779563[6] = 0;
   out_5907583319885779563[7] = 0;
   out_5907583319885779563[8] = 0;
   out_5907583319885779563[9] = 0;
   out_5907583319885779563[10] = 0;
   out_5907583319885779563[11] = 0;
   out_5907583319885779563[12] = 0;
   out_5907583319885779563[13] = 0;
   out_5907583319885779563[14] = 1;
   out_5907583319885779563[15] = 0;
   out_5907583319885779563[16] = 0;
   out_5907583319885779563[17] = 0;
}
void h_30(double *state, double *unused, double *out_6420652903810990987) {
   out_6420652903810990987[0] = state[4];
}
void H_30(double *state, double *unused, double *out_1357258965587978098) {
   out_1357258965587978098[0] = 0;
   out_1357258965587978098[1] = 0;
   out_1357258965587978098[2] = 0;
   out_1357258965587978098[3] = 0;
   out_1357258965587978098[4] = 1;
   out_1357258965587978098[5] = 0;
   out_1357258965587978098[6] = 0;
   out_1357258965587978098[7] = 0;
   out_1357258965587978098[8] = 0;
}
void h_26(double *state, double *unused, double *out_1418979636150145342) {
   out_1418979636150145342[0] = state[7];
}
void H_26(double *state, double *unused, double *out_4902577311793326753) {
   out_4902577311793326753[0] = 0;
   out_4902577311793326753[1] = 0;
   out_4902577311793326753[2] = 0;
   out_4902577311793326753[3] = 0;
   out_4902577311793326753[4] = 0;
   out_4902577311793326753[5] = 0;
   out_4902577311793326753[6] = 0;
   out_4902577311793326753[7] = 1;
   out_4902577311793326753[8] = 0;
}
void h_27(double *state, double *unused, double *out_6566216540079153322) {
   out_6566216540079153322[0] = state[3];
}
void H_27(double *state, double *unused, double *out_817504346212446813) {
   out_817504346212446813[0] = 0;
   out_817504346212446813[1] = 0;
   out_817504346212446813[2] = 0;
   out_817504346212446813[3] = 1;
   out_817504346212446813[4] = 0;
   out_817504346212446813[5] = 0;
   out_817504346212446813[6] = 0;
   out_817504346212446813[7] = 0;
   out_817504346212446813[8] = 0;
}
void h_29(double *state, double *unused, double *out_5750748910700158548) {
   out_5750748910700158548[0] = state[1];
}
void H_29(double *state, double *unused, double *out_1867490309902370282) {
   out_1867490309902370282[0] = 0;
   out_1867490309902370282[1] = 1;
   out_1867490309902370282[2] = 0;
   out_1867490309902370282[3] = 0;
   out_1867490309902370282[4] = 0;
   out_1867490309902370282[5] = 0;
   out_1867490309902370282[6] = 0;
   out_1867490309902370282[7] = 0;
   out_1867490309902370282[8] = 0;
}
void h_28(double *state, double *unused, double *out_6538660297744367731) {
   out_6538660297744367731[0] = state[0];
}
void H_28(double *state, double *unused, double *out_3214908707167160292) {
   out_3214908707167160292[0] = 1;
   out_3214908707167160292[1] = 0;
   out_3214908707167160292[2] = 0;
   out_3214908707167160292[3] = 0;
   out_3214908707167160292[4] = 0;
   out_3214908707167160292[5] = 0;
   out_3214908707167160292[6] = 0;
   out_3214908707167160292[7] = 0;
   out_3214908707167160292[8] = 0;
}
void h_31(double *state, double *unused, double *out_3804624550919326526) {
   out_3804624550919326526[0] = state[8];
}
void H_31(double *state, double *unused, double *out_5528785414026678229) {
   out_5528785414026678229[0] = 0;
   out_5528785414026678229[1] = 0;
   out_5528785414026678229[2] = 0;
   out_5528785414026678229[3] = 0;
   out_5528785414026678229[4] = 0;
   out_5528785414026678229[5] = 0;
   out_5528785414026678229[6] = 0;
   out_5528785414026678229[7] = 0;
   out_5528785414026678229[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_9051704423920016590) {
  err_fun(nom_x, delta_x, out_9051704423920016590);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_8262520073968428420) {
  inv_err_fun(nom_x, true_x, out_8262520073968428420);
}
void car_H_mod_fun(double *state, double *out_8032114232992966708) {
  H_mod_fun(state, out_8032114232992966708);
}
void car_f_fun(double *state, double dt, double *out_4567942705157615163) {
  f_fun(state,  dt, out_4567942705157615163);
}
void car_F_fun(double *state, double dt, double *out_3687136313485263185) {
  F_fun(state,  dt, out_3687136313485263185);
}
void car_h_25(double *state, double *unused, double *out_2888373885214709447) {
  h_25(state, unused, out_2888373885214709447);
}
void car_H_25(double *state, double *unused, double *out_1161073992919270529) {
  H_25(state, unused, out_1161073992919270529);
}
void car_h_24(double *state, double *unused, double *out_3830852376846248192) {
  h_24(state, unused, out_3830852376846248192);
}
void car_H_24(double *state, double *unused, double *out_5907583319885779563) {
  H_24(state, unused, out_5907583319885779563);
}
void car_h_30(double *state, double *unused, double *out_6420652903810990987) {
  h_30(state, unused, out_6420652903810990987);
}
void car_H_30(double *state, double *unused, double *out_1357258965587978098) {
  H_30(state, unused, out_1357258965587978098);
}
void car_h_26(double *state, double *unused, double *out_1418979636150145342) {
  h_26(state, unused, out_1418979636150145342);
}
void car_H_26(double *state, double *unused, double *out_4902577311793326753) {
  H_26(state, unused, out_4902577311793326753);
}
void car_h_27(double *state, double *unused, double *out_6566216540079153322) {
  h_27(state, unused, out_6566216540079153322);
}
void car_H_27(double *state, double *unused, double *out_817504346212446813) {
  H_27(state, unused, out_817504346212446813);
}
void car_h_29(double *state, double *unused, double *out_5750748910700158548) {
  h_29(state, unused, out_5750748910700158548);
}
void car_H_29(double *state, double *unused, double *out_1867490309902370282) {
  H_29(state, unused, out_1867490309902370282);
}
void car_h_28(double *state, double *unused, double *out_6538660297744367731) {
  h_28(state, unused, out_6538660297744367731);
}
void car_H_28(double *state, double *unused, double *out_3214908707167160292) {
  H_28(state, unused, out_3214908707167160292);
}
void car_h_31(double *state, double *unused, double *out_3804624550919326526) {
  h_31(state, unused, out_3804624550919326526);
}
void car_H_31(double *state, double *unused, double *out_5528785414026678229) {
  H_31(state, unused, out_5528785414026678229);
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
