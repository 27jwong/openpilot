#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_3388086590141056177);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_1633734341039361389);
void pose_H_mod_fun(double *state, double *out_3284972142892485625);
void pose_f_fun(double *state, double dt, double *out_7294513523227393093);
void pose_F_fun(double *state, double dt, double *out_1315688866139806167);
void pose_h_4(double *state, double *unused, double *out_6849640214971072753);
void pose_H_4(double *state, double *unused, double *out_6217410264759244649);
void pose_h_10(double *state, double *unused, double *out_2538664534589622554);
void pose_H_10(double *state, double *unused, double *out_5499394725381253526);
void pose_h_13(double *state, double *unused, double *out_228516151720916839);
void pose_H_13(double *state, double *unused, double *out_9017059983617974166);
void pose_h_14(double *state, double *unused, double *out_4519675338605033391);
void pose_H_14(double *state, double *unused, double *out_3134621832463872353);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}