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
void car_err_fun(double *nom_x, double *delta_x, double *out_4603381077007055755);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_5273785100438362063);
void car_H_mod_fun(double *state, double *out_8414806225118962866);
void car_f_fun(double *state, double dt, double *out_8599373732669577078);
void car_F_fun(double *state, double dt, double *out_5796812835580827553);
void car_h_25(double *state, double *unused, double *out_5539409521994919118);
void car_H_25(double *state, double *unused, double *out_838749622678351513);
void car_h_24(double *state, double *unused, double *out_8112980868095491689);
void car_H_24(double *state, double *unused, double *out_8379929264962004878);
void car_h_30(double *state, double *unused, double *out_5264215459710413229);
void car_H_30(double *state, double *unused, double *out_3688946707449256685);
void car_h_26(double *state, double *unused, double *out_2659810138901715578);
void car_H_26(double *state, double *unused, double *out_2902753696195704711);
void car_h_27(double *state, double *unused, double *out_2109935651316415224);
void car_H_27(double *state, double *unused, double *out_1465352636265313468);
void car_h_29(double *state, double *unused, double *out_1988367646962956640);
void car_H_29(double *state, double *unused, double *out_3178715363134864501);
void car_h_28(double *state, double *unused, double *out_1166425798414752789);
void car_H_28(double *state, double *unused, double *out_8261114380204395075);
void car_h_31(double *state, double *unused, double *out_9176859050248941468);
void car_H_31(double *state, double *unused, double *out_869395584555311941);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}