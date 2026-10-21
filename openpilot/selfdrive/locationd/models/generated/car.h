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
void car_err_fun(double *nom_x, double *delta_x, double *out_3628762341002155077);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_8326913521565900806);
void car_H_mod_fun(double *state, double *out_1936405662837462559);
void car_f_fun(double *state, double dt, double *out_5241560014263660552);
void car_F_fun(double *state, double dt, double *out_8799266228203575332);
void car_h_25(double *state, double *unused, double *out_1070812002986692230);
void car_H_25(double *state, double *unused, double *out_7317150184959851820);
void car_h_24(double *state, double *unused, double *out_8298197210527653023);
void car_H_24(double *state, double *unused, double *out_5144500585954352254);
void car_h_30(double *state, double *unused, double *out_5700023223363658706);
void car_H_30(double *state, double *unused, double *out_8611260930242451169);
void car_h_26(double *state, double *unused, double *out_8348768769064263898);
void car_H_26(double *state, double *unused, double *out_7825067918988899195);
void car_h_27(double *state, double *unused, double *out_6986442756720554566);
void car_H_27(double *state, double *unused, double *out_6387666859058507952);
void car_h_29(double *state, double *unused, double *out_546427834096514762);
void car_H_29(double *state, double *unused, double *out_8101029585928058985);
void car_h_28(double *state, double *unused, double *out_5128975417745875440);
void car_H_28(double *state, double *unused, double *out_5263315470711962057);
void car_h_31(double *state, double *unused, double *out_2521443395615575397);
void car_H_31(double *state, double *unused, double *out_8451276021222250671);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}