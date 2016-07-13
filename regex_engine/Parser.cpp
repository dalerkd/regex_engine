#include <set>
#include <iostream>
#include <cctype>
#include "Parser.h"	


using std::endl;
using std::cout;
using std::string;
using std::pair;
using std::set;

Parser::Parser(const std::string &regex, const std::string &matchContent) 
	: lex_(regex), token_(TokenType::END, "\0"), matchContent_(matchContent)
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
	ASTNode* root = Regex();
	
	if (!Match(TokenType::END))
		error_ = true;

	if (error_)
	{
		cout << "������ʽ�﷨����" << endl;
	}
	else
	{
		cout << "������ʽ�﷨��ȷ" << endl;
		if (root)
		{
			auto nodePair = root->ConstructNFA();
			nodePair.second->accept = true;
			if (RunNFA(nodePair.first, matchContent_))
				cout << "ƥ��ɹ�" << endl;
			else
				cout << "ƥ��ʧ��" << endl;
		}
		
	}
	
}

ASTNode* Parser::Regex()
{
	ASTNode* node = Term();
	while (Match(TokenType::OR))
	{
		GetNextToken();
		node = new ASTOR(node, Term());
	}
	
	return node;
}
 
ASTNode* Parser::Term()
{
	ASTNode* node = Factor();

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
			node = new ASTConcat(node, Factor());
			into = true;
			break;
		default:
			into = false;
			break;
		}
		if (!into)
			break;
	}

	return node;
}

ASTNode* Parser::Factor()
{
	ASTNode* node = Atom();
	std::pair<int, int> minAndMax{ 1,1 };
	switch (token_.type_)
	{
	case TokenType::LBRACE:
	case TokenType::ZERO_OR_MORE:
	case TokenType::ONE_OR_MORE:
	case TokenType::ZERO_OR_ONE:
		minAndMax = Repeat();
		break;
	default:
		return node;		// ��û���ظ���Χ��ֱ�ӷ��� node
	}
	
	//if (minAndMax.first == 0 && minAndMax.second == -1)				// �� ab*
	//{
	//	return new ASTStar(node);
	//}
	//else if (minAndMax.first == 1 && minAndMax.second == -1)		// �� ab+
	//{
	//	return new ASTConcat(node, new ASTStar(node));
	//}
	//else if (minAndMax.first == 0 && minAndMax.second == 1)			// �� ab?
	//{
	//	return new ASTOR(new ASTFactor('\0'), node);
	//}
	//else
	//{
	//	ASTNode* resultPart1 = new ASTFactor('\0');
	//	for (int i = 1; i <= minAndMax.first; ++i)
	//		resultPart1 = new ASTConcat(resultPart1, node);

	//	if (minAndMax.second == -1)
	//	{
	//		return new ASTConcat(resultPart1, new ASTStar(node));
	//	}

	//	
	//	ASTNode* resultPart2 = new ASTFactor('\0');
	//	for (int i = 1; i <= minAndMax.second - minAndMax.first; ++i)
	//	{
	//		ASTNode* temp = new ASTFactor('\0');
	//		for (int j = 1; j <= i; ++j)
	//		{
	//			temp = new ASTConcat(temp, node);
	//		}
	//		resultPart2 = new ASTOR(resultPart2, temp);
	//	}
	//	
	//	return new ASTConcat(resultPart1, resultPart2);
	//}

	
	// ��ȡ�����ӵĵ�һ����
	ASTNode* resultPart1 = new ASTFactor('\0');
	for (int i = 1; i <= minAndMax.first; ++i)
		resultPart1 = new ASTConcat(resultPart1, node);

	ASTNode* resultPart2 = new ASTFactor('\0');
	if (minAndMax.second == -1)
	{
		resultPart2 = new ASTStar(node);
	}
	else
	{
		for (int i = 1; i <= minAndMax.second - minAndMax.first; ++i)
		{
			ASTNode* temp = new ASTFactor('\0');
			for (int j = 1; j <= i; ++j)
			{
				temp = new ASTConcat(temp, node);
			}
			resultPart2 = new ASTOR(resultPart2, temp);
		}
	}

	return new ASTConcat(resultPart1, resultPart2);
}

