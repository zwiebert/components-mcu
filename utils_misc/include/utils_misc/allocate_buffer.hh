#include <memory>

template<typename buf_type = char, typename Alloc = std::allocator<buf_type> >
class AllocateBuffer {

public:

  using pointer = buf_type*;
  using const_pointer = const buf_type*;

public:

  constexpr AllocateBuffer(size_t buf_size = 0) {
    if (buf_size) {
      m_buf = m_alloc.allocate(buf_size);
      m_buf_size = m_buf ? buf_size : 0;
    } else {
      m_buf_size = 0;
      m_buf = nullptr;
    }
  }

  ~AllocateBuffer() {
    m_alloc.deallocate(m_buf, m_buf_size);
  }

public:

  bool resize(size_t new_size) {
    m_buf = m_alloc.reallocate(m_buf, new_size);
    m_buf_size = m_buf ? new_size : 0;
    return m_buf != nullptr;
  }

public:

  size_t size() const {
    return m_buf_size;
  }

  operator pointer() {
    return m_buf;
  }

  operator const_pointer() const {
    return m_buf;
  }

  operator bool() const {
    return m_buf != nullptr;
  }

private:

  [[no_unique_address]] Alloc m_alloc;
  size_t m_buf_size;
  pointer m_buf;
};
