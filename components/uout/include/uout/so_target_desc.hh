#pragma once
#include "so_out.h"
#include "cli_out.h"
#include "so_target_bits.h"
#include "status_json.hh"
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <misc/itoa.h>

struct TargetDesc {
public:
  TargetDesc(so_target_bits tgt = SO_TGT_NONE) :
      myTgt(tgt) {
  }

  TargetDesc(const TargetDesc&) = delete;
  TargetDesc& operator=(const TargetDesc&) = delete;
  virtual ~TargetDesc() {
  }
public:
  int write(const char *s, ssize_t len = -1, bool final = false) const {
    if (len < 0) {
      len = strlen(s);
    }
    return priv_write(s, len, final);
  }

  int writeln(const char *s, ssize_t len = -1, bool final = false) const {
    if (len < 0) {
      len = strlen(s);
    }

    if (ssize_t n = priv_write(s, len, final); n == len)
      if (write('\n') == 1)
        return n + 1;
    return -1;
  }

  int write(const char c) const {
    return write(&c, 1);
  }

  so_target_bits tgt() const {
    return myTgt;
  }
  void add_tgt_flag(so_target_bits flag) {
    myTgt = static_cast<so_target_bits>(myTgt | flag);
  }

  StatusJsonT& sj() const {
    return mySj;
  }
  StatusTxtT& st() const {
    return myStxt;
  }
  const class SoOut& so() const {
    return mySo;
  }

  friend const TargetDesc& operator<<(const TargetDesc& lhs, const char *s) {
    lhs.write(s);
    return lhs;
  }
  friend const TargetDesc& operator<<(const TargetDesc& lhs, char c) {
    lhs.write(c);
    return lhs;
  }
  friend const TargetDesc& operator<<(const TargetDesc& lhs, int n) {
    char buf[20];
    itoa(n, buf, 10);
    lhs.write(buf);
    return lhs;
  }

private:
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


struct TargetDescCon: public TargetDesc {
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
  int myFd = 1;
  void *myReq = nullptr;
  writeReq_fnT myWriteReqFn = nullptr;
};



struct TargetDescWs: public TargetDesc {
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
