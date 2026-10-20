#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_4013660299866450489);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_4438646948403156526);
void pose_H_mod_fun(double *state, double *out_8172130496533507169);
void pose_f_fun(double *state, double dt, double *out_5004053374026539222);
void pose_F_fun(double *state, double dt, double *out_4632470071587184948);
void pose_h_4(double *state, double *unused, double *out_5642004684309586938);
void pose_H_4(double *state, double *unused, double *out_2173071738203631897);
void pose_h_10(double *state, double *unused, double *out_6635142105328933338);
void pose_H_10(double *state, double *unused, double *out_8640756480625150988);
void pose_h_13(double *state, double *unused, double *out_2521909184006486810);
void pose_H_13(double *state, double *unused, double *out_5385345563535964698);
void pose_h_14(double *state, double *unused, double *out_1314376768125748605);
void pose_H_14(double *state, double *unused, double *out_6136312594543116426);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}