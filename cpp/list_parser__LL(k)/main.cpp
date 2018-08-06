#include <iostream>
#include <exception>
#include <stdexcept>
#include <vector>

using std::string;

static const string tokenNames[] = 
            { "n/a", "<EOF>", "NAME", "COMMA", "LBRACK", "RBRACK", "EQUALS" };

class Token
{
public:
    int id; string text;
    
    Token () {}
    Token(int _id, string _text)
          : id(_id), text(_text) {}

    string const toString() const {
        return "<'" + text + "', " + tokenNames[id] + ">";
    }
};            

class Lexer
{
public:
    static const int iEOF = -1;
    static const int EOF_TYPE = 1;
    
    char c;
    string input;
    int p;
    Lexer(string str) : input(str), p(0) { c = input[0]; }
    
    virtual Token nextToken()=0;
};

class ListLexer : public Lexer
{
public:
    static const int NAME   = 2,
                     COMMA  = 3,
                     LBRACK = 4,
                     RBRACK = 5,
                     EQUALS = 6;
                     
    bool isLetter() const {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }
    
    ListLexer(string str) : Lexer(str) {}
    
    Token nextToken() {
        while(c != iEOF) {
            switch(c) {
                case ' ': case '\t': case '\n': case '\r': WS(); continue;
                case ',' : consume(); return Token(COMMA, ",");
                case '[' : consume(); return Token(LBRACK, "[");
                case ']' : consume(); return Token(RBRACK, "]");
                case '=' : consume(); return Token(EQUALS, "=");
                default:
                    if( isLetter() ) return _NAME();
                    throw std::runtime_error((string)"Invalid character: " + c);
            }
        }
        return Token(EOF_TYPE, "<EOF>");
    }
    
    Token _NAME() {
        string str = "";
        do { str += c; consume(); } while( isLetter() );
        return Token(NAME, str);
    }
    
    void consume() {
        ++p;
        if(p >= input.length()) c = iEOF;
        else c = input[p];
    }
    
    void WS() {
        while(c == ' ' || c == '\n' || c == '\t' || c == '\r')
            consume();
    }
    
    void match(char x) {
        if(c == x) consume();
        else throw std::runtime_error((string)"expected " + x + (string)"; found: " + c);
    }
};

class Parser
{
public:
    std::vector<Token> tokens; // circular list, see consume()
    Lexer* input;
    int k, p;
    
    Parser(Lexer* lexer, int _k) : k(_k), p(0), input(lexer) {
        tokens.resize(k);
        for(int i = 0; i < k; ++i)
            consume();
    }

    void consume() {
        tokens[p] = input->nextToken();
        std::cout << LT(1).text << " ";
        p = (p+1) % k;
    }
    
    Token LT(int i) {
        return tokens[(p+i-1) % k];
    }
    
    int LA(int x) {
        return LT(x).id;
    }
    
    void match(char x) {
        if(LA(1) == x) consume();
        else throw std::runtime_error((string)"Expecting " + tokenNames[p] + (string)"; found: " + LT(1).toString());
    }
    
    void list() {
        match(ListLexer::LBRACK); elements(); match(ListLexer::RBRACK);
    }
    
    void elements() {
        element();
        while(LA(1) == ListLexer::COMMA) {
            match(ListLexer::COMMA); element();
        }
    }
    
    void element() {
        if(LA(1) == ListLexer::NAME && LA(2) == ListLexer::EQUALS) {
            match(ListLexer::NAME);
            match(ListLexer::EQUALS);
            match(ListLexer::NAME);
        }
        else if(LA(1) == ListLexer::NAME) match(ListLexer::NAME);
        else if(LA(1) == ListLexer::LBRACK) list();
        else throw std::runtime_error((string)"Expecting name or list; found: " + LT(1).toString());
    }
};

int main(int argc, char **argv)
{
    ListLexer l("[foo, [spam , tl = dr,\ttest], bar ]");
    Parser p(&l, 2);
    
    p.list();
    
    return 0;
}
