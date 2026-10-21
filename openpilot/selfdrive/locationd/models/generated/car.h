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
void car_err_fun(double *nom_x, double *delta_x, double *out_8940518425730742921);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_928144761037053321);
void car_H_mod_fun(double *state, double *out_9149626453786373432);
void car_f_fun(double *state, double dt, double *out_8450363153625730423);
void car_F_fun(double *state, double dt, double *out_1973945452823698766);
void car_h_25(double *state, double *unused, double *out_483576256057780874);
void car_H_25(double *state, double *unused, double *out_6311960329387577407);
void car_h_24(double *state, double *unused, double *out_4979738999913224721);
void car_H_24(double *state, double *unused, double *out_2329637177989809509);
void car_h_30(double *state, double *unused, double *out_8246841512878453315);
void car_H_30(double *state, double *unused, double *out_8830293287894826034);
void car_h_26(double *state, double *unused, double *out_1244078809085387536);
void car_H_26(double *state, double *unused, double *out_2570457010513521183);
void car_h_27(double *state, double *unused, double *out_3520362307502113559);
void car_H_27(double *state, double *unused, double *out_7392856714630782365);
void car_h_29(double *state, double *unused, double *out_3245168245217607670);
void car_H_29(double *state, double *unused, double *out_9106219441500333398);
void car_h_28(double *state, double *unused, double *out_4463872160403775994);
void car_H_28(double *state, double *unused, double *out_4258125615139687644);
void car_h_31(double *state, double *unused, double *out_7071404182534076037);
void car_H_31(double *state, double *unused, double *out_1944248908280169707);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}