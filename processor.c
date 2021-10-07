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


#include<string.h>
#include <stdio.h>
#include "processor.h"
#include "ini.h"
#include "options.h"

static void trim_l (char *str) {
    int index =0, i, j ;

    /*  the last index of leading space character */
    while (str[index] == ' ' || str[index] == '\t' || str[index] == '\n') {
        index++;
    }

    if (index > 0) {
        for (i = index, j = 0; str[i] != '\0'; i++, j++) {
            str[j] = str[i];
        }
        str[j] = '\0'; 
    }
}

void initialise_options (Options *options) {
    options->isfilter = false;
    memset (options->filter_types, '\0', FILTER_LEN);
    options->iscopy = false;
    memset (options->copy_types, '\0', FILTER_LEN);
    memset (options->base_dir, '\0', MAX_PATH_LEN);
    memset (options->dest_dir, '\0', MAX_PATH_LEN);
    memset (options->conf_file, '\0', MAX_PATH_LEN);
    memset (options->full_cmd, '\0', MAX_FULL_CMD_LEN);
    memset (options->cvrt_type, '\0', FILE_TYPE_LEN);
}

uint8_t process(int argc, char **argv, Options *options) {

    if (get_options(argc, argv, options))
        return 1;

    char *config_file = strlen(options->conf_file) == 0 ? DEFAULT_CONFIG : options->conf_file;

    /* config file handler */
    ini_t *config = NULL;

    char *file_types = NULL, *cmd = NULL, *base_dir = NULL, *dest_dir = NULL, *cvrt_type = NULL;

    if ((config = ini_load(config_file)) == NULL){
        /* create the folder first */
        fprintf(stderr, "Error: configuration file %s is missing\n", config_file);
        return 1;
    } 

    /* config file loaded */
    if (strcmp(ini_get(config, "filter", "enabled"), "true") == 0) {  
        options->isfilter = true;
        if ((file_types = (char *)ini_get(config, "filter", "file_types")) == NULL) {
            fprintf(stderr, "Error: 'file_types' configuration is missing\n");
            return 1;
        }
        strncpy(options->filter_types, file_types, FILTER_LEN - 1);
        options->filter_types[FILTER_LEN - 1] = '\0';

        /* copy other files settings */
        if (strcmp(ini_get(config, "copy", "enabled"), "true") == 0) {  
            options->iscopy = true;
            if ((file_types = (char *)ini_get(config, "copy", "file_types")) == NULL) 
                strcpy(options->copy_types, "*");
            else {
                strncpy(options->copy_types, file_types, FILTER_LEN - 1);
            }
        } else {
            options->iscopy = false;
        }
    }

    base_dir = (char *)ini_get(config, "core", "base_dir");
    dest_dir = (char *)ini_get(config, "core", "dest_dir");

    if (strlen(options->base_dir) == 0) {
        if (base_dir == NULL) {
            fprintf(stderr, "Error: 'base_dir' configuration is missing\n");
            return 1;
        } else {
            size_t baselen = strlen(base_dir);
            if (baselen < MAX_PATH_LEN) {
                if (base_dir[baselen - 1] == '/')
                    base_dir[baselen - 1] = '\0';
                strcpy(options->base_dir, base_dir);
            } else {
                fprintf(stderr, "Destination directory path is too long");
                return 1;
            }
        }
    }

    if (strlen(options->dest_dir) == 0) {
        if (dest_dir == NULL) {
            fprintf(stderr, "Error: 'dest_dir' configuration is missing\n");
            return 1;
        } else {
            size_t destlen = strlen(dest_dir);
            if (destlen < MAX_PATH_LEN) {
                if (dest_dir[destlen - 1] == '/')
                    dest_dir[destlen - 1] = '\0';
                strcpy(options->dest_dir, dest_dir);
            } else {
                fprintf(stderr, "Destination directory path is too long");
                return 1;
            }
        }
    }

    if ((cmd = (char *)ini_get(config, "core", "cmd")) == NULL) {
        fprintf(stderr, "Error: 'cmd' configuration is missing\n");
        return 1;
    } else {
        trim_l(cmd);
        strncpy(options->full_cmd, cmd, MAX_FULL_CMD_LEN -2);
        options->full_cmd[MAX_FULL_CMD_LEN - 1] = '\0';
    }

    if ((cvrt_type = (char *)ini_get(config, "core", "cvrt_type")) == NULL) {
        fprintf(stderr, "Error: 'cvrt_type' configuration is missing\n");
        return 1;
    } else {
        trim_l(cvrt_type);
        if(cvrt_type[0] != '.') 
            snprintf(options->cvrt_type, FILE_TYPE_LEN - 1, ".%s", cvrt_type);
        else
            strncpy(options->cvrt_type, cvrt_type, FILE_TYPE_LEN - 1);
        options->cvrt_type[FILE_TYPE_LEN - 1] = '\0';
    }
    return 0;
}
