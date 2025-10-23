#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void live_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_9(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_12(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_35(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_32(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_33(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_H(double *in_vec, double *out_4851806503631677);
void live_err_fun(double *nom_x, double *delta_x, double *out_3875369336362817395);
void live_inv_err_fun(double *nom_x, double *true_x, double *out_2645324593160890975);
void live_H_mod_fun(double *state, double *out_6561409780629387220);
void live_f_fun(double *state, double dt, double *out_4125999630483981146);
void live_F_fun(double *state, double dt, double *out_7442995747034510422);
void live_h_4(double *state, double *unused, double *out_616819498130942584);
void live_H_4(double *state, double *unused, double *out_2550913927795916992);
void live_h_9(double *state, double *unused, double *out_6999757141510273012);
void live_H_9(double *state, double *unused, double *out_2792103574425507637);
void live_h_10(double *state, double *unused, double *out_9127203571950312560);
void live_H_10(double *state, double *unused, double *out_3561658941965941475);
void live_h_12(double *state, double *unused, double *out_2499031427278358051);
void live_H_12(double *state, double *unused, double *out_7570370335827878787);
void live_h_35(double *state, double *unused, double *out_8252615349203803334);
void live_H_35(double *state, double *unused, double *out_5917575985168524368);
void live_h_32(double *state, double *unused, double *out_6699054471095787912);
void live_H_32(double *state, double *unused, double *out_6727863716333414690);
void live_h_13(double *state, double *unused, double *out_577456594247802333);
void live_H_13(double *state, double *unused, double *out_6278128523780275173);
void live_h_14(double *state, double *unused, double *out_6999757141510273012);
void live_H_14(double *state, double *unused, double *out_2792103574425507637);
void live_h_33(double *state, double *unused, double *out_6265834110182436014);
void live_H_33(double *state, double *unused, double *out_9068132989807381972);
void live_predict(double *in_x, double *in_P, double *in_Q, double dt);
}