#include "FusionBCF.h"

#include <string>
#include <vector>

#include <string.h>

#include "Timestamp.h"

using namespace rapidxml;

namespace
{
  std::string GetSortableTimestamp(
    const std::string& inputDate
  )
  {
    static std::vector<std::string> timestamps = { "%Y-%m-%dT%H:%M:%S.%E3f%Ez", "%Y-%m-%dT%H:%M:%S%Ez", "%Y-%m-%dT%H:%M:%S.%E5fZ", "%Y-%m-%dT%H:%M:%S" };
    for (auto && format : timestamps) {
      if (Timestamp::IsValid(inputDate, format)) {
        Timestamp oldFormat(inputDate, format);
        return oldFormat.InLocalTime("%Y%m%dT%H%M%S.%E5f%Ez");
      }
    }

    Timestamp now;
    return now.InLocalTime("%Y%m%dT%H%M%S.%E5f%Ez");
  }
}

void FusionBCF::UnionDocuments(const Document& a, const Document& b, Document& doc)
{
  const Node* aNode = a.first_node("Markup");
  const Node* bNode = b.first_node("Markup");

  if (aNode != nullptr && bNode != nullptr) {
    auto root = doc.allocate_node(node_element, "Markup");
    UnionAtributes(aNode, root, doc);
    UnionAtributes(bNode, root, doc);
    UnionNodes(aNode->first_node(), bNode->first_node(), root, doc);
    SortCommentsByDate(root);
    SortViewpoint(root);
    SortDocumentTop(root);

    xml_node<>* decl = doc.allocate_node(node_declaration);
    decl->append_attribute(doc.allocate_attribute("version", "1.0"));
    decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
    doc.prepend_node(decl);

    doc.append_node(root);
  }
}

void FusionBCF::UnionNodes(const Node *priorityDocument, const Node *secondaryDocument, Node *destination, Document& doc) {
  NodeMultiMap map;
  KeySet keys;
  EnumerateNodes(priorityDocument, keys, map);
  EnumerateNodes(secondaryDocument, keys, map);  

  for (auto&& key : keys) {
    if (key.Unify()) {
      UnifyAppendMulti(key, map, destination, doc);
    }
    else {
      UnifyAppend(key, map, destination, doc);
    }
  }
}

void FusionBCF::RemoveDuplicateKeys(const Node* nodeToMerge, KeySet& keys) {
  NodeMultiMap map;
  KeySet oldKeys;
  EnumerateNodes(nodeToMerge->first_node(), oldKeys, map);
  for (auto&& key : oldKeys) {
    keys.erase(key);
  }
}

void FusionBCF::UnionChildNodes(const Node *appendNode, Node *destination, Document& doc) {
  NodeMultiMap map;
  KeySet keys;
  EnumerateNodes(appendNode, keys, map);
  RemoveDuplicateKeys(destination, keys);

  for (auto && key : keys) {
    if (key.Unify()) {
      UnifyAppendMulti(key, map, destination, doc);
    }
    else {
      AppendNode(key, map, destination, doc);
    }
  }
}

void FusionBCF::AppendNode(const KeyType &key, NodeMultiMap& map, Node *destination, Document& doc) {
  auto simblingsSameTag = map.equal_range(key);
  for (auto it = simblingsSameTag.first; it != simblingsSameTag.second; ++it) {
    if (auto node = it->second) {
      destination->append_node(doc.clone_node(node));
    }
  }
}

void FusionBCF::UnifyAppendMulti(const KeyType &key, NodeMultiMap& map, Node *destination, Document& doc) {
  if (map.count(key) > 1) {
    auto accumulator = doc.allocate_node(node_element, doc.allocate_string(key.Name().c_str()));
    auto simblingsSameTag = map.equal_range(key);
    for (auto it = simblingsSameTag.first; it != simblingsSameTag.second; ++it) {
      const Node* appendNode = it->second;
      UnionAtributes(appendNode, accumulator, doc);
      UnionValues(appendNode, accumulator, doc);
      UnionChildNodes(appendNode->first_node(), accumulator, doc);
    }
    destination->append_node(accumulator);
  }
  else {
    AppendNode(key, map, destination, doc);
  }
}

