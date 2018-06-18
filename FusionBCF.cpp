#include "FusionBCF.h"

using namespace rapidxml;
using namespace std;

void FusionBCF::UnionDocuments(const Document& a, const Document& b, Document& c) {
  destDoc = nullptr;
  destDoc = &c;
  const Node *aNode = a.first_node();
  const Node *bNode = b.first_node();
  auto root = destDoc->allocate_node(node_element, aNode->name());
  UnionAtributes(aNode, root);
  UnionAtributes(bNode, root);
  UnionNodes(aNode->first_node(), bNode->first_node(), root);
  SortDocumentTop(root);

  c.append_node(root);
}

void FusionBCF::UnionNodes(const Node *priorityDocument, const Node *secondaryDocument, Node *destination) {
  NodeMultiMap map;
  KeySet keys;
  EnumerateNodes(priorityDocument, keys, map);
  EnumerateNodes(secondaryDocument, keys, map);  

  for (auto&& key : keys) {
    if (key.Unify()) {
      UnifyAppendMulti(key, map, destination);
    }
    else {
      UnifyAppend(key, map, destination);
    }
  }
}

void FusionBCF::RemoveDuplicateKeys(const Node* nodeToMerge, KeySet& keys) {
  NodeMultiMap map;
  KeySet oldKeys;
  EnumerateNodes(nodeToMerge->first_node(), oldKeys, map);
  for (auto key : oldKeys) {
    keys.erase(key);
  }
}

void FusionBCF::UnionChildNodes(const Node *appendNode, Node *destination) {
  NodeMultiMap map;
  KeySet keys;
  EnumerateNodes(appendNode, keys, map);
  RemoveDuplicateKeys(destination, keys);

  for (auto&& key : keys) {
    if (key.Unify()) {
      UnifyAppendMulti(key, map, destination);
    }
    else {
      AppendNode(key, map, destination);
    }
  }
}

void FusionBCF::AppendNode(const KeyType &key, NodeMultiMap& map, Node *destination) {
  auto simblingsSameTag = map.equal_range(key);
  for (auto it = simblingsSameTag.first; it != simblingsSameTag.second; ++it) {
    if (auto node = it->second) {
      destination->append_node(destDoc->clone_node(node));
    }
  }
}

void FusionBCF::UnifyAppendMulti(const KeyType &key, NodeMultiMap& map, Node *destination) {
  if (map.count(key) > 1) {
    auto accumulator = destDoc->allocate_node(node_element, destDoc->allocate_string(key.Name().c_str()));
    auto simblingsSameTag = map.equal_range(key);
    for (auto it = simblingsSameTag.first; it != simblingsSameTag.second; ++it) {
      const Node* appendNode = it->second;
      UnionAtributes(appendNode, accumulator);
      UnionValues(appendNode, accumulator);
      UnionChildNodes(appendNode->first_node(), accumulator);
    }
    destination->append_node(accumulator);
  }
  else {
    AppendNode(key, map, destination);
  }
}

void FusionBCF::UnifyAppend(const KeyType &key, NodeMultiMap& map, Node *destination) {
  auto accumulatorNode = destDoc->allocate_node(node_element, destDoc->allocate_string(key.Name().c_str()));
  auto listIterator = map.find(key);
  const Node* appendNode = listIterator->second;
  UnionAtributes(appendNode, accumulatorNode);
  UnionValues(appendNode, accumulatorNode);
  UnionNodes(appendNode->first_node(), accumulatorNode->first_node(), accumulatorNode);
  destination->append_node(accumulatorNode);
}

void FusionBCF::UnionValues(const Node *appendNode, Node *parentDestination) {
  if (appendNode->value_size() > 0) {
    parentDestination->value(destDoc->allocate_string(appendNode->value()));
  }
}

void FusionBCF::UnionAtributes(const Node *appendNode, Node *parentDestination) {
  NodeAtributeMap map;
  EnumerateAtributes(appendNode, map);
  
  const char* ch_filter = idFilter.c_str();
  for (auto&& item : map) {
    Atribute* atr = item.second;
    if (Atribute* hasAtribute = parentDestination->first_attribute(atr->name())) {
      parentDestination->remove_attribute(hasAtribute);
    }

    Atribute* cloneAtribute = destDoc->allocate_attribute(atr->name(), atr->value());
    if (strcmp(ch_filter, cloneAtribute->name()) == 0) {
      parentDestination->prepend_attribute(cloneAtribute);
    }
    else {
      parentDestination->append_attribute(cloneAtribute);
    }
  }
}

void FusionBCF::EnumerateAtributes(const Node* node, NodeAtributeMap& unionMap) const {
  if (node != nullptr) {
    for (Atribute* atribute = node->first_attribute(); atribute != nullptr; atribute = atribute->next_attribute()) {
      if (char* name = atribute->name()) {
        unionMap.emplace(string(name), atribute);
      }
    }
  }
}

void FusionBCF::EnumerateNodes(const Node *_node, KeySet& keys, NodeMultiMap &map) const {
  if (_node != nullptr) {
    const char* filter = idFilter.c_str();
    for (const Node* node = _node; node != nullptr; node = node->next_sibling()) {
      if (Atribute *atribute = node->first_attribute(filter)) {
        KeyType key(node->name(), atribute->value(), true);
        keys.emplace(key);
        map.emplace(key, node);
      }
      else {
        if (char* name = node->name()) {
          if (strcmp(name, "") != 0) {
            KeyType key(node->name());
            keys.emplace(key);
            map.emplace(key, node);
          }
        }
      }
    }
  }
}

void FusionBCF::PreppendNode(const string& name, Node* parentDestination) {
  if (Node* node = parentDestination->first_node(name.c_str())) {
    parentDestination->insert_node(parentDestination->first_node(), destDoc->clone_node(node));
    node->remove_all_nodes();
    parentDestination->remove_node(node);
  }
}

void FusionBCF::SortDocumentTop(Node* root) {
  PreppendNode("Topic", root);
  PreppendNode("Header", root);
}

