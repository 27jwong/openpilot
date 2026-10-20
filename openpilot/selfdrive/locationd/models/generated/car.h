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
void car_err_fun(double *nom_x, double *delta_x, double *out_9051704423920016590);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_8262520073968428420);
void car_H_mod_fun(double *state, double *out_8032114232992966708);
void car_f_fun(double *state, double dt, double *out_4567942705157615163);
void car_F_fun(double *state, double dt, double *out_3687136313485263185);
void car_h_25(double *state, double *unused, double *out_2888373885214709447);
void car_H_25(double *state, double *unused, double *out_1161073992919270529);
void car_h_24(double *state, double *unused, double *out_3830852376846248192);
void car_H_24(double *state, double *unused, double *out_5907583319885779563);
void car_h_30(double *state, double *unused, double *out_6420652903810990987);
void car_H_30(double *state, double *unused, double *out_1357258965587978098);
void car_h_26(double *state, double *unused, double *out_1418979636150145342);
void car_H_26(double *state, double *unused, double *out_4902577311793326753);
void car_h_27(double *state, double *unused, double *out_6566216540079153322);
void car_H_27(double *state, double *unused, double *out_817504346212446813);
void car_h_29(double *state, double *unused, double *out_5750748910700158548);
void car_H_29(double *state, double *unused, double *out_1867490309902370282);
void car_h_28(double *state, double *unused, double *out_6538660297744367731);
void car_H_28(double *state, double *unused, double *out_3214908707167160292);
void car_h_31(double *state, double *unused, double *out_3804624550919326526);
void car_H_31(double *state, double *unused, double *out_5528785414026678229);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}