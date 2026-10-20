#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_2510165527669125945);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_2827775544900411455);
void pose_H_mod_fun(double *state, double *out_6581160706960496502);
void pose_f_fun(double *state, double dt, double *out_1715652979675739070);
void pose_F_fun(double *state, double dt, double *out_7854120589136729219);
void pose_h_4(double *state, double *unused, double *out_3221272873403501279);
void pose_H_4(double *state, double *unused, double *out_5826999704905783895);
void pose_h_10(double *state, double *unused, double *out_3970686424555567202);
void pose_H_10(double *state, double *unused, double *out_4590171680468794279);
void pose_h_13(double *state, double *unused, double *out_649122268436072591);
void pose_H_13(double *state, double *unused, double *out_2614725879573451094);
void pose_h_14(double *state, double *unused, double *out_113050053768907397);
void pose_H_14(double *state, double *unused, double *out_1863758848566299366);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}