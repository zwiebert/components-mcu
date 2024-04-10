#include <unity.h>
#ifdef TEST_HOST
#include <test_runner.h>
#endif

//#define final
#include <uout/uout_writer.hh>
#include <array>

class UoutWriterTest final : public UoutWriter {
  typedef int (*writeReq_fnT)(void *req, const char *s, ssize_t len, bool final);

public:
  UoutWriterTest(so_target_bits tgt) :
      UoutWriter(tgt) {
  }

  UoutWriterTest(const UoutWriterConsole&) = delete;
  virtual ~UoutWriterTest() {
  }

public:
private:
  virtual int priv_write(const char *s, ssize_t len, bool final) override {
    for (ssize_t i = 0; i < len; ++i,++wbuf_idx_) {
      wbuf_[wbuf_idx_] = s[i];
    }
     wbuf_[wbuf_idx_] = '\0';
    return len;
  }

public:
  mutable std::array<char, 200> wbuf_ = { };
  mutable unsigned wbuf_idx_ = 0;
  void clear_buf() const {
    wbuf_idx_ = 0;
    wbuf_[0] = '\0';
  }
  void clear() {
    clear_buf();
    sj().free_buffer();
  }
};

void test_td() {
  UoutWriterTest td(SO_TGT_ANY);
  const char *str = &td.wbuf_[0];

  td.write('a');
  TEST_ASSERT_EQUAL(1, td.wbuf_idx_);
  TEST_ASSERT_EQUAL_STRING("a", str);
  td.write("bcdef");
  TEST_ASSERT_EQUAL_STRING("abcdef", str);
  td.clear_buf();
  TEST_ASSERT_EQUAL_STRING("", str);
}




TEST_CASE("td", "[uout]")
{
  test_td();
}
