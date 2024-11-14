#pragma once

#include <iostream>
#include <stack>
#include <vector>
using namespace std;

namespace raisinc {

namespace raisincLexer {
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
                Token(),
                this->error("unterminated comment", startln, startcol));
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

    pair<vector<raisincLexer::Token>, string> lex() {
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
                        return pair<vector<raisincLexer::Token>, string>(
                            vector<Token>(), res.second);
                    }
                    tokens.push_back(res.first);
                } break;

                case '#': {
                    this->advance();
                    Token res = this->collectIdent(Tokentypes::Macro, true);
                    if (res == "") {
                        return pair<vector<raisincLexer::Token>, string>(
                            vector<Token>(),
                            this->error("macro names cannot be empty"));
                    }
                    tokens.push_back(res);
                } break;

                default:
                    return pair<vector<raisincLexer::Token>, string>(
                        vector<Token>(), this->illegalCharacter());
            }
        }

        return pair<vector<raisincLexer::Token>, string>(tokens, "");
    }
};

}  // namespace raisincLexer

namespace raisincParser {
using raisinc::raisincLexer::Token, raisinc::raisincLexer::Tokentypes;

const string nodetypeStrings[] = {"CommandNode", "MacroDefinitionNode",
                                  "MacroCallNode"};

typedef enum Nodetype { Command, MacroDefinition, MacroCall } Nodetypes;

class Node {
   protected:
    const Nodetype type;

   public:
    Node(Nodetype type) : type(type) {}

    bool operator==(Nodetype t) { return this->type == t; }

    string asString() { return nodetypeStrings[this->type] + "{}"; }
};

class CommandNode : public Node {
   public:
    const Token command;

    CommandNode(Token command) : Node(Nodetypes::Command), command(command) {}

    string asString() {
        return nodetypeStrings[this->type] + "{ " +
               ((Token)this->command).asString() + " }";
    }
};

class MacroDefinitionNode : public Node {
   public:
    const Token name;
    const vector<Node*> contents;

    MacroDefinitionNode(Token name, vector<Node*> contents)
        : Node(Nodetypes::MacroDefinition), name(name), contents(contents) {}

    bool empty() { return this->contents.empty(); }

    string asString() {
        string s =
            nodetypeStrings[this->type] + "{ " + ((Token)this->name).asString();
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
   public:
    const Token name;

    MacroCallNode(Token name) : Node(Nodetypes::MacroCall), name(name) {}

    string asString() {
        return nodetypeStrings[this->type] + "{ " +
               ((Token)this->name).asString() + " }";
    }
};

class Parser {
   private:
    vector<Token> tokens;
    int idx;
    Token tok;

    void advance(int amount = 1) {
        this->idx += amount;
        this->tok =
            this->idx < this->tokens.size() ? this->tokens[this->idx] : Token();
    }

    Token peek() {
        if (this->idx + 1 < this->tokens.size())
            return this->tokens[this->idx + 1];
        return Token();
    }

    string error(string msg, Token t = Token()) {
        if (t == Tokentypes::Empty) t = this->tok;
        return "error on line " + to_string(t.getLn() + 1) + ", col " +
               to_string(t.getCol() + 1) + ": " + msg;
    }

