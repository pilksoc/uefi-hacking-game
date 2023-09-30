#include <Uefi.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/Timer.h>
#include <Library/UefiBootServicesTableLib.h>

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
    setColour(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
}

void __hg_print_with_colour_at(const CHAR16 *format, int colour, int x, int y)
{
    setColour(colour);
    moveCursorTo(x, y);
    Print(format);
    resetColour();
}

void __hg_print_at(const CHAR16 *format, int colour, int x, int y)
{
    resetColour();
    moveCursorTo(x, y);
    Print(format);
}
