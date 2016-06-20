#ifndef PARSER_H
#define PARSER_H

#include <string>

class Parser
{
public:
	Parser(std::string regualrExpr);

	void	Parse();
	void	Regex();
	void	Concat();
	void	Element();
	void	Factor();


private:
	bool	IsEnd();
	void	Read();					// ��ȡ������ʽ���¸��ַ�
	bool	Match(char ch);			// ����ǰ�ַ��� ch ƥ��
	bool	MatchAndRead(char ch);	// ����ǰ�ַ��� ch ƥ�䣬���ɹ����ȡ�¸��ַ�����ʧ�ܣ��� error_ ��Ϊ true


	std::string					regex_;
	bool						error_ = false;
	std::string::size_type		index_ = 0;
	char						ch_;
};

#endif