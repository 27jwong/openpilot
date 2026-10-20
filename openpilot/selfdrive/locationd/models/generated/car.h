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
void car_err_fun(double *nom_x, double *delta_x, double *out_1207077157841363728);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_5167403064917713258);
void car_H_mod_fun(double *state, double *out_5732271557167151234);
void car_f_fun(double *state, double dt, double *out_4596155381636715650);
void car_F_fun(double *state, double dt, double *out_8271283846288930916);
void car_h_25(double *state, double *unused, double *out_1071579631746279652);
void car_H_25(double *state, double *unused, double *out_947424562669153677);
void car_h_24(double *state, double *unused, double *out_3648611834353910669);
void car_H_24(double *state, double *unused, double *out_1225225036336345889);
void car_h_30(double *state, double *unused, double *out_8237447157279420782);
void car_H_30(double *state, double *unused, double *out_3465757521176402304);
void car_h_26(double *state, double *unused, double *out_5927733075186470326);
void car_H_26(double *state, double *unused, double *out_4251950532429954278);
void car_h_27(double *state, double *unused, double *out_3930093635914565993);
void car_H_27(double *state, double *unused, double *out_1290994209375977393);
void car_h_29(double *state, double *unused, double *out_2479462243285682826);
void car_H_29(double *state, double *unused, double *out_3975988865490794488);
void car_h_28(double *state, double *unused, double *out_6296653325891602595);
void car_H_28(double *state, double *unused, double *out_1106410151578736086);
void car_h_31(double *state, double *unused, double *out_4454477804804743461);
void car_H_31(double *state, double *unused, double *out_3625742430196602802);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}