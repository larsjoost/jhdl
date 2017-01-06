/*
 * Text.h
 *
 *  Created on: 5 Jan 2017
 *      Author: lars_
 */

#ifndef SRC_AST_TEXT_H_
#define SRC_AST_TEXT_H_

namespace ast {

class TextEof {};

class Text {
	const char* text;
	int position;
	int size;
	int line_number;
	int line_start;
	int column_number;
	std::true_type caseSentitive;
public:
	Text(const char* t, std::true_type caseSensitive = true);
	void get(Text& t);
	void set(const Text& t);
	char lookAhead(int n);
	void incrementPosition();
	void advancePosition(int size);
	int getLine();
	int getColumn();
	int getPosition();
	void setSize(int s);
	int equals(Text& t);
};

}

#endif /* SRC_AST_TEXT_H_ */
