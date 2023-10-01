#pragma once
#include <Uefi.h>
#include "model.h"

struct hg_cursor_t {
    int x;
    int y;
} typedef hg_cursor_t;

void __hg_clear_screen();
void __hg_move_cursor_to(int x, int y);
void __hg_set_colour(int colour);
void __hg_reset_colour();

/// const CHAR16 *, int, int, int, ...
/// Due to the stanky code you sometime need to add ,0 to calls
#define __hg_print_with_colour_at(format, colour, x, y, ...) \
    __hg_set_colour(colour); \
    __hg_move_cursor_to(x, y); \
    Print(format, __VA_ARGS__); \
    __hg_reset_colour();

/// const CHAR16 *, int, int, int, ...
/// Due to the stanky code you sometime need to add ,0 to calls
#define __hg_print_at(format, colour, x, y, ...) \
    __hg_reset_colour(); \
    __hg_move_cursor_to(x, y); \
    Print(format, __VA_ARGS__);

void hg_draw_screen(hg_game_state_t *state, size_t cursor_x, size_t cursor_y, hg_submit_event_t last);
