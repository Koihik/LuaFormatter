#include <iostream>

#include "FormatVisitor.h"
#include "LuaLexer.h"

using namespace std;
using namespace antlr4;

// #define LOG_FLAG

#ifdef LOG_FLAG
#define LOGVAR(arg) cout << "" #arg " = " << (arg) << endl
#define LOG(arg) cout << arg << endl
#else
#define LOG(arg) ;
#define LOGVAR(arg) ;
#endif

#define SIMPLE_TABLE_THRESHOLD 50

bool _isBlankChar(char c) { return c == ' ' || c == '\t' || c == '\u000C'; }

bool _isLastWhiteSpace(stringstream& ss) {
    string s = ss.str();
    return s.size() > 0 && s[s.size() - 1] == ' ';
}

int _lastIdx(tree::ParseTree* t) {
    if (t == NULL) {
        return -1;
    }
    ParserRuleContext* c = dynamic_cast<ParserRuleContext*>(t);
    if (c != NULL) {
        return c->getStop()->getTokenIndex();
    }
    tree::TerminalNode* n = dynamic_cast<tree::TerminalNode*>(t);
    if (n != NULL) {
        return n->getSymbol()->getTokenIndex();
    }
    return INT_MAX;
}

// commentAfter returns comments between a ParseTreeNode and the next Node.
// expect: return expect string if no comments found
string FormatVisitor::commentAfter(tree::ParseTree* node, const string& expect) {
    int l = _lastIdx(node);
    stringstream ss;
    // No space on left of first comment
    bool lastComment = node != NULL;
    for (int i = l + 1; i < tokens.size(); i++) {
        auto token = tokens[i];
        if (token->getType() == LuaLexer::COMMENT) {
            if (lastComment) {
                ss << " ";
            }
            ss << token->getText();
            lastComment = true;
        } else if (token->getType() == LuaLexer::LINE_COMMENT) {
            if (lastComment) {
                ss << " ";
            }
            ss << formatLineComment(token);
            ss << indent();
            lastComment = false;
        } else if (token->getType() == LuaLexer::SHEBANG) {
            ss << token->getText() << "\n\n";
        } else if (token->getType() == LuaLexer::WS) {
        } else {
            break;
        }
    }
    if (!ss.tellp()) {
        ss << expect;
    } else if (lastComment) {
        ss << " ";
    }
    return ss.str();
}

// commentAfterNewLine returns comments between a ParseTreeNode and the next Node.
// This function always expect a line break.
// intdentSize: inc or dec indent after a line break.
string FormatVisitor::commentAfterNewLine(tree::ParseTree* node, int indentSize) {
    ParserRuleContext* ctx = dynamic_cast<ParserRuleContext*>(node);
    if (ctx != NULL && ctx->children.size() == 0) {
        _indent += indentSize;
        return "";
    }
    int l = _lastIdx(node);
    if (indentSize > 0) {
        _indent += indentSize;
    }
    stringstream ss;
    bool customNewLine = false;
    bool lastComment = true;
    bool lastestNewLine = false;
    for (int i = l + 1; i < tokens.size(); i++) {
        auto token = tokens[i];
        if (token->getType() == LuaLexer::COMMENT) {
            if (lastestNewLine) {
                ss << indent();
            } else if (lastComment) {
                ss << " ";
            }
            ss << token->getText();
            lastComment = true;
            lastestNewLine = false;
        } else if (token->getType() == LuaLexer::LINE_COMMENT) {
            if (lastestNewLine) {
                ss << indent();
            } else if (lastComment) {
                ss << " ";
            }
            ss << formatLineComment(token);
            customNewLine = true;
            lastComment = false;
            lastestNewLine = true;
        } else if (token->getType() == LuaLexer::WS) {
            int ln = token->getText().find("\n");
            int rn = token->getText().rfind("\n");
            if (ln != string::npos) {
                if (ln != rn && !lastestNewLine) {
                    ss << "\n\n";
                } else {
                    ss << "\n";
                }
                customNewLine = true;
                lastComment = false;
                lastestNewLine = true;
            }
        } else {
            break;
        }
    }
    if (!customNewLine) {
        ss << endl;
        lastestNewLine = true;
    }
    if (indentSize < 0) {
        _indent += indentSize;
    }
    if (lastComment && !lastestNewLine) {
        ss << "\n";
    }
    return ss.str();
}

// Add a white space before line comment
string FormatVisitor::formatLineComment(Token* token) {
    string comment = token->getText();
    char firstChar = comment[2];
    if (_isBlankChar(firstChar) || firstChar == '-' || firstChar == '\r' || firstChar == '\n') {
        // do nothing
    } else {
        comment = "-- " + comment.substr(2, comment.size());
    }
    return comment;
}

