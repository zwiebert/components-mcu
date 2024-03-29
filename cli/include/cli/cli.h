#pragma once

/**
 * \file cli/cli.h
 * \brief cli framework
 * \author bertw
 */

#include "uout/uo_types.h"
#include "cli_types.h"
#include <jsmn/jsmn_iterate.hh>


/**
 * \brief           Set this to point to your parameter handlers. Needed at least by "help all;"
 */
extern const struct parm_handlers *cli_parm_handlers;

/**
 * \brief            Provide this callback to map a parameter name to a parameter handler
 * \param    key     the parameter name, which is the first word of a command line.
 * \return           a parameter handler struct
 */
extern const parm_handler* (*cli_parmHandler_find_cb)(const char *key);

/**
 * \brief            Set this optional callback to verify the cli password. Its called after parsing is done.
 * \param p,len      parameter list
 * \return           if it returns true, only then the parsed parameters will be processed.
 */
extern bool (*cli_hook_checkPassword)(clpar p[], int len, class UoutWriter &td);

/**
 * \brief dispatch the parsed parameter list to its respective handler
 * \param p          parameter list
 * \param len        length of parameter list
 * \param td         target descriptor to print any output to
 * \returns          If an error occurs, the result will be -1.
 */
int cli_processParameters(clpar p[], int len, class UoutWriter &td);

/**
 * \brief Set this to hook into \link cli_process_json \endlink
 * \param json      json command string
 * \return          if true is returned, parsing and processing JSON in \link cli_process_json \endlink will be skipped
 *
 */
extern bool (*cli_hook_process_json)(char *json);


/**
 * \brief           Set this to get an iterator to the "json" command object
 *
 *                  This is for small objects only. The jsmn parser holds only a small amount of tokens.
 *                  If you need bigger objects, use \link cli_hook_process_json \endlink
 * \param it        reference to iterator copy. the state/position at return does not matter
 * \return          if false is returned the object may be processed differently.
 */
extern bool (*cli_hook_process_json_obj)(class UoutWriter &td, Jsmn_String::Iterator &it);

/**
 * \brief the parser stores the mid option in this global variable (FIXME)
 */
extern uint16_t cli_msgid;


/**
 * \brief            Gets a command line from a console. Allows quoting strings and backspace to edit typos.
 * \param buf        provides the destination buffer
 * \param getc_fun   provides a pointer to a function to read the characters from
 * \return           a state, to check if the commandline is complete or otherwise. If CMDL_BUF_FULL is returned, you should
 *                   enlarge the buffer with \link cliBuf_enlarge \endlink .
 *                   If CMDL_DONE is returned, the buffer can be passed to \link cli_process_cmdline \endlink or \link cli_process_json \endlink
 */
enum cli_get_commline_retT cli_get_commandline(struct cli_buf *buf, getc_funT getc_fun);


/**
 * \brief            Parse a command line and process it
 *
 * \param line       command line without any terminating character like ';' or '\n'
 * \param proc_parm  pointer to a function to process the parsed line. default is \link cli_processParameters \endlink
 * \param td         target descriptor to print any output to
 *
 * \return
 *
 */
void cli_process_cmdline(char *line, class UoutWriter &td, process_parm_cb proc_parm = cli_processParameters);

/**
 * \brief            Parse a JSON command "line" and process it
 *
 * \param json       command and options in JSON format  without amy terminating character like ';' or '\n'
 * \param proc_parm  pointer to a function to process the parsed line. default is \link cli_processParameters \endlink
 * \param td         target descriptor to print any output to
 *
 * \return
 *
 */
void cli_process_json(char *json, class UoutWriter &td, process_parm_cb proc_parm = cli_processParameters);


/**
 * \brief           Enlarges a buffer using realloc(3)
 * \param  buf      The buffer to enlarge
 * \return          true for success
 */
bool cliBuf_enlarge(struct cli_buf *buf);

/**
 * \brief          converts a 0/1 value string from parameter to an integer.
 * \param  val     pointer to value, or NULL
 * \return         0 if val was '0'.  1 if val was '1' or NULL (if value was omitted).  -1 otherwise.
 */
int asc2bool(const char *val);

/**
 * \brief worker function of cli framework. Call it if input is available.
 *  If the provided get-character for \link cli_get_commandline \endlink function does block, then it should be called from a loop inside a dedicated task
 */
void cli_loop(void);

/**
 * \brief creates or deletes a cli task
 * \param enable   If true a task is (re)started.  If false, delete the previously created task.
 */
void cli_setup_task(bool enable);


#ifndef __cplusplus
#error
#endif