ASTNode* Parser::Atom()
{
	ASTNode* node = nullptr;
	bool negate = false;
	set<char> range;
	switch (token_.type_)
	{
	case TokenType::LP: 
		GetNextToken(); 
		node = Regex(); 
		if (!Match(TokenType::RP)) Error("ȱ�������� ')'");
		else GetNextToken();
		break;

	case TokenType::LBRACKET:
		GetNextToken();
		if (Match(TokenType::NEGATE))
		{
			GetNextToken();
			negate = true;
		}
		node = Charclass(negate);
		if (!Match(TokenType::RBRACKET))
			Error("ȱ�� ']'");
		else
			GetNextToken();
		break;

	default:
		range = Character();
		for (auto ch : range)
			node = new ASTOR(node, new ASTFactor(ch));
	}
	
	return node;
}

std::pair<int, int> Parser::Repeat()
{
	int min = -1;
	int max = -1;
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
				if (min > max)
				{
					Error("{ } �еķ�Χ����");
				}
				if (!Match(TokenType::RBRACE))
					Error("ȱ�� '}'");
				else
					GetNextToken();
			}
		}
		else if (Match(TokenType::RBRACE))
		{
			max = min;
			GetNextToken();
		}
		else
		{
			Error("'{' '}'���﷨����");
		}
		break;

	case TokenType::ZERO_OR_MORE:
		min = 0;
		GetNextToken();
		break;

	case TokenType::ONE_OR_MORE:
		min = 1;
		GetNextToken();
		break;

	case TokenType::ZERO_OR_ONE:
		min = 0;
		max = 1;
		GetNextToken();
		break;
	default:
		Error(string("����ķ��� '") + token_.lexeme_ + '\'');
	}
	return std::pair<int, int>{min, max};
}

set<char> Parser::Character()
{
	const auto type = token_.type_;
	set<char> chSet;

	switch (type)
	{
	case TokenType::SIMPLE_CHAR:
		chSet.insert(token_.lexeme_[0]);
		GetNextToken(); 	
		break;

	case TokenType::TAB:
		chSet.insert('\t');
		GetNextToken();
		break;

	case TokenType::NEWLINE:
		chSet.insert('\n');
		GetNextToken();
		break;

	case TokenType::DIGIT:
		for (char ch = '0'; ch <= '9'; ++ch)
			chSet.insert(ch);
		GetNextToken();
		break;

	case TokenType::NOT_DIGIT:
		for (char ch = ' '; ch <= '~'; ++ch)
			if (!std::isdigit(ch))
				chSet.insert(ch);
		GetNextToken();
		break;

	case TokenType::SPACE:
		chSet.insert(' ');
		GetNextToken();
		break;

	case TokenType::NOT_SPACE:
		for (char ch = ' ' + 1; ch <= '~'; ++ch)
			chSet.insert(ch);
		GetNextToken();;
		break;

	case TokenType::WORD:
		for (char ch = ' '; ch <= '~'; ++ch)
			if (std::isalnum(ch) || ch == '_')
				chSet.insert(ch);
		GetNextToken();
		break;

	case TokenType::NOT_WORD:
		for (char ch = ' '; ch <= '~'; ++ch)
			if (!(std::isalnum(ch) || ch == '_'))
				chSet.insert(ch);
		GetNextToken();
		break;
	default:
		Error("���������");
	}
	
	return chSet;
}

ASTNode* Parser::Charclass(bool negate)
{
	set<char> chSet;
	set<char> negateCHSet;
	while (!Match(TokenType::RBRACKET))
	{
		set<char> range = Charrange();
		for (auto ch : range)
			chSet.insert(ch);
	}
	// �������Ϊ�տ�����

	ASTNode *node = nullptr;

	if (negate)
	{
		for (char ch = ' '; ch <= '~'; ++ch)
			if (chSet.find(ch) == chSet.end())
				negateCHSet.insert(ch);
		for (auto ch : negateCHSet)
			node = new ASTOR(node, new ASTFactor(ch));
	}
	else
	{
		for (auto ch : chSet)
			node = new ASTOR(node, new ASTFactor(ch));
	}

	return node;	// ASTOR �е��ӽڵ����Ϊ nullptr
}

set<char> Parser::Charrange()
{
	set<char> begin = Character();
	set<char> end;
	bool range = false;
	if (Match(TokenType::HYPHEN))
	{
		GetNextToken();
		end = Character();
		range = true;
	}

	if (range)
	{
		if (begin.size() != 1 || end.size() != 1)
		{
			Error("�ַ����� [] �з�Χ����");
			return begin;
		}

		if (*begin.begin() > *end.begin())
		{
			Error("�ַ����� [] �з�Χ����");
			return begin;
		}

		set<char> range;
		for (char ch = *begin.begin(); ch <= *end.begin(); ++ch)
			range.insert(ch);	
		return range;
	}
	else
	{
		return begin;
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