antlrcpp::Any FormatVisitor::visitChunk(LuaParser::ChunkContext* ctx) {
    LOG("visitChunk");
    stringstream ss;
    ss << commentAfter(NULL, "")  //
       << visitBlock(ctx->block()).as<string>();
    string comment = commentAfterNewLine(ctx->block(), 0);

    ss << comment;
    // If there is no line break at the end of the file, add one
    for (int i = comment.size() - 1; i >= 0; i--) {
        if (_isBlankChar(comment[i])) {
            continue;
        }
        if (comment[i] != '\n') {
            ss << "\n";
        }
        break;
    }
    return ss.str();
}

// stat* retstat?
antlrcpp::Any FormatVisitor::visitBlock(LuaParser::BlockContext* ctx) {
    LOG("visitBlock");
    stringstream ss;
    auto stats = ctx->stat();
    int n = stats.size();
    for (int i = 0; i < n; i++) {
        bool isSemi = stats[i]->SEMI() != NULL;
        bool needComment = i < n - 1 || ctx->retstat() != NULL;
        bool isNextSemi = i + 1 < n && stats[i + 1]->SEMI() != NULL;
        if (!isSemi) {
            ss << visitStat(stats[i]).as<string>();
        }
        bool previousWhiteSpace = isSemi || _isLastWhiteSpace(ss);
        if (needComment) {
            if (isNextSemi) {
                string comment = commentAfter(stats[i], "");
                if (previousWhiteSpace && comment.size() > 0 && comment[0] == ' ') {
                    ss << comment.substr(1, comment.size() - 1);
                } else {
                    ss << comment;
                }
            } else {
                string comment = commentAfterNewLine(stats[i], 0);
                if (previousWhiteSpace && comment.size() > 0 && comment[0] == ' ') {
                    ss << comment.substr(1, comment.size() - 1);
                } else {
                    ss << comment;
                }
            }
        }
    }
    if (ctx->retstat() != NULL) {
        ss << visitRetstat(ctx->retstat()).as<string>();
    }
    if (ctx->retstat() == NULL && _isLastWhiteSpace(ss)) {
        string blockComment = commentAfter(ctx, "");
        if (blockComment.size() > 0 && blockComment[0] == ' ') {
            string ret = ss.str();
            return ret.substr(0, ret.size() - 1);
        }
    }
    return ss.str();
}

antlrcpp::Any FormatVisitor::visitStat(LuaParser::StatContext* ctx) {
    LOG("visitStat");
    stringstream ss;
    ss << indent();
    ss << visitChildren(ctx).as<string>();
    return ss.str();
}

// varlist EQL explist;
antlrcpp::Any FormatVisitor::visitVarDecl(LuaParser::VarDeclContext* ctx) {
    LOG("visitVarDecl");
    stringstream ss;
    ss << visitVarlist(ctx->varlist()).as<string>()  //
       << commentAfter(ctx->varlist(), " ")          //
       << ctx->EQL()->getText()                      //
       << commentAfter(ctx->EQL(), " ")              //
       << visitExplist(ctx->explist()).as<string>();
    if (ctx->SEMI() != NULL) {
        ss << commentAfter(ctx->explist(), "");
        if (shouldKeepSemicolon(ctx->explist(), ctx->SEMI())) {
            ss << ctx->SEMI()->getText();
        }
    }
    return ss.str();
}

// varOrExp nameAndArgs+;
antlrcpp::Any FormatVisitor::visitFunctioncall(LuaParser::FunctioncallContext* ctx) {
    LOG("visitFunctioncall");
    stringstream ss;
    ss << visitVarOrExp(ctx->varOrExp()).as<string>()  //
       << commentAfter(ctx->varOrExp(), "");

    int n = ctx->nameAndArgs().size();
    for (int i = 0; i < n; i++) {
        ss << visitNameAndArgs(ctx->nameAndArgs()[i]).as<string>();
        if (i != n - 1) {
            ss << commentAfter(ctx->nameAndArgs()[i], "");
        }
    }
    if (ctx->SEMI() != NULL) {
        ss << commentAfter(ctx->nameAndArgs().back(), "");
        if (shouldKeepSemicolon(ctx->nameAndArgs().back(), ctx->SEMI())) {
            ss << ctx->SEMI()->getText();
        }
    }
    return ss.str();
}

// GOTO NAME;
antlrcpp::Any FormatVisitor::visitGotoStat(LuaParser::GotoStatContext* ctx) {
    LOG("visitGotoStat");
    stringstream ss;
    ss << ctx->GOTO()->getText()          //
       << commentAfter(ctx->GOTO(), " ")  //
       << ctx->NAME()->getText();
    return ss.str();
}

// DO block END;
antlrcpp::Any FormatVisitor::visitDoStat(LuaParser::DoStatContext* ctx) {
    LOG("visitDoStat");
    stringstream ss;
    ss << ctx->DO()->getText();                   //
    ss << commentAfterNewLine(ctx->DO(), 1);      //
    ss << visitBlock(ctx->block()).as<string>();  //
    ss << commentAfterNewLine(ctx->block(), -1);  //
    ss << indent()                                //
       << ctx->END()->getText();
    return ss.str();
}

