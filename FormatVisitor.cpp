#include <iostream>

#include "FormatVisitor.h"
#include "antlr4-runtime/LuaLexer.h"

using namespace std;
using namespace antlr4;

#define LOGVAR(arg) cout << "" #arg " = " << (arg) << endl
#define LOG(arg) cout << arg << endl

int lastIdx(tree::ParseTree* t) {
    ParserRuleContext* c = dynamic_cast<ParserRuleContext*>(t);
    if (c != NULL) {
        if (c->children.empty()) {
            return c->getStop()->getTokenIndex() - 1;
        }
        return lastIdx(c->children.back());
    }
    tree::TerminalNode* n = dynamic_cast<tree::TerminalNode*>(t);
    if (n != NULL) {
        return n->getSymbol()->getTokenIndex();
    }
    return -1;
}

string FormatVisitor::commentAfter(tree::ParseTree* a, const string& expect) {
    int l = lastIdx(a);
    stringstream ss;
    bool lastComment = true;
    for (int i = l + 1; i < commentTokens.size(); i++) {
        auto token = commentTokens[i];
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
            ss << token->getText();
            ss << indent();
            lastComment = false;
        } else if (token->getType() == LuaLexer::WS) {
        } else {
            break;
        }
    }
    if (ss.tellp() == 0) {
        ss << expect;
    } else if (lastComment) {
        ss << " ";
    }
    return ss.str();
}

string FormatVisitor::commentAfterNewLine(tree::ParseTree* a, int intdentSize) {
    ParserRuleContext* ctx = dynamic_cast<ParserRuleContext*>(a);
    if (ctx != NULL && ctx->children.size() == 0) {
        _indent += intdentSize;
        return "";
    }
    int l = lastIdx(a);
    if (intdentSize > 0) {
        _indent += intdentSize;
    }
    stringstream ss;
    bool customNewLine = false;
    bool lastComment = true;
    bool lastestNewLine = false;
    for (int i = l + 1;; i++) {
        auto token = commentTokens[i];
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
            ss << token->getText();
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
    }
    if (intdentSize < 0) {
        _indent += intdentSize;
    }
    return ss.str();
}

antlrcpp::Any FormatVisitor::visitChunk(LuaParser::ChunkContext* ctx) {
    LOG("visitChunk");
    return visitChildren(ctx);
}

// stat* retstat?
antlrcpp::Any FormatVisitor::visitBlock(LuaParser::BlockContext* ctx) {
    LOG("visitBlock");
    stringstream ss;
    auto stats = ctx->stat();
    int n = stats.size();
    for (int i = 0; i < n; i++) {
        if (stats[i]->SEMI() == NULL) {
            ss << visitStat(stats[i]).as<string>();
        }
        if (i != n - 1 || ctx->retstat() != NULL) {
            if (i + 1 >= n || stats[i + 1]->SEMI() == NULL) {
                ss << commentAfterNewLine(stats[i], 0);
            }
        }
    }
    if (ctx->retstat() != NULL) {
        ss << visitRetstat(ctx->retstat()).as<string>();
        // ss << commentAfterNewLine(ctx->retstat(), 0);
    }
    return ss.str();
}

antlrcpp::Any FormatVisitor::visitStat(LuaParser::StatContext* ctx) {
    LOG("visitStat");
    stringstream ss;
    ss << indent() << visitChildren(ctx).as<string>();
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
    return ss.str();
}

// varOrExp nameAndArgs+;
antlrcpp::Any FormatVisitor::visitFunctioncall(LuaParser::FunctioncallContext* ctx) {
    LOG("visitFunctioncall");
    stringstream ss;
    string ret = visitChildren(ctx);
    ss << ret;
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
    ss << ctx->DO()->getText()                   //
       << commentAfterNewLine(ctx->DO(), 1)      //
       << visitBlock(ctx->block()).as<string>()  //
       << commentAfterNewLine(ctx->block(), -1)  //
       << indent()                               //
       << ctx->END()->getText();
    return ss.str();
}

// WHILE exp DO block END;
antlrcpp::Any FormatVisitor::visitWhileStat(LuaParser::WhileStatContext* ctx) {
    LOG("visitWhileStat");
    stringstream ss;
    ss << ctx->WHILE()->getText()                //
       << commentAfter(ctx->WHILE(), " ")        //
       << visitExp(ctx->exp()).as<string>()      //
       << commentAfter(ctx->exp(), " ")          //
       << ctx->DO()->getText()                   //
       << commentAfterNewLine(ctx->DO(), 1)      //
       << visitBlock(ctx->block()).as<string>()  //
       << commentAfterNewLine(ctx->block(), -1)  //
       << indent()                               //
       << ctx->END()->getText();
    return ss.str();
}

