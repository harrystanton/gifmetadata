#include "cli.h"

// Allocate a new CLI flag argument linked list. Can be freed with
// 'cli_flag_arg_item' however parent struct 'cli_user_args_free'
// will also free field 'comment_flags'.
cli_flag_arg *cli_flag_arg_new() {
    cli_flag_arg *new_flag_arg = malloc(sizeof(cli_flag_arg));
    if (new_flag_arg == NULL) {
        return NULL;
    }
    new_flag_arg->string = NULL;
    new_flag_arg->string_len = 0;
    new_flag_arg->next = NULL;
    return new_flag_arg;
}

// Free existing CLI flag argument linked list, will destroy all
// members of the list. 'cli_user_args_free' calls this method on
// field 'comment_flags'.
void cli_flag_arg_free(cli_flag_arg *arg) {
    cli_flag_arg *item = arg;
    while (item != NULL) {
        if (item->string != NULL) {
            free(item->string);
        }
        cli_flag_arg *to_free = item;
        item = item->next;
        free(to_free);
    } 
}

// Append a new item to the CLI flag argument linked list. 'src'
// must be allocated using 'cli_flag_arg_new'.
void cli_flag_arg_append(cli_flag_arg *dst, cli_flag_arg *src) {
    cli_flag_arg *end = dst;
    while (dst->next != NULL) {
        end = end->next;
    }
    end->next = src;
}

cli_user_args *cli_user_args_new() {
    cli_user_args *a = malloc(sizeof(cli_user_args));
    if (a == NULL) {
        return NULL;
    }
    memset(a, 0, sizeof(cli_user_args));
    a->comment_flags = NULL;
    a->output_flag = NULL;
    a->filename = NULL;
    return a;
}

void cli_user_args_free(cli_user_args *a) {
    if (a == NULL)
        return;

    if (a->filename != NULL) free(a->filename);
    if (a->comment_flags != NULL) cli_flag_arg_free(a->comment_flags);

    free(a);
}

int cli_parse(cli_user_args *a, int argc, char **argv) {
    if (a == NULL) return CLI_ALLOC_FAILURE;

    cli_flag_arg *awaiting_flag_arg = NULL;

    // loop each command line arguments
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];

        // determine len of arg for looping character-by-character
        size_t arg_len = strnlen(arg, CLI_ARG_MAX_LEN);
        if (arg_len == CLI_ARG_MAX_LEN) {
            return CLI_EXCEEDS_ARG_MAX_LEN;
        }

        // if awaiting flag arg, capture
        if (awaiting_flag_arg != NULL) {
            awaiting_flag_arg->string = malloc(arg_len + 1);
            if (awaiting_flag_arg->string == NULL) {
                return CLI_ALLOC_FAILURE;
            }
            awaiting_flag_arg->string_len = arg_len + 1;
            strncpy(awaiting_flag_arg->string, arg, arg_len);
            awaiting_flag_arg->string[arg_len] = 0;

            awaiting_flag_arg = NULL;
            a->invalid_flag = 0;
            continue;
        }
        
        if (arg[0] == '-' && arg_len > 1) {
            // parse flags
            for (int arg_i = 1; arg_i < arg_len; arg_i++) {
                char flag_c = arg[arg_i];
                switch (flag_c) {
                case 'h':
                    a->help_flag = 1;
                    break;
                case 'a':
                    a->all_flag = 1;
                    break;
                case 'v':
                    a->verbose_flag = 1;
                    break;
                case 's':
                    a->scrub_flag = 1;
                    break;
                case 'c':
                    awaiting_flag_arg = cli_flag_arg_new();
                    if (awaiting_flag_arg == NULL) {
                        return CLI_ALLOC_FAILURE;
                    }
                    if (a->comment_flags != NULL) {
                        cli_flag_arg_append(a->comment_flags, awaiting_flag_arg);
                    } else {
                        a->comment_flags = awaiting_flag_arg;
                    }
                    // used for error messages if the awaiting flag arg is
                    // never fulfilled
                    a->invalid_flag = flag_c;
                    break;
                case 'o':
                    if (a->output_flag != NULL) {
                        printf("ERROR more than one output provided\n");
                    }
                    a->output_flag = cli_flag_arg_new();
                    if (a->output_flag == NULL) {
                        return CLI_ALLOC_FAILURE;
                    }
                    awaiting_flag_arg = a->output_flag;
                    break;
                default:
                    a->invalid_flag = flag_c;
                    return CLI_INVALID_FLAG;
                }
            }
        } else {
            // filename
            if (a->filename != NULL) {
                return CLI_MULTIPLE_INPUTS;
            }
            // copy the filename to its own buffer
            char *filename = malloc(arg_len + 1);
            if (filename == NULL) {
                return CLI_ALLOC_FAILURE;
            }
            strncpy(filename, arg, arg_len);
            filename[arg_len] = 0;
            a->filename = filename;
            a->filename_len = arg_len + 1;
        }
    }

    if (awaiting_flag_arg != NULL) {
        return CLI_MISSING_FLAG_ARG;
    }

    return CLI_SUCCESS;
}