// WHILE exp DO block END;
antlrcpp::Any FormatVisitor::visitWhileStat(LuaParser::WhileStatContext* ctx) {
    LOG("visitWhileStat");
    stringstream ss;
    ss << ctx->WHILE()->getText()                 //
       << commentAfter(ctx->WHILE(), " ")         //
       << visitExp(ctx->exp()).as<string>()       //
       << commentAfter(ctx->exp(), " ")           //
       << ctx->DO()->getText();                   //
    ss << commentAfterNewLine(ctx->DO(), 1);      //
    ss << visitBlock(ctx->block()).as<string>();  //
    ss << commentAfterNewLine(ctx->block(), -1);  //
    ss << indent()                                //
       << ctx->END()->getText();
    return ss.str();
}

// REPEAT block UNTIL exp;
antlrcpp::Any FormatVisitor::visitRepeatStat(LuaParser::RepeatStatContext* ctx) {
    LOG("visitRepeatStat");
    stringstream ss;
    ss << ctx->REPEAT()->getText();               //
    ss << commentAfterNewLine(ctx->REPEAT(), 1);  //
    ss << visitBlock(ctx->block()).as<string>();  //
    ss << commentAfterNewLine(ctx->block(), -1);  //
    ss << indent()                                //
       << ctx->UNTIL()->getText()                 //
       << commentAfter(ctx->UNTIL(), " ")         //
       << visitExp(ctx->exp()).as<string>();      //
    if (ctx->SEMI() != NULL) {
        ss << commentAfter(ctx->exp(), "");
        if (shouldKeepSemicolon(ctx->exp(), ctx->SEMI())) {
            ss << ctx->SEMI()->getText();
        }
    }
    return ss.str();
}

// IF exp THEN block (ELSEIF exp THEN block)* (ELSE block)? END;
antlrcpp::Any FormatVisitor::visitIfStat(LuaParser::IfStatContext* ctx) {
    LOG("visitIfStat");
    stringstream ss;
    ss << ctx->IF()->getText()                            //
       << commentAfter(ctx->IF(), " ")                    //
       << visitExp(ctx->exp().front()).as<string>()       //
       << commentAfter(ctx->exp().front(), " ")           //
       << ctx->THEN().front()->getText();                 //
    ss << commentAfterNewLine(ctx->THEN().front(), 1);    //
    ss << visitBlock(ctx->block().front()).as<string>();  //
    ss << commentAfterNewLine(ctx->block().front(), -1);
    int n = ctx->ELSEIF().size();
    for (int i = 0; i < n; i++) {
        ss << indent()                                       //
           << ctx->ELSEIF()[i]->getText()                    //
           << commentAfter(ctx->ELSEIF()[i], " ")            //
           << visitExp(ctx->exp()[i + 1]).as<string>()       //
           << commentAfter(ctx->exp()[i + 1], " ")           //
           << ctx->THEN()[i + 1]->getText();                 //
        ss << commentAfterNewLine(ctx->THEN()[i + 1], 1);    //
        ss << visitBlock(ctx->block()[i + 1]).as<string>();  //
        ss << commentAfterNewLine(ctx->block()[i + 1], -1);
    }
    if (ctx->ELSE() != NULL) {
        ss << indent()                                       //
           << ctx->ELSE()->getText();                        //
        ss << commentAfterNewLine(ctx->ELSE(), 1);           //
        ss << visitBlock(ctx->block().back()).as<string>();  //
        ss << commentAfterNewLine(ctx->block().back(), -1);
    }
    ss << indent()  //
       << ctx->END()->getText();
    return ss.str();
}

// FOR NAME EQL exp COMMA exp (COMMA exp)? DO block END;
antlrcpp::Any FormatVisitor::visitForStat(LuaParser::ForStatContext* ctx) {
    LOG("visitForStat");
    stringstream ss;
    ss << ctx->FOR()->getText()                  //
       << commentAfter(ctx->FOR(), " ")          //
       << ctx->NAME()->getText()                 //
       << commentAfter(ctx->NAME(), " ")         //
       << ctx->EQL()->getText()                  //
       << commentAfter(ctx->EQL(), " ")          //
       << visitExp(ctx->exp()[0]).as<string>()   //
       << commentAfter(ctx->exp()[0], "")        //
       << ctx->COMMA()[0]->getText()             //
       << commentAfter(ctx->COMMA()[0], " ")     //
       << visitExp(ctx->exp()[1]).as<string>();  //
    if (ctx->COMMA().size() > 1) {
        ss << commentAfter(ctx->exp()[1], "")       //
           << ctx->COMMA()[1]->getText()            //
           << commentAfter(ctx->COMMA()[1], " ")    //
           << visitExp(ctx->exp()[2]).as<string>()  //
           << commentAfter(ctx->exp()[2], " ");     //
    } else {
        ss << commentAfter(ctx->exp()[1], " ");
    }
    ss << ctx->DO()->getText();                   //
    ss << commentAfterNewLine(ctx->DO(), 1);      //
    ss << visitBlock(ctx->block()).as<string>();  //
    ss << commentAfterNewLine(ctx->block(), -1);  //
    ss << indent()                                //
       << ctx->END()->getText();

    return ss.str();
}