// REPEAT block UNTIL exp;
antlrcpp::Any FormatVisitor::visitRepeatStat(LuaParser::RepeatStatContext* ctx) {
    LOG("visitRepeatStat");
    stringstream ss;
    ss << ctx->REPEAT()->getText()               //
       << commentAfterNewLine(ctx->REPEAT(), 1)  //
       << visitBlock(ctx->block()).as<string>()  //
       << commentAfterNewLine(ctx->block(), -1)  //
       << indent()                               //
       << ctx->UNTIL()->getText()                //
       << commentAfter(ctx->UNTIL(), " ")        //
       << visitExp(ctx->exp()).as<string>();     //
    return ss.str();
}

// IF exp THEN block (ELSEIF exp THEN block)* (ELSE block)? END;
antlrcpp::Any FormatVisitor::visitIfStat(LuaParser::IfStatContext* ctx) {
    LOG("visitIfStat");
    stringstream ss;
    ss << ctx->IF()->getText()                           //
       << commentAfter(ctx->IF(), " ")                   //
       << visitExp(ctx->exp().front()).as<string>()      //
       << commentAfter(ctx->exp().front(), " ")          //
       << ctx->THEN().front()->getText()                 //
       << commentAfterNewLine(ctx->THEN().front(), 1)    //
       << visitBlock(ctx->block().front()).as<string>()  //
       << commentAfterNewLine(ctx->block().front(), -1);
    int n = ctx->ELSEIF().size();
    for (int i = 0; i < n; i++) {
        LOG("visitIfStat 2");
        ss << indent()                                      //
           << ctx->ELSEIF()[i]->getText()                   //
           << commentAfter(ctx->ELSEIF()[i], " ")           //
           << visitExp(ctx->exp()[i + 1]).as<string>()      //
           << commentAfter(ctx->exp()[i + 1], " ")          //
           << ctx->THEN()[i + 1]->getText()                 //
           << commentAfterNewLine(ctx->THEN()[i + 1], 1)    //
           << visitBlock(ctx->block()[i + 1]).as<string>()  //
           << commentAfterNewLine(ctx->block()[i + 1], -1);
    }
    if (ctx->ELSE() != NULL) {
        LOG("visitIfStat 3");
        ss << indent()                                      //
           << ctx->ELSE()->getText()                        //
           << commentAfterNewLine(ctx->ELSE(), 1)           //
           << visitBlock(ctx->block().back()).as<string>()  //
           << commentAfterNewLine(ctx->block().back(), -1);
    }
    ss << indent()  //
       << ctx->END()->getText();
    return ss.str();
}

