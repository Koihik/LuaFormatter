#pragma once

#include "Config.h"
#include "LuaBaseVisitor.h"
#include "LuaParser.h"
#include "SourceWriter.h"

using namespace std;
using namespace antlr4;


enum BlockType { CONTROL_BLOCK, FUNCTION_BLOCK };
enum NewLineIndent { NONE_INDENT, INC_INDENT, DEC_INDENT, INC_CONTINUATION_INDENT, DEC_CONTINUATION_INDENT };

class FormatVisitor : public LuaBaseVisitor {
   public:
    FormatVisitor(const vector<Token*>& tokens, const Config& config) : tokens_(tokens), config_(config) {}

    antlrcpp::Any visitChunk(LuaParser::ChunkContext* context) override;
    antlrcpp::Any visitBlock(LuaParser::BlockContext* context) override;
    antlrcpp::Any visitStat(LuaParser::StatContext* context) override;
    antlrcpp::Any visitFunctioncall(LuaParser::FunctioncallContext* context) override;
    antlrcpp::Any visitVarDecl(LuaParser::VarDeclContext* context) override;
    antlrcpp::Any visitGotoStat(LuaParser::GotoStatContext* context) override;
    antlrcpp::Any visitDoStat(LuaParser::DoStatContext* context) override;
    antlrcpp::Any visitWhileStat(LuaParser::WhileStatContext* context) override; 
    antlrcpp::Any visitRepeatStat(LuaParser::RepeatStatContext* context) override;
    antlrcpp::Any visitIfStat(LuaParser::IfStatContext* context) override;
    antlrcpp::Any visitForStat(LuaParser::ForStatContext* context) override;
    antlrcpp::Any visitForInStat(LuaParser::ForInStatContext* context) override;
    antlrcpp::Any visitFuncStat(LuaParser::FuncStatContext* context) override;
    antlrcpp::Any visitLocalFuncStat(LuaParser::LocalFuncStatContext* context) override;
    antlrcpp::Any visitLocalVarDecl(LuaParser::LocalVarDeclContext* context) override;

    antlrcpp::Any visitRetstat(LuaParser::RetstatContext* context) override;
    antlrcpp::Any visitVarlist(LuaParser::VarlistContext* context) override;
    antlrcpp::Any visitNamelist(LuaParser::NamelistContext* context) override;
    antlrcpp::Any visitAttnamelist(LuaParser::AttnamelistContext* context) override;
    antlrcpp::Any visitAttrib(LuaParser::AttribContext* context) override;
    antlrcpp::Any visitExplist(LuaParser::ExplistContext* context) override;

    antlrcpp::Any visitExp(LuaParser::ExpContext* context) override;
    antlrcpp::Any visitString(LuaParser::StringContext* ctx) override;
    antlrcpp::Any visitPrefixexp(LuaParser::PrefixexpContext* context) override;
    antlrcpp::Any visitVarOrExp(LuaParser::VarOrExpContext* context) override;
    antlrcpp::Any visitVar(LuaParser::VarContext* context) override;
    antlrcpp::Any visitVarSuffix(LuaParser::VarSuffixContext* context) override;
    antlrcpp::Any visitNameAndArgs(LuaParser::NameAndArgsContext* context) override;
    antlrcpp::Any visitArgs(LuaParser::ArgsContext* context) override;

    antlrcpp::Any visitFunctiondef(LuaParser::FunctiondefContext* context) override;
    antlrcpp::Any visitFuncbody(LuaParser::FuncbodyContext* context) override;
    antlrcpp::Any visitParlist(LuaParser::ParlistContext* context) override;

    antlrcpp::Any visitTableconstructor(LuaParser::TableconstructorContext* context) override;
    antlrcpp::Any visitFieldlist(LuaParser::FieldlistContext* context) override;
    antlrcpp::Any visitField(LuaParser::FieldContext* context) override;
    antlrcpp::Any visitFieldsep(LuaParser::FieldsepContext* context) override;

    antlrcpp::Any visitChildren(tree::ParseTree* tree) override;
    antlrcpp::Any visitTerminal(tree::TerminalNode* node) override;

   private:
    bool chop_down_block_ = true;
    bool chop_down_table_ = true;

    vector<SourceWriter*> writers_;

    // stack to record column of first table field
    vector<int> firstTableFieldColumn_;

    // stack to record did a chained method call has increased indent
    vector<bool> chainedMethodCallHasIncIndent_;

    // stack to record is a chained method call is the first call
    vector<bool> chainedMethodCallIsFirst_;

    int indent_ = 0;
    int indentForAlign_ = 0;
    const vector<Token*>& tokens_;
    const Config& config_;

    string formatLineComment(Token* token);

    bool needKeepBlockOneLine(tree::ParseTree* previousNode, LuaParser::BlockContext* ctx, BlockType blockType);
    bool isBlockEmpty(LuaParser::BlockContext* ctx);
    void visitBlockAndComment(tree::ParseTree* previousNode, LuaParser::BlockContext* ctx, BlockType blockType);
    void visitNextNameAndArgs(LuaParser::VarSuffixContext* ctx);

    void pushWriter();
    void pushWriterWithColumn();
    void popWriter();
    bool fastTestColumnLimit(tree::ParseTree* node);
    SourceWriter& cur_writer();
    int cur_columns();

    string indent();
    string indentWithAlign();
    string lineBreak();
    string commentAfter(tree::ParseTree* a, const string& expect);
    string commentAfterNewLine(tree::ParseTree* a, NewLineIndent newLineIndent);
    void incIndent();
    void decIndent();
    void incContinuationIndent();
    void decContinuationIndent();
    void incIndentForAlign(int indent);
    void decIndentForAlign(int indent);

    // Auxiliary to visitFunctioncall and visitPrefixexp
    string buildFirstArgumentWs(vector<LuaParser::NameAndArgsContext*> v);
    void buildArguments(vector<LuaParser::NameAndArgsContext*> v);
};
