/**
 * \file   uout/uout_writer.hh
 * \brief  Provide an output class hierarchy which knows about the required format (text or JSON) and write functions provided by the target.
 */
#pragma once
#include "so_out.h"
#include "cli_out.h"
#include "so_target_bits.h"
#include "uout_builder_json.hh"
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <utils_misc/itoa.h>
#include <functional>
#include <utility>

/**
 * \brief   Abstract class which implements public write methods and operators which then call an abstract private write method.
 *
 */
class UoutRawWriter {

protected:
  virtual ~UoutRawWriter() = default;
  ;
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
  friend UoutRawWriter& operator<<(UoutRawWriter &lhs, const char *s) {
    lhs.write(s);
    return lhs;
  }
  /// \brief  write to output (non-final)
  friend UoutRawWriter& operator<<(UoutRawWriter &lhs, char c) {
    lhs.write(c);
    return lhs;
  }
  /// \brief  write to output (non-final)
  friend UoutRawWriter& operator<<(UoutRawWriter &lhs, int n) {
    char buf[20];
    itoa(n, buf, 10);
    lhs.write(buf);
    return lhs;
  }
  /// \brief  modifier for << operator (final, line-feed)  XXX: currently unused/untested
  struct mod {
    bool fin :1 = false;
    bool lf :1 = false;
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

  virtual int priv_write(const char *s, ssize_t len, bool last) = 0;
};

/**
 * \brief  Abstract class which contains various data builders
 *
 *        The builders are passed a *this reference, so they can call our write methods to output the build data.
 *        Subclasses will implement the abstract priv_write() method, so the data has a sink to go.
 *
 */
class UoutWriter: public UoutRawWriter {
public:
  /**
   * \param tgt    Flags describing the requested format(s) and/or the target.
   */
  UoutWriter(so_target_bits tgt = SO_TGT_NONE) :
      myTgt(tgt) {
  }

  UoutWriter(const UoutWriter&) = delete;
  UoutWriter& operator=(const UoutWriter&) = delete;

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
  so_target_bits myTgt = SO_TGT_NONE; ///< target to write to and format to output
  ///////////////////////////////
  mutable UoutBuilderJson mySj = { this };  ///< builder for JSON format
  mutable UoutBuilderPlaintext myStxt = { *this }; ///< builder for plain text format
  const UoutPrinter mySo = { *this }; ///< builder which selects format by content of \ref myTgt
};

/**
 * \brief build data and write to local or remote console (file or socket descriptor)
 */
class UoutWriterConsole final: public UoutWriter {
  typedef int (*writeReq_fnT)(void *req, const char *s, ssize_t len, bool last);
public:
  /**
   * \brief      construct for local console (fd = STDOUT_FILENO)
   * \param tgt   target to write to (XXX:?) and data format
   */
  UoutWriterConsole(so_target_bits tgt = SO_TGT_NONE) :
      UoutWriter(tgt) {
  }
  /**
   * \brief construct for remote console
   * \param fd     file/socket descriptor
   * \param tgt   target to write to (XXX:?) and data format
   */
  UoutWriterConsole(int fd, so_target_bits tgt = SO_TGT_NONE) :
      UoutWriter(tgt), myFd(fd) {
  }
  UoutWriterConsole(const UoutWriterConsole&) = delete;

public:

private:
  virtual int priv_write(const char *s, ssize_t len, bool last) override;

private:
  int myFd = STDOUT_FILENO;
};

/**
 * \brief  Build data without an actual data sink for passing to external builder functions expecting a UoutWriter reference.
 *
 *         We implement the priv_write() method with a dummy. This should cause the internal builders to use (growing) internal buffers for building.
 *
 *
 */
class UoutWriterBuilder final: public UoutWriter {
public:
  UoutWriterBuilder(so_target_bits tgt = SO_TGT_NONE) :
      UoutWriter(tgt) {
  }
private:
  /*
   * \brief does nothing. This should cause the builder to grow its buffer
   * \return  returns bytes written, which is always zero.
   */
  virtual int priv_write(const char *s, ssize_t len, bool last) override {
    return 0;
  }
};

/**
 * \brief  Build data and, while building, write them in chunks using a user provided callback function object (for http req, web-socket, ...)
 *
 *        The function object may be created using std::bind to hold any data needed (e.g. file handles/descriptors, http requests).
 */
class UoutWriterFunction final: public UoutWriter {
public:
  /**
   * \brief  callback function object
   *  \param      src             Data to be written
   *  \param      src_len         Data length or -1 for zero terminated strings
   *  \param      chunk_status    Chunk number of data, to control output (e.g. 4 chunk sequence: -1,-2,-3,+4)
   *                                 - 0 for a single (first and final) "chunk"
   *                                 - -1 is the first chunk
   *                                 - -2 ... -n   are in-between chunks
   *                                 - +n          is the final chunk
   *  \return    number of bytes written.  If zero, the builder will just grow its buffer and may call again later.
   */
  using write_callback_type = std::function<int(const char *src,
      ssize_t src_len, int chunk_status)>;
public:

  /**
   * \param write_callback  Callback function object for writing out our data
   * \param fmt             specify which data formats should be build by mySo()
   */
  UoutWriterFunction(write_callback_type write_callback, so_target_bits fmt = SO_TGT_FLAG_JSON) :
      UoutWriter(fmt), m_write_callback(write_callback) {
  }

  UoutWriterFunction(const UoutWriterFunction&) = delete;
  virtual ~UoutWriterFunction() {
    mySj.write_json(true); // write all remaining json
  }

private:
  virtual int priv_write(const char *src, ssize_t src_len, bool is_final) override {
    bool no_fragments = is_final && !nmb_chunks_written;

    int chunk_status = no_fragments ? 0 : (is_final ? (nmb_chunks_written + 1) : (nmb_chunks_written + 1) * -1);

    int n = m_write_callback(src, src_len, chunk_status);

    if (n > 0)
      ++nmb_chunks_written;

    if (is_final)
      nmb_chunks_written = 0;

    return n;
  }

private:
  write_callback_type m_write_callback;
  int nmb_chunks_written = 0;
};

/**
 * \brief Build data and, while building,  write them to a file while building
 */
class UoutWriterFile final: public UoutWriter {
  typedef int (*writeReq_fnT)(void *req, const char *s, ssize_t len, bool last);
public:
  /**
   * \param file_name       Named file to write to
   * \param fmt             Specify the data format (e.g. JSON)
   */
  UoutWriterFile(const char *file_name, so_target_bits fmt = SO_TGT_FLAG_JSON);
  UoutWriterFile(const UoutWriterFile&) = delete;
  virtual ~UoutWriterFile();
public:

private:
  virtual int priv_write(const char *s, ssize_t len, bool last) override;
private:
  int myFd = STDOUT_FILENO;
  int m_ofd = -1;
};
