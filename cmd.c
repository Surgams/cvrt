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

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "cmd.h"

#define DELIMETER " "
#define CMD_ARG 200
#define FTYPES_SIZE 31
char **ftypes;


static inline int cmd_arg_split (char *cmd, char **cmd_array, const char *delimiter) {
    char *token;
    int counter = 0;

    printf("--cmd: %s\n", cmd);
    while ((token = strsep(&cmd, delimiter)) != NULL) {
        bool isempty = true;
        while (*token != '\0') {
            if (!isspace(*token)) {
                isempty = false;
                break;
            }
        }
        if (isempty)
            continue;

        cmd_array[counter] = (char *) malloc((strlen(token) + 1) * sizeof(char));
        sprintf(cmd_array[counter], "%s", token);

        printf("--token: [%s]\tcmdarray: [%s]\tcounter [%d]\n", token, cmd_array[counter], counter);

        if (counter++ >= CMD_ARG - 1)
            return 1;
    }
    cmd_array[counter] = NULL;
    return 0;
}

static bool check_filter (char *filetypes, const char *delimiter, char *extension) {
    static bool isprocessed = false;

    if(!isprocessed) {
        ftypes = malloc(FTYPES_SIZE * sizeof(char));
        if (cmd_arg_split(filetypes, ftypes, delimiter)) 
            return false;
        isprocessed = true;
    }
    for (int i = 0; ftypes[i] != NULL && i < FTYPES_SIZE; i++) {
        if (strcasecmp(extension, ftypes[i]) == 0)
            return true;
    }
    return false;
}

int create_dir (char *path) {
    struct stat st = {0};

    if (stat(path, &st) == - 1) {
        return mkdir(path, 0700);
    }
    return 0;
}

void convert_files_recursively (Options options) {
    char *base_path = options.base_dir;
    char *dest_path = options.dest_dir;

    int output, pid;
    char path[MAX_PATH_LEN], new_path[MAX_PATH_LEN];
    struct dirent *dp;
    char *point;
    
    DIR *dir = opendir(base_path);
    if (!dir) {
        return;
    }

    if ((output = create_dir(dest_path)) != 0) {
        fprintf(stderr, "Error number %d: %s\n", errno, strerror(errno));
        return;
    }
    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            if ((point = strrchr(dp->d_name,'.')) != NULL) {
                if (check_filter(options.filter_types, DELIMETER, point)) {

                    /* Preparing input and output file */
                    char  i_tmp[MAX_PATH_LEN] = {}, o_tmp[MAX_PATH_LEN] = {};
                    snprintf(o_tmp, sizeof(o_tmp) - 1, "%s/%.*s.opus", dest_path, (int)(strlen(dp->d_name) - strlen(point)), dp->d_name);
                    snprintf(i_tmp, sizeof(i_tmp) - 1, "%s/%s", base_path, dp->d_name);

                    char *fullcmd = strdup(options.full_cmd);

                    /* Preparing the arguments array */
                    char *args[CMD_ARG] = {0};
                    cmd_arg_split(fullcmd, args, DELIMETER); 

                    /* Replacing the placeholders '?' with input and output filenames */
                    bool is_inputfile = true;
                    for (int i = 0; args[i] != NULL && i < CMD_ARG; i++) {
                        if (strcmp(args[i], "?") == 0) {
                            if (is_inputfile) {
                                args[i] = (char *)realloc (args[i], strlen(i_tmp) + 1);
                                strcpy(args[i], i_tmp);
                                args[i][strlen(i_tmp)] = '\0';
                                is_inputfile = false;
                            } else {
                                args[i] = (char *)realloc (args[i], strlen(o_tmp) + 1);
                                strcpy(args[i], o_tmp);
                                args[i][strlen(o_tmp)] = '\0';
                            }
                        }
                    }
                    if((pid = fork()) == 0) {
                        execvp(args[0], (char *const *)args);
                        _exit(0);
                    } else if (pid > 0) {
                        wait(NULL);
                        /* Freeing the args array */
                        for(int i = 0; args[i] != NULL && i < CMD_ARG; i++) {
                            free(args[i]);
                            args[i] = NULL;
                        }
                        if (fullcmd != NULL) {
                            free(fullcmd);
                            fullcmd = NULL;
                        }
                    }
                }
            }

            /* Construct new path from our base path */
            sprintf(path, "%s/%s", base_path, dp->d_name);
            sprintf(new_path, "%s/%s", dest_path, dp->d_name);

            if ((output = create_dir(dest_path)) != 0) {
                fprintf(stderr, "Error number %d: %s\n", errno, strerror(errno));
                return;
            }
            Options tmp_options;
            strcpy(tmp_options.base_dir, path);
            strcpy(tmp_options.dest_dir, new_path);
            strcpy(tmp_options.full_cmd, options.full_cmd);
            strcpy(tmp_options.cvrt_type, options.cvrt_type);
            strcpy(tmp_options.filter_types, options.filter_types);
            convert_files_recursively(tmp_options);
        }
    }
    if (dp == NULL)
    closedir(dir);
}

void free_cmd () {
    for(int i = 0; ftypes != NULL && ftypes[i] != NULL && i < FTYPES_SIZE; i++) {
        free(ftypes[i]);
        ftypes[i] = NULL;
    }
    if(ftypes != NULL) {
        free(ftypes);
        ftypes = NULL;
    }
}

