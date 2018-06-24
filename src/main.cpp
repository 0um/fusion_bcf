#pragma warning(push, 0)  
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
#pragma warning(pop)  

#include <iostream>

#include "FusionBCF.h"

using namespace rapidxml;

void ShowHelp() {
  std::cout << "fusion_bcf.exe inputA inputB <optional OutputC>" << std::endl;
}

int main(int argc, char* argv[]) {
  if (argc >= 3 && argc <= 4) {
    const std::string empty = "";
    std::string inputA = argv[1];
    std::string inputB = argv[2];
    std::string outputC;
    if (argc > 3) {
      outputC = argv[3];
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
      FusionBCF fusion;
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