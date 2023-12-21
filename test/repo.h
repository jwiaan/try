#pragma once

#include <memory>
#include <string>
#include <typeindex>

struct Stub;
struct Repo {
  static Repo &repo();
  virtual ~Repo() = default;
  virtual void reset() = 0;
  virtual void add(const std::type_index &, const std::string &,
                   const std::shared_ptr<Stub> &) = 0;
  virtual std::shared_ptr<Stub> get(const std::type_index &,
                                    const std::string &) = 0;
};

template <typename T>
void add(const std::string &s, const std::shared_ptr<T> &p) {
  Repo::repo().add(typeid(T), s, p);
}

template <typename T> std::shared_ptr<T> get(const std::string &s) {
  return std::dynamic_pointer_cast<T>(Repo::repo().get(typeid(T), s));
}