// FOR NAME EQL exp COMMA exp (COMMA exp)? DO block END;
antlrcpp::Any FormatVisitor::visitForStat(LuaParser::ForStatContext* ctx) {
    LOG("visitForStat");
    stringstream ss;
    ss << ctx->FOR()->getText()                 //
       << commentAfter(ctx->FOR(), " ")         //
       << ctx->NAME()->getText()                //
       << commentAfter(ctx->NAME(), " ")        //
       << ctx->EQL()->getText()                 //
       << commentAfter(ctx->EQL(), " ")         //
       << visitExp(ctx->exp()[0]).as<string>()  //
       << commentAfter(ctx->exp()[0], " ")      //
       << ctx->COMMA()[0]->getText()            //
       << commentAfter(ctx->COMMA()[0], " ")    //
       << visitExp(ctx->exp()[1]).as<string>()  //
       << commentAfter(ctx->exp()[1], " ");     //
    auto commas = ctx->COMMA();
    int n = commas.size();
    for (int i = 1; i < n; i++) {
        ss << ctx->COMMA()[i]->getText()                //
           << commentAfter(ctx->COMMA()[i], " ")        //
           << visitExp(ctx->exp()[i + 1]).as<string>()  //
           << commentAfter(ctx->exp()[i + 1], " ");     //
    }
    ss << ctx->DO()->getText()                   //
       << commentAfterNewLine(ctx->DO(), 1)      //
       << visitBlock(ctx->block()).as<string>()  //
       << commentAfterNewLine(ctx->block(), -1)  //
       << indent()                               //
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
       << ctx->DO()->getText()                         //
       << commentAfterNewLine(ctx->DO(), 1)            //
       << visitBlock(ctx->block()).as<string>()        //
       << commentAfterNewLine(ctx->block(), -1)        //
       << indent()                                     //
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

// LOCAL namelist (EQL explist)?;
antlrcpp::Any FormatVisitor::visitLocalVarDecl(LuaParser::LocalVarDeclContext* ctx) {
    LOG("visitLocalVarDecl");
    stringstream ss;
    LOG("visitLocalVarDecl 1");
    LOGVAR(ctx->LOCAL()->getText());
    LOGVAR(ctx->namelist());
    ss << ctx->LOCAL()->getText();  //
    LOG("visitLocalVarDecl 1");
    ss << commentAfter(ctx->LOCAL(), " ");  //
    LOG("visitLocalVarDecl 1");
    ss << visitNamelist(ctx->namelist()).as<string>();  //
    if (ctx->EQL() != NULL) {
        LOG("visitLocalVarDecl 2");
        ss << commentAfter(ctx->namelist(), " ")  //
           << ctx->EQL()->getText()               //
           << commentAfter(ctx->EQL(), " ")       //
           << visitExplist(ctx->explist()).as<string>();
    }
    LOG("visitLocalVarDecl 3");
    return ss.str();
}

// RETURN explist? SEMI?;
antlrcpp::Any FormatVisitor::visitRetstat(LuaParser::RetstatContext* ctx) {
    LOG("visitRetstat");
    stringstream ss;
    ss << indent() << ctx->RETURN()->getText();
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
    } else {
        return visitChildren(ctx);
    }
}

// varOrExp nameAndArgs*;
antlrcpp::Any FormatVisitor::visitPrefixexp(LuaParser::PrefixexpContext* ctx) {
    LOG("visitPrefixexp");
    stringstream ss;
    ss << visitVarOrExp(ctx->varOrExp()).as<string>()  //
       << commentAfter(ctx->varOrExp(), "");
    int n = ctx->nameAndArgs().size();
    for (int i = 0; i < n; i++) {
        ss << visitNameAndArgs(ctx->nameAndArgs()[i]).as<string>();
        if (i != n - 1) {
            ss << commentAfter(ctx->nameAndArgs()[i], " ");
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
    LOG("visitVar 1");
    int n = ctx->varSuffix().size();
    for (int i = startAt; i < n; i++) {
        ss << visitVarSuffix(ctx->varSuffix()[i]).as<string>();
        if (i != n - 1) {
            ss << commentAfter(ctx->varSuffix()[i], "");
        }
    }
    LOG("visitVar 2");
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
        ss << ctx->LSB()->getText()              //
           << commentAfter(ctx->LSB(), "")       //
           << visitExp(ctx->exp()).as<string>()  //
           << commentAfter(ctx->exp(), "")       //
           << ctx->RSB()->getText();
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
    ss << ctx->RP()->getText()                   //
       << commentAfterNewLine(ctx->RP(), 1)      //
       << visitBlock(ctx->block()).as<string>()  //
       << commentAfterNewLine(ctx->block(), -1)  //
       << indent()                               //
       << ctx->END()->getText();                 //
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
        ss << commentAfterNewLine(ctx->LB(), 1)              //
           << visitFieldlist(ctx->fieldlist()).as<string>()  //
           << commentAfterNewLine(ctx->fieldlist(), -1)      //
           << indent()                                       //
           << ctx->RB()->getText();
    } else {
        ss << commentAfter(ctx->LB(), "")  //
           << ctx->RB()->getText();
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
            ss << seps[i - 1]->getText();
            ss << commentAfterNewLine(seps[i - 1], 0);
            ss << visitField(fields[i]).as<string>();
            if (i != n - 1 || seps.size() == n) {
                ss << commentAfter(fields[i], "");
            }
        }
        if (seps.size() == n) {
            ss << seps.back()->getText();
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

string FormatVisitor::indent() {
    stringstream ss;
    for (int i = 0; i < _indent; i++) {
        ss << "    ";
    }
    return ss.str();
}

string FormatVisitor::newLine() {
    stringstream ss;
    ss << endl;
    return ss.str();
}

string FormatVisitor::newLineIncIndent() {
    _indent++;
    stringstream ss;
    ss << endl;
    return ss.str();
}

string FormatVisitor::newLineDecIndent() {
    _indent--;
    stringstream ss;
    ss << endl;
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