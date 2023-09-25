#include<iostream>
#include<fstream>
#include"lexer.hpp"
#include"MIPS.hpp"


int main()
{
    freopen("test.s","r",stdin);
    // freopen("test.txt","w",stdout);
    lexer::token_stream str(std::cin);
    auto it=str.iter();
    auto res=MIPS::mapper["syscall"].parser->parse(it);
    res={};
}