#include <iostream>
#include <stack>
#include <vector>
using namespace std;

namespace raisic {
namespace raisicLexer {
typedef enum Tokentype {
    Command,
    Comment,
    Macro,
    BuiltinFunction,
    OpeningParenthese,
    ClosingParenthese,
    OpeningBrace,
    ClosingBrace
} Tokentypes;

class Token {
   private:
    Tokentype type;
    string literal;
    int start;
    int end;
    int ln;

   public:
    Token(Tokentype type, string literal, int start, int end, int ln) {
        this->type = type;
        this->literal = literal;
        this->start = start;
        this->end = end;
        this->ln = ln;
    }

    bool operator==(Token t) {
        return this->operator==(t.type) && this->operator==(t.literal);
    }

    bool operator==(Tokentype tt) { return this->type == tt; }

    bool operator==(string l) { return this->literal == l; }

    bool operator==(vector<Token> vt) {
        for (Token t : vt) {
            if (this->operator==(t)) {
                return true;
            }
        }
        return false;
    }

    bool operator==(vector<Tokentype> vtt) {
        for (Tokentype tt : vtt) {
            if (this->operator==(tt)) {
                return true;
            }
        }
        return false;
    }

    bool operator==(vector<string> vl) {
        for (string l : vl) {
            if (this->operator==(l)) {
                return true;
            }
        }
        return false;
    }

    bool operator!=(Token t) { return !this->operator==(t); }
    bool operator!=(Tokentype tt) { return !this->operator==(tt); }
    bool operator!=(string l) { return !this->operator==(l); }
    bool operator!=(vector<Token> vt) { return !this->operator==(vt); };
    bool operator!=(vector<Tokentype> vtt) { return !this->operator==(vtt); }
    bool operator!=(vector<string> vl) { return !this->operator==(vl); }

    string AsString() {
        string types[] = {
            "Command",         "Comment",           "Macro",
            "BuiltinFunction", "OpeningParenthese", "ClosingParenthese",
            "OpeningBrace",    "ClosingBrace"};
        return "Token{ " + types[this->type] + ", '" + this->literal + "', " +
               to_string(this->start) + ".." + to_string(this->end) + ", " +
               to_string(this->ln) + " }";
    }
};

typedef struct LexerResult {
   public:
    LexerResult(vector<Token>* tokens, string err = "") {
        this->tokens = tokens;
        this->err = err;
    }
    vector<Token>* tokens;
    string err;
} LexerResult;

/*
typedef struct CollectionResult {
   public:
    CollectionResult(Token* token = nullptr, string err = "") {
        this->token = token;
        this->err = err;
    }
    Token* token;
    string err;

} CollectionResult;
*/

class Lexer {
   private:
    string text;
    int ln;
    int col;
    int idx;
    char ch;

    void advance() {
        this->idx++;
        this->col++;
        this->ch = this->idx >= this->text.size() ? -1 : this->text[this->idx];
        if (this->ch == '\n') {
            this->ln++;
            this->col = 0;
        }
    }

    string illegalCharacter() {
        return "error on line " + to_string(this->ln) + ", col " +
               to_string(this->col) + ": illegal character '" + this->ch + "'";
    }

    Token chtoken(Tokentype type) {
        string s = " ";
        s[0] = this->ch;
        return *(new Token(type, s, this->col, this->col, this->ln));
    }

    pair<Token*, string>* collectComment() {
        int start = this->idx;
        int startln = this->ln;
        string com = "";

        this->advance();
        while (this->ch == ' ' || this->ch == '\n' || this->ch == '\t') {
            this->advance();
        }

        while (this->ch != -1 && this->ch != '/') {
            com += this->ch;
            this->advance();
        }

        while (com[com.size() - 1] == ' ' || com[com.size() - 1] == '\n' ||
               com[com.size() - 1] == '\t') {
            com.pop_back();
        }

        this->advance();

        return new pair<Token*, string>(
            new Token(Tokentypes::Comment, com, start, this->idx - 1, startln),
            "");
    }

   public:
    Lexer(string text) {
        this->text = text;
        this->ln = 0;
        this->col = 0;
        this->idx = -1;
        this->ch = -1;

        this->advance();
    }

    LexerResult* lex() {
        vector<Token>* tokens = new vector<Token>();

        while (this->ch != -1) {
            switch (this->ch) {
                case ' ':
                case '\t':
                case '\n':
                    this->advance();
                    break;

                case '+':
                case '-':
                case '>':
                case '<':
                case '[':
                case ']':
                case '.':
                case ',':
                case '_':
                case '\'':
                case '`':
                    tokens->push_back(this->chtoken(Tokentypes::Command));
                    this->advance();
                    break;

                case '(':
                    tokens->push_back(
                        this->chtoken(Tokentypes::OpeningParenthese));
                    this->advance();
                    break;

                case ')':
                    tokens->push_back(
                        this->chtoken(Tokentypes::ClosingParenthese));
                    this->advance();
                    break;

                case '{':
                    tokens->push_back(this->chtoken(Tokentypes::OpeningBrace));
                    this->advance();
                    break;

                case '}':
                    tokens->push_back(this->chtoken(Tokentypes::ClosingBrace));
                    this->advance();
                    break;

                case '/': {
                    pair<Token*, string>* res = new pair<Token*, string>();
                    res = this->collectComment();
                    if (!res->second.empty()) {
                        return new LexerResult(new vector<Token>(),
                                               res->second);
                    }
                    tokens->push_back(*res->first);
                } break;

                default:
                    return new LexerResult(new vector<Token>(),
                                           this->illegalCharacter());
            }
        }

        return new LexerResult(tokens);
    }
};

}  // namespace raisicLexer

typedef struct CompResult {
   public:
    CompResult(vector<char>* bytes, string err = "") {
        this->bytes = bytes;
        this->err = err;
    }
    vector<char>* bytes;
    string err;
} CompResult;

CompResult* Compile(string code) {
    vector<char>* bytes = new vector<char>();

    raisicLexer::Lexer* lexer = new raisicLexer::Lexer(code);
    raisicLexer::LexerResult* lexerResult = lexer->lex();
    if (!lexerResult->err.empty()) {
        return new CompResult(bytes, lexerResult->err);
    }

    for (raisicLexer::Token t : *lexerResult->tokens) {
        cout << t.AsString() << "\n";
    }

    return new CompResult(bytes, "");
}

}  // namespace raisic