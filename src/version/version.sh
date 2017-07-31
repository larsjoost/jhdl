#!/bin/bash

VERSION=0.0.1
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
echo "    const std::string id = \"${GIT_HASH}\";" 
echo "    const std::string date = \"$DATE\";"
echo "    const std::string version = \"$VERSION\";"
echo
echo "  };"
echo "}"
echo
echo "#endif" 

