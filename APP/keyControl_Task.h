#ifndef __KEYCONTROL_TASK_H
#define __KEYCONTROL_TASK_H
#include "system.h"

extern _Bool setting_mode;
extern _Bool need_redraw_main;
extern uint8_t cursor_pos;
extern uint8_t exit_count;

void keyControl_Task(void *pvParameters);
void draw_setting_screen(void);
void update_cursor_position(uint8_t new_pos);
void handle_exit_count(void);
void check_and_exit(void);

#endif
