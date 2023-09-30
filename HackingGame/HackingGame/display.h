#pragma once

#include <Uefi.h>
#include "model.h"

#define HG_RES_X 22
#define HG_RES_Y 57

void hg_draw_screen(hg_game_state_t *state);

void __hg_clear_screen();
void __hg_move_cursor_to(int x, int y);
void __hg_set_colour(int colour);
void __hg_reset_colour();
void __hg_print_at(const CHAR16 *format, int colour, int x, int y);
void __hg_print_with_colour_at(const CHAR16 *format, int colour, int x, int y);
