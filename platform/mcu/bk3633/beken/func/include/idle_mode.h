#ifndef _IDLE_MODE_H
#define _IDLE_MODE_H


typedef void (*idle_task_app_hook_t)(void);
typedef void (*idle_task_app_sleep_start_t)(void);
idle_task_app_hook_t idle_task_get_hook(void);
void idle_task_register_hook(idle_task_app_hook_t app_hook);
idle_task_app_sleep_start_t idle_task_sleep_start(void);
void idle_task_register_sleep_start(idle_task_app_sleep_start_t sleep_start);
void idle_task_set_sleep_start_time(uint32_t start_time);

uint8_t idle_mode(void);
void sleep_mode_enable(uint8_t enable);
uint8_t get_sleep_flag(void);
uint8_t get_rw_sleep_flag(void);
void idle_mode_never_sleep(uint8_t sleep_state);
uint8_t idle_model_get_never_sleep_state(void);
#endif