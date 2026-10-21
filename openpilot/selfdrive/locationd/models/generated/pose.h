#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_2136964252019170465);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_1227563649279098162);
void pose_H_mod_fun(double *state, double *out_497819547195898390);
void pose_f_fun(double *state, double dt, double *out_4587333856526582363);
void pose_F_fun(double *state, double dt, double *out_494582711598413239);
void pose_h_4(double *state, double *unused, double *out_916469630118270672);
void pose_H_4(double *state, double *unused, double *out_1695767029078796494);
void pose_h_10(double *state, double *unused, double *out_8613013218873323766);
void pose_H_10(double *state, double *unused, double *out_4891296382714452251);
void pose_h_13(double *state, double *unused, double *out_1875641267677564388);
void pose_H_13(double *state, double *unused, double *out_4908040854411129295);
void pose_h_14(double *state, double *unused, double *out_8565313325299850314);
void pose_H_14(double *state, double *unused, double *out_1387021403216575802);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}