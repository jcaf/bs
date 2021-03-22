#ifndef LOGF_MENU_COMMON_H_
#define LOGF_MENU_COMMON_H_

int8_t mLogf_common_digInput_job(int8_t *opt_dest);
int8_t mLogf_common_anInput_job(int8_t *opt_dest);
int8_t mLogf_common_relay_job(int8_t *opt_dest);
int8_t mLogf_common_logicalOP_job(int8_t *opt_dest);
int8_t mLogf_common_relationalOP_job(int8_t *opt_dest);
int8_t mLogf_common_arithmeticOP_job(int8_t *opt_dest);
int8_t mLogf_common_asociativityOP_job(int8_t *opt_dest);
int8_t mLogf_common_punctuationmarkOP_job(int8_t *opt_dest);
int8_t mLogf_common_assignmentOP_job(int8_t *opt_dest);
int8_t mLogf_common_literal_job(float *valf_dest);
int8_t mLogf_common_assign_relays_available_job(int8_t *opt_dest, int8_t piz_action_subtype);
int8_t mLogf_common_assign_fx_timer_job(uint8_t *rele_dest, float *valf_dest, int8_t piz_action_subtype);
int8_t mLogf_common_select_logexp_job(int8_t *opt_dest);
int8_t mLogf_common_view_logExp_job(uint8_t logexp);
#endif // LOGF_MENU_COMMON_H_
