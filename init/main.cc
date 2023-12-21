void print(const char *s, int n) {
  asm("syscall" ::"a"(1), "D"(1), "S"(s), "d"(n));
}

class C {
public:
  C(char c) {
    s[2] = c;
    print(&s[1], 3);
  }
  ~C() { print(s, 4); }

private:
  char s[5] = "~C@\n";
} c0('0'), c1('1');

int main() { print("main\n", 5); }
