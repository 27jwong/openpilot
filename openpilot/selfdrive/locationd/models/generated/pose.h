#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_7121613786142052873);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_4360274276000378130);
void pose_H_mod_fun(double *state, double *out_6957806766333672764);
void pose_f_fun(double *state, double dt, double *out_5556993601589438608);
void pose_F_fun(double *state, double dt, double *out_5351909948517241954);
void pose_h_4(double *state, double *unused, double *out_358390589776990143);
void pose_H_4(double *state, double *unused, double *out_1907921248207480922);
void pose_h_10(double *state, double *unused, double *out_3316959461566506895);
void pose_H_10(double *state, double *unused, double *out_913840009049565840);
void pose_h_13(double *state, double *unused, double *out_2811325568776580864);
void pose_H_13(double *state, double *unused, double *out_1304352577124851879);
void pose_h_14(double *state, double *unused, double *out_470700127672570802);
void pose_H_14(double *state, double *unused, double *out_2055319608132003607);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}