// FOR namelist IN explist DO block END;
antlrcpp::Any FormatVisitor::visitForInStat(LuaParser::ForInStatContext* ctx) {
    LOG("visitForInStat");
    stringstream ss;
    ss << ctx->FOR()->getText()                        //
       << commentAfter(ctx->FOR(), " ")                //
       << visitNamelist(ctx->namelist()).as<string>()  //
       << commentAfter(ctx->namelist(), " ")           //
       << ctx->IN()->getText()                         //
       << commentAfter(ctx->IN(), " ")                 //
       << visitExplist(ctx->explist()).as<string>()    //
       << commentAfter(ctx->explist(), " ")            //
       << ctx->DO()->getText();                        //
    ss << commentAfterNewLine(ctx->DO(), 1);           //
    ss << visitBlock(ctx->block()).as<string>();       //
    ss << commentAfterNewLine(ctx->block(), -1);       //
    ss << indent()                                     //
       << ctx->END()->getText();

    return ss.str();
}

// FUNCTION funcname funcbody;
antlrcpp::Any FormatVisitor::visitFuncStat(LuaParser::FuncStatContext* ctx) {
    LOG("visitFuncStat");
    stringstream ss;
    ss << ctx->FUNCTION()->getText()                   //
       << commentAfter(ctx->FUNCTION(), " ")           //
       << visitFuncname(ctx->funcname()).as<string>()  //
       << commentAfter(ctx->funcname(), "")            //
       << visitFuncbody(ctx->funcbody()).as<string>();
    return ss.str();
}

// LOCAL FUNCTION NAME funcbody;
antlrcpp::Any FormatVisitor::visitLocalFuncStat(LuaParser::LocalFuncStatContext* ctx) {
    LOG("visitLocalFuncStat");
    stringstream ss;
    ss << ctx->LOCAL()->getText()             //
       << commentAfter(ctx->LOCAL(), " ")     //
       << ctx->FUNCTION()->getText()          //
       << commentAfter(ctx->FUNCTION(), " ")  //
       << ctx->NAME()->getText()              //
       << commentAfter(ctx->NAME(), "")       //
       << visitFuncbody(ctx->funcbody()).as<string>();
    return ss.str();
}

// LOCAL namelist (EQL explist)? SEMI?;
antlrcpp::Any FormatVisitor::visitLocalVarDecl(LuaParser::LocalVarDeclContext* ctx) {
    LOG("visitLocalVarDecl");
    stringstream ss;
    ss << ctx->LOCAL()->getText();                      //
    ss << commentAfter(ctx->LOCAL(), " ");              //
    ss << visitNamelist(ctx->namelist()).as<string>();  //
    if (ctx->EQL() != NULL) {
        ss << commentAfter(ctx->namelist(), " ")  //
           << ctx->EQL()->getText()               //
           << commentAfter(ctx->EQL(), " ")       //
           << visitExplist(ctx->explist()).as<string>();
    }
    if (ctx->SEMI() != NULL) {
        if (ctx->EQL() == NULL) {
            ss << commentAfter(ctx->namelist(), "");
        } else {
            ss << commentAfter(ctx->explist(), "");
        }
        if (shouldKeepSemicolon(ctx->explist(), ctx->SEMI())) {
            ss << ctx->SEMI()->getText();
        }
    }
    return ss.str();
}

// RETURN explist? SEMI?;
antlrcpp::Any FormatVisitor::visitRetstat(LuaParser::RetstatContext* ctx) {
    LOG("visitRetstat");
    stringstream ss;
    ss << indent();
    ss << ctx->RETURN()->getText();
    if (ctx->explist() != NULL) {
        ss << commentAfter(ctx->RETURN(), " ")            //
           << visitExplist(ctx->explist()).as<string>();  //
    }
    return ss.str();
}

// var (COMMA var)*;
antlrcpp::Any FormatVisitor::visitVarlist(LuaParser::VarlistContext* ctx) {
    LOG("visitVarlist");
    stringstream ss;
    ss << visitVar(ctx->var().front()).as<string>();
    int n = ctx->COMMA().size();
    if (n > 0) {
        ss << commentAfter(ctx->var().front(), "");
    }
    for (int i = 0; i < n; i++) {
        ss << ctx->COMMA()[i]->getText()          //
           << commentAfter(ctx->COMMA()[i], " ")  //
           << visitVar(ctx->var()[i + 1]).as<string>();
        if (i != n - 1) {
            ss << commentAfter(ctx->var()[i + 1], "");
        }
    }
    return ss.str();
}

