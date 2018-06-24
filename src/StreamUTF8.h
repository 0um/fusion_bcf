#ifndef FUSION_XML_STREAMUTF8_H
#define FUSION_XML_STREAMUTF8_H

#include <sstream>
#include <locale>
#include <codecvt>

namespace SUTF8 {
  std::wstring GetWString(const char *c) {
    const size_t cSize = strlen(c) + 1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs(wc, c, cSize);

    std::wstring w = wc;
    delete wc;
    return w;
  }

  void ReadInUTF8(const std::wstring& filePath, std::wstringstream& stream) {
    std::wifstream wif(filePath);
    wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::consume_header>));
    stream << wif.rdbuf();
  }

  void WriteInUTF8(const std::wstring& filePath, const std::wstring& content) {
    std::wofstream wof;
    wof.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));
    wof.open(filePath);
    wof << content;
    wof.close();
  }

  void ReadInUTF8C(const char* filePath, std::wstringstream& stream) {
    ReadInUTF8(GetWString(filePath), stream);
  }

  void WriteInUTF8C(const char* filePath, const std::wstring& content) {
    WriteInUTF8(GetWString(filePath), content);
  }
}

#endif