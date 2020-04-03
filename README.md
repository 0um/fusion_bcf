# fusion_bcf
Command line tool to merge two bcf files

Build
```
git submodule update --init --recursive
cmake -G "Visual Studio 15 2017 Win64" . -DBUILD_TESTING=false
```

Run
```
fusion_bcf.exe inputA inputB <optional OutputC>
```

Note rapidxml under licenses
1. Boost Software License - Version 1.0 - August 17th, 2003
2. The MIT License

[rapidxml.sourceforge.net/license.txt](rapidxml.sourceforge.net/license.txt)
