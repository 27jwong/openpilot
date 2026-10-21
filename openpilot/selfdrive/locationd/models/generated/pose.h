#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_5537634756637633787);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_6641296844440448094);
void pose_H_mod_fun(double *state, double *out_1952919505077041985);
void pose_f_fun(double *state, double dt, double *out_5673854303566172016);
void pose_F_fun(double *state, double dt, double *out_2879053897486576154);
void pose_h_4(double *state, double *unused, double *out_7225039158807375856);
void pose_H_4(double *state, double *unused, double *out_7801001311829000706);
void pose_h_10(double *state, double *unused, double *out_1527157142141944609);
void pose_H_10(double *state, double *unused, double *out_8499975729958205534);
void pose_h_13(double *state, double *unused, double *out_4139399010471460228);
void pose_H_13(double *state, double *unused, double *out_4588727486496667905);
void pose_h_14(double *state, double *unused, double *out_6562515905070331452);
void pose_H_14(double *state, double *unused, double *out_3837760455489516177);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}