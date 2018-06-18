#ifndef FUSION_XML_FUSIONXML_H
#define FUSION_XML_FUSIONXML_H

#include <string>
#include <unordered_set>
#include <unordered_map>

#include "rapidxml/rapidxml.hpp"

#include "Key.h"

class FusionBCF {
public:
  using Document = rapidxml::xml_document<char>;
  using Node = rapidxml::xml_node<char>;

  FusionBCF() = delete;
  explicit FusionBCF(const std::string& idFilter) : idFilter(idFilter) {};
  virtual ~FusionBCF() = default;


  void UnionDocuments(const Document &a, const Document &b, Document &c);
  void UnionNodes(const Node *a, const Node *b, Node *c);
  void UnionChildNodes(const Node *a, Node *c);
  void UnionAtributes(const Node *a, Node *b);
  void UnionValues(const Node *a, Node *b);

private:
  using Atribute = rapidxml::xml_attribute<char>;
  using NodeAtributeMap = std::unordered_map<std::string, Atribute*>;
  using KeyType = Key<std::string, char>;
  using NodeMultiMap = std::unordered_multimap<Key<std::string, char>, const Node*>;
  using KeySet = std::unordered_set<Key<std::string, char>>;

  void Append(const KeyType &k, NodeMultiMap& map, Node *c);
  void UnifyAppend(const KeyType &k, NodeMultiMap& map, Node *c);
  void UnifyAppendMultiTags(const KeyType &k, NodeMultiMap& map, Node *c);
  void EnumerateAtributes(const Node* a, NodeAtributeMap& unionMap) const;
  void EnumerateNodes(const Node *node, KeySet& keys, NodeMultiMap &map) const;
  void SortTagTopic(Node* root);
  void Preppend(const std::string& name, Node* parent);
  void RemoveDuplicateKeys(const Node* nodeToMerge, KeySet& keys);

  std::string idFilter;
  Document* doc = nullptr;
};


#endif

