#include "scanner.hpp"
#include "token.hpp"
#include "common.hpp"
#include "node.hpp"
#include "parser.hpp"
#include "ast_printer.hpp"

#include <string_view>
#include <iostream>
#include <fstream>
#include <sstream>

static void RunFile(std::string_view filename)
{
    std::ifstream file{filename.data()};
    if (!file)
    {
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code{buffer.str()};

    lox::Scanner scanner{code};

    // lox::Token t;
    // do
    // {
    //     t = scanner.NextToken();
    //     std::cout << "lexeme: " << t.Lexeme() << " type: " << t.TypeInt() << std::endl;
    // } while (t.Type() != lox::TokenType::Eof && t.Type() != lox::TokenType::Error);



    lox::Parser parser{&scanner};
    auto root = parser.Parse();

    lox::ASTPrinter printer;
    std::cout << printer.Visit(root) << std::endl;
}


int main(int argc, char** argv)
{
    if (argc > 1)
    {
        RunFile(argv[1]);
    }

    return 0;
}