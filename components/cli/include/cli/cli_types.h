#pragma once

/**
 * \file cli/cli_types.h
 * \brief cli framework:  vocabulary types
 */

#include "stdint.h"
#include "stdbool.h"

struct TargetDesc;

typedef struct {
  char *key;
  const char *val;
} clpar;

struct parm_handler {
  const char *parm;
  int (*process_parmX)(clpar p[], int len, const struct TargetDesc &td);
  const char *help;
};

struct parm_handlers {
  const struct parm_handler *handlers;
  int count;
};

struct cli_buf {
  char *buf;
  unsigned buf_size;
  int buf_idx;
  int quote_count;
};

struct cli_parm {
  clpar *par;
  unsigned size;
};

typedef int (*getc_funT)(void);

/// \brief status of get command line
enum cli_get_commline_retT {
  CMDL_DONE, ///< line complete
  CMDL_ERROR, ///< error occurec
  CMDL_INCOMPLETE, ///< line is still incomplete
  CMDL_LINE_BUF_FULL ///< buffer full, call /link cliBuf_enlarge /endlink
};

typedef int (*process_parm_cb)(clpar parm[], int parm_len, const struct TargetDesc &td);


