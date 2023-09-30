#include <Uefi.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/Timer.h>
#include <Library/UefiBootServicesTableLib.h>
#include "./model.h"
#include "./display.h"
#include "./words.h"

static void __hg_clear_screen()
{
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);
    gST->ConOut->ClearScreen(gST->ConOut);
}

static void __hg_move_cursor_to(int x, int y)
{
    gST->ConOut->SetCursorPosition(gST->ConOut, x - 1, y - 1);
}

static void __hg_set_colour(int colour)
{
    gST->ConOut->SetAttribute(gST->ConOut, colour);
}

static void __hg_reset_colour()
{
    __hg_set_colour(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
}

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

static char __hg_get_char(hg_game_state_t *state, hg_game_tile_t tile, size_t word_index, size_t word_offset)
{
    hg_word_t ret;
    int s = 0;

    switch (tile) {
    case HG_WORD:
        s = hg_word_at(state->word_indexes[word_index], ret);
        if (s) {
            return ret[word_offset] - 'a' + 'A';
        } else {
            return '-';
        }
    case HG_WORD_DUD:
        return '.';
    case HG_ANGULAR_OPEN:
        return '<';
    case HG_ANGULAR_CLOSE:
        return '>';
    case HG_ROUND_OPEN:
        return '(';
    case HG_ROUND_CLOSE:
        return ')';
    case HG_SQUARE_OPEN:
        return '[';
    case HG_SQUARE_CLOSE:
        return ']';
    case HG_SQUIGGLE_OPEN:
        return '{';
    case HG_SQUIGGLE_CLOSE:
        return '}';
    case HG_NOISE_1:
        return '!';
    case HG_NOISE_2:
        return ',';
    case HG_NOISE_3:
        return '.';
    case HG_NOISE_4:
        return '%';
    case HG_NOISE_5:
        return '|';
    case HG_NOISE_6:
        return '#';
    case HG_NOISE_7:
        return ':';
    case HG_NOISE_8:
        return '+';
    case HG_NOISE_9:
        return '/';
    case HG_NOISE_10:
        return '?';
    case HG_NOISE_11:
        return '"';
    case HG_NOISE_12:
        return '\'';
    case HG_NOISE_13:
        return '^';
    default:
        return '-';
    }
}

void hg_draw_screen(hg_game_state_t *state)
{
    __hg_clear_screen();
    __hg_print_with_colour_at(L"WELCOME TO ROBCO INDUSTRIES (TM) TERMLINK\n\0", EFI_GREEN, 0, 0, 0);
    __hg_print_with_colour_at(L"ENTER PASSWORD NOW\n\0", EFI_GREEN, 0, 2, 0);

    size_t attemps_y = 4;
    __hg_print_with_colour_at(L"%d ATTEMPTS LEFT:\0", EFI_GREEN, 0, attemps_y, state->retries);

    for (size_t i = 0; i < state->retries; i++) {
        __hg_print_with_colour_at(L"X\0", EFI_GREEN, sizeof("12 ATTEMPTS LEFT:  ") + i * 2, attemps_y, 0);

        if (i == state->retries - 1) {
            __hg_print_with_colour_at(L"\n\0", EFI_GREEN, sizeof("12 ATTEMPTS LEFT:  ") + 2 + i * 2, attemps_y, 0);
        }
    }

    size_t count = 0;
    size_t word_offset = 0;

    for (size_t y_raw = 0; y_raw < HG_GRID_COLS / 2; y_raw++, count++) {
        size_t y = y_raw / 2;
        size_t offset_y = y / 2 + 5;
        __hg_print_with_colour_at(L"\n0xF7%02d\0", EFI_GREEN, 0, offset_y, count);

        for (size_t x = 0; x < HG_GRID_ROWS; x++) {
            if (state->grid[x][y] == HG_WORD) {
                word_offset++;
            } else {
                word_offset = 0;
            }

            __hg_print_with_colour_at(L"%c\0", EFI_GREEN, x + 8, offset_y, __hg_get_char(state, state->grid[x][y], __hg_get_word_no_at(state, x, y), word_offset));
        }

        y = y_raw / 2 + 1;
        size_t offset_x = sizeof("0faaaa    ") + HG_GRID_ROWS;
        __hg_print_with_colour_at(L"0xF7%02d\0", EFI_GREEN, offset_x, offset_y, count);

        for (size_t x = 0; x < HG_GRID_ROWS; x++) {
            if (state->grid[x][y] == HG_WORD) {
                word_offset++;
            } else {
                word_offset = 0;
            }

            __hg_print_with_colour_at(L"%c\0", EFI_GREEN, x + 8 + offset_x, offset_y, __hg_get_char(state, state->grid[x][y], __hg_get_word_no_at(state, x, y), word_offset));
        }
    }

    __hg_print_with_colour_at(L">\n\0", EFI_GREEN, 0, HG_GRID_COLS + 6, 0);
}
