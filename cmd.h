#ifndef CMD_H
#define CMD_H

#include "cvrt.h"


int create_dir (char *path);
void convert_files_recursively (Options options);
void free_cmd ();

#endif
