#include "gifmetadata.h"

gifmetadata_state *gifmetadata_state_new() {
    gifmetadata_state *state = malloc(sizeof(gifmetadata_state));
    if (state == NULL)
        return NULL;

    state->file_i = 0;

    // configure the scratchpad

    state->scratchpad = malloc(GIFMETADATA_SCRATCHPAD_CHUNK_SIZE);
    if (state->scratchpad == NULL) {
        free(state);
        return NULL;
    }
    state->scratchpad_size = GIFMETADATA_SCRATCHPAD_CHUNK_SIZE;
    state->scratchpad_i = 0;
    state->scratchpad_len = 0;

    // initial values
    state->read_state = header;
    state->color_table_size = 0;
    state->color_table_len = 0;
    state->color_resolution = -1;
    state->global_color_table_flag = -1;

    state->canvas_width = -1;
    state->canvas_height = -1;

    return state;
}

void gifmetadata_state_free(gifmetadata_state *state) {
    if (state->scratchpad != NULL)
        free(state->scratchpad);
    free(state);
}

int gifmetadata_parse_gif(
    gifmetadata_state *s,
    unsigned char *chunk,
    size_t chunk_len,
    void (*extension_cb)(gifmetadata_state*, gifmetadata_extension_info*),
    void (*state_cb)(gifmetadata_state*, enum gifmetadata_read_state));

