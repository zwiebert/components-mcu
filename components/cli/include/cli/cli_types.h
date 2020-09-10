#pragma once

#include "stdint.h"
#include "stdbool.h"

typedef struct {
  char *key;
  char *val;
} clpar;

struct parm_handler {
  const char *parm;
  int (*process_parmX)(clpar p[], int len);
  const char *help;
};

struct parm_handlers {
  struct parm_handler *handlers;
  int count;
};

struct cli_buf {
  char *cli_buf;
  unsigned size;
  int cli_buf_idx, quote_count;
};

struct cli_parm {
  clpar *par;
  unsigned size;
};

typedef int (*getc_funT)(void);
enum cli_get_commline_retT {
  CMDL_DONE, CMDL_ERROR, CMDL_INCOMPLETE, CMDL_LINE_BUF_FULL
};
typedef int (*process_parm_cb)(clpar parm[], int parm_len);


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

