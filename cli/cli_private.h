#pragma once


#include "cli/cli.h"


/**
 * \brief   Allocate buffer and read command line into it. A command line is terminated by semicolon.
 * \note    This function is not reentrant
 * \return  pointer to null terminated command line without the semicolon
 */
char* get_commandline(void);

int cli_processParameters(clpar p[], int len, const class UoutWriter &td);
int cli_parseCommandline(char *cl, struct cli_parm *clp);

