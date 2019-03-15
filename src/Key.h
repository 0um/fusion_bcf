#ifndef FUSION_XML_KEY_H
#define FUSION_XML_KEY_H

#include <string>

#if defined(VLD_ENABLE)
#include <vld.h>
#endif

template <typename String, typename Char>
class Key {
public:
  Key() = default;
  Key(const String& _name) : name(_name) {};
  Key(const String _name, const String& _id, bool _unify) : name(_name), id(_id), unify(_unify) {};
  ~Key() = default;
  bool operator<(const Key& k) const { return name < k.name || (name == k.name && id < k.id); };
  bool operator==(const Key& k) const { return name == k.name && id == k.id; };
  const String& Name() const { return name; };
  const String& ID() const { return id; };
  bool Unify() const { return unify; };

private:
  String name{ "" };
  String id{ "" };
  bool unify{ false };
};

namespace std {
  template <typename String, typename  Char>
  struct hash<Key<String, Char>> : public unary_function<Key<String, Char>, size_t> {
    size_t operator()(const Key<String, Char>& value) const {
      return std::hash<String>{}(value.Name() + value.ID());
    }
  };

  template <typename String, typename  Char>
  struct equal_to<Key<String, Char>> : public unary_function<Key<String, Char>, bool> {
    bool operator()(const Key<String, Char>& x, const Key<String, Char>& y) const {
      return x == y;
    }
  };
}

#endif