#include "FusionBCF.h"

using namespace rapidxml;
using namespace std;

void FusionBCF::UnionDocuments(const Document& a, const Document& b, Document& c) {
  doc = nullptr;
  doc = &c;
  const Node *aNode = a.first_node();
  const Node *bNode = b.first_node(); //deve encontrar primeiramente uma raiz em comum
  auto root = doc->allocate_node(node_element, aNode->name());
  UnionAtributes(aNode, root);
  UnionAtributes(bNode, root);
  UnionNodes(aNode->first_node(), bNode->first_node(), root);
  SortTagTopic(root);

  c.append_node(root);
}

void FusionBCF::UnionNodes(const Node *a, const Node *b, Node *c) {
  NodeMultiMap map;
  KeySet keys;
  EnumerateNodes(a, keys, map);
  EnumerateNodes(b, keys, map);  

  for (auto&& k : keys) {
    if (k.Unify()) {
      UnifyAppendMultiTags(k, map, c);
    }
    else {
      UnifyAppend(k, map, c);
    }
  }
}

void FusionBCF::RemoveDuplicateKeys(const Node* nodeToMerge, KeySet& keys) {
  NodeMultiMap map;
  KeySet keysOld;
  EnumerateNodes(nodeToMerge->first_node(), keysOld, map);
  for (auto v : keysOld) {
    keys.erase(v);
  }
}

void FusionBCF::UnionChildNodes(const Node *a, Node *c) {
  NodeMultiMap map;
  KeySet keys;
  EnumerateNodes(a, keys, map);
  RemoveDuplicateKeys(c, keys);

  for (auto&& k : keys) {
    if (k.Unify()) {
      UnifyAppendMultiTags(k, map, c);
    }
    else {
      Append(k, map, c);
    }
  }
}

void FusionBCF::Append(const KeyType &k, NodeMultiMap& map, Node *c) {
  auto listNodes = map.equal_range(k);
  for (auto it = listNodes.first; it != listNodes.second; ++it) {
    if (auto node = it->second) {
      c->append_node(doc->clone_node(node));
    }
  }
}

void FusionBCF::UnifyAppendMultiTags(const KeyType &k, NodeMultiMap& map, Node *c) {
  if (map.count(k) > 1) {
    auto accumulator = doc->allocate_node(node_element, doc->allocate_string(k.Name().c_str()));
    auto simblingsSameTag = map.equal_range(k);
    for (auto it = simblingsSameTag.first; it != simblingsSameTag.second; ++it) {
      const Node* node = it->second;
      UnionAtributes(node, accumulator);
      UnionValues(node, accumulator);
      UnionChildNodes(node->first_node(), accumulator);
    }
    c->append_node(accumulator);
  }
  else {
    Append(k, map, c);
  }
}

void FusionBCF::UnifyAppend(const KeyType &k, NodeMultiMap& map, Node *c) {
  auto accumulator = doc->allocate_node(node_element, doc->allocate_string(k.Name().c_str()));
  auto p = map.find(k);
  if (auto node = p->second) {
    UnionAtributes(node, accumulator);
    UnionValues(node, accumulator);
    UnionNodes(node->first_node(), accumulator->first_node(), accumulator);
    c->append_node(accumulator);
  }
}

void FusionBCF::UnionValues(const Node *a, Node *b) {
    if (a->value_size() > 0) {
      b->value(doc->allocate_string(a->value()));
    }
}

void FusionBCF::UnionAtributes(const Node *a, Node *b) {
  NodeAtributeMap map;
  EnumerateAtributes(a, map);
  
  const char* ch_filter = idFilter.c_str();
  for (auto&& p : map) {
    auto atr = p.second;
    if (auto has = b->first_attribute(atr->name())) {
      b->remove_attribute(has);
    }

    Atribute* clone_atr = doc->allocate_attribute(atr->name(), atr->value());
    if (strcmp(ch_filter, clone_atr->name()) == 0) {
      b->prepend_attribute(clone_atr);
    }
    else {
      b->append_attribute(clone_atr);
    }
  }
}

void FusionBCF::EnumerateAtributes(const Node* node, NodeAtributeMap& unionMap) const {
  if (node != nullptr) {
    for (Atribute* attr = node->first_attribute(); attr != nullptr; attr = attr->next_attribute()) {
      if (char* name = attr->name()) {
        unionMap.emplace(string(name), attr);
      }
    }
  }
}

void FusionBCF::EnumerateNodes(const Node *_node, KeySet& keys, NodeMultiMap &map) const {
  if (_node != nullptr) {
    const char* filter = idFilter.c_str();
    for (const Node* node = _node; node != nullptr; node = node->next_sibling()) {
      if (Atribute *attr = node->first_attribute(filter)) {
        KeyType k(node->name(), attr->value(), true);
        keys.emplace(k);
        map.emplace(k, node);
      }
      else {
        if (char* name = node->name()) {
          if (strcmp(name, "") != 0) {
            KeyType k(node->name());
            keys.emplace(k);
            map.emplace(k, node);
          }
        }
      }
    }
  }
}

void FusionBCF::Preppend(const string& name, Node* parent) {
  if (Node* node = parent->first_node(name.c_str())) {
    parent->insert_node(parent->first_node(), doc->clone_node(node));
    node->remove_all_nodes();
    parent->remove_node(node);
  }
}

void FusionBCF::SortTagTopic(Node* root) {
  Preppend("Topic", root);
  Preppend("Header", root);
}

