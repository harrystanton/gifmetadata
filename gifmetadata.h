#ifndef GIFMETADATA_H
#define GIFMETADATA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// error
#define GIFMETADATA_SUCCESS 0 
#define GIFMETADATA_INVALID_SIG -1
#define GIFMETADATA_COMMENT_EXCEEDS_BOUNDS -2
#define GIFMETADATA_ALLOC_FAILURE -3

#define GIFMETADATA_SCRATCHPAD_CHUNK_SIZE 256

enum gifmetadata_gif_version {
    gif87a = 1,
    gif89a = 2
};

// lsd = local screen descriptor
enum gifmetadata_lsd_state {
    width,
    height,
    packed,
    bg_color,
    pixel_aspect_ratio
};

// extension callbacks

enum gifmetadata_extension_type {
    plain_text,
    application,
    application_subblock,
    comment
};

// state callbacks

enum gifmetadata_read_state {
    header,
    logical_screen_descriptor,
    global_color_table,
    control_extension,
    image_descriptor,
    local_color_table,
    image_data,
    extension,
    known_extension,
    unknown_extension,
    trailer,
    searching
};

typedef struct gifmetadata_state {
    enum gifmetadata_read_state read_state;

    // externally managed buffers provided at each parse, do not
    // attempt to edit or free
    unsigned char *chunk;
    size_t chunk_len;
    int chunk_i;

    // file_i
    int file_i;

    int color_table_size;
    int color_table_len;

    // scratchpad is a buffer used for reading/writing while traversing through
    // the file. must be initalized before struct use, can be realloc'd later.
    unsigned char *scratchpad;
    // allocated size of the buffer
    size_t scratchpad_size;
    // i is max index of written data in the buffer
    int scratchpad_i;
    // len is the expected length to write into the buffer according
    // to an unsigned char that prefixes data in the GIF data, can
    // be inaccurate for comments because some applications overloaded
    // comment data
    int scratchpad_len;

    // local screen descriptor
    enum gifmetadata_lsd_state local_lsd_state;
    enum gifmetadata_extension_type local_extension_type;

    int global_color_table_flag;
    int color_resolution;

    // info
    uint16_t canvas_width;
    uint16_t canvas_height;

    enum gifmetadata_gif_version gif_version;
} gifmetadata_state;

typedef struct gifmetadata_extension_info {
    enum gifmetadata_extension_type type;
    unsigned char *buffer;
    size_t buffer_len;
} gifmetadata_extension_info;

// Implementation can be found in gif.c
int gifmetadata_parse_gif(
    gifmetadata_state *s,
    unsigned char *chunk,
    size_t chunk_len,
    void (*extension_cb)(gifmetadata_state*, gifmetadata_extension_info*),
    void (*state_cb)(gifmetadata_state*, enum gifmetadata_read_state));

gifmetadata_state *gifmetadata_state_new();
void gifmetadata_state_free(gifmetadata_state *state);

#endif