// NAME (COMMA NAME)*;
antlrcpp::Any FormatVisitor::visitNamelist(LuaParser::NamelistContext* ctx) {
    LOG("visitNamelist");
    stringstream ss;
    ss << ctx->NAME().front()->getText();
    int n = ctx->COMMA().size();
    if (n > 0) {
        ss << commentAfter(ctx->NAME().front(), "");
    }
    for (int i = 0; i < n; i++) {
        ss << ctx->COMMA()[i]->getText()          //
           << commentAfter(ctx->COMMA()[i], " ")  //
           << ctx->NAME()[i + 1]->getText();
        if (i != n - 1) {
            ss << commentAfter(ctx->NAME()[i + 1], "");
        }
    }
    return ss.str();
}

// exp (COMMA exp)*;
antlrcpp::Any FormatVisitor::visitExplist(LuaParser::ExplistContext* ctx) {
    LOG("visitExplist");
    stringstream ss;
    ss << visitExp(ctx->exp().front()).as<string>();
    int n = ctx->COMMA().size();
    if (n > 0) {
        ss << commentAfter(ctx->exp().front(), "");
    }
    for (int i = 0; i < n; i++) {
        ss << ctx->COMMA()[i]->getText()          //
           << commentAfter(ctx->COMMA()[i], " ")  //
           << visitExp(ctx->exp()[i + 1]).as<string>();
        if (i != n - 1) {
            ss << commentAfter(ctx->exp()[i + 1], "");
        }
    }
    return ss.str();
}

// exp:
// 	NIL
// 	| FALSE
// 	| TRUE
// 	| number
// 	| string
// 	| ELLIPSIS
// 	| prefixexp
// 	| functiondef
// 	| tableconstructor
// 	| exp linkOperator exp
// 	| unaryOperator exp;

antlrcpp::Any FormatVisitor::visitExp(LuaParser::ExpContext* ctx) {
    LOG("visitExp");
    if (ctx->linkOperator() != NULL) {
        stringstream ss;
        ss << visitExp(ctx->exp()[0]).as<string>()    //
           << commentAfter(ctx->exp()[0], " ")        //
           << ctx->linkOperator()->getText()          //
           << commentAfter(ctx->linkOperator(), " ")  //
           << visitExp(ctx->exp()[1]).as<string>();
        return ss.str();
    } else if (ctx->unaryOperator() != NULL) {
        stringstream ss;
        ss << ctx->unaryOperator()->getText();
        if (ctx->unaryOperator()->getText() == "not") {
            ss << commentAfter(ctx->unaryOperator(), " ");
        } else {
            if (ctx->unaryOperator()->getText() == "-" && ctx->exp().front()->getText()[0] == '-') {
                ss << commentAfter(ctx->unaryOperator(), " ");
            } else {
                ss << commentAfter(ctx->unaryOperator(), "");
            }
        }
        ss << visitExp(ctx->exp().front()).as<string>();
        return ss.str();
    } else if (ctx->prefixexp() != NULL) {
        return visitPrefixexp(ctx->prefixexp()).as<string>();
    } else if (ctx->functiondef() != NULL) {
        return visitFunctiondef(ctx->functiondef()).as<string>();
    } else if (ctx->tableconstructor() != NULL) {
        return visitTableconstructor(ctx->tableconstructor()).as<string>();
    } else {
        return ctx->getText();
    }
}

// varOrExp nameAndArgs*;
antlrcpp::Any FormatVisitor::visitPrefixexp(LuaParser::PrefixexpContext* ctx) {
    LOG("visitPrefixexp");
    stringstream ss;
    ss << visitVarOrExp(ctx->varOrExp()).as<string>();
    int n = ctx->nameAndArgs().size();
    if (n > 0) {
        ss << commentAfter(ctx->varOrExp(), "");
    }
    for (int i = 0; i < n; i++) {
        ss << visitNameAndArgs(ctx->nameAndArgs()[i]).as<string>();
        if (i != n - 1) {
            ss << commentAfter(ctx->nameAndArgs()[i], "");
        }
    }
    return ss.str();
}

// var | LP exp RP;
antlrcpp::Any FormatVisitor::visitVarOrExp(LuaParser::VarOrExpContext* ctx) {
    LOG("visitVarOrExp");
    if (ctx->exp() != NULL) {
        stringstream ss;
        ss << ctx->LP()->getText()               //
           << commentAfter(ctx->LP(), "")        //
           << visitExp(ctx->exp()).as<string>()  //
           << commentAfter(ctx->exp(), "")       //
           << ctx->RP()->getText();
        return ss.str();
    } else {
        return visitVar(ctx->var()).as<string>();
    }
}

