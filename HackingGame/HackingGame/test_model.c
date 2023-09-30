#include "./test_words.h"
#include "../../testing.h/testing.h"
#include "./model.h"
#include "words.h"
#include <string.h>

static int test_noise_map()
{
    ASSERT(HG_NOISE_VARIENTS == sizeof(HG_NOISE_MAP) - 1);
    for (hg_game_tile_t i = __HG_NOISE_SEG_START + 1; i < __HG_NOISE_SEG_END; i++) {
        ASSERT(HG_NOISE_MAP[i - __HG_NOISE_SEG_START - 1] != 0);
    }
    return 1;
}

static int test_init_game_state()
{
    hg_game_state_t state;
    memset(&state, 0xFF, sizeof(state));

    hg_game_state_init(&state);

    // Test the initial state
    ASSERT(state.retries > 0);
    ASSERT(state.correct_word_index >= 0);
    ASSERT(state.correct_word_index < hg_word_count());

    for (size_t x = 0; x < HG_GRID_ROWS; x++) {
        for(size_t y = 0; y < HG_GRID_COLS; y++) {
            ASSERT(state.grid[x][y] != 0xFF);
            ASSERT(state.grid[x][y] < __HG_NOISE_SEG_END);
            ASSERT(state.grid[x][y] != __HG_NOISE_SEG_START);
            ASSERT(state.grid[x][y] != __HG_NOISE_SEG_END);
        }
    }

    // Test word placement
    size_t words = 0;
    for(size_t y = 0; y < HG_GRID_COLS; y++) {
        for (size_t x = 0; x < HG_GRID_ROWS; x++) {
            if (state.grid[x][y] != HG_WORD) {
                continue;
            }
            words++;

            size_t offset = x + y * HG_GRID_ROWS;
            for (size_t i = 0; i < HG_WORD_LENGTH; i++) {
                size_t grid_offset = offset + i;
                size_t x = grid_offset % HG_GRID_ROWS;
                size_t y = grid_offset / HG_GRID_ROWS;

                ASSERT(state.grid[x][y] == HG_WORD);
            }

            size_t grid_offset = offset + 6;
            x = grid_offset % HG_GRID_ROWS;
            y = grid_offset / HG_GRID_ROWS;
            ASSERT(state.grid[x][y] != HG_WORD);
        }
    }

    ASSERT(words == HG_WORD_COUNT);

    return 1;
}

static int test_get_word_at_1()
{
    hg_game_state_t state;
    memset(&state, 0, sizeof(state));
    memset(state.grid, HG_WORD, sizeof(state.grid));

    ASSERT(__hg_get_word_no_at(&state, 0, 0) == 0);
    return 1;
}

static int test_get_word_at_2()
{
    hg_game_state_t state;
    memset(&state, 0, sizeof(state));
    memset(state.grid, HG_NOISE_1, sizeof(state.grid));

    state.grid[0][0] = HG_WORD;
    state.grid[0][2] = HG_WORD;
    state.grid[0][4] = HG_WORD;
    state.grid[0][6] = HG_WORD;
    state.grid[0][7] = HG_WORD;
    state.grid[10][10] = HG_WORD;

    ASSERT(__hg_get_word_no_at(&state, 10, 10) == 5);
    return 1;
}

static int test_submit_invalid_word()
{
    hg_game_state_t state;
    memset(&state, 0, sizeof(state));
    memset(state.grid, HG_NOISE_1, sizeof(state.grid));
    state.correct_word_index = 123;
    state.word_indexes[0] = 123;

    state.grid[0][0] = HG_WORD;
    state.grid[0][2] = HG_WORD;
    state.grid[0][4] = HG_WORD;
    state.grid[0][6] = HG_WORD;
    state.grid[0][7] = HG_WORD;
    state.grid[10][10] = HG_WORD;

    ASSERT(__hg_submit_event_handle_word(&state, 10, 10) == hg_submit_event(&state, 10, 10));

    state.retries = 1;
    ASSERT(__hg_submit_event_handle_word(&state, 10, 10) == HG_SUBMIT_WORD_FAIL);
    ASSERT(state.retries == 0);
    return 1;
}

static int test_submit_valid_word()
{
    hg_game_state_t state;
    memset(&state, 0, sizeof(state));
    memset(&state.grid, HG_NOISE_1, sizeof(state.grid));
    state.correct_word_index = 123;
    state.word_indexes[0] = 123;

    state.grid[0][0] = HG_WORD;
    state.grid[0][2] = HG_WORD;
    state.grid[0][4] = HG_WORD;
    state.grid[0][6] = HG_WORD;
    state.grid[0][7] = HG_WORD;
    state.grid[10][10] = HG_WORD;

    ASSERT(__hg_submit_event_handle_word(&state, 0, 0) == hg_submit_event(&state, 0, 0));
    ASSERT(__hg_submit_event_handle_word(&state, 0, 0) == HG_SUBMIT_WORD_SUCCESS);
    ASSERT(state.retries == 0);
    return 1;
}

SUB_TEST(test_model, {&test_noise_map, "Test noise map"},
{&test_init_game_state, "Test game state init"},
{&test_get_word_at_1, "Test get word index 1"},
{&test_get_word_at_2, "Test get word index 2"},
{&test_submit_invalid_word, "Test submit invalid word"},
{&test_submit_valid_word, "Test submit valid word"})
