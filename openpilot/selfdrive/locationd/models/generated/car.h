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
void car_err_fun(double *nom_x, double *delta_x, double *out_8532074412831680005);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_8887769849182574222);
void car_H_mod_fun(double *state, double *out_2434822093009109592);
void car_f_fun(double *state, double dt, double *out_7489300736878445586);
void car_F_fun(double *state, double dt, double *out_5688594807383263913);
void car_h_25(double *state, double *unused, double *out_4593071790268116288);
void car_H_25(double *state, double *unused, double *out_3739161203201081212);
void car_h_24(double *state, double *unused, double *out_8003921618768497979);
void car_H_24(double *state, double *unused, double *out_4646084155782705221);
void car_h_30(double *state, double *unused, double *out_7633295100594421359);
void car_H_30(double *state, double *unused, double *out_3177529138290535543);
void car_h_26(double *state, double *unused, double *out_5811775705454284612);
void car_H_26(double *state, double *unused, double *out_7480664522075137436);
void car_h_27(double *state, double *unused, double *out_8851999015780589683);
void car_H_27(double *state, double *unused, double *out_6043263462144746193);
void car_h_29(double *state, double *unused, double *out_1831479801108289492);
void car_H_29(double *state, double *unused, double *out_7756626189014297226);
void car_h_28(double *state, double *unused, double *out_6557959006484629248);
void car_H_28(double *state, double *unused, double *out_5792995917448970975);
void car_h_31(double *state, double *unused, double *out_5357011602177695336);
void car_H_31(double *state, double *unused, double *out_3708515241324120784);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}