// (NAME | LP exp RP varSuffix) varSuffix*;
antlrcpp::Any FormatVisitor::visitVar(LuaParser::VarContext* ctx) {
    LOG("visitVar");
    stringstream ss;
    int startAt;
    if (ctx->exp() != NULL) {
        ss << ctx->LP()->getText()               //
           << commentAfter(ctx->LP(), "")        //
           << visitExp(ctx->exp()).as<string>()  //
           << commentAfter(ctx->exp(), "")       //
           << ctx->RP()->getText()               //
           << commentAfter(ctx->RP(), "")        //
           << visitVarSuffix(ctx->varSuffix().front()).as<string>();
        if (ctx->varSuffix().size() > 1) {
            ss << commentAfter(ctx->varSuffix().front(), "");
        }
        startAt = 1;
    } else {
        ss << ctx->NAME()->getText();
        if (ctx->varSuffix().size() > 0) {
            ss << commentAfter(ctx->NAME(), "");
        }
        startAt = 0;
    }
    int n = ctx->varSuffix().size();
    for (int i = startAt; i < n; i++) {
        ss << visitVarSuffix(ctx->varSuffix()[i]).as<string>();
        if (i != n - 1) {
            ss << commentAfter(ctx->varSuffix()[i], "");
        }
    }
    return ss.str();
}

// nameAndArgs* (LSB exp RSB | DOT NAME);
antlrcpp::Any FormatVisitor::visitVarSuffix(LuaParser::VarSuffixContext* ctx) {
    LOG("visitVarSuffix");
    stringstream ss;
    for (auto na : ctx->nameAndArgs()) {
        ss << visitNameAndArgs(na).as<string>()  //
           << commentAfter(na, "");
    }
    if (ctx->exp() != NULL) {
        string expString = ctx->exp()->getText();
        // if table key is a nested string, keep the whitespace
        // example:
        // x = {}
        // x[ [[key]] ] = "value"
        if (expString.size() > 0 && expString[0] == '[') {
            ss << ctx->LSB()->getText()              //
               << commentAfter(ctx->LSB(), " ")      //
               << visitExp(ctx->exp()).as<string>()  //
               << commentAfter(ctx->exp(), " ")      //
               << ctx->RSB()->getText();
        } else {
            ss << ctx->LSB()->getText()              //
               << commentAfter(ctx->LSB(), "")       //
               << visitExp(ctx->exp()).as<string>()  //
               << commentAfter(ctx->exp(), "")       //
               << ctx->RSB()->getText();
        }
    } else {
        ss << ctx->DOT()->getText()         //
           << commentAfter(ctx->DOT(), "")  //
           << ctx->NAME()->getText();
    }
    return ss.str();
}

// (COLON NAME)? args;
antlrcpp::Any FormatVisitor::visitNameAndArgs(LuaParser::NameAndArgsContext* ctx) {
    LOG("visitNameAndArgs");
    if (ctx->COLON() != NULL) {
        stringstream ss;
        ss << ctx->COLON()->getText()         //
           << commentAfter(ctx->COLON(), "")  //
           << ctx->NAME()->getText()          //
           << commentAfter(ctx->NAME(), "")   //
           << visitArgs(ctx->args()).as<string>();
        return ss.str();
    } else {
        return visitArgs(ctx->args()).as<string>();
    }
}

// LP explist? RP | tableconstructor | string;
antlrcpp::Any FormatVisitor::visitArgs(LuaParser::ArgsContext* ctx) {
    LOG("visitArgs");
    if (ctx->LP() != NULL) {
        stringstream ss;
        ss << ctx->LP()->getText()  //
           << commentAfter(ctx->LP(), "");
        if (ctx->explist() != NULL) {
            ss << visitExplist(ctx->explist()).as<string>()  //
               << commentAfter(ctx->explist(), "");
        }
        ss << ctx->RP()->getText();
        return ss.str();
    } else {
        return visitChildren(ctx);
    }
}

// FUNCTION funcbody;
antlrcpp::Any FormatVisitor::visitFunctiondef(LuaParser::FunctiondefContext* ctx) {
    LOG("visitFunctiondef");
    stringstream ss;
    ss << ctx->FUNCTION()->getText()         //
       << commentAfter(ctx->FUNCTION(), "")  //
       << visitFuncbody(ctx->funcbody()).as<string>();
    return ss.str();
}

// LP parlist? RP block END
antlrcpp::Any FormatVisitor::visitFuncbody(LuaParser::FuncbodyContext* ctx) {
    LOG("visitFuncbody");
    stringstream ss;
    ss << ctx->LP()->getText()          //
       << commentAfter(ctx->LP(), "");  //
    if (ctx->parlist() != NULL) {
        ss << visitParlist(ctx->parlist()).as<string>()  //
           << commentAfter(ctx->parlist(), "");          //
    }
    if (config.keep_simple_function_one_line() && isFunctionSimple(ctx)) {
        if (ctx->block()->stat().size() == 0 && ctx->block()->retstat() == NULL) {
            ss << ctx->RP()->getText()          //
               << commentAfter(ctx->RP(), " ")  //
               << ctx->END()->getText();        //
        } else {
            ss << ctx->RP()->getText()           //
               << commentAfter(ctx->RP(), " ");  //

            // ignore indent when function is a single line
            int temp = _indent;
            _indent = 0;
            ss << visitBlock(ctx->block()).as<string>();
            _indent = temp;
            ss << commentAfter(ctx->block(), " ")  //
               << ctx->END()->getText();           //
        }
        return ss.str();
    }
    ss << ctx->RP()->getText();                   //
    ss << commentAfterNewLine(ctx->RP(), 1);      //
    ss << visitBlock(ctx->block()).as<string>();  //
    ss << commentAfterNewLine(ctx->block(), -1);  //
    ss << indent()                                //
       << ctx->END()->getText();                  //
    return ss.str();
}

