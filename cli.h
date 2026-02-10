#ifndef GIFMETADATA_CLI_H
#define GIFMETADATA_CLI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Max bytes allowed for a CLI argument buffer.
#define CLI_ARG_MAX_LEN 4096

#define CLI_SUCCESS 0
#define CLI_INVALID_FLAG -1
// Failed to alloc or given argument was NULL. Former is most
// likely due to memory issues on host.
#define CLI_ALLOC_FAILURE -2
#define CLI_EXCEEDS_ARG_MAX_LEN -3
// Multiple inputs provided when expecting one.
#define CLI_MULTIPLE_INPUTS -4
// Multiple outputs provided when expecting one.
#define CLI_MULTIPLE_OUTPUTS -5
// Flag called which expects an input, e.g. -a <input>
#define CLI_MISSING_FLAG_ARG -6

// Linked list member for containing CLI flag arguments. Used for
// comment insertion flag '-c' which allows for multiple comments.
typedef struct cli_flag_arg {
    // String buffer, null terminated.
    char *string;
    // Length of string buffer, string length will always match
    // buffer size.
    size_t string_len;
    // Next member in the list
    struct cli_flag_arg *next;
} cli_flag_arg;

// CLI user arguments provided at runtime, contains static expected
// members not allowing for dynamic flags.
typedef struct cli_user_args {
    // -a Display all extensions, not only comments: plain_text,
    //    application, application_subblock, comment
    int all_flag;
    // -v Display additional GIF metadata: file size, width,
    //    height, etc
    int verbose_flag;
    // -h Display program help
    int help_flag;
    // -s Scrub existing comments upon output GIF
    int scrub_flag;

    // Linked list of all comments provided by -c flag for
    // comment insertion
    cli_flag_arg *comment_flags;
    // Single linked list item of -o output flag. Item is a linked
    // item but 'next' field is always NULL.
    cli_flag_arg *output_flag;

    // Set to the invalid input flag that caused the error.
    char invalid_flag;

    // Filename string buffer, can be NULL in which case stdin is
    // used as input. Null terminated.
    char *filename;
    // Filename string buffer length, string length will always match
    // buffer size.
    size_t filename_len;
} cli_user_args;

// Allocate a new CLI user arguments struct to be provided
// at runtime. Can be freed with 'cli_user_args_free'.
cli_user_args *cli_user_args_new();
// Free existing CLI user arguments struct. All allocated member
// fields will be appropriately freed using their respective methods.
void cli_user_args_free(cli_user_args *a);
// Parse argv and produce a CLI user arguments struct. Given struct
// argument must already be allocated using 'cli_user_args_new'.
// See 'cli.h' for error code descriptions.
int cli_parse(cli_user_args *a, int argc, char **argv);

#endif
