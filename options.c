/*

   This is free and unencumbered software released into the public domain.

   Anyone is free to copy, modify, publish, use, compile, sell, or
   distribute this software, either in source code form or as a compiled
   binary, for any purpose, commercial or non-commercial, and by any
   means.

   In jurisdictions that recognize copyright laws, the author or authors
   of this software dedicate any and all copyright interest in the
   software to the public domain. We make this dedication for the benefit
   of the public at large and to the detriment of our heirs and
   successors. We intend this dedication to be an overt act of
   relinquishment in perpetuity of all present and future rights to this
   software under copyright law.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.

   For more information, please refer to <http://unlicense.org/>
*/


#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <strings.h>
#include "options.h"
#include "versdts.h"


void display_help() {
    printf("Usage: bserver options" \
            "\n\t[-b --base]\t<BASE_FOLDER>" \
            "\n\t[-d --dest]\t<DESTINATION_FOLDER>" \
            "\n\t[-f --conf]\t<CONF_FILE>" \
            "\n\t[-h --help]\n" \
            "\n\t[-v --version]\n" \
            "\nDefault location:~/.config/cvrt/config.ini\n" \
            "\nSample config.ini" \
            "\n------------------" \
            "\n\t[filter]" \
            "\n\t; enable filtering by file type, if not enabled" \
            "\n\t; then all the files will be converted using the" \
            "\n\t; external command" \
            "\n\tenabled = true" \
            "\n\t; set the required file types, if filter is enabled" \
            "\n\t; then this field is mandatory" \
            "\n\tfile_types = .wav .flac" \
            "\n\t[copy]" \
            "\n\t; if filter is enabled, then other files types" \
            "\n\t; will be copied to the new location" \
            "\n\tenabled = true" \
            "\n\t; if copy is enabled, will copy the file types below" \
            "\n\t; * or blank for all files other than" \
            "\n\t; the above filtered ones" \
            "\n\tfile_types =" \
            "\n" \
            "\n\t[core]" \
            "\n\t; external command to execute, ? will be replaced by variables" \
            "\n\tbase_dir = Music" \
            "\n\tdest_dir = Music2" \
            "\n\tcvrt_type = opus" \
            "\n\t; External command to execute, expecting 2 '?'" \
            "\n\t; '?'s are place-holders for input and output files." \
            "\n\tcmd = ffmpeg -i ? -c:a libopus ?\n\n"); 
}


uint8_t get_options (int argc, char **argv, Options *options) {
    int choice;

    while (1) {
        static struct option long_options[] = {
            /* This options set the secure flag. */
            {"base",   required_argument, 0,'b'},
            {"dest",   required_argument, 0,'d'},
            {"conf",   required_argument, 0,'f'},
            {"help",   no_argument,       0, 'h'},
            {"version", no_argument,      0, 'v'},
            {0,        0,                 0, 0}
        };

        /* getopt_long stores the option index here. */
        int option_index = 0;

        choice = getopt_long (argc, argv, "vhb:d:f:", long_options, 
                &option_index);

        size_t optarg_len = 0;

        /* Detect the end of the options. */
        if (choice == -1)
            break;

        switch (choice) {
            case 0:
                if (strncasecmp(long_options[option_index].name, "help", 5) == 0) {
                    display_help();
                    return 1;
                }
                break;

            case 'b':
                optarg_len = strlen(optarg);
                if(optarg_len <= MAX_PATH_LEN - 1) {
                    strncpy(options->base_dir, optarg, optarg_len);
                    options->base_dir[optarg_len] = '\0';
                    if (options->base_dir[optarg_len -1] == '/')
                        options->base_dir[optarg_len-1] ='\0';
                } else {
                    fprintf(stderr, "Base directory path is too long");
                    return 1;
                }
                break;
            case 'd':
                optarg_len = strlen(optarg);
                if(optarg_len <= MAX_PATH_LEN - 1) {
                    strncpy(options->dest_dir, optarg, optarg_len);
                    options->dest_dir[optarg_len] = '\0';
                    if (options->dest_dir[optarg_len -1] == '/')
                        options->dest_dir[optarg_len-1] ='\0';
                } else {
                    fprintf(stderr, "Destination directory path is too long");
                    return 1;
                }
                break;
            case 'f':
                optarg_len = strlen(optarg);
                if(optarg_len <= MAX_PATH_LEN - 1) {
                    strncpy(options->conf_file, optarg, optarg_len);
                    options->conf_file[optarg_len] = '\0';
                    if (options->conf_file[optarg_len -1] == '/')
                        options->conf_file[optarg_len-1] ='\0';
                } else {
                    fprintf(stderr, "Conf file path is too long");
                    return 1;
                }
                break;
            case 'h':
                display_help();
                return 1;
            case 'v':
                display_system_details(); 
                display_current_version();
                return 1;
            case '?':
                /* getopt_long already printed an error message. */
                break;
            case ':':   /* missing option argument */
                fprintf(stderr, "%s: option `-%c' requires an argument\n", argv[0], optopt);
                display_help();
                break;
            default:
                return 1;
        }
    }
    /* Print any remaining command line arguments (not options). */
    if (optind < argc) {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
        return -1;
    }
    return 0;
}

