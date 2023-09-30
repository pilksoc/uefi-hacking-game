#include <Uefi.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/Timer.h>
#include <Library/UefiBootServicesTableLib.h>

void moveCursorTo(int x, int y)
{
    gST->ConOut->SetCursorPosition(gST->ConOut, x - 1, y - 1);
}

void setColour(int colour)
{
    gST->ConOut->SetAttribute(gST->ConOut, colour);
}

void resetColour()
{
    setColour(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
}

void printAtWithColour(const CHAR16 *format, int colour, int x, int y)
{
    setColour(colour);
    moveCursorTo(x, y);
    Print(format);
    resetColour();
}

int testThis()
{
    moveCursorTo(1, 1);
    Print("hi mum");
    printAtWithColour("hi mum", EFI_LIGHTCYAN, 5, 5);
}