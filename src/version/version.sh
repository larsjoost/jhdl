#!/bin/bash

VERSION=0.1
GIT_HASH=$(git rev-parse --short HEAD)
DATE=$(date -I)

echo "#ifndef _VERSION_H" 
echo "#define _VERSION_H" 
echo
echo "#include <string>"
echo
echo "namespace version {"
echo
echo "  struct Version {";
echo "    const static std::string git_hash = \"${GIT_HASH}\"" 
echo "    const static std::string date = \"$DATE\""
echo "    const static std::string version = \"$VERSION\""
echo
echo "  };"
echo "}"
echo
echo "#endif" 

