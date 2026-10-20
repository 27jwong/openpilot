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
void car_err_fun(double *nom_x, double *delta_x, double *out_4405174283250457033);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_3715751562580217979);
void car_H_mod_fun(double *state, double *out_5892011097711960144);
void car_f_fun(double *state, double dt, double *out_7606416760447182432);
void car_F_fun(double *state, double dt, double *out_5926993682079281873);
void car_h_25(double *state, double *unused, double *out_7732569018122326573);
void car_H_25(double *state, double *unused, double *out_9031752326008099795);
void car_h_24(double *state, double *unused, double *out_2592205880786391203);
void car_H_24(double *state, double *unused, double *out_6854537902400949822);
void car_h_30(double *state, double *unused, double *out_3870885541693824921);
void car_H_30(double *state, double *unused, double *out_6513419367500851168);
void car_h_26(double *state, double *unused, double *out_4698435374544781740);
void car_H_26(double *state, double *unused, double *out_5673488428827395597);
void car_h_27(double *state, double *unused, double *out_1732205883580384278);
void car_H_27(double *state, double *unused, double *out_4289825296316907951);
void car_h_29(double *state, double *unused, double *out_5038629342769966658);
void car_H_29(double *state, double *unused, double *out_6003188023186458984);
void car_h_28(double *state, double *unused, double *out_87916189219147600);
void car_H_28(double *state, double *unused, double *out_7361157033453562058);
void car_h_31(double *state, double *unused, double *out_4493797872740211074);
void car_H_31(double *state, double *unused, double *out_9001106364131139367);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}