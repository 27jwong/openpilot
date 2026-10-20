#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_3945905870871886629);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_3515743109708110314);
void pose_H_mod_fun(double *state, double *out_2942188750116967597);
void pose_f_fun(double *state, double dt, double *out_8009815601597605847);
void pose_F_fun(double *state, double dt, double *out_9201886331103740614);
void pose_h_4(double *state, double *unused, double *out_3270913839475586541);
void pose_H_4(double *state, double *unused, double *out_6766359091509594881);
void pose_h_10(double *state, double *unused, double *out_7309022143129653139);
void pose_H_10(double *state, double *unused, double *out_8881959942302034495);
void pose_h_13(double *state, double *unused, double *out_2388800272598135721);
void pose_H_13(double *state, double *unused, double *out_3554085266177262080);
void pose_h_14(double *state, double *unused, double *out_2407078509949300141);
void pose_H_14(double *state, double *unused, double *out_2803118235170110352);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}