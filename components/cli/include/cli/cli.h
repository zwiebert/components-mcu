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

// user interface
typedef int (*getc_funT)(void);
enum cli_get_commline_retT { CMDL_DONE, CMDL_ERROR, CMDL_INCOMPLETE, CMDL_LINE_BUF_FULL };
enum cli_get_commline_retT cli_get_commandline(char *cbuf, unsigned buf_size, int *cmd_buf_idx, int *quote_count, getc_funT getc_fun);
void cli_process_cmdline(char *line, so_target_bits tgt);
void cli_process_json(char *json, so_target_bits tgt);
void cli_loop(void);
void cli_setup_task(bool enable);

// XXX: functions working on a static buffer
char *get_commandline(void);


// implementation interface
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

extern const struct parm_handlers parm_handlers; // defined by app

extern uint16_t cli_msgid;
#define MAX_PAR 20
extern clpar cli_par[MAX_PAR];

#define CMD_BUF_SIZE 128
extern char cmd_buf[CMD_BUF_SIZE];

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
int cli_parseCommandline(char *cl);
void cli_replySuccess(void);
int cli_replyFailure(void);
bool cli_replyResult(bool success);
bool  asc2uint8_t(const char *s, uint8_t *n, uint8_t limit);
int cli_processParameters(clpar p[], int len);
void cli_loop(void);
void cli_print_json(const char *json); //FIXME



