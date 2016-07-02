#ifndef PARSER_H
#define PARSER_H

#include <string>
#include "Ast.h"
#include "Lexer.h"	

class Parser
{
public:
	Parser(std::string regualrExpr, std::string matchContent);

	void		Parse();
	AstNode*	Regex();
	AstNode*	Concat();
	AstNode*	Element();
	AstNode*	Factor();


private:
	void	Read();					// ��ȡ������ʽ���¸��ַ�
	bool	Match(char ch);			// ����ǰ�ַ��� ch ƥ��
	// bool	MatchAndRead(char ch);	// ����ǰ�ַ��� ch ƥ�䣬���ɹ����ȡ�¸��ַ�����ʧ�ܣ��� error_ ��Ϊ true


	std::string					regex_;
	std::string					matchContent_;
	bool						error_ = false;
	std::string::size_type		index_ = 0;
	char						ch_;
};

#endif