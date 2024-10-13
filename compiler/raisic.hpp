#pragma once

#include <iostream>
#include <stack>
#include <vector>
using namespace std;

namespace raisic {

namespace raisicLexer {
const string tokentypeStrings[] = {
    "Empty",           "Command",           "Comment",           "Macro",
    "BuiltinFunction", "OpeningParenthese", "ClosingParenthese", "OpeningBrace",
    "ClosingBrace"};

typedef enum Tokentype {
    Empty,
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

    Token() {
        this->type = Tokentypes::Empty;
        this->literal = "";
        this->start = -1;
        this->end = -1;
        this->ln = -1;
    }

    int getLn() { return this->ln; }

    int getCol() { return this->start; }

    string getLit() { return this->literal; }

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

    string stype() { return tokentypeStrings[this->type]; }

    string asString() {
        return "Token{ " + tokentypeStrings[this->type] + ", '" +
               this->literal + "', " + to_string(this->start) + ".." +
               to_string(this->end) + ", " + to_string(this->ln) + " }";
    }
};

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

    char peek() {
        return this->idx + 1 < this->text.size() ? this->text[this->idx + 1]
                                                 : -1;
    }

    string error(string msg, int ln = -1, int col = -1) {
        if (ln < 0) ln = this->ln;
        if (col < 0) col = this->col;
        return "error on line " + to_string(ln + 1) + ", col " +
               to_string(col + 1) + ": " + msg;
    }

    string illegalCharacter() {
        string s = "";
        return error(s + "illegal character '" + this->ch + "'");
    }

    Token chtoken(Tokentype type) {
        string s = " ";
        s[0] = this->ch;
        return Token(type, s, this->col, this->col, this->ln);
    }

    pair<Token, string> collectComment() {
        int start = this->idx;
        int startcol = this->col;
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

        if (this->ch != '/') {
            return pair<Token, string>(
                Token(), this->error("unterminated ", startln, startcol));
        }
        this->advance();

        while (com[com.size() - 1] == ' ' || com[com.size() - 1] == '\n' ||
               com[com.size() - 1] == '\t') {
            com.pop_back();
        }

        return pair<Token, string>(
            Token(Tokentypes::Comment, com, start, this->idx - 1, startln), "");
    }

    Token collectIdent(Tokentype returnAs, bool alreadyMoved = false) {
        int start = this->col;
        if (alreadyMoved) start--;
        int startln = this->ln;
        string ident = "";

        while (this->ch != -1 &&
               ((this->ch >= 'a' && this->ch <= 'z') ||
                (this->ch >= 'A' && this->ch <= 'Z') ||
                (this->ch >= '0' && this->ch <= '9') || this->ch == '_')) {
            ident += this->ch;
            this->advance();
        }

        return Token(returnAs, ident, start, this->col - 1, startln);
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

    pair<vector<raisicLexer::Token>, string> lex() {
        vector<Token> tokens = vector<Token>();

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
                    tokens.push_back(this->chtoken(Tokentypes::Command));
                    this->advance();
                    break;

                    /*
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
                    */

                case '{':
                    tokens.push_back(this->chtoken(Tokentypes::OpeningBrace));
                    this->advance();
                    break;

                case '}':
                    tokens.push_back(this->chtoken(Tokentypes::ClosingBrace));
                    this->advance();
                    break;

                case '/': {
                    pair<Token, string> res = this->collectComment();
                    if (!res.second.empty()) {
                        return pair<vector<raisicLexer::Token>, string>(
                            vector<Token>(), res.second);
                    }
                    tokens.push_back(res.first);
                } break;

                case '#': {
                    this->advance();
                    Token res = this->collectIdent(Tokentypes::Macro, true);
                    if (res == "") {
                        return pair<vector<raisicLexer::Token>, string>(
                            vector<Token>(),
                            this->error("macro names cannot be empty"));
                    }
                    tokens.push_back(res);
                } break;

                default:
                    return pair<vector<raisicLexer::Token>, string>(
                        vector<Token>(), this->illegalCharacter());
            }
        }

        return pair<vector<raisicLexer::Token>, string>(tokens, "");
    }
};

}  // namespace raisicLexer

