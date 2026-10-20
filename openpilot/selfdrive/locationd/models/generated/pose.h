#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_1111971644641323647);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_2323187048211860073);
void pose_H_mod_fun(double *state, double *out_5105792754762297630);
void pose_f_fun(double *state, double dt, double *out_493911584477072580);
void pose_F_fun(double *state, double dt, double *out_5423918991428774343);
void pose_h_4(double *state, double *unused, double *out_5685577583451539137);
void pose_H_4(double *state, double *unused, double *out_3094512129210927137);
void pose_h_10(double *state, double *unused, double *out_6387867301137382468);
void pose_H_10(double *state, double *unused, double *out_9062747238086022329);
void pose_h_13(double *state, double *unused, double *out_1513057542253796280);
void pose_H_13(double *state, double *unused, double *out_7741600736181923550);
void pose_h_14(double *state, double *unused, double *out_5536059940238760705);
void pose_H_14(double *state, double *unused, double *out_7057752985550411666);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}