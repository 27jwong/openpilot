#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_1586140269824446038);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_9166849630934128431);
void pose_H_mod_fun(double *state, double *out_1085718029145294855);
void pose_f_fun(double *state, double dt, double *out_1181361488543532072);
void pose_F_fun(double *state, double dt, double *out_6679695245946987401);
void pose_h_4(double *state, double *unused, double *out_564364644927851281);
void pose_H_4(double *state, double *unused, double *out_4857525238989202427);
void pose_h_10(double *state, double *unused, double *out_4709684773907818943);
void pose_H_10(double *state, double *unused, double *out_2903583333991674534);
void pose_h_13(double *state, double *unused, double *out_9183852149661263008);
void pose_H_13(double *state, double *unused, double *out_8069799064321535228);
void pose_h_14(double *state, double *unused, double *out_1305949636277284093);
void pose_H_14(double *state, double *unused, double *out_1774736806693830131);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}