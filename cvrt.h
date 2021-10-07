#ifndef CVRT_H
#define CVRT_H

#include <stdbool.h>

#define MAX_PATH_LEN 1500
#define MAX_FULL_CMD_LEN 1500
#define DEFAULT_CONFIG "~/.config/cvrt/config.ini"
#define FILE_TYPE_LEN 10
#define FILTER_LEN 100

typedef struct Options_ {
    bool isfilter;
    char filter_types[FILTER_LEN];
    bool iscopy;
    char copy_types[FILTER_LEN];
    char base_dir[MAX_PATH_LEN];
    char dest_dir[MAX_PATH_LEN];
    char conf_file[MAX_PATH_LEN];
    char full_cmd[MAX_FULL_CMD_LEN];
    char cvrt_type[FILE_TYPE_LEN];
} Options;


#endif
