#include "scanner.hpp"
#include "token.hpp"


namespace lox
{
    static bool IsDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    static bool IsAlpha(char c) 
    {
        return (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                c == '_';
    }

    void Scanner::SkipWhitespace()
    {
        while (true) 
        {
            char c = Peek();
            switch (c) 
            {
                case ' ':
                case '\r':
                case '\t':
                    Advance();
                    break;
                case '\n':
                    ++line;
                    Advance();
                    break;
                case '/':
                    if (PeekNext() == '/')
                    {
                        while (Peek() != '\n' && !IsAtEnd())
                        {
                            Advance();  
                        } 
                    }
                    else // This is not a comment.
                    {
                        return;
                    }
                    break;
                default:
                    return;
            }
        }
    }
    
    TokenType Scanner::IdentifierType()
    {
        // a Trie
        switch (text[start]) 
        {
            case 'a': return CheckKeyword(1, "nd", TokenType::And);
            case 'c': return CheckKeyword(1, "lass", TokenType::Class);
            case 'e': return CheckKeyword(1, "lse", TokenType::Else);
            
            case 'f':
                if (current - start > 1) 
                {
                    switch (text[start+1]) 
                    {
                        case 'a': return CheckKeyword(2, "lse", TokenType::False);
                        case 'o': return CheckKeyword(2, "r", TokenType::For);
                        case 'u': return CheckKeyword(2, "n", TokenType::Fun);
                    }
                }
                break;

            case 'i': return CheckKeyword(1, "f", TokenType::If);
            case 'n': return CheckKeyword(1, "il", TokenType::Nil);
            case 'o': return CheckKeyword(1, "r", TokenType::Or);
            case 'p': return CheckKeyword(1, "rint", TokenType::Print);
            case 'r': return CheckKeyword(1, "eturn", TokenType::Return);
            case 's': return CheckKeyword(1, "uper", TokenType::Super);
            
            case 't':
                if (current - start > 1) 
                {
                    switch (text[start+1]) 
                    {
                        case 'h': return CheckKeyword(2, "is", TokenType::This);
                        case 'r': return CheckKeyword(2, "ue", TokenType::True);
                    }
                }
                break;
            
            case 'v': return CheckKeyword(1, "ar", TokenType::Var);
            case 'w': return CheckKeyword(1, "hile", TokenType::While);
        }
        return TokenType::Identifier;
    }

    Token Scanner::Identifier()
    {
        while (IsAlpha(Peek()) || IsDigit(Peek())) Advance();
         return MakeToken(IdentifierType());
    }


    Token Scanner::String()
    {
        while (Peek() != '"' && !IsAtEnd())
        {
            if (Peek() == '\n') ++line;
            Advance();
        }

        if (IsAtEnd())
        {
            return ErrorToken("Unterminated string.");
        }

        // Closing quote.
        Advance();
        return MakeToken(TokenType::String);
    }

    Token Scanner::Number()
    {
        while (IsDigit(Peek())) Advance();
    
        // Look for fractional part.
        if (Peek() == '.' && IsDigit(PeekNext()))
        {
            // Consume '.'
            Advance();
            while (IsDigit(Peek())) Advance();
        }

        return MakeToken(TokenType::Number);
    }


    Token Scanner::NextToken()
    {
        SkipWhitespace();
        start = current;

        if (IsAtEnd())
        {
            return MakeToken(TokenType::Eof);
        }

        char c = Advance();

        if (IsAlpha(c)) return Identifier();
        if (IsDigit(c)) return Number();

        switch (c)
        {
            case '(': return MakeToken(TokenType::LeftParen);
            case ')': return MakeToken(TokenType::RightParen);
            case '{': return MakeToken(TokenType::LeftBrace);
            case '}': return MakeToken(TokenType::RightBrace);
            case ';': return MakeToken(TokenType::Semicolon);
            case ',': return MakeToken(TokenType::Comma);
            case '.': return MakeToken(TokenType::Dot);
            case '-': return MakeToken(TokenType::Minus);
            case '+': return MakeToken(TokenType::Plus);
            case '/': return MakeToken(TokenType::Slash);
            case '*': return MakeToken(TokenType::Star);

            case '!': return MakeToken(Match('=') ? TokenType::BangEqual : TokenType::Bang);
            case '=': return MakeToken(Match('=') ? TokenType::EqualEqual : TokenType::Equal);
            case '<': return MakeToken(Match('=') ? TokenType::LessEqual : TokenType::Less);
            case '>': return MakeToken(Match('=') ? TokenType::GreaterEqual : TokenType::Greater);

            case '"': return String();
        }

        return ErrorToken("Unexpected character.");
    } 
} // namespace lox
