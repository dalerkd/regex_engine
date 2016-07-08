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
		cout << "������ʽ�﷨����" << endl;
	else
		cout << "������ʽ�﷨��ȷ" << endl;
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
		switch (token_.type_)		// �����һ�� token ���� factor ����� factor������ term ���ý���
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
		if (!Match(TokenType::RP)) Error("ȱ�������� ')'");
		else GetNextToken();
		break;
	case TokenType::LBRACKET:
		GetNextToken();
		/*if (Match(TokenType::NEGATE))
		{
			GetNextToken();
			Charclass();
			if (!Match(TokenType::RBRACKET))
				Error("ȱ�� ']'");
			else
				GetNextToken();
		}
		else
		{
			Charclass();
			if (!Match(TokenType::RBRACKET))
				Error("ȱ�� ']'");
			else
				GetNextToken();
		}*/
		if (Match(TokenType::NEGATE))
			GetNextToken();
		Charclass();
		if (!Match(TokenType::RBRACKET))
			Error("ȱ�� ']'");
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
					Error("ȱ�� '}'");
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
			Error("'{' '}'���﷨����");
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
		Error(string("����ķ��� '") + token_.lexeme_ + '\'');
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
		Error("���������");
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
		Error("ȱ���ظ�����");
	else
		return std::stoi(buffer);
	
	return 0;
}
