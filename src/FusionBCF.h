#ifndef FUSION_XML_FUSIONXML_H
#define FUSION_XML_FUSIONXML_H

#include <string>
#include <map>
#include <unordered_set>
#include <unordered_map>

#pragma warning(push, 0)  
#include "rapidxml.hpp"
#pragma warning(pop)  

#if defined(VLD_ENABLE)
#include <vld.h>
#endif

#include "Key.h"

class FusionBCF {
public:
  using Document = rapidxml::xml_document<char>;
  using Node = rapidxml::xml_node<char>;

  FusionBCF() = default;
  virtual ~FusionBCF() = default;

  void UnionDocuments(const Document &a, const Document &b, Document &c);
  void UnionNodes(const Node *a, const Node *b, Node *c, Document& doc);
  void UnionChildNodes(const Node *a, Node *c, Document& doc);
  void UnionAtributes(const Node *a, Node *b, Document& doc);
  void UnionValues(const Node *a, Node *b, Document& doc);

private:
  using Atribute = rapidxml::xml_attribute<char>;
  using NodeAtributeMap = std::unordered_map<std::string, Atribute*>;
  using KeyType = Key<std::string, char>;
  using NodeMultiMap = std::unordered_multimap<Key<std::string, char>, const Node*>;
  using KeySet = std::unordered_set<Key<std::string, char>>;
  using SortMap = std::map<std::string, Node*>;

  void AppendNode(const KeyType &k, NodeMultiMap& map, Node *c, Document& doc);
  void UnifyAppend(const KeyType &k, NodeMultiMap& map, Node *c, Document& doc);
  void UnifyAppendMulti(const KeyType &k, NodeMultiMap& map, Node *c, Document& doc);
  void EnumerateAtributes(const Node* a, NodeAtributeMap& unionMap) const;
  void EnumerateNodes(const Node *node, KeySet& keys, NodeMultiMap &map) const;
  void SortDocumentTop(Node* root);
  void SortViewpoint(Node* root);
  void SortCommentsByDate(Node* root);
  void PreppendNode(const std::string& name, Node* parent);
  void RemoveDuplicateKeys(const Node* nodeToMerge, KeySet& keys);

  std::string idFilter = "Guid";
};

#endif

