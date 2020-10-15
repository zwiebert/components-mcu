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
 * \brief   Target descriptor base class.
 */
struct TargetDesc {
public:
  /**
   * \param tgt    Flags describing the requested format(s) and/or the target.
   */
  TargetDesc(so_target_bits tgt = SO_TGT_NONE) :
      myTgt(tgt) {
  }

  TargetDesc(const TargetDesc&) = delete;
  TargetDesc& operator=(const TargetDesc&) = delete;
  virtual ~TargetDesc() {
  }
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
      if (write("\n",1,final) == 1)
        return n + 1;
    return -1;
  }

  /**
   * \brief   write a single character (non final)
   */
  int write(const char c) const {
    return write(&c, 1);
  }

  /// \brief Get target bit flags
  so_target_bits tgt() const {
    return myTgt;
  }
  /// \brief Add a target bit flag
  void add_tgt_flag(so_target_bits flag) {
    myTgt = static_cast<so_target_bits>(myTgt | flag);
  }

  /// \brief  Get the related StatusJsonT object
  StatusJsonT& sj() const {
    return mySj;
  }
  /// \brief  Get the related StatusTxtT object
  StatusTxtT& st() const {
    return myStxt;
  }
  /// \brief  Get the related SoOut object
  const class SoOut& so() const {
    return mySo;
  }
  /// \brief  write to output (non-final)
  friend const TargetDesc& operator<<(const TargetDesc& lhs, const char *s) {
    lhs.write(s);
    return lhs;
  }
  /// \brief  write to output (non-final)
  friend const TargetDesc& operator<<(const TargetDesc& lhs, char c) {
    lhs.write(c);
    return lhs;
  }
  /// \brief  write to output (non-final)
  friend const TargetDesc& operator<<(const TargetDesc& lhs, int n) {
    char buf[20];
    itoa(n, buf, 10);
    lhs.write(buf);
    return lhs;
  }
  /// \brief  modifier for << opterator (final, line-feed)  XXX?
  struct mod {
    bool fin:1 = false;
    bool lf:1 = false;
  };
  /// \brief  write to output with modifiers XXX?
  friend const TargetDesc& operator<<(const TargetDesc &lhs, const std::pair<TargetDesc::mod, const char*> &mod_s) {
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
protected:
  so_target_bits myTgt = SO_TGT_NONE;
  ///////////////////////////////
  mutable StatusJsonT mySj = {this};
  mutable StatusTxtT myStxt = {*this};
  const SoOut mySo = {*this};
};

/**
 * \brief Target descriptor for console (e.g. USART, TCP)
 */
struct TargetDescCon final: public TargetDesc {
  typedef int (*writeReq_fnT)(void *req, const char *s, ssize_t len, bool final);
public:
  TargetDescCon(so_target_bits tgt = SO_TGT_NONE) :
    TargetDesc(tgt) {
  }
  TargetDescCon(int fd, so_target_bits tgt = SO_TGT_NONE) :
    TargetDesc(tgt), myFd(fd) {
  }
  TargetDescCon(const TargetDescCon&) = delete;
  virtual ~TargetDescCon() {

  }
public:


private:
  virtual int priv_write(const char *s, ssize_t len, bool final) const {
    const size_t size = len;
    assert(size < 2046);
    return ::write(myFd, s, size);
  }
private:
  int myFd = STDOUT_FILENO;
  void *myReq = nullptr;
  writeReq_fnT myWriteReqFn = nullptr;
};


/**
 * \brief  Target descriptor for web-socket
 */
struct TargetDescWs final: public TargetDesc {
  typedef int (*writeReq_fnT)(void *req, const char *s, ssize_t len, bool final);
public:
  TargetDescWs(void *req, so_target_bits tgt, writeReq_fnT writeReq_fn) :
    TargetDesc(tgt), myReq(req), myWriteReqFn(writeReq_fn) {
  }

  TargetDescWs(const TargetDescWs&) = delete;
  virtual ~TargetDescWs() {
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
