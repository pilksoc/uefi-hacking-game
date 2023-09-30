#include <Uefi.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/Timer.h>
#include <Library/UefiBootServicesTableLib.h>

#include "model.h"

void __hg_clear_screen()
{
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);
    gST->ConOut->ClearScreen(gST->ConOut);
}

void __hg_move_cursor_to(int x, int y)
{
    gST->ConOut->SetCursorPosition(gST->ConOut, x - 1, y - 1);
}

void __hg_set_colour(int colour)
{
    gST->ConOut->SetAttribute(gST->ConOut, colour);
}

void __hg_reset_colour()
{
    __hg_set_colour(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
}

void __hg_print_with_colour_at(const CHAR16 *format, int colour, int x, int y)
{
    __hg_set_colour(colour);
    __hg_move_cursor_to(x, y);
    Print(format);
    __hg_reset_colour();
}

void __hg_print_at(const CHAR16 *format, int colour, int x, int y)
{
    __hg_reset_colour();
    __hg_move_cursor_to(x, y);
    Print(format);
}

void hg_draw_screen(hg_game_state_t *state)
{
    __hg_clear_screen();
    __hg_print_with_colour_at(L"Welcome to ROBCO Industries (TM) Termlink\n\0", EFI_GREEN, 0, 0);
    __hg_print_with_colour_at(L"Password required\n\0", EFI_GREEN, 0, 2);
}