void FusionBCF::UnifyAppend(const KeyType &key, NodeMultiMap& map, Node *destination, Document& doc) {
  auto accumulatorNode = doc.allocate_node(node_element, doc.allocate_string(key.Name().c_str()));
  auto listIterator = map.find(key);
  const Node* appendNode = listIterator->second;
  UnionAtributes(appendNode, accumulatorNode, doc);
  UnionValues(appendNode, accumulatorNode, doc);
  UnionNodes(appendNode->first_node(), accumulatorNode->first_node(), accumulatorNode, doc);
  destination->append_node(accumulatorNode);
}

void FusionBCF::UnionValues(const Node* appendNode, Node* parentDestination, Document& doc) {
  if (appendNode->value() != nullptr && appendNode->value_size() > 0) {
    parentDestination->value(doc.allocate_string(appendNode->value()));
  }
}

void FusionBCF::UnionAtributes(const Node *appendNode, Node *parentDestination, Document& doc) {
  NodeAtributeMap map;
  EnumerateAtributes(appendNode, map);

  const char* ch_filter = idFilter.c_str();
  for (auto && item : map) {
    Atribute* atr = item.second;
    if (Atribute* hasAtribute = parentDestination->first_attribute(atr->name())) {
      if (atr->value() != nullptr && atr->value_size() > 0) {
        parentDestination->value(doc.allocate_string(appendNode->value()));
      }
    }
    else {
      Atribute* cloneAtribute = doc.allocate_attribute(doc.allocate_string(atr->name()), doc.allocate_string(atr->value()));
      if (strcmp(ch_filter, cloneAtribute->name()) == 0) {
        parentDestination->prepend_attribute(cloneAtribute);
      }
      else {
        parentDestination->append_attribute(cloneAtribute);
      }
    }
  }
}

void FusionBCF::EnumerateAtributes(const Node* node, NodeAtributeMap& unionMap) const {
  if (node != nullptr) {
    for (Atribute* atribute = node->first_attribute(); atribute != nullptr; atribute = atribute->next_attribute()) {
      if (char* name = atribute->name()) {
        unionMap.emplace(std::string(name), atribute);
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

void FusionBCF::PreppendNode(const std::string& name, Node* parentDestination) {
  if (Node* node = parentDestination->first_node(name.c_str())) {
    parentDestination->remove_node(node);
    parentDestination->prepend_node(node);
  }
}

void FusionBCF::SortDocumentTop(Node* root) {
  PreppendNode("Topic", root);
  PreppendNode("Header", root);
}

void FusionBCF::SortViewpoint(Node* root) {
  SortMap viewpoints;
  for (Node* node = root->first_node(); node != nullptr; node = node->next_sibling()) {
    if (strcmp(node->name(), "Viewpoints") == 0) {
      if (Atribute* atribute = node->first_attribute("Guid")) {
        viewpoints.emplace(atribute->value(), node);
      }
    }
  }

  for (auto&& it : viewpoints) {
    auto viewpoint = it.second;
    root->remove_node(viewpoint);
    root->append_node(viewpoint);
  }
}

void FusionBCF::SortCommentsByDate(Node* root) {
  std::vector<Node*> commentsToRemove;
  SortMap comments;
  for (Node* node = root->first_node("Comment"); node != nullptr; node = node->next_sibling()) {
    if (strcmp(node->name(), "Comment") == 0) {
      if (auto && date = node->first_node("Date")) {
        comments.emplace(GetSortableTimestamp(date->value()), node);
      }
    }
  }

  for (auto&& it : comments) {
    auto comment = it.second;
    root->remove_node(comment);
    root->append_node(comment);
  }
}

