/*
 * Text.cpp
 *
 *  Created on: 5 Jan 2017
 *      Author: lars_
 */

#include "Text.h"

namespace ast {

Text::Text(const char* t) {
	text = t;
	size = strlen(t);
	position = 0;
	line_number = 0;
	column_number = 0;
}

void Text::incrementPosition() {
	if ((position + 1) >= size) {
		throw TextEof();
	} else {
		if (text[position] == '\n') {
			line_number++;
			column_number = 0;
		} else {
			column_number++;
		}
		position++;
	}
}

void Text::advancePosition(int n) {
	for (int i = 0; i < n; i++) {
		incrementPosition();
	}
}

void Text::get(Text& t) {
	t.text = text;
	t.position = position;
	t.size = size;
	t.line_number = line_number;
	t.column_number = column_number;
}

void Text::set(const Text& t) {
	text = t.text;
	position = t.position;
	size = t.size;
	line_number = t.line_number;
	column_number = t.column_number;
}

char Text::lookAhead(int n) {
	int i = position + n;
	if (i >= size) {
		throw TextEof();
	}
	return text[i];
}

int Text::getLine() {
	return line_number;
}

int Text::getColumn() {
	return column_number;
}

void Text::setSize(int s) {
	size = s;
}

}