// namelist (COMMA ELLIPSIS)? | ELLIPSIS
antlrcpp::Any FormatVisitor::visitParlist(LuaParser::ParlistContext* ctx) {
    LOG("visitParlist");
    stringstream ss;
    if (ctx->namelist() != NULL) {
        ss << visitNamelist(ctx->namelist()).as<string>();  //
        if (ctx->COMMA() != NULL) {
            ss << commentAfter(ctx->namelist(), "")  //
               << ctx->COMMA()->getText()            //
               << commentAfter(ctx->COMMA(), " ")    //
               << ctx->ELLIPSIS()->getText();
        }
    } else {
        ss << ctx->ELLIPSIS()->getText();
    }

    return ss.str();
}

// LB fieldlist? RB;
antlrcpp::Any FormatVisitor::visitTableconstructor(LuaParser::TableconstructorContext* ctx) {
    LOG("visitTableconstructor");
    stringstream ss;
    ss << ctx->LB()->getText();
    if (ctx->fieldlist() != NULL) {
        if (config.keep_simple_table_one_line() && isTableSimple(ctx)) {
            ss << commentAfter(ctx->LB(), " ");  //
            int temp = _indent;
            _indent = 0;
            ss << visitFieldlist(ctx->fieldlist()).as<string>();  //
            _indent = temp;
            ss << commentAfter(ctx->fieldlist(), " ");  //
            ss << ctx->RB()->getText();
        } else {
            ss << commentAfterNewLine(ctx->LB(), 1);              //
            ss << visitFieldlist(ctx->fieldlist()).as<string>();  //
            if (config.extra_sep_at_table_end() && ctx->fieldlist()->field().size() > 0) {
                ss << config.table_sep();
            }
            ss << commentAfterNewLine(ctx->fieldlist(), -1);  //
            ss << indent();                                   //
            ss << ctx->RB()->getText();
        }
    } else {
        string comment = commentAfter(ctx->LB(), "");
        // if comment contains line break, then keep it.
        if (comment.find("\n") != string::npos) {
            ss << commentAfterNewLine(ctx->LB(), 1);  //
            ss << decIndent();                        //
            ss << ctx->RB()->getText();
        } else {
            // comment not contains line break, format the table to one line.
            ss << commentAfter(ctx->LB(), "")  //
               << ctx->RB()->getText();
        }
    }
    return ss.str();
}

// field (fieldsep field)* fieldsep?;
antlrcpp::Any FormatVisitor::visitFieldlist(LuaParser::FieldlistContext* ctx) {
    LOG("visitFieldlist");
    stringstream ss;
    auto fields = ctx->field();
    auto seps = ctx->fieldsep();
    ss << visitField(fields.front()).as<string>();
    if (fields.size() > 1 || seps.size() > 0) {
        ss << commentAfter(fields.front(), "");
        int n = fields.size();
        for (int i = 1; i < n; i++) {
            ss << visitFieldsep(seps[i - 1]).as<string>();
            ss << commentAfterNewLine(seps[i - 1], 0);
            ss << visitField(fields[i]).as<string>();
            if (i != n - 1 || seps.size() == n) {
                ss << commentAfter(fields[i], "");
            }
        }
        if (seps.size() == n) {
            ss << visitFieldsep(seps.back()).as<string>();
        }
    }

    return ss.str();
}

// LSB exp RSB EQL exp | NAME EQL exp | exp;
antlrcpp::Any FormatVisitor::visitField(LuaParser::FieldContext* ctx) {
    stringstream ss;
    if (ctx->LSB() != NULL) {
        ss << indent()                               //
           << ctx->LSB()->getText()                  //
           << commentAfter(ctx->LSB(), "")           //
           << visitExp(ctx->exp()[0]).as<string>()   //
           << commentAfter(ctx->exp()[0], "")        //
           << ctx->RSB()->getText()                  //
           << commentAfter(ctx->RSB(), " ")          //
           << ctx->EQL()->getText()                  //
           << commentAfter(ctx->EQL(), " ")          //
           << visitExp(ctx->exp()[1]).as<string>();  //
    } else if (ctx->NAME() != NULL) {
        ss << indent()                                    //
           << ctx->NAME()->getText()                      //
           << commentAfter(ctx->NAME(), " ")              //
           << ctx->EQL()->getText()                       //
           << commentAfter(ctx->EQL(), " ")               //
           << visitExp(ctx->exp().front()).as<string>();  //
    } else {
        ss << indent()  //
           << visitExp(ctx->exp().front()).as<string>();
    }
    return ss.str();
}

