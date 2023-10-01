#include <Uefi.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/Timer.h>
#include <Library/UefiBootServicesTableLib.h>

#include "model.h"
#include "display.h"

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.



**/
EFI_STATUS EFIAPI
UefiMain(IN
         EFI_HANDLE ImageHandle,
         IN
         EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_INPUT_KEY key;

    hg_game_state_t gState;
    hg_game_state_init(&gState);

    EFI_EVENT events[1];
    events[0] = gST->ConIn->WaitForKey;

    gST->ConOut->EnableCursor(gST->ConOut, 0);

    UINTN eventType;
    hg_cursor_t cursor_loc;

    cursor_loc.x = 0;
    cursor_loc.y = 0;

    hg_submit_event_t last = HG_SUBMIT_INVALID;

    do {
        hg_draw_screen(&gState, cursor_loc.x, cursor_loc.y, last);
        gBS->WaitForEvent(1, events, &eventType);

        if (eventType != 0) {
            continue;
        }
        gST->ConIn->ReadKeyStroke(gST->ConIn, &key);
        switch (key.ScanCode) {
        case SCAN_UP:
            if (cursor_loc.y - 1 > 0) {
                cursor_loc.y--;
            }
            break;
        case SCAN_DOWN:
            if (cursor_loc.y + 1 < HG_GRID_COLS) {
                cursor_loc.y++;
            }
            break;
        case SCAN_LEFT:
            if (cursor_loc.x - 1 > 0) {
                cursor_loc.x--;
            } else {
                if (cursor_loc.y - 1 > 0) {
                    cursor_loc.x = HG_GRID_ROWS - 1;
                    cursor_loc.y--;
                }
            }
            break;
        case SCAN_RIGHT:
            if (cursor_loc.x + 1 < HG_GRID_ROWS) {
                cursor_loc.x++;
            } else {
                if (cursor_loc.y + 1 < HG_GRID_COLS) {
                    cursor_loc.x = 0;
                    cursor_loc.y++;
                }
            }
            break;
        default:
            // TODO: This is stanky
            last = hg_submit_event(&gState, cursor_loc.x, cursor_loc.y);
            break;
        }
    } while (key.ScanCode != SCAN_END && gState.retries > 0 && last != HG_SUBMIT_WORD_SUCCESS);
    hg_draw_screen(&gState, cursor_loc.x, cursor_loc.y, last);

    return EFI_SUCCESS;
}
