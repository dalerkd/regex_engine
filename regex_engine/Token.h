#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType
{
	SIMPLE_CHAR,
	
	LP,				// (   �Ǽ��ַ���ת��֮���Ϊ���ַ�
	RP,				// )
	ZERO_OR_MORE,	// *
	ONE_OR_ZERO,	// +
	ZERO_OR_ONE,	// ?
	HYPHEN,			// -   ���Ӻ�
	ANY,			// .
	LBRACKET,		// [
	RBRACKET,		// ]
	BACKSLASH,		// \\  ��б��
	NEGATE,			// ^   ȡ��
	LBRACE,			// {
	RBRACE,			// }
	OR,				// |

	
	TAB,			// \t
	NEWLINE,		// \n
	DIGIT,			// \d  ==  [0-9]
	NOT_DIGIT,		// \D  ==  [^0-9]
	SPACE,			// \s  ==  [ \t\n]
	NOT_SPACE,		// \S  ==  [^ \t\n]
	WORD,			// \w  ==  [a-zA-Z0-9_]
	NOT_WORD,		// \W  ==  [^a-zA-Z0-9_]

	INTEGER			// ������ֻ���� regex{min, max}
};

struct Token
{
	TokenType		type_;
	std::string		lexeme;
};

#endif
