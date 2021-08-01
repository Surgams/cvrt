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

/*
   ==========================================================================
    A small tool to convert given directory contents using an external tool,
    and then save the converted files to a new given directory tree respecting the same hierarchy of the original structure. 
    The code will call an external command to conduct for the conversion. 
    The code is written in ANSI C11

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "cvrt.h"
#include "processor.h"
#include "cmd.h"

int main(int argc, char * const argv[]) {
 
    /* Create configuration folder */
    char config_path[FILTER_LEN] = {'\0'};
    snprintf(config_path, FILTER_LEN -1, "%s/.config/cvrt", getenv("HOME"));
    if (create_dir (config_path) == -1) {
        fprintf(stderr, "[fun: %s - %s:%d] Error: unable to create config folder: (%s)\n", __func__, __FILE__, __LINE__, strerror(errno));
        return 1;
    }

    Options *options = (Options *) malloc(sizeof(Options));

    initialise_options(options);
    if (process(argc, (char **)argv, options))
        return 0;

    convert_files_recursively(*options);
    free_cmd();
    free(options);
    return 0;
}


