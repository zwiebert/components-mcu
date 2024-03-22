/**
 * \file   uout/so_target_desc.hh
 * \brief  Provide an output class hierarchy which knows about the required format (text or JSON) and write functions provided by the target.
 */
#pragma once
#include "so_out.h"
#include "cli_out.h"
#include "so_target_bits.h"
#include "status_json.hh"
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <utils_misc/itoa.h>
#include <functional>
#include <utility>

/**
 * \brief   Write basic data (strings and numbers) without any formatting
 *          The derived class can implement the virtual member function priv_write to produce any real output
 */
class UoutRawWriter {

protected:
  virtual ~UoutRawWriter() {};
public:
  /**
   * \brief        write output
   * \param s      output string
   * \param len    length of output string, or -1 for a null terminated string
   * \param last   On subsequent writes, mark the last write as final (required for e.g. web-socket target)
   * \return       Number of bytes written.  On error returns -1.
   */
  int write(const char *s, ssize_t len = -1, bool last = false);


  /**
   * \brief        write output line
   * \param s      output string (a line-feed will be appended)
   * \param len    length of output string, or -1 for a null terminated string
   * \param last  On subsequent writes, mark the last write as final (required for e.g. web-socket target)
   * \return       Number of bytes written.  On error returns -1.
   */

  int writeln(const char *s, ssize_t len = -1, bool last = false);


  /**
   * \brief   write a single character (non final)
   */
  int write(const char c);

public:
  /// \brief  write to output (non-final)
  friend  UoutRawWriter& operator<<( UoutRawWriter& lhs, const char *s) {
    lhs.write(s);
    return lhs;
  }
  /// \brief  write to output (non-final)
  friend  UoutRawWriter& operator<<( UoutRawWriter& lhs, char c) {
    lhs.write(c);
    return lhs;
  }
  /// \brief  write to output (non-final)
  friend  UoutRawWriter& operator<<( UoutRawWriter& lhs, int n) {
    char buf[20];
    itoa(n, buf, 10);
    lhs.write(buf);
    return lhs;
  }
  /// \brief  modifier for << operator (final, line-feed)  XXX: currently unused/untested
  struct mod {
    bool fin:1 = false;
    bool lf:1 = false;
  };
  /// \brief  write to output with modifiers XXX: currently unused/untested
  friend UoutRawWriter& operator<<(UoutRawWriter &lhs, const std::pair<UoutRawWriter::mod, const char*> &mod_s) {
    if (mod_s.first.lf)
      lhs.writeln(mod_s.second, -1, mod_s.first.fin);
    else
      lhs.write(mod_s.second, -1, mod_s.first.fin);
    return lhs;
  }
private:
  /**
   * \brief        do-nothing-stub for a write function to be overridden
   * \param s,len  string and string length to write
   * \param last  On subsequent writes, mark the last write as final (required for e.g. web-socket target)
   * \return   -1
   */

  virtual int priv_write(const char *s, ssize_t len, bool last)  {
    // do nothing
    return -1;
  }
};

/**
 * \brief   Target descriptor base class.
 *
 */
class UoutWriter : public UoutRawWriter {
public:
  /**
   * \param tgt    Flags describing the requested format(s) and/or the target.
   */
  UoutWriter(so_target_bits tgt = SO_TGT_NONE) :
      myTgt(tgt) {
  }

  UoutWriter(const UoutWriter&) = delete;
  UoutWriter& operator=(const UoutWriter&) = delete;
  virtual ~UoutWriter() {
  }

public:

  /// \brief Get target bit flags
  so_target_bits tgt() const {
    return myTgt;
  }
  /// \brief Add a target bit flag
  void add_tgt_flag(so_target_bits flag) {
    myTgt = static_cast<so_target_bits>(myTgt | flag);
  }

  /// \brief  Get the related UoutBuilderJson object
  UoutBuilderJson& sj() const {
    return mySj;
  }
  /// \brief  Get the related UoutBuilderPlaintext object
  UoutBuilderPlaintext& st() const {
    return myStxt;
  }
  /// \brief  Get the related UoutPrinter object
  const class UoutPrinter& so() const {
    return mySo;
  }

protected:
  so_target_bits myTgt = SO_TGT_NONE;
  ///////////////////////////////
  mutable UoutBuilderJson mySj = {this};
  mutable UoutBuilderPlaintext myStxt = {*this};
  const UoutPrinter mySo = {*this};
};

/**
 * \brief Target descriptor for console (e.g. USART, TCP)
 */
class UoutWriterConsole final: public UoutWriter {
  typedef int (*writeReq_fnT)(void *req, const char *s, ssize_t len, bool last);
public:
  UoutWriterConsole(so_target_bits tgt = SO_TGT_NONE) :
    UoutWriter(tgt) {
  }
  UoutWriterConsole(int fd, so_target_bits tgt = SO_TGT_NONE) :
    UoutWriter(tgt), myFd(fd) {
  }
  UoutWriterConsole(const UoutWriterConsole&) = delete;
  virtual ~UoutWriterConsole() {

  }
public:


private:
  virtual int priv_write(const char *s, ssize_t len, bool last) override;

private:
  int myFd = STDOUT_FILENO;
};


/**
 * \brief  Target descriptor with callback function object (for http req, web-socket, ...)
 */
class UoutWriterFunction final: public UoutWriter {
public:
  using write_callback_type = std::function<int(const char *src, ssize_t src_len, int is_final)>;
public:
  UoutWriterFunction(write_callback_type write_callback, so_target_bits tgt) :
    UoutWriter(tgt), m_write_callback(write_callback) {
  }

  UoutWriterFunction(const UoutWriterFunction&) = delete;
  virtual ~UoutWriterFunction() {
    mySj.write_json(true); // write all remaining json
  }

private:
  virtual int priv_write(const char *src, ssize_t src_len, bool is_final) override {

    int chunk_status = is_final ? -1 : 0;
    if (is_final && nmb_chunks_written)
      chunk_status = nmb_chunks_written;

    int n = m_write_callback(src, src_len, chunk_status);

    if (n > 0)
      ++nmb_chunks_written;

    return n;
  }

private:
  write_callback_type m_write_callback;
  int nmb_chunks_written = 0;
};

/**
 * \brief Target descriptor for files
 */
class UoutWriterFile final: public UoutWriter {
  typedef int (*writeReq_fnT)(void *req, const char *s, ssize_t len, bool last);
public:
  UoutWriterFile(so_target_bits tgt = SO_TGT_NONE) :
    UoutWriter(tgt) {
  }
  UoutWriterFile(int fd, so_target_bits tgt = SO_TGT_NONE) :
    UoutWriter(tgt), myFd(fd) {
  }
  UoutWriterFile(const char *file_name, so_target_bits tgt = SO_TGT_NONE);
  UoutWriterFile(const UoutWriterFile&) = delete;
  virtual ~UoutWriterFile();
public:


private:
  virtual int priv_write(const char *s, ssize_t len, bool last) override;
private:
  int myFd = STDOUT_FILENO;
  int m_ofd = -1;
};
