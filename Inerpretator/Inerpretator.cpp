#include <iostream>
#include "Parser.h"

int main()
{
    prs::Parser parser;
    parser.fromFile("programm.txt");

    //prs::Lexeme* lex = new prs::ExpressionLexeme("0123456789");
    //prs::Lexeme lex1("HELLO");
    //prs::Lexeme lex2("HEL");
    //std::cout << std::boolalpha << (lex1 == lex2);

    return 0;
}
