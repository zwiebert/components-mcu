#pragma once
/**
 * \file uout/cli_out.h
 * \brief Textual output for console CLI
 */

#include <stdint.h>


void cli_warning_optionUnknown(class UoutWriter &td, const char *key);
void reply_id_message(class UoutWriter &td, uint16_t id, const char *tag, const char *msg);
void reply_message(class UoutWriter &td, const char *tag, const char *msg);

bool cli_replyResult(class UoutWriter &td, bool success);
void cli_replySuccess(class UoutWriter &td);
int cli_replyFailure(class UoutWriter &td);
void print_enr(class UoutWriter &td);

/// print command prompt
void cli_msg_ready(class UoutWriter &td);

/**
 * \brief  Print key value pairs as text lines with limited length
 */
class UoutBuilderPlaintext {
  void  cli_out_entry(const char *key, const char *val, int len);
  void cli_out_start_reply();
public:
  UoutBuilderPlaintext(class UoutWriter &td): myTd(td) {}
public:

  /**
   * \brief set the tag, which is later printed at start of each output line
   */
  void cli_out_set_x(const char *obj_tag);

  /**
   * \brief                prints out a key value pair. If line has become to long start a new one.
   * \param key,val        key value pair
   * \param len            (obsolete) used to provide the length of characters written outside of this function
   *                       so it can be added to internally stored line length (used to calculate when to start a new line)
   */
  void cli_out_x_reply_entry(const char *key, const char *val, int len = 0);
  /**
   * \brief         end the current printing. Writes line terminator and line feed.
   */
  void cli_out_close();

private:
  int myLength = 0;
  class UoutWriter &myTd;
  char Obj_tag[16] = "";
};

