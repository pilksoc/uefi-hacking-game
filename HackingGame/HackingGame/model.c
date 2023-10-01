#include "./model.h"
#include "words.h"

#define HG_STARTING_RETRIES 5
#define HG_WORD_MIN_GAP 3
/// The subtraction now means you can do HG_WORD_MIN_GAP + random() % HG_WORD_MAX_GAP
#define HG_WORD_MAX_GAP (7 - HG_WORD_MIN_GAP)

static size_t random()
{
    static unsigned long x=123456789, y=362436069, z=521288629;
    static unsigned long t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;
    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;

    return t;
}

void hg_game_state_init(hg_game_state_t *state)
{
    state->retries = HG_STARTING_RETRIES;

    // Generate random words
    for (size_t i = 0; i < HG_WORD_COUNT; i++) {
        state->word_indexes[i] = random() % hg_word_count();
    }
    state->correct_word_index = state->word_indexes[random() % HG_WORD_COUNT];

    // Populate the grid with noise
    for (size_t x = 0; x < HG_GRID_ROWS; x++) {
        for (size_t y = 0; y < HG_GRID_COLS; y++) {
            state->grid[x][y] = __HG_NOISE_SEG_START + 1 + random() % HG_NOISE_VARIENTS;

            if (random() % 13 == 0) {
                state->grid[x][y] = HG_ANGULAR_OPEN + random() % (HG_SQUIGGLE_CLOSE - HG_ANGULAR_OPEN);
            }
        }
    }

    // Add words to the grid
    size_t grid_offset = -HG_WORD_MIN_GAP; // This allows the start to be a word
    for (size_t i = 0; i < HG_WORD_COUNT; i++) {
        grid_offset += HG_WORD_MIN_GAP + random() % HG_WORD_MAX_GAP;
        for (size_t j = 0; j < HG_WORD_LENGTH; j++) {
            size_t x = grid_offset % HG_GRID_ROWS;
            size_t y = grid_offset / HG_GRID_ROWS;
            state->grid[x][y] = HG_WORD;
            grid_offset++;
        }
    }
}

size_t __hg_get_word_no_at(hg_game_state_t *state, size_t x_in, size_t y_in)
{
    size_t word_cnt = 0;
    int matching_word = 0;
    for (size_t y = 0; y < HG_GRID_COLS; y++) {
        for (size_t x = 0; x < HG_GRID_ROWS; x++) {
            if (y >= y_in && x > x_in) {
                return word_cnt -1;
            }

            if ((state->grid[x][y] == HG_WORD || state->grid[x][y] == HG_WORD_DUD) && !matching_word) {
                word_cnt++;
            }

            matching_word = state->grid[x][y] == HG_WORD || state->grid[x][y] == HG_WORD_DUD;
        }
    }

    return word_cnt - 1;
}

hg_submit_event_t __hg_submit_event_handle_word(hg_game_state_t *state, size_t x, size_t y)
{
    size_t word_no = __hg_get_word_no_at(state, x, y);
    if (state->word_indexes[word_no] == state->correct_word_index) {
        return HG_SUBMIT_WORD_SUCCESS;
    }
    state->retries--;
    return HG_SUBMIT_WORD_FAIL;
}

/// Returns true if the brackets match
static int __hg_submit_event_handle_open_brackets_look_forward(hg_game_state_t *state, size_t x_in, size_t y_in)
{
    for (size_t y = y_in; y < HG_GRID_COLS; y++) {
        for (size_t x = 0; x < HG_GRID_ROWS; x++) {
            if (y == y_in && x < x_in) {
                continue;
            }

            switch(state->grid[x][y]) {
            case HG_SQUIGGLE_CLOSE:
                return state->grid[x_in][y_in] == HG_SQUIGGLE_OPEN;
            case HG_SQUARE_CLOSE:
                return state->grid[x_in][y_in] == HG_SQUARE_OPEN;
            case HG_ROUND_CLOSE:
                return state->grid[x_in][y_in] == HG_ROUND_OPEN;
            case HG_ANGULAR_CLOSE:
                return state->grid[x_in][y_in] == HG_ANGULAR_OPEN;
            case HG_WORD:
            case HG_WORD_DUD:
                return 0;
            default:
                continue;
            }
        }
    }
    return 0;
}

static void __hg_make_dud(hg_game_state_t *state, size_t dud_index)
{
    size_t word_cnt = 0;
    int matching_word = 0;
    for (size_t y = 0; y < HG_GRID_COLS; y++) {
        for (size_t x = 0; x < HG_GRID_ROWS; x++) {
            if ((state->grid[x][y] == HG_WORD || state->grid[x][y] == HG_WORD_DUD) && !matching_word) {
                word_cnt++;
            }

            if (word_cnt > dud_index) {
                return;
            }

            matching_word = state->grid[x][y] == HG_WORD || state->grid[x][y] == HG_WORD_DUD;

            if (matching_word && word_cnt == dud_index) {
                state->grid[x][y] = HG_WORD_DUD;
            }
        }
    }
}

hg_submit_event_t __hg_submit_event_handle_open_brackets(hg_game_state_t *state, size_t x, size_t y, size_t (*random)())
{
    int ret = __hg_submit_event_handle_open_brackets_look_forward(state, x, y);
    if (ret) {
        state->grid[x][y] = HG_NOISE_1; // Janky lmao
        if (random() & 2) {
            state->retries++;
            return HG_SUBMIT_FOUND_RETRY;
        } else {
            size_t dud_index = random() % HG_WORD_COUNT;
            size_t initial = dud_index;
            while (state->correct_word_index == dud_index || state->word_indexes[dud_index] == HG_DUD_INDEX) {
                dud_index++;
                dud_index %= HG_WORD_COUNT;
                if (dud_index == initial) {
                    return HG_SUBMIT_INVALID;
                }
            }
            state->word_indexes[dud_index] = HG_DUD_INDEX;
            __hg_make_dud(state, dud_index);
            return HG_SUBMIT_FOUND_DUD;
        }
    }
    return HG_SUBMIT_INVALID;
}

hg_submit_event_t hg_submit_event(hg_game_state_t *state, size_t x, size_t y)
{
    switch(state->grid[x][y]) {
    case HG_WORD:
        return __hg_submit_event_handle_word(state, x, y);
    case HG_SQUIGGLE_OPEN:
    case HG_SQUARE_OPEN:
    case HG_ROUND_OPEN:
    case HG_ANGULAR_OPEN:
        return __hg_submit_event_handle_open_brackets(state, x, y, &random);
    default:
        return HG_SUBMIT_INVALID;
    }
}
