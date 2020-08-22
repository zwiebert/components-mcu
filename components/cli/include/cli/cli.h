/*
 * cli.h
 *
 * Created: 24.08.2017 21:46:06
 *  Author: bertw
 */


#pragma once

#include "userio/status_output.h"
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

bool cliBuf_enlarge(struct cli_buf *buf);

// user interface
typedef int (*getc_funT)(void);
enum cli_get_commline_retT { CMDL_DONE, CMDL_ERROR, CMDL_INCOMPLETE, CMDL_LINE_BUF_FULL };
enum cli_get_commline_retT cli_get_commandline(struct cli_buf *buf, getc_funT getc_fun);
typedef int (*process_parm_cb)(clpar parm[], int parm_len);
void cli_process_cmdline2(char *line, so_target_bits tgt, process_parm_cb proc_parm);
void cli_process_json2(char *json, so_target_bits tgt, process_parm_cb proc_parm);
void cli_process_cmdline(char *line, so_target_bits tgt);
void cli_process_json(char *json, so_target_bits tgt);
void cli_loop(void);
void cli_setup_task(bool enable);

extern bool (*cli_hook_process_json)(char *json);
extern bool (*cli_hook_checkPassword)(clpar p[], int len, so_target_bits tgt);

// XXX: functions working on a static buffer
char *get_commandline(void);


// implementation interface


extern const struct parm_handlers parm_handlers; // defined by app

extern uint16_t cli_msgid;

void print_enr(void);
void msg_print(const char *msg, const char *tag);
void cli_reply_print(const char *tag);
void reply_message(const char *tag, const char *msg);
void reply_id_message(uint16_t id, const char *tag, const char *msg);

void cli_out_set_x(const char *obj_tag);
void cli_out_close(void);
void cli_out_x_reply_entry(const char *key, const char *val, int len);


/* cli.c */
void cli_warning_optionUnknown(const char *key);
void cli_msg_ready(void);
char *get_commandline(void);
int asc2bool(const char *s);
int cli_parseCommandline(char *cl, struct cli_parm *clp);
void cli_replySuccess(void);
int cli_replyFailure(void);
bool cli_replyResult(bool success);
bool  asc2uint8_t(const char *s, uint8_t *n, uint8_t limit);
int cli_processParameters(clpar p[], int len);
void cli_print_json(const char *json); //FIXME



