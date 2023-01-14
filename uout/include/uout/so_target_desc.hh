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
   * \param final  On subsequent writes, mark the last write as final (required for e.g. web-socket target)
   * \return       Number of bytes written.  On error returns -1.
   */
  int write(const char *s, ssize_t len = -1, bool final = false) const {
    if (len < 0) {
      len = strlen(s);
    }

    return priv_write(s, len, final);
  }

  /**
   * \brief        write output line
   * \param s      output string (a line-feed will be appended)
   * \param len    length of output string, or -1 for a null terminated string
   * \param final  On subsequent writes, mark the last write as final (required for e.g. web-socket target)
   * \return       Number of bytes written.  On error returns -1.
   */
  int writeln(const char *s, ssize_t len = -1, bool final = false) const {
    if (len < 0) {
      len = strlen(s);
    }

    if (ssize_t n = priv_write(s, len, false); n == len)
      if (write("\n", 1, final) == 1)
        return n + 1;
    return -1;
  }

  /**
   * \brief   write a single character (non final)
   */
  int write(const char c) const {
    return write(&c, 1);
  }

public:
  /// \brief  write to output (non-final)
  friend const UoutRawWriter& operator<<(const UoutRawWriter& lhs, const char *s) {
    lhs.write(s);
    return lhs;
  }
  /// \brief  write to output (non-final)
  friend const UoutRawWriter& operator<<(const UoutRawWriter& lhs, char c) {
    lhs.write(c);
    return lhs;
  }
  /// \brief  write to output (non-final)
  friend const UoutRawWriter& operator<<(const UoutRawWriter& lhs, int n) {
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
  friend const UoutRawWriter& operator<<(const UoutRawWriter &lhs, const std::pair<UoutRawWriter::mod, const char*> &mod_s) {
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
   * \param final  On subsequent writes, mark the last write as final (required for e.g. web-socket target)
   * \return   -1
   */
  virtual int priv_write(const char *s, ssize_t len, bool final) const {
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
  typedef int (*writeReq_fnT)(void *req, const char *s, ssize_t len, bool final);
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
  virtual int priv_write(const char *s, ssize_t len, bool final) const {
    const size_t size = len;
    assert(size < 2046);
#if 1
    const char crlf[] = "\r\n";
    const unsigned crlf_len = sizeof crlf - 1;
    char prev_c = 0;
    int last_chunk = 0;

    for (size_t i = 0; i < size; ++i) {
      const char c = s[i];
      if (c == '\r' || (c == '\n' && prev_c == '\r'))
        continue;

      if (c == '\n') {
        const int chunk_len = i - last_chunk;
        if (chunk_len > 0 && ::write(myFd, s + last_chunk, chunk_len) < 1)
          return -1;
        last_chunk = i + 1;
        if (::write(myFd, crlf, crlf_len) < 1)
          return -1;
      } else if (i + 1 == size) {
        const int chunk_len = i - last_chunk + 1;
        if (chunk_len > 0 && ::write(myFd, s + last_chunk, chunk_len) < 1)
          return -1;
        break;
      }
      prev_c = c;
    }

    return len;

#elif 1
    for (size_t i=0; i < size; ++i) {
      const char c = s[i];
      if (c == '\r')
        continue;
      if (c == '\n') {
        char r = '\r';
        if (::write(myFd, &r, 1) < 1)
          return -1;
      }
      if (::write(myFd, &c, 1) < 1)
        return -1;
    }
    return len;
#else
    return ::write(myFd, s, size);
#endif
  }
private:
  int myFd = STDOUT_FILENO;
};


/**
 * \brief  Target descriptor for web-socket
 */
class UoutWriterWebsocket final: public UoutWriter {
  using writeReq_fnT = int (*)(void *req, const char *s, ssize_t len, bool final);
public:
  UoutWriterWebsocket(void *req, so_target_bits tgt, writeReq_fnT writeReq_fn) :
    UoutWriter(tgt), myReq(req), myWriteReqFn(writeReq_fn) {
  }

  UoutWriterWebsocket(const UoutWriterWebsocket&) = delete;
  virtual ~UoutWriterWebsocket() {
    mySj.write_json(true); // write all json synchronously and close websocket request
  }
public:


private:
  virtual int priv_write(const char *s, ssize_t len, bool final) const {
    if (myReq && myWriteReqFn) {
      return myWriteReqFn(myReq, s, len, final);
    }
    return -1;
  }
private:
  void *myReq = nullptr;
  writeReq_fnT myWriteReqFn = nullptr;
};

//#include "so_target_desc_inline.hh"
