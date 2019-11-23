
// Generated from Lua.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"
#include "LuaVisitor.h"


/**
 * This class provides an empty implementation of LuaVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  LuaBaseVisitor : public LuaVisitor {
public:

  virtual antlrcpp::Any visitChunk(LuaParser::ChunkContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBlock(LuaParser::BlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStat(LuaParser::StatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitVarDecl(LuaParser::VarDeclContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFunctioncall(LuaParser::FunctioncallContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitGotoStat(LuaParser::GotoStatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitDoStat(LuaParser::DoStatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitWhileStat(LuaParser::WhileStatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRepeatStat(LuaParser::RepeatStatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitIfStat(LuaParser::IfStatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitForStat(LuaParser::ForStatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitForInStat(LuaParser::ForInStatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncStat(LuaParser::FuncStatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLocalFuncStat(LuaParser::LocalFuncStatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLocalVarDecl(LuaParser::LocalVarDeclContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRetstat(LuaParser::RetstatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLabel(LuaParser::LabelContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncname(LuaParser::FuncnameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitVarlist(LuaParser::VarlistContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNamelist(LuaParser::NamelistContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAttnamelist(LuaParser::AttnamelistContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAttrib(LuaParser::AttribContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitExplist(LuaParser::ExplistContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitExp(LuaParser::ExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPrefixexp(LuaParser::PrefixexpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitVarOrExp(LuaParser::VarOrExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitVar(LuaParser::VarContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitVarSuffix(LuaParser::VarSuffixContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNameAndArgs(LuaParser::NameAndArgsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitArgs(LuaParser::ArgsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFunctiondef(LuaParser::FunctiondefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncbody(LuaParser::FuncbodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitParlist(LuaParser::ParlistContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTableconstructor(LuaParser::TableconstructorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFieldlist(LuaParser::FieldlistContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitField(LuaParser::FieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFieldsep(LuaParser::FieldsepContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLinkOperator(LuaParser::LinkOperatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitUnaryOperator(LuaParser::UnaryOperatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNumber(LuaParser::NumberContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitString(LuaParser::StringContext *ctx) override {
    return visitChildren(ctx);
  }


};

