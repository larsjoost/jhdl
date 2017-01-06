/*
 * VhdlScanner.cpp
 *
 *  Created on: 4 Jan 2017
 *      Author: lars_
 */

#include <fstream>
#include <string>
#include <cstring>

#include "Scanner.h"
#include "Text.h"

namespace ast {

int Identifier::equals(Identifier& i) {
	return text.equals(i.text);
}

void Scanner::setText(const char* s) {
	text.text = s;
	text.position = 0;
	text.size = strlen(s);
}

void Scanner::loadFile(const char* filename) {
	 std::ifstream ifs(filename);
	 std::string content( (std::istreambuf_iterator<char>(ifs) ),
	                        (std::istreambuf_iterator<char>()    ) );
	 setText(content.c_str());
}

int Scanner::match(const char* t) {
	int len = strlen(t);
	try {
		for (int i=0; i<len; i++) {
			if (text.lookAhead(i) != t[i]) {
				return 0;
			}
		}
	} catch (...) {return 0;}
	return len;
}

int Scanner::accept(const char* t) {
	int len = match(t);
	text.advancePosition(len);
	return len;
}

int Scanner::optional(const char* t) {
	return accept(t);
}

int Scanner::expect(const char* t) {
	int len = accept(t);
	if (!len) {
		throw UnexpectedToken();
	}
	return len;
}

void Scanner::getText(Text& t) {
	text.get(t);
}

void Scanner::setText(const Text& t) {
	text.set(t);
}

int Scanner::getPosition() {
	return text.getPosition();
}

int Scanner::isWhiteSpace() {
	char a = text.lookAhead(0);
	if (a == ' ' || a == '\t' || a == '\n') {
		return 1;
	}
	return 0;
}

int Scanner::skipWhiteSpace() {
	int i = 0;
	try {
		if (isWhiteSpace()) {
			text.incrementPosition();
			i++;
		}
	} catch(...) {};
	return i;
}

int Scanner::skipUntilEndOfLine() {
	int i = 0;
	try {
		while (text.lookAhead(0) != '\n') {
			text.incrementPosition();
			i++;
		}
	} catch(...) {};
	return i;
}

void Scanner::print(const char* severity, const char* msg) {
	fprintf(stderr, "#%s in file %s at %i, %i: %s\n", severity, filename, text.getLine(), text.getColumn(), msg);
}

void Scanner::error(const char* msg) {
	number_of_errors++;
	print("Error", msg);
}

void Scanner::warning(const char* msg) {
	print("Warning", msg);
}

void Scanner::critical(const char* msg) {
	print("Critical", msg);
}

int Scanner::getNumberOfErrors() {
	return number_of_errors;
}

}