namespace raisicParser {
using raisic::raisicLexer::Token, raisic::raisicLexer::Tokentypes;

const string nodetypeStrings[] = {"CommandNode", "MacroDefinitionNode",
                                  "MacroCallNode"};

typedef enum Nodetype { Command, MacroDefinition, MacroCall } Nodetypes;

class Node {
   protected:
    Nodetype type;

   public:
    Node(Nodetype type) { this->type = type; }

    bool operator==(Nodetype t) { return this->type == t; }

    string asString() { return nodetypeStrings[this->type] + "{}"; }
};

class CommandNode : public Node {
   private:
    Token command;

   public:
    CommandNode(Token command) : Node(Nodetypes::Command) {
        this->command = command;
    }

    string asString() {
        return nodetypeStrings[this->type] + "{ " + this->command.asString() +
               " }";
    }
};

class MacroDefinitionNode : public Node {
   private:
    Token name;
    vector<Node*> contents;

   public:
    MacroDefinitionNode(Token name, vector<Node*> contents)
        : Node(Nodetypes::MacroDefinition) {
        this->name = name;
        this->contents = contents;
    }

    bool empty() { return this->contents.empty(); }

    string asString() {
        string s = nodetypeStrings[this->type] + "{ " + this->name.asString();
        if (!this->contents.empty()) {
            s += ", ";
            for (Node* t : this->contents) {
                s += t->asString() + ", ";
            }
            s[s.size() - 2] = ' ';
            s[s.size() - 1] = '}';
        } else {
            s += " }";
        }
        return s;
    }
};

class MacroCallNode : public Node {
   private:
    Token name;

   public:
    MacroCallNode(Token name) : Node(Nodetypes::MacroCall) {
        this->name = name;
    }

    string asString() {
        return nodetypeStrings[this->type] + "{ " + this->name.asString() +
               " }";
    }
};

class Parser {
   private:
    vector<Token> tokens;
    int idx;
    Token tok;

    void advance(int amount = 1) {
        this->idx += amount;
        if (this->idx < this->tokens.size()) {
            tok = this->tokens[this->idx];
        } else {
            tok = *(new Token());
        }
    }

    Token peek() {
        if (this->idx + 1 < this->tokens.size()) {
            return this->tokens[this->idx + 1];
        }
        return *(new Token());
    }

    string error(string msg, Token t = *(new Token())) {
        if (t == Tokentypes::Empty) t = this->tok;
        return "error on line " + to_string(t.getLn() + 1) + ", col " +
               to_string(t.getCol() + 1) + ": " + msg;
    }

    pair<MacroDefinitionNode*, string> collectMacroDefinition(Token name) {
        vector<Node*> macroContent = vector<Node*>();
        Token openingBrace = this->peek();

        stack<Token> jmpAhead = stack<Token>();
        stack<Token> jmpBack = stack<Token>();

        while (this->tok != Tokentypes::Empty &&
               this->tok != Tokentypes::ClosingBrace) {
            if (this->tok == Tokentypes::Command) {
                if (this->tok == "[") {
                    jmpAhead.push(this->tok);
                } else if (this->tok == "]") {
                    jmpBack.push(this->tok);
                }

                macroContent.push_back(new CommandNode(this->tok));
                this->advance();
            } else if (this->tok == Tokentypes::Macro) {
                macroContent.push_back(new MacroCallNode(this->tok));
                this->advance();
            } else {
                return pair<MacroDefinitionNode*, string>(
                    new MacroDefinitionNode(Token(), vector<Node*>()),
                    this->error("unexpected token '" + this->tok.getLit() +
                                "'"));
            }
        }

        if (this->tok == Tokentypes::Empty) {
            return pair<MacroDefinitionNode*, string>(
                new MacroDefinitionNode(Token(), vector<Node*>()),
                this->error("no '}' to close '{'", openingBrace));
        } else if (jmpAhead.size() != jmpBack.size()) {
            string br = "[";
            Token t = jmpAhead.top();
            if (jmpBack.size() > jmpAhead.size()) {
                t = jmpBack.top();
                br = "]";
            }

            return pair<MacroDefinitionNode*, string>(
                new MacroDefinitionNode(Token(), vector<Node*>()),
                this->error("unmatched " + br, t));
        }

        return pair<MacroDefinitionNode*, string>(
            new MacroDefinitionNode(name, macroContent), "");
    }

