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
void car_err_fun(double *nom_x, double *delta_x, double *out_6734524038688205591);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_6126652950877688806);
void car_H_mod_fun(double *state, double *out_2786804655155788198);
void car_f_fun(double *state, double dt, double *out_2423327080634798879);
void car_F_fun(double *state, double dt, double *out_3740104839002625692);
void car_h_25(double *state, double *unused, double *out_709584225704138086);
void car_H_25(double *state, double *unused, double *out_5134533377818071607);
void car_h_24(double *state, double *unused, double *out_8657280578141947059);
void car_H_24(double *state, double *unused, double *out_8692458976620394743);
void car_h_30(double *state, double *unused, double *out_2927865302549884264);
void car_H_30(double *state, double *unused, double *out_7652866336325320234);
void car_h_26(double *state, double *unused, double *out_4095609103626026001);
void car_H_26(double *state, double *unused, double *out_1393030058944015383);
void car_h_27(double *state, double *unused, double *out_272499867631160026);
void car_H_27(double *state, double *unused, double *out_8570283666200288165);
void car_h_29(double *state, double *unused, double *out_8153772518385209211);
void car_H_29(double *state, double *unused, double *out_8163097680639712418);
void car_h_28(double *state, double *unused, double *out_5274173135292005671);
void car_H_28(double *state, double *unused, double *out_3080698663570181844);
void car_h_31(double *state, double *unused, double *out_984778287988643975);
void car_H_31(double *state, double *unused, double *out_766821956710663907);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}