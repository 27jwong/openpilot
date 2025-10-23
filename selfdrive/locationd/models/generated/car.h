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
void car_err_fun(double *nom_x, double *delta_x, double *out_345657473849428930);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_7904680352313940648);
void car_H_mod_fun(double *state, double *out_3477656150477313209);
void car_f_fun(double *state, double dt, double *out_5247221098079988295);
void car_F_fun(double *state, double dt, double *out_3843490184204667838);
void car_h_25(double *state, double *unused, double *out_9212583378466553495);
void car_H_25(double *state, double *unused, double *out_4666770324178786990);
void car_h_24(double *state, double *unused, double *out_8338416765466038827);
void car_H_24(double *state, double *unused, double *out_6892478108157655552);
void car_h_30(double *state, double *unused, double *out_6665202913546333755);
void car_H_30(double *state, double *unused, double *out_6863283408039147871);
void car_h_26(double *state, double *unused, double *out_4132195986851883519);
void car_H_26(double *state, double *unused, double *out_925267005304730766);
void car_h_27(double *state, double *unused, double *out_2956758656507506241);
void car_H_27(double *state, double *unused, double *out_2362668065235122009);
void car_h_29(double *state, double *unused, double *out_5683118820211035643);
void car_H_29(double *state, double *unused, double *out_5047662721349939104);
void car_h_28(double *state, double *unused, double *out_7958431924168351886);
void car_H_28(double *state, double *unused, double *out_7011292992915265355);
void car_h_31(double *state, double *unused, double *out_5575133850212531145);
void car_H_31(double *state, double *unused, double *out_4697416286055747418);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}