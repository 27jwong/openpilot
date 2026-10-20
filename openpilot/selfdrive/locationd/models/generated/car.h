#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_3483464759245960511);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_7987823426960705532);
void car_H_mod_fun(double *state, double *out_7112895520366101703);
void car_f_fun(double *state, double dt, double *out_1262489871201988902);
void car_F_fun(double *state, double dt, double *out_8743492844107181083);
void car_h_25(double *state, double *unused, double *out_3773728404526364058);
void car_H_25(double *state, double *unused, double *out_7272976555244193865);
void car_h_24(double *state, double *unused, double *out_3463504132316116315);
void car_H_24(double *state, double *unused, double *out_145415078432733028);
void car_h_30(double *state, double *unused, double *out_4107544246227685127);
void car_H_30(double *state, double *unused, double *out_6646071188337749553);
void car_h_26(double *state, double *unused, double *out_3504228361551207610);
void car_H_26(double *state, double *unused, double *out_7432264199591301527);
void car_h_27(double *state, double *unused, double *out_8690091829877045805);
void car_H_27(double *state, double *unused, double *out_4471307876537324642);
void car_h_29(double *state, double *unused, double *out_5089628339562983239);
void car_H_29(double *state, double *unused, double *out_7156302532652141737);
void car_h_28(double *state, double *unused, double *out_284435010232819152);
void car_H_28(double *state, double *unused, double *out_2073903515582611163);
void car_h_31(double *state, double *unused, double *out_2323097011897480891);
void car_H_31(double *state, double *unused, double *out_6806056097357950051);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}