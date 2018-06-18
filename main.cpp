#pragma warning(push, 0)  
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"
#pragma warning(pop)  

#include <iostream>

#include "FusionBCF.h"

using namespace rapidxml;

void ShowHelp() {
  std::cout << "fusion_bcf.exe -id filter inputA inputB <optional OutputC>" << std::endl;
  std::cout << "<optional id filer> -id <filter>" << std::endl;
}

int main(int argc, char* argv[]) {
  if (argc >= 5 && argc <= 6) {
    const std::string empty = "";
    std::string filter = argv[2];
    std::string inputA = argv[3];
    std::string inputB = argv[4];
    std::string outputC;
    if (argc > 5) {
      outputC = argv[5];
    }

    if (inputA != empty && inputB != empty) {
      xml_document<char> a;
      xml_document<char> b;
      file<char> aFile(inputA.c_str());
      file<char> bFile(inputB.c_str());
      try {
        a.parse<parse_default>(const_cast<char*>(aFile.data()));
        b.parse<parse_default>(const_cast<char*>(bFile.data()));
      }
      catch (...) {
        return 1;
      }

      xml_document<char> c;
      FusionBCF fusion(filter);
      fusion.UnionDocuments(a, b, c);
      std::string xmlAsString;
      print(back_inserter(xmlAsString), c);

      if (outputC != empty) {
        std::ofstream fileStream(outputC);
        fileStream << xmlAsString;
        fileStream.close();
      }
      else {
        std::cout << xmlAsString.c_str() << std::endl;
      }
    }
  }
  else {
    ShowHelp();
  }
  return 0;
}