#include <cstdlib>
#include <iostream>
#include "Parser.h"

using std::endl;
using std::cout;
using std::cin;

int main()
{
	//Lexer lex("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&',/:;<=>@_`~");
	//Lexer lex("()*+-.?[]^{|}");
	//Lexer lex("\\t\\n\\d\\D\\s\\S\\w\\W");
	//Lexer lex("\\0\\1\\2\\3\\4\\5\\6\\7\\8\\9\\a\\b\\c\\d\\e\\f\\g\\h\\i\\j\\k\\l\\m\\n\\o\\p\\q\\r\\s\\t\\u\\v\\w\\x\\y\\z");
	//Lexer lex("\\!\\\"\\#\\$\\%\\&\\'\\(\\)\\*\\+\\,\\-\\.\\/\\:\\;\\<\\=\\>\\?\\@\\[\\\\\\]\\^\\_\\`\\{\\|\\}\\~");

	Parser engine("abc*");
	engine.Parse();

	system("pause");
	return 0;
}