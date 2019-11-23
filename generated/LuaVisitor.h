
// Generated from Lua.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"
#include "LuaParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by LuaParser.
 */
class  LuaVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by LuaParser.
   */
    virtual antlrcpp::Any visitChunk(LuaParser::ChunkContext *context) = 0;

    virtual antlrcpp::Any visitBlock(LuaParser::BlockContext *context) = 0;

    virtual antlrcpp::Any visitStat(LuaParser::StatContext *context) = 0;

    virtual antlrcpp::Any visitVarDecl(LuaParser::VarDeclContext *context) = 0;

    virtual antlrcpp::Any visitFunctioncall(LuaParser::FunctioncallContext *context) = 0;

    virtual antlrcpp::Any visitGotoStat(LuaParser::GotoStatContext *context) = 0;

    virtual antlrcpp::Any visitDoStat(LuaParser::DoStatContext *context) = 0;

    virtual antlrcpp::Any visitWhileStat(LuaParser::WhileStatContext *context) = 0;

    virtual antlrcpp::Any visitRepeatStat(LuaParser::RepeatStatContext *context) = 0;

    virtual antlrcpp::Any visitIfStat(LuaParser::IfStatContext *context) = 0;

    virtual antlrcpp::Any visitForStat(LuaParser::ForStatContext *context) = 0;

    virtual antlrcpp::Any visitForInStat(LuaParser::ForInStatContext *context) = 0;

    virtual antlrcpp::Any visitFuncStat(LuaParser::FuncStatContext *context) = 0;

    virtual antlrcpp::Any visitLocalFuncStat(LuaParser::LocalFuncStatContext *context) = 0;

    virtual antlrcpp::Any visitLocalVarDecl(LuaParser::LocalVarDeclContext *context) = 0;

    virtual antlrcpp::Any visitRetstat(LuaParser::RetstatContext *context) = 0;

    virtual antlrcpp::Any visitLabel(LuaParser::LabelContext *context) = 0;

    virtual antlrcpp::Any visitFuncname(LuaParser::FuncnameContext *context) = 0;

    virtual antlrcpp::Any visitVarlist(LuaParser::VarlistContext *context) = 0;

    virtual antlrcpp::Any visitNamelist(LuaParser::NamelistContext *context) = 0;

    virtual antlrcpp::Any visitAttnamelist(LuaParser::AttnamelistContext *context) = 0;

    virtual antlrcpp::Any visitAttrib(LuaParser::AttribContext *context) = 0;

    virtual antlrcpp::Any visitExplist(LuaParser::ExplistContext *context) = 0;

    virtual antlrcpp::Any visitExp(LuaParser::ExpContext *context) = 0;

    virtual antlrcpp::Any visitPrefixexp(LuaParser::PrefixexpContext *context) = 0;

    virtual antlrcpp::Any visitVarOrExp(LuaParser::VarOrExpContext *context) = 0;

    virtual antlrcpp::Any visitVar(LuaParser::VarContext *context) = 0;

    virtual antlrcpp::Any visitVarSuffix(LuaParser::VarSuffixContext *context) = 0;

    virtual antlrcpp::Any visitNameAndArgs(LuaParser::NameAndArgsContext *context) = 0;

    virtual antlrcpp::Any visitArgs(LuaParser::ArgsContext *context) = 0;

    virtual antlrcpp::Any visitFunctiondef(LuaParser::FunctiondefContext *context) = 0;

    virtual antlrcpp::Any visitFuncbody(LuaParser::FuncbodyContext *context) = 0;

    virtual antlrcpp::Any visitParlist(LuaParser::ParlistContext *context) = 0;

    virtual antlrcpp::Any visitTableconstructor(LuaParser::TableconstructorContext *context) = 0;

    virtual antlrcpp::Any visitFieldlist(LuaParser::FieldlistContext *context) = 0;

    virtual antlrcpp::Any visitField(LuaParser::FieldContext *context) = 0;

    virtual antlrcpp::Any visitFieldsep(LuaParser::FieldsepContext *context) = 0;

    virtual antlrcpp::Any visitLinkOperator(LuaParser::LinkOperatorContext *context) = 0;

    virtual antlrcpp::Any visitUnaryOperator(LuaParser::UnaryOperatorContext *context) = 0;

    virtual antlrcpp::Any visitNumber(LuaParser::NumberContext *context) = 0;

    virtual antlrcpp::Any visitString(LuaParser::StringContext *context) = 0;


};

