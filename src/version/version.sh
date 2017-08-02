#!/bin/bash

VERSION=0.2.0
ID=$(git rev-parse --short HEAD)
DATE=$(date -I)

echo "#ifndef _VERSION_H" 
echo "#define _VERSION_H" 
echo
echo "#include <string>"
echo
echo "namespace version {"
echo
echo "  struct Version {";
echo "    const std::string id = \"$ID\";" 
echo "    const std::string date = \"$DATE\";"
echo "    const std::string version = \"$VERSION\";"
echo
echo "  };"
echo "}"
echo
echo "#endif" 

