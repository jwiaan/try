#include "print.h"
#include <memory>

using namespace std;

struct ProductA {
  virtual ~ProductA() = default;
  virtual void print() = 0;
};

struct ProductA1 : ProductA {
  void print() override { PRINT("A1"); }
};

struct ProductA2 : ProductA {
  void print() override { PRINT("A2"); }
};

struct ProductB {
  virtual ~ProductB() = default;
  virtual void print() = 0;
};

struct ProductB1 : ProductB {
  void print() override { PRINT("B1"); }
};

struct ProductB2 : ProductB {
  void print() override { PRINT("B2"); }
};

struct Factory {
  virtual ~Factory() = default;
  virtual shared_ptr<ProductA> produceA() = 0;
  virtual shared_ptr<ProductB> produceB() = 0;
};

struct Factory1 : Factory {
  shared_ptr<ProductA> produceA() override { return make_shared<ProductA1>(); }
  shared_ptr<ProductB> produceB() override { return make_shared<ProductB1>(); }
};

struct Factory2 : Factory {
  shared_ptr<ProductA> produceA() override { return make_shared<ProductA2>(); }
  shared_ptr<ProductB> produceB() override { return make_shared<ProductB2>(); }
};

int main() {
  shared_ptr<Factory> f = make_shared<Factory1>();
  auto a = f->produceA();
  a->print();
  auto b = f->produceB();
  b->print();

  f = make_shared<Factory2>();
  a = f->produceA();
  a->print();
  b = f->produceB();
  b->print();
}