antlrcpp::Any FormatVisitor::visitFieldsep(LuaParser::FieldsepContext* context) { return config.table_sep(); }

antlrcpp::Any FormatVisitor::visitChildren(tree::ParseTree* node) {
    stringstream ss;
    size_t n = node->children.size();
    for (size_t i = 0; i < n; i++) {
        string str = node->children[i]->accept(this);
        ss << str;
    }
    return ss.str();
}

antlrcpp::Any FormatVisitor::visitTerminal(tree::TerminalNode* node) { return node->getText(); }

// If both of the following conditions are met, the custom semicolon should not be deleted:
// 1. There is a variable token or function call token at end of previous statement
//    e.g. 'local x = y' or 'y:func()'
// 2. next statement is start with a '(', like '(function() end)()'
bool FormatVisitor::shouldKeepSemicolon(ParserRuleContext* ctx, tree::TerminalNode* node) {
    if (ctx == NULL) {
        return false;
    }

    bool returnVar = false;

    // varDecl or localVarDecl
    // 'local x, y = 1, print;'
    LuaParser::ExplistContext* explistCtx = dynamic_cast<LuaParser::ExplistContext*>(ctx);
    if (explistCtx != NULL) {
        LuaParser::ExpContext* lastExp = explistCtx->exp().back();
        while (!lastExp->exp().empty()) {
            lastExp = lastExp->exp().back();
        }
        returnVar = lastExp->prefixexp() != NULL;
    }

    // repeat statement
    // repeat
    //     print("123")
    // until y;
    LuaParser::ExpContext* expCtx = dynamic_cast<LuaParser::ExpContext*>(ctx);
    if (expCtx != NULL) {
        LuaParser::ExpContext* lastExp = expCtx;
        while (!lastExp->exp().empty()) {
            lastExp = lastExp->exp().back();
        }
        returnVar = lastExp->prefixexp() != NULL;
    }

    // function call
    LuaParser::NameAndArgsContext* naCtx = dynamic_cast<LuaParser::NameAndArgsContext*>(ctx);
    if (naCtx != NULL) {
        returnVar = true;
    }

    if (!returnVar) {
        return false;
    }

    int idx = node->getSymbol()->getTokenIndex();
    bool startWithLP = false;
    for (int i = idx + 1; i < tokens.size(); i++) {
        size_t type = tokens[i]->getType();
        // ignore comments and white space
        if (type == LuaLexer::COMMENT ||       //
            type == LuaLexer::LINE_COMMENT ||  //
            type == LuaLexer::WS) {
            continue;
        }

        startWithLP = type == LuaLexer::LP;
        break;
    }
    return startWithLP;
}

// Judge a function is 'simple function' or not
bool FormatVisitor::isFunctionSimple(LuaParser::FuncbodyContext* ctx) {
    int stats = 0;
    for (auto& s : ctx->block()->stat()) {
        if (s->SEMI() == NULL) {
            stats++;
        }
    }
    if (ctx->block()->retstat() != NULL) {
        stats++;
    }
    // 1. function contains more than one statement.
    if (stats > 1) {
        return false;
    }
    // 2. function contains a line break.
    string preComment = commentAfter(ctx->RP(), "");
    if (preComment.find("\n") != string::npos) {
        return false;
    }
    string blockContent = visitBlock(ctx->block()).as<string>();
    if (blockContent.find("\n") != string::npos) {
        return false;
    }
    string postComment = commentAfter(ctx->block(), "");
    if (postComment.find("\n") != string::npos) {
        return false;
    }
    return true;
}

// Judge a table is 'simple table' or not
bool FormatVisitor::isTableSimple(LuaParser::TableconstructorContext* ctx) {
    // 1. table has more than one field.
    if (ctx->fieldlist()->field().size() > 1) {
        return false;
    }
    // 2. key-value table.
    if (ctx->fieldlist()->field().front()->EQL() != NULL) {
        return false;
    }
    // 3. table contains line break.
    string preComment = commentAfter(ctx->LB(), "");
    if (preComment.find("\n") != string::npos) {
        return false;
    }
    string fieldContent = visitFieldlist(ctx->fieldlist()).as<string>();
    if (fieldContent.find("\n") != string::npos) {
        return false;
    }
    string postComment = commentAfter(ctx->fieldlist(), "");
    if (postComment.find("\n") != string::npos) {
        return false;
    }
    return true;
}

string FormatVisitor::indent() {
    stringstream ss;
    for (int i = 0; i < _indent; i++) {
        ss << config.indent();
    }
    return ss.str();
}

string FormatVisitor::incIndent() {
    _indent++;
    return indent();
}

string FormatVisitor::decIndent() {
    _indent--;
    return indent();
}