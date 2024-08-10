template <typename T>
std::map<uint16_t, std::function<bool(T *, const std::string &)>> create() {
  return {};
}

template <typename T, typename U, typename... V>
std::map<uint16_t, std::function<bool(T *, const std::string &)>>
create(bool (T::*f)(const U &), bool (T::*...v)(const V &)) {
  auto a = create<T>(v...);
  a[U::GetDescriptor()->index()] = [f](T *t, const std::string &s) {
    U u;
    auto b = u.ParseFromString(s);
    assert(b);
    return (t->*f)(u);
  };

  return a;
}
