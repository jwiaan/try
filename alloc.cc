#include <cassert>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>

namespace {
inline void print(const char *s) { write(STDOUT_FILENO, s, strlen(s)); }
inline void *header(void *p) { return static_cast<size_t *>(p) - 1; }
inline size_t size(void *p) { return *static_cast<size_t *>(header(p)); }
inline size_t length(size_t s) { return sizeof(s) + s + sysconf(_SC_PAGESIZE); }
inline size_t align(size_t s, int d) {
  auto m = sysconf(_SC_PAGESIZE) - 1;
  s += d ? m : 0;
  return s & ~m;
}
} // namespace

extern "C" void *malloc(size_t s) {
  print("malloc\n");
  auto l = length(s);
  auto a = align(l, 1);
  auto p = static_cast<char *>(mmap(nullptr, a, PROT_READ | PROT_WRITE,
                                    MAP_PRIVATE | MAP_ANONYMOUS, 0, 0));
  assert(p != MAP_FAILED);

  auto ps = sysconf(_SC_PAGESIZE);
  if (mprotect(p + a - ps, ps, PROT_READ) < 0)
    assert(!"mprotect");

  p += a - l;
  *reinterpret_cast<size_t *>(p) = s;
  return p + sizeof(s);
}

extern "C" void free(void *p) {
  print("free\n");
  if (p) {
    auto a = align(length(size(p)), 1);
    p = reinterpret_cast<void *>(align(reinterpret_cast<size_t>(header(p)), 0));
    if (munmap(p, a) < 0)
      assert(!"munmap");
  }
}

extern "C" void *calloc(size_t n, size_t s) {
  print("calloc\n");
  return malloc(n * s);
}

extern "C" void *realloc(void *p, size_t s) {
  print("realloc\n");
  if (p) {
    auto n = size(p);
    auto q = malloc(s);
    memcpy(q, p, s < n ? s : n);
    free(p);
    return q;
  }

  return malloc(s);
}

int main() {
  size_t ps = sysconf(_SC_PAGESIZE);
  for (size_t i = 0; i < ps; ++i)
    assert(align(i, 0) == 0);

  assert(align(ps, 0) == ps);
  assert(align(0, 1) == 0);
  for (size_t i = 1; i <= ps; ++i)
    assert(align(i, 1) == ps);
}
