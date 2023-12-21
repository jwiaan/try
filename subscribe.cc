#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <memory>

using namespace std;

struct Message {
  size_t seq, sig;
};

struct Handler {
  virtual ~Handler() = default;
  virtual void handle(const Message &) = 0;
};

struct Eraser {
  virtual ~Eraser() = default;
  virtual void erase() = 0;
};

template <typename Req> struct Sender {
  virtual ~Sender() = default;
  virtual void send(const Req &) = 0;
};

class Manager {
public:
  ~Manager() { assert(handlers.empty()); }
  void erase(size_t k) { handlers.erase(k); }
  void insert(size_t k, shared_ptr<Handler> h) { handlers[k] = h; }
  void notify(const Message &m) { handlers.at(m.seq)->handle(m); }

private:
  map<size_t, shared_ptr<Handler>> handlers;
};

template <typename Ctx, typename Msg>
using Pointer = void (*)(shared_ptr<Eraser>, Ctx &, const Msg &);

template <typename Ctx>
using Function = function<void(shared_ptr<Eraser>, Ctx &, const Message &)>;

template <typename Ctx> using Functions = map<size_t, Function<Ctx>>;

template <typename Req, typename Ctx>
class Client : public Sender<Req>,
               public Handler,
               public Eraser,
               public enable_shared_from_this<Client<Req, Ctx>> {
public:
  Client(size_t s, const Ctx &c, weak_ptr<Manager> m, const Functions<Ctx> &f)
      : seq(s), ctx(c), manager(m), functions(f) {}
  void send(const Req &) override{};
  void erase() override { manager.lock()->erase(seq); }
  void handle(const Message &m) override {
    functions.at(m.sig)(this->shared_from_this(), ctx, m);
  }

private:
  size_t seq;
  Ctx ctx;
  weak_ptr<Manager> manager;
  Functions<Ctx> functions;
};

template <typename Ctx> Functions<Ctx> subscribe() { return {}; }

template <typename Ctx, typename Msg, typename... Msgs>
Functions<Ctx> subscribe(Pointer<Ctx, Msg> p, Pointer<Ctx, Msgs>... ppp) {
  auto f = subscribe<Ctx>(ppp...);
  f[Msg::ID] = [p](shared_ptr<Eraser> e, Ctx &c, const Message &m) {
    p(e, c, Msg(m));
  };
  return f;
}

template <typename Req, typename Ctx, typename... Msgs>
shared_ptr<Sender<Req>> create(shared_ptr<Manager> m, const Ctx &c,
                               Pointer<Ctx, Msgs>... ppp) {
  auto f = subscribe(ppp...);
  auto s = make_shared<Client<Req, Ctx>>(0, c, m, f);
  m->insert(0, s);
  return s;
}

struct Req {};

struct Cfm {
  enum { ID = 1 };
  Cfm(const Message &) {}
};

struct Rej {
  enum { ID = 2 };
  Rej(const Message &) {}
};

void cfm(shared_ptr<Eraser> e, int &i, const Cfm &) {
  cout << __PRETTY_FUNCTION__ << ++i << endl;
}

void rej(shared_ptr<Eraser> e, int &i, const Rej &) {
  cout << __PRETTY_FUNCTION__ << ++i << endl;
  e->erase();
}

int main() {
  auto mgr = make_shared<Manager>();
  auto s = create<Req>(mgr, 0, cfm, rej);
  s->send(Req());
  Message msg = {};
  msg.sig = 1;
  mgr->notify(msg);
  msg.sig = 2;
  mgr->notify(msg);
}