    pair<MacroDefinitionNode*, string> collectMacroDefinition(Token name) {
        vector<Node*> macroContent = vector<Node*>();
        this->advance();
        Token openingBrace = this->peek();

        stack<Token> jmpAhead = stack<Token>();
        stack<Token> jmpBack = stack<Token>();

        this->advance();

        while (this->tok != Tokentypes::Empty &&
               this->tok != Tokentypes::ClosingBrace) {
            // cout << this->tok.asString() << "\n";
            if (this->tok == Tokentypes::Command) {
                if (this->tok == "[")
                    jmpAhead.push(this->tok);
                else if (this->tok == "]")
                    jmpBack.push(this->tok);

                macroContent.push_back((Node*)new CommandNode(this->tok));
                this->advance();
            } else if (this->tok == Tokentypes::Macro) {
                macroContent.push_back((Node*)new MacroCallNode(this->tok));
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

        this->advance();

        return pair<MacroDefinitionNode*, string>(
            new MacroDefinitionNode(name, macroContent), "");
    }

   public:
    Parser(vector<Token> tokens) {
        this->tokens = vector<Token>();
        for (Token t : tokens) {
            if (t != Tokentypes::Comment) this->tokens.push_back(t);
        }

        if (!tokens.empty()) {
            if (tokens.back() != Tokentypes::Empty) tokens.push_back(Token());
        }

        this->idx = -1;
        this->tok = Token();
        this->advance();
    }

    pair<vector<Node*>, string> parse() {
        vector<Node*> nodes = vector<Node*>();

        stack<Token> jmpAhead = stack<Token>();
        stack<Token> jmpBack = stack<Token>();

        while (this->tok != Tokentypes::Empty) {
            if (this->tok == Tokentypes::Command) {
                if (this->tok == "[")
                    jmpAhead.push(this->tok);
                else if (this->tok == "]")
                    jmpBack.push(this->tok);

                nodes.push_back((Node*)new CommandNode(this->tok));
                this->advance();
            } else if (this->tok == Tokentypes::Macro) {
                if (this->peek() == Tokentypes::OpeningBrace) {
                    auto r = this->collectMacroDefinition(this->tok);
                    if (!r.second.empty())
                        return pair<vector<Node*>, string>(vector<Node*>(),
                                                           r.second);
                    nodes.push_back((Node*)(r.first));
                } else {
                    nodes.push_back((Node*)new MacroCallNode(this->tok));
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
}  // namespace raisincParser

namespace raisincCodeGeneration {
using raisinc::raisincLexer::Token, raisinc::raisincParser::Node,
    raisinc::raisincParser::CommandNode,
    raisinc::raisincParser::MacroDefinitionNode,
    raisinc::raisincParser::MacroCallNode, raisinc::raisincParser::Nodetypes;

map<string, char> commandCodes = {{"+", 0}, {"-", 1}, {">", 2}, {"<", 3},
                                  {"[", 4}, {"]", 5}, {".", 6}, {",", 7},
                                  {"_", 8}, {"'", 9}, {"`", 10}};

pair<vector<char>, string> generateCode(vector<raisincParser::Node*> nodes) {
    auto macros = map<string, MacroDefinitionNode*>();
    auto bytes = vector<char>();

    for (raisincParser::Node* n : nodes) {
        if (*n == Nodetypes::MacroDefinition) {
            string* name = new string();
            *name = ((Token)((MacroDefinitionNode*)n)->name).getLit();
            if (macros.count(*name) == 1)
                return pair<vector<char>, string>(
                    vector<char>(), "cannot redefine macro '" + *name + "'");

            macros[*name] = (MacroDefinitionNode*)n;
            delete name;
        }
    }

    for (raisincParser::Node* n : nodes) {
        if (*n == Nodetypes::Command) {
            bytes.push_back(
                commandCodes[((Token)((MacroCallNode*)n)->name).getLit()]);
        } else if (*n == Nodetypes::MacroCall) {
            string* name = new string();
            *name = ((Token)((MacroCallNode*)n)->name).getLit();
            if (macros.count(*name) == 0)
                return pair<vector<char>, string>(
                    vector<char>(), "macro '" + *name + "' is undefined");

            auto r = generateCode(macros[*name]->contents);
            if (!r.second.empty())
                return pair<vector<char>, string>(vector<char>(), r.second);

            for (char b : r.first) bytes.push_back(b);

            delete name;
        }
    }

    return pair<vector<char>, string>(bytes, "");
}
}  // namespace raisincCodeGeneration

pair<vector<char>, string> Compile(string code, bool showTokens = false,
                                   bool showNodes = false) {
    raisincLexer::Lexer lexer(code);
    pair<vector<raisincLexer::Token>, string> lexerResult = lexer.lex();
    if (!lexerResult.second.empty()) {
        return pair<vector<char>, string>(vector<char>(), lexerResult.second);
    }

    if (showTokens) {
        for (raisincLexer::Token t : lexerResult.first)
            cout << t.asString() << "\n";
        if (showNodes) cout << "\n";
    }

    raisincParser::Parser parser(lexerResult.first);

    pair<vector<raisincParser::Node*>, string> parserResult = parser.parse();
    if (!lexerResult.second.empty())
        return pair<vector<char>, string>(vector<char>(), parserResult.second);

    if (showNodes) {
        for (raisincParser::Node* n : parserResult.first) {
            if (*n == raisincParser::Nodetypes::Command) {
                cout << ((raisincParser::CommandNode*)n)->asString() << "\n";
            } else if (*n == raisincParser::Nodetypes::MacroDefinition) {
                cout << ((raisincParser::MacroDefinitionNode*)n)->asString()
                     << "\n";
            } else if (*n == raisincParser::Nodetypes::MacroCall) {
                cout << ((raisincParser::MacroCallNode*)n)->asString() << "\n";
            } else {
                cout << n->asString() << "\n";
            }
        }
    }

    return raisincCodeGeneration::generateCode(parserResult.first);
}

}  // namespace raisinc