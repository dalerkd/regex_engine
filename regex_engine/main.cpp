#include <cstdlib>
#include <iostream>
#include "Parser.h"

using std::endl;
using std::cout;
using std::cin;

int main()
{
	Parser p("(a|b)*abb", "77"); 
	p.Parse();

	system("pause");
	return 0;
}