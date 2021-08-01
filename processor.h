#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "cvrt.h"

void initialise_options (Options *options);
uint8_t process(int argc, char **argv, Options *options);

#endif
