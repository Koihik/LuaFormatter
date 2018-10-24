#include "antlr4-runtime/LuaBaseVisitor.h"
#include "antlr4-runtime/LuaParser.h"

using namespace std;
using namespace antlr4;

class FormatVisitor : public LuaBaseVisitor {
   public:
    FormatVisitor(const vector<Token*>& t) : commentTokens(t) {}

    // | varDecl
    // | functioncall
    // | label
    // | gotoStat
    // | doStat
    // | whileStat
    // | repeatStat
    // | ifStat
    // | forStat
    // | forInStat
    // | funcStat
    // | localFuncStat
    // | localVarDecl;

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
    antlrcpp::Any visitExplist(LuaParser::ExplistContext* context) override;

    antlrcpp::Any visitExp(LuaParser::ExpContext* context) override;
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

    antlrcpp::Any visitChildren(tree::ParseTree* tree) override;
    antlrcpp::Any visitTerminal(tree::TerminalNode* node) override;

   private:
    int _indent = 0;
    const vector<Token*>& commentTokens;

    string indent();
    string commentAfter(tree::ParseTree* a, const string& expect);
    string commentAfterNewLine(tree::ParseTree* a, int intdentSize);
    string newLine();
    string newLineIncIndent();
    string newLineDecIndent();
    string incIndent();
    string decIndent();
};