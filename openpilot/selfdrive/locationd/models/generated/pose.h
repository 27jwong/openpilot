#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_1557287137713055710);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_6312955019060494268);
void pose_H_mod_fun(double *state, double *out_2002785547247954198);
void pose_f_fun(double *state, double dt, double *out_7460364811199515324);
void pose_F_fun(double *state, double dt, double *out_3737951720851394665);
void pose_h_4(double *state, double *unused, double *out_8295372454373326831);
void pose_H_4(double *state, double *unused, double *out_183086821341657337);
void pose_h_10(double *state, double *unused, double *out_7644256498264067236);
void pose_H_10(double *state, double *unused, double *out_9222592977772692444);
void pose_h_13(double *state, double *unused, double *out_2771420024160106737);
void pose_H_13(double *state, double *unused, double *out_3650668641960866687);
void pose_h_14(double *state, double *unused, double *out_352453830606757886);
void pose_H_14(double *state, double *unused, double *out_2899701610953714959);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}