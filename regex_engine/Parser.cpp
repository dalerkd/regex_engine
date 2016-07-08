#include <iostream>
#include <cctype>
#include "Parser.h"	

using std::endl;
using std::cout;
using std::string;


Parser::Parser(const std::string &regex) : lex_(regex), token_(TokenType::END, "\0")
{
	GetNextToken();
}

void Parser::GetNextToken()
{
	token_ = lex_.GetNextToken();
}

bool Parser::Match(TokenType type)
{
	return token_.type_ == type;
}


void Parser::Error(const string &info)
{
	cout << info << endl;
	error_ = true;
	GetNextToken();
}

void Parser::Parse()
{
	Regex();
	if (!Match(TokenType::END))
		error_ = true;
	if (error_)
		cout << "正则表达式语法错误" << endl;
	else
		cout << "正则表达式语法正确" << endl;
}

void Parser::Regex()
{
	Term();
	while (Match(TokenType::OR))
	{
		GetNextToken();
		Term();
	}
}
 
void Parser::Term()
{
	Factor();

	while (true)
	{
		bool into = false;
		switch (token_.type_)		// 如果下一个 token 属于 factor 则调用 factor，否则 term 调用结束
		{
		case TokenType::LBRACKET:
		case TokenType::LP:
		case TokenType::SIMPLE_CHAR:
		case TokenType::TAB:
		case TokenType::NEWLINE:
		case TokenType::DIGIT:
		case TokenType::NOT_DIGIT:
		case TokenType::SPACE:
		case TokenType::NOT_SPACE:
		case TokenType::WORD:
		case TokenType::NOT_WORD:
			Factor();
			into = true;
			break;
		default:
			into = false;
			break;
		}
		if (!into)
			break;
	}
}

void Parser::Factor()
{
	Atom();
	switch (token_.type_)
	{
	case TokenType::LBRACE:
	case TokenType::ZERO_OR_MORE:
	case TokenType::ONE_OR_MORE:
	case TokenType::ZERO_OR_ONE:
		Repeat(); 
		break;
	default:
		// do nothing
		break;
	}
}

void Parser::Atom()
{
	switch (token_.type_)
	{
	case TokenType::LP: 
		GetNextToken(); 
		Regex(); 
		if (!Match(TokenType::RP)) Error("缺少右括号 ')'");
		else GetNextToken();
		break;
	case TokenType::LBRACKET:
		GetNextToken();
		/*if (Match(TokenType::NEGATE))
		{
			GetNextToken();
			Charclass();
			if (!Match(TokenType::RBRACKET))
				Error("缺少 ']'");
			else
				GetNextToken();
		}
		else
		{
			Charclass();
			if (!Match(TokenType::RBRACKET))
				Error("缺少 ']'");
			else
				GetNextToken();
		}*/
		if (Match(TokenType::NEGATE))
			GetNextToken();
		Charclass();
		if (!Match(TokenType::RBRACKET))
			Error("缺少 ']'");
		else
			GetNextToken();
		break;
	default:
		Character();
	}
}

void Parser::Repeat()
{
	int min = 0;
	int max = 0;
	switch (token_.type_)
	{
	case TokenType::LBRACE:
		GetNextToken();
		min = Digit();
		if (Match(TokenType::SIMPLE_CHAR) && token_.lexeme_ == ",")
		{
			GetNextToken();
			if (Match(TokenType::RBRACE))
			{
				GetNextToken();
			}
			else
			{
				max = Digit();
				if (!Match(TokenType::RBRACE))
					Error("缺少 '}'");
				else
					GetNextToken();
			}
		}
		else if (Match(TokenType::RBRACE))
		{
			GetNextToken();
		}
		else
		{
			Error("'{' '}'内语法错误");
		}
		break;
	case TokenType::ZERO_OR_MORE:
		GetNextToken();
		break;
	case TokenType::ONE_OR_MORE:
		GetNextToken();
		break;
	case TokenType::ZERO_OR_ONE:
		GetNextToken();
		break;
	default:
		Error(string("错误的符号 '") + token_.lexeme_ + '\'');
	}
}

void Parser::Character()
{
	const auto type = token_.type_;
	switch (type)
	{
	case TokenType::SIMPLE_CHAR:
		GetNextToken(); 
		break;
	case TokenType::TAB:
	case TokenType::NEWLINE:
	case TokenType::DIGIT:
	case TokenType::NOT_DIGIT:
	case TokenType::SPACE:
	case TokenType::NOT_SPACE:
	case TokenType::WORD:
	case TokenType::NOT_WORD:
		GetNextToken();
		break;
	default:
		Error("错误的因子");
	}
}

void Parser::Charclass()
{
	while (!Match(TokenType::RBRACKET))
	{
		Charrange();
	}
}

void Parser::Charrange()
{
	Character();
	if (Match(TokenType::HYPHEN))
	{
		GetNextToken();
		Character();
	}
}

int Parser::Digit()
{
	string buffer;
	while (Match(TokenType::SIMPLE_CHAR) && std::isdigit(token_.lexeme_[0]))
	{
		buffer += token_.lexeme_[0];
		GetNextToken();
	}
	if (buffer.empty())
		Error("缺少重复次数");
	else
		return std::stoi(buffer);
	
	return 0;
}
