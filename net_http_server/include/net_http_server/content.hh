/**
 * \file    webapp/content.h
 * \brief   Definitions to make embedded files accessible from C code
 * \note    Mandatory files should be compressed by gzip, because its widely supported.  Brotli may be used for source maps.
 */
#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


class ContentReader {
public:
  virtual ~ContentReader() = default;
  virtual int open(const char *name, const char *query) = 0;
  virtual int read(int fd, char *buf, unsigned buf_size) = 0;
  virtual int close(int fd) = 0;
};

class FileContentReader final: public ContentReader {
public:
  virtual int open(const char *name, const char *query = 0) {
    return ::open(name, O_RDONLY);
  }
  virtual int read(int fd, char *buf, unsigned buf_size) {
    return ::read(fd, buf, buf_size);
  }
  virtual int close(int fd) {
    return ::close(fd);
  }
};

struct web_content {
  const char *content, *content_encoding;
  unsigned content_length;
};

struct file_map {
  const char *uri, *type;
  struct web_content wc;
  ContentReader *content_reader;  ///< provide content
};

const struct file_map* wc_getContent(const char *uri);

extern const web_content wapp_html_gz_fm;
extern const web_content wapp_js_gz_fm;
extern const web_content wapp_js_map_gz_fm;
extern const web_content wapp_css_gz_fm;
extern const web_content wapp_css_map_gz_fm;

extern const web_content wapp_html_br_fm;
extern const web_content wapp_js_br_fm;
extern const web_content wapp_js_map_br_fm;
extern const web_content wapp_css_br_fm;
extern const web_content wapp_css_map_br_fm;