   public:
    Parser(vector<Token> tokens) {
        this->tokens = vector<Token>();
        for (Token t : tokens) {
            if (t != Tokentypes::Comment) {
                this->tokens.push_back(t);
            }
        }
        this->advance();
    }

    pair<vector<Node*>, string> parse() {
        vector<Node*> nodes = vector<Node*>();

        stack<Token> jmpAhead = stack<Token>();
        stack<Token> jmpBack = stack<Token>();

        while (this->tok != Tokentypes::Empty) {
            cout << this->tok.asString() << "\n";
            if (this->tok == Tokentypes::Command) {
                if (this->tok == "[") {
                    jmpAhead.push(this->tok);
                } else if (this->tok == "]") {
                    jmpBack.push(this->tok);
                }

                nodes.push_back((Node*)new CommandNode(this->tok));
                this->advance();
            } else if (this->tok == Tokentypes::Macro) {
                if (this->peek() == Tokentypes::OpeningBrace) {
                    pair<MacroDefinitionNode*, string>* res =
                        (pair<MacroDefinitionNode*, string>*)malloc(
                            sizeof(pair<MacroDefinitionNode*, string>*));
                    *res = this->collectMacroDefinition(this->tok);
                    if (!res->second.empty()) {
                        return pair<vector<Node*>, string>(vector<Node*>(),
                                                           res->second);
                    }

                    if (!res->first->empty()) {
                        nodes.push_back((Node*)res->first);
                    }
                } else {
                    nodes.push_back(new MacroCallNode(this->tok));
                    this->advance();
                }
            } else {
                return pair<vector<Node*>, string>(
                    vector<Node*>(), this->error("unexpected token '" +
                                                 this->tok.getLit() + "'"));
            }
        }

        if (jmpAhead.size() != jmpBack.size()) {
            string br = "[";
            Token t = jmpAhead.top();
            if (jmpBack.size() > jmpAhead.size()) {
                t = jmpBack.top();
                br = "]";
            }

            return pair<vector<Node*>, string>(
                vector<Node*>(), this->error("unmatched " + br, t));
        }

        return pair<vector<Node*>, string>(nodes, "");
    }
};
}  // namespace raisicParser

namespace raisicCodeGeneration {
vector<char> generatonCode() {}
}  // namespace raisicCodeGeneration

pair<vector<char>, string> Compile(string code, bool showTokens = false,
                                   bool showNodes = false) {
    raisicLexer::Lexer lexer(code);
    pair<vector<raisicLexer::Token>, string> lexerResult = lexer.lex();
    if (!lexerResult.second.empty()) {
        return pair<vector<char>, string>(vector<char>(), lexerResult.second);
    }

    if (showTokens) {
        for (raisicLexer::Token t : lexerResult.first) {
            cout << t.asString() << "\n";
        }
    }

    raisicParser::Parser parser(lexerResult.first);

    pair<vector<raisicParser::Node*>, string> parserResult = parser.parse();
    if (!lexerResult.second.empty()) {
        return pair<vector<char>, string>(vector<char>(), parserResult.second);
    }

    if (showNodes) {
        for (raisicParser::Node* n : parserResult.first) {
            if (*n == raisicParser::Nodetypes::Command) {
                cout << ((raisicParser::CommandNode*)n)->asString() << "\n";
            } else if (*n == raisicParser::Nodetypes::MacroDefinition) {
                cout << ((raisicParser::MacroDefinitionNode*)n)->asString()
                     << "\n";
            } else if (*n == raisicParser::Nodetypes::MacroCall) {
                cout << ((raisicParser::MacroCallNode*)n)->asString() << "\n";
            } else {
                cout << n->asString() << "\n";
            }
        }
    }

    return pair<vector<char>, string>(vector<char>(), "");
}

}  // namespace raisic