
// Generated from Lua.g4 by ANTLR 4.7.1


#include "LuaVisitor.h"

#include "LuaParser.h"


using namespace antlrcpp;
using namespace antlr4;

LuaParser::LuaParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

LuaParser::~LuaParser() {
  delete _interpreter;
}

std::string LuaParser::getGrammarFileName() const {
  return "Lua.g4";
}

const std::vector<std::string>& LuaParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& LuaParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- ChunkContext ------------------------------------------------------------------

LuaParser::ChunkContext::ChunkContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

LuaParser::BlockContext* LuaParser::ChunkContext::block() {
  return getRuleContext<LuaParser::BlockContext>(0);
}

tree::TerminalNode* LuaParser::ChunkContext::EOF() {
  return getToken(LuaParser::EOF, 0);
}


size_t LuaParser::ChunkContext::getRuleIndex() const {
  return LuaParser::RuleChunk;
}

antlrcpp::Any LuaParser::ChunkContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitChunk(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::ChunkContext* LuaParser::chunk() {
  ChunkContext *_localctx = _tracker.createInstance<ChunkContext>(_ctx, getState());
  enterRule(_localctx, 0, LuaParser::RuleChunk);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(82);
    block();
    setState(83);
    match(LuaParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockContext ------------------------------------------------------------------

LuaParser::BlockContext::BlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<LuaParser::StatContext *> LuaParser::BlockContext::stat() {
  return getRuleContexts<LuaParser::StatContext>();
}

LuaParser::StatContext* LuaParser::BlockContext::stat(size_t i) {
  return getRuleContext<LuaParser::StatContext>(i);
}

LuaParser::RetstatContext* LuaParser::BlockContext::retstat() {
  return getRuleContext<LuaParser::RetstatContext>(0);
}


size_t LuaParser::BlockContext::getRuleIndex() const {
  return LuaParser::RuleBlock;
}

antlrcpp::Any LuaParser::BlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitBlock(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::BlockContext* LuaParser::block() {
  BlockContext *_localctx = _tracker.createInstance<BlockContext>(_ctx, getState());
  enterRule(_localctx, 2, LuaParser::RuleBlock);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(88);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << LuaParser::FUNCTION)
      | (1ULL << LuaParser::LOCAL)
      | (1ULL << LuaParser::BREAK)
      | (1ULL << LuaParser::DO)
      | (1ULL << LuaParser::GOTO)
      | (1ULL << LuaParser::WHILE)
      | (1ULL << LuaParser::REPEAT)
      | (1ULL << LuaParser::FOR)
      | (1ULL << LuaParser::IF)
      | (1ULL << LuaParser::SEMI)
      | (1ULL << LuaParser::DCOLON)
      | (1ULL << LuaParser::LP)
      | (1ULL << LuaParser::NAME))) != 0)) {
      setState(85);
      stat();
      setState(90);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(92);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == LuaParser::RETURN) {
      setState(91);
      retstat();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StatContext ------------------------------------------------------------------

LuaParser::StatContext::StatContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::StatContext::SEMI() {
  return getToken(LuaParser::SEMI, 0);
}

LuaParser::VarDeclContext* LuaParser::StatContext::varDecl() {
  return getRuleContext<LuaParser::VarDeclContext>(0);
}

LuaParser::FunctioncallContext* LuaParser::StatContext::functioncall() {
  return getRuleContext<LuaParser::FunctioncallContext>(0);
}

LuaParser::LabelContext* LuaParser::StatContext::label() {
  return getRuleContext<LuaParser::LabelContext>(0);
}

tree::TerminalNode* LuaParser::StatContext::BREAK() {
  return getToken(LuaParser::BREAK, 0);
}

LuaParser::GotoStatContext* LuaParser::StatContext::gotoStat() {
  return getRuleContext<LuaParser::GotoStatContext>(0);
}

LuaParser::DoStatContext* LuaParser::StatContext::doStat() {
  return getRuleContext<LuaParser::DoStatContext>(0);
}

LuaParser::WhileStatContext* LuaParser::StatContext::whileStat() {
  return getRuleContext<LuaParser::WhileStatContext>(0);
}

LuaParser::RepeatStatContext* LuaParser::StatContext::repeatStat() {
  return getRuleContext<LuaParser::RepeatStatContext>(0);
}

LuaParser::IfStatContext* LuaParser::StatContext::ifStat() {
  return getRuleContext<LuaParser::IfStatContext>(0);
}

LuaParser::ForStatContext* LuaParser::StatContext::forStat() {
  return getRuleContext<LuaParser::ForStatContext>(0);
}

LuaParser::ForInStatContext* LuaParser::StatContext::forInStat() {
  return getRuleContext<LuaParser::ForInStatContext>(0);
}

LuaParser::FuncStatContext* LuaParser::StatContext::funcStat() {
  return getRuleContext<LuaParser::FuncStatContext>(0);
}

LuaParser::LocalFuncStatContext* LuaParser::StatContext::localFuncStat() {
  return getRuleContext<LuaParser::LocalFuncStatContext>(0);
}

LuaParser::LocalVarDeclContext* LuaParser::StatContext::localVarDecl() {
  return getRuleContext<LuaParser::LocalVarDeclContext>(0);
}


size_t LuaParser::StatContext::getRuleIndex() const {
  return LuaParser::RuleStat;
}

antlrcpp::Any LuaParser::StatContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitStat(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::StatContext* LuaParser::stat() {
  StatContext *_localctx = _tracker.createInstance<StatContext>(_ctx, getState());
  enterRule(_localctx, 4, LuaParser::RuleStat);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(109);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(94);
      match(LuaParser::SEMI);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(95);
      varDecl();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(96);
      functioncall();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(97);
      label();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(98);
      match(LuaParser::BREAK);
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(99);
      gotoStat();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(100);
      doStat();
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(101);
      whileStat();
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(102);
      repeatStat();
      break;
    }

    case 10: {
      enterOuterAlt(_localctx, 10);
      setState(103);
      ifStat();
      break;
    }

    case 11: {
      enterOuterAlt(_localctx, 11);
      setState(104);
      forStat();
      break;
    }

    case 12: {
      enterOuterAlt(_localctx, 12);
      setState(105);
      forInStat();
      break;
    }

    case 13: {
      enterOuterAlt(_localctx, 13);
      setState(106);
      funcStat();
      break;
    }

    case 14: {
      enterOuterAlt(_localctx, 14);
      setState(107);
      localFuncStat();
      break;
    }

    case 15: {
      enterOuterAlt(_localctx, 15);
      setState(108);
      localVarDecl();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarDeclContext ------------------------------------------------------------------

LuaParser::VarDeclContext::VarDeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

LuaParser::VarlistContext* LuaParser::VarDeclContext::varlist() {
  return getRuleContext<LuaParser::VarlistContext>(0);
}

tree::TerminalNode* LuaParser::VarDeclContext::EQL() {
  return getToken(LuaParser::EQL, 0);
}

LuaParser::ExplistContext* LuaParser::VarDeclContext::explist() {
  return getRuleContext<LuaParser::ExplistContext>(0);
}

tree::TerminalNode* LuaParser::VarDeclContext::SEMI() {
  return getToken(LuaParser::SEMI, 0);
}


size_t LuaParser::VarDeclContext::getRuleIndex() const {
  return LuaParser::RuleVarDecl;
}

antlrcpp::Any LuaParser::VarDeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitVarDecl(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::VarDeclContext* LuaParser::varDecl() {
  VarDeclContext *_localctx = _tracker.createInstance<VarDeclContext>(_ctx, getState());
  enterRule(_localctx, 6, LuaParser::RuleVarDecl);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(111);
    varlist();
    setState(112);
    match(LuaParser::EQL);
    setState(113);
    explist();
    setState(115);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx)) {
    case 1: {
      setState(114);
      match(LuaParser::SEMI);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FunctioncallContext ------------------------------------------------------------------

LuaParser::FunctioncallContext::FunctioncallContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

LuaParser::VarOrExpContext* LuaParser::FunctioncallContext::varOrExp() {
  return getRuleContext<LuaParser::VarOrExpContext>(0);
}

std::vector<LuaParser::NameAndArgsContext *> LuaParser::FunctioncallContext::nameAndArgs() {
  return getRuleContexts<LuaParser::NameAndArgsContext>();
}

LuaParser::NameAndArgsContext* LuaParser::FunctioncallContext::nameAndArgs(size_t i) {
  return getRuleContext<LuaParser::NameAndArgsContext>(i);
}

tree::TerminalNode* LuaParser::FunctioncallContext::SEMI() {
  return getToken(LuaParser::SEMI, 0);
}


size_t LuaParser::FunctioncallContext::getRuleIndex() const {
  return LuaParser::RuleFunctioncall;
}

antlrcpp::Any LuaParser::FunctioncallContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitFunctioncall(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::FunctioncallContext* LuaParser::functioncall() {
  FunctioncallContext *_localctx = _tracker.createInstance<FunctioncallContext>(_ctx, getState());
  enterRule(_localctx, 8, LuaParser::RuleFunctioncall);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(117);
    varOrExp();
    setState(119); 
    _errHandler->sync(this);
    alt = 1;
    do {
      switch (alt) {
        case 1: {
              setState(118);
              nameAndArgs();
              break;
            }

      default:
        throw NoViableAltException(this);
      }
      setState(121); 
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx);
    } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER);
    setState(124);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx)) {
    case 1: {
      setState(123);
      match(LuaParser::SEMI);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- GotoStatContext ------------------------------------------------------------------

LuaParser::GotoStatContext::GotoStatContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::GotoStatContext::GOTO() {
  return getToken(LuaParser::GOTO, 0);
}

tree::TerminalNode* LuaParser::GotoStatContext::NAME() {
  return getToken(LuaParser::NAME, 0);
}


size_t LuaParser::GotoStatContext::getRuleIndex() const {
  return LuaParser::RuleGotoStat;
}

antlrcpp::Any LuaParser::GotoStatContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitGotoStat(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::GotoStatContext* LuaParser::gotoStat() {
  GotoStatContext *_localctx = _tracker.createInstance<GotoStatContext>(_ctx, getState());
  enterRule(_localctx, 10, LuaParser::RuleGotoStat);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(126);
    match(LuaParser::GOTO);
    setState(127);
    match(LuaParser::NAME);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DoStatContext ------------------------------------------------------------------

LuaParser::DoStatContext::DoStatContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::DoStatContext::DO() {
  return getToken(LuaParser::DO, 0);
}

LuaParser::BlockContext* LuaParser::DoStatContext::block() {
  return getRuleContext<LuaParser::BlockContext>(0);
}

tree::TerminalNode* LuaParser::DoStatContext::END() {
  return getToken(LuaParser::END, 0);
}


size_t LuaParser::DoStatContext::getRuleIndex() const {
  return LuaParser::RuleDoStat;
}

antlrcpp::Any LuaParser::DoStatContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitDoStat(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::DoStatContext* LuaParser::doStat() {
  DoStatContext *_localctx = _tracker.createInstance<DoStatContext>(_ctx, getState());
  enterRule(_localctx, 12, LuaParser::RuleDoStat);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(129);
    match(LuaParser::DO);
    setState(130);
    block();
    setState(131);
    match(LuaParser::END);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- WhileStatContext ------------------------------------------------------------------

LuaParser::WhileStatContext::WhileStatContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::WhileStatContext::WHILE() {
  return getToken(LuaParser::WHILE, 0);
}

LuaParser::ExpContext* LuaParser::WhileStatContext::exp() {
  return getRuleContext<LuaParser::ExpContext>(0);
}

tree::TerminalNode* LuaParser::WhileStatContext::DO() {
  return getToken(LuaParser::DO, 0);
}

LuaParser::BlockContext* LuaParser::WhileStatContext::block() {
  return getRuleContext<LuaParser::BlockContext>(0);
}

tree::TerminalNode* LuaParser::WhileStatContext::END() {
  return getToken(LuaParser::END, 0);
}


size_t LuaParser::WhileStatContext::getRuleIndex() const {
  return LuaParser::RuleWhileStat;
}

antlrcpp::Any LuaParser::WhileStatContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitWhileStat(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::WhileStatContext* LuaParser::whileStat() {
  WhileStatContext *_localctx = _tracker.createInstance<WhileStatContext>(_ctx, getState());
  enterRule(_localctx, 14, LuaParser::RuleWhileStat);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(133);
    match(LuaParser::WHILE);
    setState(134);
    exp(0);
    setState(135);
    match(LuaParser::DO);
    setState(136);
    block();
    setState(137);
    match(LuaParser::END);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RepeatStatContext ------------------------------------------------------------------

LuaParser::RepeatStatContext::RepeatStatContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::RepeatStatContext::REPEAT() {
  return getToken(LuaParser::REPEAT, 0);
}

LuaParser::BlockContext* LuaParser::RepeatStatContext::block() {
  return getRuleContext<LuaParser::BlockContext>(0);
}

tree::TerminalNode* LuaParser::RepeatStatContext::UNTIL() {
  return getToken(LuaParser::UNTIL, 0);
}

LuaParser::ExpContext* LuaParser::RepeatStatContext::exp() {
  return getRuleContext<LuaParser::ExpContext>(0);
}

tree::TerminalNode* LuaParser::RepeatStatContext::SEMI() {
  return getToken(LuaParser::SEMI, 0);
}


size_t LuaParser::RepeatStatContext::getRuleIndex() const {
  return LuaParser::RuleRepeatStat;
}

antlrcpp::Any LuaParser::RepeatStatContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitRepeatStat(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::RepeatStatContext* LuaParser::repeatStat() {
  RepeatStatContext *_localctx = _tracker.createInstance<RepeatStatContext>(_ctx, getState());
  enterRule(_localctx, 16, LuaParser::RuleRepeatStat);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(139);
    match(LuaParser::REPEAT);
    setState(140);
    block();
    setState(141);
    match(LuaParser::UNTIL);
    setState(142);
    exp(0);
    setState(144);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx)) {
    case 1: {
      setState(143);
      match(LuaParser::SEMI);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IfStatContext ------------------------------------------------------------------

LuaParser::IfStatContext::IfStatContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::IfStatContext::IF() {
  return getToken(LuaParser::IF, 0);
}

std::vector<LuaParser::ExpContext *> LuaParser::IfStatContext::exp() {
  return getRuleContexts<LuaParser::ExpContext>();
}

LuaParser::ExpContext* LuaParser::IfStatContext::exp(size_t i) {
  return getRuleContext<LuaParser::ExpContext>(i);
}

std::vector<tree::TerminalNode *> LuaParser::IfStatContext::THEN() {
  return getTokens(LuaParser::THEN);
}

tree::TerminalNode* LuaParser::IfStatContext::THEN(size_t i) {
  return getToken(LuaParser::THEN, i);
}

std::vector<LuaParser::BlockContext *> LuaParser::IfStatContext::block() {
  return getRuleContexts<LuaParser::BlockContext>();
}

LuaParser::BlockContext* LuaParser::IfStatContext::block(size_t i) {
  return getRuleContext<LuaParser::BlockContext>(i);
}

tree::TerminalNode* LuaParser::IfStatContext::END() {
  return getToken(LuaParser::END, 0);
}

std::vector<tree::TerminalNode *> LuaParser::IfStatContext::ELSEIF() {
  return getTokens(LuaParser::ELSEIF);
}

tree::TerminalNode* LuaParser::IfStatContext::ELSEIF(size_t i) {
  return getToken(LuaParser::ELSEIF, i);
}

tree::TerminalNode* LuaParser::IfStatContext::ELSE() {
  return getToken(LuaParser::ELSE, 0);
}


size_t LuaParser::IfStatContext::getRuleIndex() const {
  return LuaParser::RuleIfStat;
}

antlrcpp::Any LuaParser::IfStatContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitIfStat(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::IfStatContext* LuaParser::ifStat() {
  IfStatContext *_localctx = _tracker.createInstance<IfStatContext>(_ctx, getState());
  enterRule(_localctx, 18, LuaParser::RuleIfStat);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(146);
    match(LuaParser::IF);
    setState(147);
    exp(0);
    setState(148);
    match(LuaParser::THEN);
    setState(149);
    block();
    setState(157);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == LuaParser::ELSEIF) {
      setState(150);
      match(LuaParser::ELSEIF);
      setState(151);
      exp(0);
      setState(152);
      match(LuaParser::THEN);
      setState(153);
      block();
      setState(159);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(162);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == LuaParser::ELSE) {
      setState(160);
      match(LuaParser::ELSE);
      setState(161);
      block();
    }
    setState(164);
    match(LuaParser::END);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ForStatContext ------------------------------------------------------------------

LuaParser::ForStatContext::ForStatContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::ForStatContext::FOR() {
  return getToken(LuaParser::FOR, 0);
}

tree::TerminalNode* LuaParser::ForStatContext::NAME() {
  return getToken(LuaParser::NAME, 0);
}

tree::TerminalNode* LuaParser::ForStatContext::EQL() {
  return getToken(LuaParser::EQL, 0);
}

std::vector<LuaParser::ExpContext *> LuaParser::ForStatContext::exp() {
  return getRuleContexts<LuaParser::ExpContext>();
}

LuaParser::ExpContext* LuaParser::ForStatContext::exp(size_t i) {
  return getRuleContext<LuaParser::ExpContext>(i);
}

std::vector<tree::TerminalNode *> LuaParser::ForStatContext::COMMA() {
  return getTokens(LuaParser::COMMA);
}

tree::TerminalNode* LuaParser::ForStatContext::COMMA(size_t i) {
  return getToken(LuaParser::COMMA, i);
}

tree::TerminalNode* LuaParser::ForStatContext::DO() {
  return getToken(LuaParser::DO, 0);
}

LuaParser::BlockContext* LuaParser::ForStatContext::block() {
  return getRuleContext<LuaParser::BlockContext>(0);
}

tree::TerminalNode* LuaParser::ForStatContext::END() {
  return getToken(LuaParser::END, 0);
}


size_t LuaParser::ForStatContext::getRuleIndex() const {
  return LuaParser::RuleForStat;
}

antlrcpp::Any LuaParser::ForStatContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitForStat(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::ForStatContext* LuaParser::forStat() {
  ForStatContext *_localctx = _tracker.createInstance<ForStatContext>(_ctx, getState());
  enterRule(_localctx, 20, LuaParser::RuleForStat);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(166);
    match(LuaParser::FOR);
    setState(167);
    match(LuaParser::NAME);
    setState(168);
    match(LuaParser::EQL);
    setState(169);
    exp(0);
    setState(170);
    match(LuaParser::COMMA);
    setState(171);
    exp(0);
    setState(174);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == LuaParser::COMMA) {
      setState(172);
      match(LuaParser::COMMA);
      setState(173);
      exp(0);
    }
    setState(176);
    match(LuaParser::DO);
    setState(177);
    block();
    setState(178);
    match(LuaParser::END);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ForInStatContext ------------------------------------------------------------------

LuaParser::ForInStatContext::ForInStatContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::ForInStatContext::FOR() {
  return getToken(LuaParser::FOR, 0);
}

LuaParser::NamelistContext* LuaParser::ForInStatContext::namelist() {
  return getRuleContext<LuaParser::NamelistContext>(0);
}

tree::TerminalNode* LuaParser::ForInStatContext::IN() {
  return getToken(LuaParser::IN, 0);
}

LuaParser::ExplistContext* LuaParser::ForInStatContext::explist() {
  return getRuleContext<LuaParser::ExplistContext>(0);
}

tree::TerminalNode* LuaParser::ForInStatContext::DO() {
  return getToken(LuaParser::DO, 0);
}

LuaParser::BlockContext* LuaParser::ForInStatContext::block() {
  return getRuleContext<LuaParser::BlockContext>(0);
}

tree::TerminalNode* LuaParser::ForInStatContext::END() {
  return getToken(LuaParser::END, 0);
}


size_t LuaParser::ForInStatContext::getRuleIndex() const {
  return LuaParser::RuleForInStat;
}

antlrcpp::Any LuaParser::ForInStatContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitForInStat(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::ForInStatContext* LuaParser::forInStat() {
  ForInStatContext *_localctx = _tracker.createInstance<ForInStatContext>(_ctx, getState());
  enterRule(_localctx, 22, LuaParser::RuleForInStat);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(180);
    match(LuaParser::FOR);
    setState(181);
    namelist();
    setState(182);
    match(LuaParser::IN);
    setState(183);
    explist();
    setState(184);
    match(LuaParser::DO);
    setState(185);
    block();
    setState(186);
    match(LuaParser::END);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncStatContext ------------------------------------------------------------------

LuaParser::FuncStatContext::FuncStatContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::FuncStatContext::FUNCTION() {
  return getToken(LuaParser::FUNCTION, 0);
}

LuaParser::FuncnameContext* LuaParser::FuncStatContext::funcname() {
  return getRuleContext<LuaParser::FuncnameContext>(0);
}

LuaParser::FuncbodyContext* LuaParser::FuncStatContext::funcbody() {
  return getRuleContext<LuaParser::FuncbodyContext>(0);
}


size_t LuaParser::FuncStatContext::getRuleIndex() const {
  return LuaParser::RuleFuncStat;
}

antlrcpp::Any LuaParser::FuncStatContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitFuncStat(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::FuncStatContext* LuaParser::funcStat() {
  FuncStatContext *_localctx = _tracker.createInstance<FuncStatContext>(_ctx, getState());
  enterRule(_localctx, 24, LuaParser::RuleFuncStat);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(188);
    match(LuaParser::FUNCTION);
    setState(189);
    funcname();
    setState(190);
    funcbody();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LocalFuncStatContext ------------------------------------------------------------------

LuaParser::LocalFuncStatContext::LocalFuncStatContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::LocalFuncStatContext::LOCAL() {
  return getToken(LuaParser::LOCAL, 0);
}

tree::TerminalNode* LuaParser::LocalFuncStatContext::FUNCTION() {
  return getToken(LuaParser::FUNCTION, 0);
}

tree::TerminalNode* LuaParser::LocalFuncStatContext::NAME() {
  return getToken(LuaParser::NAME, 0);
}

LuaParser::FuncbodyContext* LuaParser::LocalFuncStatContext::funcbody() {
  return getRuleContext<LuaParser::FuncbodyContext>(0);
}


size_t LuaParser::LocalFuncStatContext::getRuleIndex() const {
  return LuaParser::RuleLocalFuncStat;
}

antlrcpp::Any LuaParser::LocalFuncStatContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitLocalFuncStat(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::LocalFuncStatContext* LuaParser::localFuncStat() {
  LocalFuncStatContext *_localctx = _tracker.createInstance<LocalFuncStatContext>(_ctx, getState());
  enterRule(_localctx, 26, LuaParser::RuleLocalFuncStat);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(192);
    match(LuaParser::LOCAL);
    setState(193);
    match(LuaParser::FUNCTION);
    setState(194);
    match(LuaParser::NAME);
    setState(195);
    funcbody();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LocalVarDeclContext ------------------------------------------------------------------

LuaParser::LocalVarDeclContext::LocalVarDeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::LocalVarDeclContext::LOCAL() {
  return getToken(LuaParser::LOCAL, 0);
}

LuaParser::AttnamelistContext* LuaParser::LocalVarDeclContext::attnamelist() {
  return getRuleContext<LuaParser::AttnamelistContext>(0);
}

tree::TerminalNode* LuaParser::LocalVarDeclContext::EQL() {
  return getToken(LuaParser::EQL, 0);
}

LuaParser::ExplistContext* LuaParser::LocalVarDeclContext::explist() {
  return getRuleContext<LuaParser::ExplistContext>(0);
}

tree::TerminalNode* LuaParser::LocalVarDeclContext::SEMI() {
  return getToken(LuaParser::SEMI, 0);
}


size_t LuaParser::LocalVarDeclContext::getRuleIndex() const {
  return LuaParser::RuleLocalVarDecl;
}

antlrcpp::Any LuaParser::LocalVarDeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitLocalVarDecl(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::LocalVarDeclContext* LuaParser::localVarDecl() {
  LocalVarDeclContext *_localctx = _tracker.createInstance<LocalVarDeclContext>(_ctx, getState());
  enterRule(_localctx, 28, LuaParser::RuleLocalVarDecl);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(197);
    match(LuaParser::LOCAL);
    setState(198);
    attnamelist();
    setState(201);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == LuaParser::EQL) {
      setState(199);
      match(LuaParser::EQL);
      setState(200);
      explist();
    }
    setState(204);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 11, _ctx)) {
    case 1: {
      setState(203);
      match(LuaParser::SEMI);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RetstatContext ------------------------------------------------------------------

LuaParser::RetstatContext::RetstatContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::RetstatContext::RETURN() {
  return getToken(LuaParser::RETURN, 0);
}

LuaParser::ExplistContext* LuaParser::RetstatContext::explist() {
  return getRuleContext<LuaParser::ExplistContext>(0);
}

tree::TerminalNode* LuaParser::RetstatContext::SEMI() {
  return getToken(LuaParser::SEMI, 0);
}


size_t LuaParser::RetstatContext::getRuleIndex() const {
  return LuaParser::RuleRetstat;
}

antlrcpp::Any LuaParser::RetstatContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitRetstat(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::RetstatContext* LuaParser::retstat() {
  RetstatContext *_localctx = _tracker.createInstance<RetstatContext>(_ctx, getState());
  enterRule(_localctx, 30, LuaParser::RuleRetstat);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(206);
    match(LuaParser::RETURN);
    setState(208);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << LuaParser::T__8)
      | (1ULL << LuaParser::T__15)
      | (1ULL << LuaParser::T__19)
      | (1ULL << LuaParser::T__20)
      | (1ULL << LuaParser::FUNCTION)
      | (1ULL << LuaParser::TRUE)
      | (1ULL << LuaParser::FALSE)
      | (1ULL << LuaParser::NIL)
      | (1ULL << LuaParser::ELLIPSIS)
      | (1ULL << LuaParser::LP)
      | (1ULL << LuaParser::LB)
      | (1ULL << LuaParser::NAME)
      | (1ULL << LuaParser::NORMALSTRING)
      | (1ULL << LuaParser::CHARSTRING)
      | (1ULL << LuaParser::LONGSTRING)
      | (1ULL << LuaParser::INT)
      | (1ULL << LuaParser::HEX)
      | (1ULL << LuaParser::FLOAT)
      | (1ULL << LuaParser::HEX_FLOAT))) != 0)) {
      setState(207);
      explist();
    }
    setState(211);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == LuaParser::SEMI) {
      setState(210);
      match(LuaParser::SEMI);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LabelContext ------------------------------------------------------------------

LuaParser::LabelContext::LabelContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> LuaParser::LabelContext::DCOLON() {
  return getTokens(LuaParser::DCOLON);
}

tree::TerminalNode* LuaParser::LabelContext::DCOLON(size_t i) {
  return getToken(LuaParser::DCOLON, i);
}

tree::TerminalNode* LuaParser::LabelContext::NAME() {
  return getToken(LuaParser::NAME, 0);
}


size_t LuaParser::LabelContext::getRuleIndex() const {
  return LuaParser::RuleLabel;
}

antlrcpp::Any LuaParser::LabelContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitLabel(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::LabelContext* LuaParser::label() {
  LabelContext *_localctx = _tracker.createInstance<LabelContext>(_ctx, getState());
  enterRule(_localctx, 32, LuaParser::RuleLabel);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(213);
    match(LuaParser::DCOLON);
    setState(214);
    match(LuaParser::NAME);
    setState(215);
    match(LuaParser::DCOLON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncnameContext ------------------------------------------------------------------

LuaParser::FuncnameContext::FuncnameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> LuaParser::FuncnameContext::NAME() {
  return getTokens(LuaParser::NAME);
}

tree::TerminalNode* LuaParser::FuncnameContext::NAME(size_t i) {
  return getToken(LuaParser::NAME, i);
}

std::vector<tree::TerminalNode *> LuaParser::FuncnameContext::DOT() {
  return getTokens(LuaParser::DOT);
}

tree::TerminalNode* LuaParser::FuncnameContext::DOT(size_t i) {
  return getToken(LuaParser::DOT, i);
}

tree::TerminalNode* LuaParser::FuncnameContext::COLON() {
  return getToken(LuaParser::COLON, 0);
}


size_t LuaParser::FuncnameContext::getRuleIndex() const {
  return LuaParser::RuleFuncname;
}

antlrcpp::Any LuaParser::FuncnameContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitFuncname(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::FuncnameContext* LuaParser::funcname() {
  FuncnameContext *_localctx = _tracker.createInstance<FuncnameContext>(_ctx, getState());
  enterRule(_localctx, 34, LuaParser::RuleFuncname);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(217);
    match(LuaParser::NAME);
    setState(222);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == LuaParser::DOT) {
      setState(218);
      match(LuaParser::DOT);
      setState(219);
      match(LuaParser::NAME);
      setState(224);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(227);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == LuaParser::COLON) {
      setState(225);
      match(LuaParser::COLON);
      setState(226);
      match(LuaParser::NAME);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarlistContext ------------------------------------------------------------------

LuaParser::VarlistContext::VarlistContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<LuaParser::VarContext *> LuaParser::VarlistContext::var() {
  return getRuleContexts<LuaParser::VarContext>();
}

LuaParser::VarContext* LuaParser::VarlistContext::var(size_t i) {
  return getRuleContext<LuaParser::VarContext>(i);
}

std::vector<tree::TerminalNode *> LuaParser::VarlistContext::COMMA() {
  return getTokens(LuaParser::COMMA);
}

tree::TerminalNode* LuaParser::VarlistContext::COMMA(size_t i) {
  return getToken(LuaParser::COMMA, i);
}


size_t LuaParser::VarlistContext::getRuleIndex() const {
  return LuaParser::RuleVarlist;
}

antlrcpp::Any LuaParser::VarlistContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitVarlist(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::VarlistContext* LuaParser::varlist() {
  VarlistContext *_localctx = _tracker.createInstance<VarlistContext>(_ctx, getState());
  enterRule(_localctx, 36, LuaParser::RuleVarlist);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(229);
    var();
    setState(234);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == LuaParser::COMMA) {
      setState(230);
      match(LuaParser::COMMA);
      setState(231);
      var();
      setState(236);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NamelistContext ------------------------------------------------------------------

LuaParser::NamelistContext::NamelistContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> LuaParser::NamelistContext::NAME() {
  return getTokens(LuaParser::NAME);
}

tree::TerminalNode* LuaParser::NamelistContext::NAME(size_t i) {
  return getToken(LuaParser::NAME, i);
}

std::vector<tree::TerminalNode *> LuaParser::NamelistContext::COMMA() {
  return getTokens(LuaParser::COMMA);
}

tree::TerminalNode* LuaParser::NamelistContext::COMMA(size_t i) {
  return getToken(LuaParser::COMMA, i);
}


size_t LuaParser::NamelistContext::getRuleIndex() const {
  return LuaParser::RuleNamelist;
}

antlrcpp::Any LuaParser::NamelistContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitNamelist(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::NamelistContext* LuaParser::namelist() {
  NamelistContext *_localctx = _tracker.createInstance<NamelistContext>(_ctx, getState());
  enterRule(_localctx, 38, LuaParser::RuleNamelist);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(237);
    match(LuaParser::NAME);
    setState(242);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(238);
        match(LuaParser::COMMA);
        setState(239);
        match(LuaParser::NAME); 
      }
      setState(244);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AttnamelistContext ------------------------------------------------------------------

LuaParser::AttnamelistContext::AttnamelistContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> LuaParser::AttnamelistContext::NAME() {
  return getTokens(LuaParser::NAME);
}

tree::TerminalNode* LuaParser::AttnamelistContext::NAME(size_t i) {
  return getToken(LuaParser::NAME, i);
}

std::vector<LuaParser::AttribContext *> LuaParser::AttnamelistContext::attrib() {
  return getRuleContexts<LuaParser::AttribContext>();
}

LuaParser::AttribContext* LuaParser::AttnamelistContext::attrib(size_t i) {
  return getRuleContext<LuaParser::AttribContext>(i);
}

std::vector<tree::TerminalNode *> LuaParser::AttnamelistContext::COMMA() {
  return getTokens(LuaParser::COMMA);
}

tree::TerminalNode* LuaParser::AttnamelistContext::COMMA(size_t i) {
  return getToken(LuaParser::COMMA, i);
}


size_t LuaParser::AttnamelistContext::getRuleIndex() const {
  return LuaParser::RuleAttnamelist;
}

antlrcpp::Any LuaParser::AttnamelistContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitAttnamelist(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::AttnamelistContext* LuaParser::attnamelist() {
  AttnamelistContext *_localctx = _tracker.createInstance<AttnamelistContext>(_ctx, getState());
  enterRule(_localctx, 40, LuaParser::RuleAttnamelist);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(245);
    match(LuaParser::NAME);
    setState(246);
    attrib();
    setState(252);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == LuaParser::COMMA) {
      setState(247);
      match(LuaParser::COMMA);
      setState(248);
      match(LuaParser::NAME);
      setState(249);
      attrib();
      setState(254);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AttribContext ------------------------------------------------------------------

LuaParser::AttribContext::AttribContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::AttribContext::LT() {
  return getToken(LuaParser::LT, 0);
}

tree::TerminalNode* LuaParser::AttribContext::NAME() {
  return getToken(LuaParser::NAME, 0);
}

tree::TerminalNode* LuaParser::AttribContext::GT() {
  return getToken(LuaParser::GT, 0);
}


size_t LuaParser::AttribContext::getRuleIndex() const {
  return LuaParser::RuleAttrib;
}

antlrcpp::Any LuaParser::AttribContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitAttrib(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::AttribContext* LuaParser::attrib() {
  AttribContext *_localctx = _tracker.createInstance<AttribContext>(_ctx, getState());
  enterRule(_localctx, 42, LuaParser::RuleAttrib);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(258);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == LuaParser::LT) {
      setState(255);
      match(LuaParser::LT);
      setState(256);
      match(LuaParser::NAME);
      setState(257);
      match(LuaParser::GT);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExplistContext ------------------------------------------------------------------

LuaParser::ExplistContext::ExplistContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<LuaParser::ExpContext *> LuaParser::ExplistContext::exp() {
  return getRuleContexts<LuaParser::ExpContext>();
}

LuaParser::ExpContext* LuaParser::ExplistContext::exp(size_t i) {
  return getRuleContext<LuaParser::ExpContext>(i);
}

std::vector<tree::TerminalNode *> LuaParser::ExplistContext::COMMA() {
  return getTokens(LuaParser::COMMA);
}

tree::TerminalNode* LuaParser::ExplistContext::COMMA(size_t i) {
  return getToken(LuaParser::COMMA, i);
}


size_t LuaParser::ExplistContext::getRuleIndex() const {
  return LuaParser::RuleExplist;
}

antlrcpp::Any LuaParser::ExplistContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitExplist(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::ExplistContext* LuaParser::explist() {
  ExplistContext *_localctx = _tracker.createInstance<ExplistContext>(_ctx, getState());
  enterRule(_localctx, 44, LuaParser::RuleExplist);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(260);
    exp(0);
    setState(265);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == LuaParser::COMMA) {
      setState(261);
      match(LuaParser::COMMA);
      setState(262);
      exp(0);
      setState(267);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpContext ------------------------------------------------------------------

LuaParser::ExpContext::ExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::ExpContext::NIL() {
  return getToken(LuaParser::NIL, 0);
}

tree::TerminalNode* LuaParser::ExpContext::FALSE() {
  return getToken(LuaParser::FALSE, 0);
}

tree::TerminalNode* LuaParser::ExpContext::TRUE() {
  return getToken(LuaParser::TRUE, 0);
}

LuaParser::NumberContext* LuaParser::ExpContext::number() {
  return getRuleContext<LuaParser::NumberContext>(0);
}

LuaParser::StringContext* LuaParser::ExpContext::string() {
  return getRuleContext<LuaParser::StringContext>(0);
}

tree::TerminalNode* LuaParser::ExpContext::ELLIPSIS() {
  return getToken(LuaParser::ELLIPSIS, 0);
}

LuaParser::PrefixexpContext* LuaParser::ExpContext::prefixexp() {
  return getRuleContext<LuaParser::PrefixexpContext>(0);
}

LuaParser::FunctiondefContext* LuaParser::ExpContext::functiondef() {
  return getRuleContext<LuaParser::FunctiondefContext>(0);
}

LuaParser::TableconstructorContext* LuaParser::ExpContext::tableconstructor() {
  return getRuleContext<LuaParser::TableconstructorContext>(0);
}

LuaParser::UnaryOperatorContext* LuaParser::ExpContext::unaryOperator() {
  return getRuleContext<LuaParser::UnaryOperatorContext>(0);
}

std::vector<LuaParser::ExpContext *> LuaParser::ExpContext::exp() {
  return getRuleContexts<LuaParser::ExpContext>();
}

LuaParser::ExpContext* LuaParser::ExpContext::exp(size_t i) {
  return getRuleContext<LuaParser::ExpContext>(i);
}

LuaParser::LinkOperatorContext* LuaParser::ExpContext::linkOperator() {
  return getRuleContext<LuaParser::LinkOperatorContext>(0);
}


size_t LuaParser::ExpContext::getRuleIndex() const {
  return LuaParser::RuleExp;
}

antlrcpp::Any LuaParser::ExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitExp(this);
  else
    return visitor->visitChildren(this);
}


LuaParser::ExpContext* LuaParser::exp() {
   return exp(0);
}

LuaParser::ExpContext* LuaParser::exp(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  LuaParser::ExpContext *_localctx = _tracker.createInstance<ExpContext>(_ctx, parentState);
  LuaParser::ExpContext *previousContext = _localctx;
  size_t startState = 46;
  enterRecursionRule(_localctx, 46, LuaParser::RuleExp, precedence);

    

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(281);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case LuaParser::NIL: {
        setState(269);
        match(LuaParser::NIL);
        break;
      }

      case LuaParser::FALSE: {
        setState(270);
        match(LuaParser::FALSE);
        break;
      }

      case LuaParser::TRUE: {
        setState(271);
        match(LuaParser::TRUE);
        break;
      }

      case LuaParser::INT:
      case LuaParser::HEX:
      case LuaParser::FLOAT:
      case LuaParser::HEX_FLOAT: {
        setState(272);
        number();
        break;
      }

      case LuaParser::NORMALSTRING:
      case LuaParser::CHARSTRING:
      case LuaParser::LONGSTRING: {
        setState(273);
        string();
        break;
      }

      case LuaParser::ELLIPSIS: {
        setState(274);
        match(LuaParser::ELLIPSIS);
        break;
      }

      case LuaParser::LP:
      case LuaParser::NAME: {
        setState(275);
        prefixexp();
        break;
      }

      case LuaParser::FUNCTION: {
        setState(276);
        functiondef();
        break;
      }

      case LuaParser::LB: {
        setState(277);
        tableconstructor();
        break;
      }

      case LuaParser::T__8:
      case LuaParser::T__15:
      case LuaParser::T__19:
      case LuaParser::T__20: {
        setState(278);
        unaryOperator();
        setState(279);
        exp(2);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(289);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 22, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExp);
        setState(283);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(284);
        linkOperator();
        setState(285);
        exp(2); 
      }
      setState(291);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 22, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- PrefixexpContext ------------------------------------------------------------------

LuaParser::PrefixexpContext::PrefixexpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

LuaParser::VarOrExpContext* LuaParser::PrefixexpContext::varOrExp() {
  return getRuleContext<LuaParser::VarOrExpContext>(0);
}

std::vector<LuaParser::NameAndArgsContext *> LuaParser::PrefixexpContext::nameAndArgs() {
  return getRuleContexts<LuaParser::NameAndArgsContext>();
}

LuaParser::NameAndArgsContext* LuaParser::PrefixexpContext::nameAndArgs(size_t i) {
  return getRuleContext<LuaParser::NameAndArgsContext>(i);
}


size_t LuaParser::PrefixexpContext::getRuleIndex() const {
  return LuaParser::RulePrefixexp;
}

antlrcpp::Any LuaParser::PrefixexpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitPrefixexp(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::PrefixexpContext* LuaParser::prefixexp() {
  PrefixexpContext *_localctx = _tracker.createInstance<PrefixexpContext>(_ctx, getState());
  enterRule(_localctx, 48, LuaParser::RulePrefixexp);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(292);
    varOrExp();
    setState(296);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(293);
        nameAndArgs(); 
      }
      setState(298);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarOrExpContext ------------------------------------------------------------------

LuaParser::VarOrExpContext::VarOrExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

LuaParser::VarContext* LuaParser::VarOrExpContext::var() {
  return getRuleContext<LuaParser::VarContext>(0);
}

tree::TerminalNode* LuaParser::VarOrExpContext::LP() {
  return getToken(LuaParser::LP, 0);
}

LuaParser::ExpContext* LuaParser::VarOrExpContext::exp() {
  return getRuleContext<LuaParser::ExpContext>(0);
}

tree::TerminalNode* LuaParser::VarOrExpContext::RP() {
  return getToken(LuaParser::RP, 0);
}


size_t LuaParser::VarOrExpContext::getRuleIndex() const {
  return LuaParser::RuleVarOrExp;
}

antlrcpp::Any LuaParser::VarOrExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitVarOrExp(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::VarOrExpContext* LuaParser::varOrExp() {
  VarOrExpContext *_localctx = _tracker.createInstance<VarOrExpContext>(_ctx, getState());
  enterRule(_localctx, 50, LuaParser::RuleVarOrExp);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(304);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 24, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(299);
      var();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(300);
      match(LuaParser::LP);
      setState(301);
      exp(0);
      setState(302);
      match(LuaParser::RP);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarContext ------------------------------------------------------------------

LuaParser::VarContext::VarContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::VarContext::NAME() {
  return getToken(LuaParser::NAME, 0);
}

tree::TerminalNode* LuaParser::VarContext::LP() {
  return getToken(LuaParser::LP, 0);
}

LuaParser::ExpContext* LuaParser::VarContext::exp() {
  return getRuleContext<LuaParser::ExpContext>(0);
}

tree::TerminalNode* LuaParser::VarContext::RP() {
  return getToken(LuaParser::RP, 0);
}

std::vector<LuaParser::VarSuffixContext *> LuaParser::VarContext::varSuffix() {
  return getRuleContexts<LuaParser::VarSuffixContext>();
}

LuaParser::VarSuffixContext* LuaParser::VarContext::varSuffix(size_t i) {
  return getRuleContext<LuaParser::VarSuffixContext>(i);
}


size_t LuaParser::VarContext::getRuleIndex() const {
  return LuaParser::RuleVar;
}

antlrcpp::Any LuaParser::VarContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitVar(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::VarContext* LuaParser::var() {
  VarContext *_localctx = _tracker.createInstance<VarContext>(_ctx, getState());
  enterRule(_localctx, 52, LuaParser::RuleVar);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(312);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case LuaParser::NAME: {
        setState(306);
        match(LuaParser::NAME);
        break;
      }

      case LuaParser::LP: {
        setState(307);
        match(LuaParser::LP);
        setState(308);
        exp(0);
        setState(309);
        match(LuaParser::RP);
        setState(310);
        varSuffix();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(317);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 26, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(314);
        varSuffix(); 
      }
      setState(319);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 26, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarSuffixContext ------------------------------------------------------------------

LuaParser::VarSuffixContext::VarSuffixContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::VarSuffixContext::LSB() {
  return getToken(LuaParser::LSB, 0);
}

LuaParser::ExpContext* LuaParser::VarSuffixContext::exp() {
  return getRuleContext<LuaParser::ExpContext>(0);
}

tree::TerminalNode* LuaParser::VarSuffixContext::RSB() {
  return getToken(LuaParser::RSB, 0);
}

tree::TerminalNode* LuaParser::VarSuffixContext::DOT() {
  return getToken(LuaParser::DOT, 0);
}

tree::TerminalNode* LuaParser::VarSuffixContext::NAME() {
  return getToken(LuaParser::NAME, 0);
}

std::vector<LuaParser::NameAndArgsContext *> LuaParser::VarSuffixContext::nameAndArgs() {
  return getRuleContexts<LuaParser::NameAndArgsContext>();
}

LuaParser::NameAndArgsContext* LuaParser::VarSuffixContext::nameAndArgs(size_t i) {
  return getRuleContext<LuaParser::NameAndArgsContext>(i);
}


size_t LuaParser::VarSuffixContext::getRuleIndex() const {
  return LuaParser::RuleVarSuffix;
}

antlrcpp::Any LuaParser::VarSuffixContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitVarSuffix(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::VarSuffixContext* LuaParser::varSuffix() {
  VarSuffixContext *_localctx = _tracker.createInstance<VarSuffixContext>(_ctx, getState());
  enterRule(_localctx, 54, LuaParser::RuleVarSuffix);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(323);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << LuaParser::COLON)
      | (1ULL << LuaParser::LP)
      | (1ULL << LuaParser::LB)
      | (1ULL << LuaParser::NORMALSTRING)
      | (1ULL << LuaParser::CHARSTRING)
      | (1ULL << LuaParser::LONGSTRING))) != 0)) {
      setState(320);
      nameAndArgs();
      setState(325);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(332);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case LuaParser::LSB: {
        setState(326);
        match(LuaParser::LSB);
        setState(327);
        exp(0);
        setState(328);
        match(LuaParser::RSB);
        break;
      }

      case LuaParser::DOT: {
        setState(330);
        match(LuaParser::DOT);
        setState(331);
        match(LuaParser::NAME);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NameAndArgsContext ------------------------------------------------------------------

LuaParser::NameAndArgsContext::NameAndArgsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

LuaParser::ArgsContext* LuaParser::NameAndArgsContext::args() {
  return getRuleContext<LuaParser::ArgsContext>(0);
}

tree::TerminalNode* LuaParser::NameAndArgsContext::COLON() {
  return getToken(LuaParser::COLON, 0);
}

tree::TerminalNode* LuaParser::NameAndArgsContext::NAME() {
  return getToken(LuaParser::NAME, 0);
}


size_t LuaParser::NameAndArgsContext::getRuleIndex() const {
  return LuaParser::RuleNameAndArgs;
}

antlrcpp::Any LuaParser::NameAndArgsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitNameAndArgs(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::NameAndArgsContext* LuaParser::nameAndArgs() {
  NameAndArgsContext *_localctx = _tracker.createInstance<NameAndArgsContext>(_ctx, getState());
  enterRule(_localctx, 56, LuaParser::RuleNameAndArgs);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(336);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == LuaParser::COLON) {
      setState(334);
      match(LuaParser::COLON);
      setState(335);
      match(LuaParser::NAME);
    }
    setState(338);
    args();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ArgsContext ------------------------------------------------------------------

LuaParser::ArgsContext::ArgsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::ArgsContext::LP() {
  return getToken(LuaParser::LP, 0);
}

tree::TerminalNode* LuaParser::ArgsContext::RP() {
  return getToken(LuaParser::RP, 0);
}

LuaParser::ExplistContext* LuaParser::ArgsContext::explist() {
  return getRuleContext<LuaParser::ExplistContext>(0);
}

LuaParser::TableconstructorContext* LuaParser::ArgsContext::tableconstructor() {
  return getRuleContext<LuaParser::TableconstructorContext>(0);
}

LuaParser::StringContext* LuaParser::ArgsContext::string() {
  return getRuleContext<LuaParser::StringContext>(0);
}


size_t LuaParser::ArgsContext::getRuleIndex() const {
  return LuaParser::RuleArgs;
}

antlrcpp::Any LuaParser::ArgsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitArgs(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::ArgsContext* LuaParser::args() {
  ArgsContext *_localctx = _tracker.createInstance<ArgsContext>(_ctx, getState());
  enterRule(_localctx, 58, LuaParser::RuleArgs);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(347);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case LuaParser::LP: {
        enterOuterAlt(_localctx, 1);
        setState(340);
        match(LuaParser::LP);
        setState(342);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << LuaParser::T__8)
          | (1ULL << LuaParser::T__15)
          | (1ULL << LuaParser::T__19)
          | (1ULL << LuaParser::T__20)
          | (1ULL << LuaParser::FUNCTION)
          | (1ULL << LuaParser::TRUE)
          | (1ULL << LuaParser::FALSE)
          | (1ULL << LuaParser::NIL)
          | (1ULL << LuaParser::ELLIPSIS)
          | (1ULL << LuaParser::LP)
          | (1ULL << LuaParser::LB)
          | (1ULL << LuaParser::NAME)
          | (1ULL << LuaParser::NORMALSTRING)
          | (1ULL << LuaParser::CHARSTRING)
          | (1ULL << LuaParser::LONGSTRING)
          | (1ULL << LuaParser::INT)
          | (1ULL << LuaParser::HEX)
          | (1ULL << LuaParser::FLOAT)
          | (1ULL << LuaParser::HEX_FLOAT))) != 0)) {
          setState(341);
          explist();
        }
        setState(344);
        match(LuaParser::RP);
        break;
      }

      case LuaParser::LB: {
        enterOuterAlt(_localctx, 2);
        setState(345);
        tableconstructor();
        break;
      }

      case LuaParser::NORMALSTRING:
      case LuaParser::CHARSTRING:
      case LuaParser::LONGSTRING: {
        enterOuterAlt(_localctx, 3);
        setState(346);
        string();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FunctiondefContext ------------------------------------------------------------------

LuaParser::FunctiondefContext::FunctiondefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::FunctiondefContext::FUNCTION() {
  return getToken(LuaParser::FUNCTION, 0);
}

LuaParser::FuncbodyContext* LuaParser::FunctiondefContext::funcbody() {
  return getRuleContext<LuaParser::FuncbodyContext>(0);
}


size_t LuaParser::FunctiondefContext::getRuleIndex() const {
  return LuaParser::RuleFunctiondef;
}

antlrcpp::Any LuaParser::FunctiondefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitFunctiondef(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::FunctiondefContext* LuaParser::functiondef() {
  FunctiondefContext *_localctx = _tracker.createInstance<FunctiondefContext>(_ctx, getState());
  enterRule(_localctx, 60, LuaParser::RuleFunctiondef);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(349);
    match(LuaParser::FUNCTION);
    setState(350);
    funcbody();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncbodyContext ------------------------------------------------------------------

LuaParser::FuncbodyContext::FuncbodyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::FuncbodyContext::LP() {
  return getToken(LuaParser::LP, 0);
}

tree::TerminalNode* LuaParser::FuncbodyContext::RP() {
  return getToken(LuaParser::RP, 0);
}

LuaParser::BlockContext* LuaParser::FuncbodyContext::block() {
  return getRuleContext<LuaParser::BlockContext>(0);
}

tree::TerminalNode* LuaParser::FuncbodyContext::END() {
  return getToken(LuaParser::END, 0);
}

LuaParser::ParlistContext* LuaParser::FuncbodyContext::parlist() {
  return getRuleContext<LuaParser::ParlistContext>(0);
}


size_t LuaParser::FuncbodyContext::getRuleIndex() const {
  return LuaParser::RuleFuncbody;
}

antlrcpp::Any LuaParser::FuncbodyContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitFuncbody(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::FuncbodyContext* LuaParser::funcbody() {
  FuncbodyContext *_localctx = _tracker.createInstance<FuncbodyContext>(_ctx, getState());
  enterRule(_localctx, 62, LuaParser::RuleFuncbody);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(352);
    match(LuaParser::LP);
    setState(354);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == LuaParser::ELLIPSIS

    || _la == LuaParser::NAME) {
      setState(353);
      parlist();
    }
    setState(356);
    match(LuaParser::RP);
    setState(357);
    block();
    setState(358);
    match(LuaParser::END);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParlistContext ------------------------------------------------------------------

LuaParser::ParlistContext::ParlistContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

LuaParser::NamelistContext* LuaParser::ParlistContext::namelist() {
  return getRuleContext<LuaParser::NamelistContext>(0);
}

tree::TerminalNode* LuaParser::ParlistContext::COMMA() {
  return getToken(LuaParser::COMMA, 0);
}

tree::TerminalNode* LuaParser::ParlistContext::ELLIPSIS() {
  return getToken(LuaParser::ELLIPSIS, 0);
}


size_t LuaParser::ParlistContext::getRuleIndex() const {
  return LuaParser::RuleParlist;
}

antlrcpp::Any LuaParser::ParlistContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitParlist(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::ParlistContext* LuaParser::parlist() {
  ParlistContext *_localctx = _tracker.createInstance<ParlistContext>(_ctx, getState());
  enterRule(_localctx, 64, LuaParser::RuleParlist);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(366);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case LuaParser::NAME: {
        enterOuterAlt(_localctx, 1);
        setState(360);
        namelist();
        setState(363);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == LuaParser::COMMA) {
          setState(361);
          match(LuaParser::COMMA);
          setState(362);
          match(LuaParser::ELLIPSIS);
        }
        break;
      }

      case LuaParser::ELLIPSIS: {
        enterOuterAlt(_localctx, 2);
        setState(365);
        match(LuaParser::ELLIPSIS);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TableconstructorContext ------------------------------------------------------------------

LuaParser::TableconstructorContext::TableconstructorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::TableconstructorContext::LB() {
  return getToken(LuaParser::LB, 0);
}

tree::TerminalNode* LuaParser::TableconstructorContext::RB() {
  return getToken(LuaParser::RB, 0);
}

LuaParser::FieldlistContext* LuaParser::TableconstructorContext::fieldlist() {
  return getRuleContext<LuaParser::FieldlistContext>(0);
}


size_t LuaParser::TableconstructorContext::getRuleIndex() const {
  return LuaParser::RuleTableconstructor;
}

antlrcpp::Any LuaParser::TableconstructorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitTableconstructor(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::TableconstructorContext* LuaParser::tableconstructor() {
  TableconstructorContext *_localctx = _tracker.createInstance<TableconstructorContext>(_ctx, getState());
  enterRule(_localctx, 66, LuaParser::RuleTableconstructor);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(368);
    match(LuaParser::LB);
    setState(370);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << LuaParser::T__8)
      | (1ULL << LuaParser::T__15)
      | (1ULL << LuaParser::T__19)
      | (1ULL << LuaParser::T__20)
      | (1ULL << LuaParser::FUNCTION)
      | (1ULL << LuaParser::TRUE)
      | (1ULL << LuaParser::FALSE)
      | (1ULL << LuaParser::NIL)
      | (1ULL << LuaParser::ELLIPSIS)
      | (1ULL << LuaParser::LP)
      | (1ULL << LuaParser::LB)
      | (1ULL << LuaParser::LSB)
      | (1ULL << LuaParser::NAME)
      | (1ULL << LuaParser::NORMALSTRING)
      | (1ULL << LuaParser::CHARSTRING)
      | (1ULL << LuaParser::LONGSTRING)
      | (1ULL << LuaParser::INT)
      | (1ULL << LuaParser::HEX)
      | (1ULL << LuaParser::FLOAT)
      | (1ULL << LuaParser::HEX_FLOAT))) != 0)) {
      setState(369);
      fieldlist();
    }
    setState(372);
    match(LuaParser::RB);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FieldlistContext ------------------------------------------------------------------

LuaParser::FieldlistContext::FieldlistContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<LuaParser::FieldContext *> LuaParser::FieldlistContext::field() {
  return getRuleContexts<LuaParser::FieldContext>();
}

LuaParser::FieldContext* LuaParser::FieldlistContext::field(size_t i) {
  return getRuleContext<LuaParser::FieldContext>(i);
}

std::vector<LuaParser::FieldsepContext *> LuaParser::FieldlistContext::fieldsep() {
  return getRuleContexts<LuaParser::FieldsepContext>();
}

LuaParser::FieldsepContext* LuaParser::FieldlistContext::fieldsep(size_t i) {
  return getRuleContext<LuaParser::FieldsepContext>(i);
}


size_t LuaParser::FieldlistContext::getRuleIndex() const {
  return LuaParser::RuleFieldlist;
}

antlrcpp::Any LuaParser::FieldlistContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitFieldlist(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::FieldlistContext* LuaParser::fieldlist() {
  FieldlistContext *_localctx = _tracker.createInstance<FieldlistContext>(_ctx, getState());
  enterRule(_localctx, 68, LuaParser::RuleFieldlist);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(374);
    field();
    setState(380);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(375);
        fieldsep();
        setState(376);
        field(); 
      }
      setState(382);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 36, _ctx);
    }
    setState(384);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == LuaParser::SEMI

    || _la == LuaParser::COMMA) {
      setState(383);
      fieldsep();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FieldContext ------------------------------------------------------------------

LuaParser::FieldContext::FieldContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::FieldContext::LSB() {
  return getToken(LuaParser::LSB, 0);
}

std::vector<LuaParser::ExpContext *> LuaParser::FieldContext::exp() {
  return getRuleContexts<LuaParser::ExpContext>();
}

LuaParser::ExpContext* LuaParser::FieldContext::exp(size_t i) {
  return getRuleContext<LuaParser::ExpContext>(i);
}

tree::TerminalNode* LuaParser::FieldContext::RSB() {
  return getToken(LuaParser::RSB, 0);
}

tree::TerminalNode* LuaParser::FieldContext::EQL() {
  return getToken(LuaParser::EQL, 0);
}

tree::TerminalNode* LuaParser::FieldContext::NAME() {
  return getToken(LuaParser::NAME, 0);
}


size_t LuaParser::FieldContext::getRuleIndex() const {
  return LuaParser::RuleField;
}

antlrcpp::Any LuaParser::FieldContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitField(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::FieldContext* LuaParser::field() {
  FieldContext *_localctx = _tracker.createInstance<FieldContext>(_ctx, getState());
  enterRule(_localctx, 70, LuaParser::RuleField);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(396);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(386);
      match(LuaParser::LSB);
      setState(387);
      exp(0);
      setState(388);
      match(LuaParser::RSB);
      setState(389);
      match(LuaParser::EQL);
      setState(390);
      exp(0);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(392);
      match(LuaParser::NAME);
      setState(393);
      match(LuaParser::EQL);
      setState(394);
      exp(0);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(395);
      exp(0);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FieldsepContext ------------------------------------------------------------------

LuaParser::FieldsepContext::FieldsepContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::FieldsepContext::COMMA() {
  return getToken(LuaParser::COMMA, 0);
}

tree::TerminalNode* LuaParser::FieldsepContext::SEMI() {
  return getToken(LuaParser::SEMI, 0);
}


size_t LuaParser::FieldsepContext::getRuleIndex() const {
  return LuaParser::RuleFieldsep;
}

antlrcpp::Any LuaParser::FieldsepContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitFieldsep(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::FieldsepContext* LuaParser::fieldsep() {
  FieldsepContext *_localctx = _tracker.createInstance<FieldsepContext>(_ctx, getState());
  enterRule(_localctx, 72, LuaParser::RuleFieldsep);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(398);
    _la = _input->LA(1);
    if (!(_la == LuaParser::SEMI

    || _la == LuaParser::COMMA)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LinkOperatorContext ------------------------------------------------------------------

LuaParser::LinkOperatorContext::LinkOperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t LuaParser::LinkOperatorContext::getRuleIndex() const {
  return LuaParser::RuleLinkOperator;
}

antlrcpp::Any LuaParser::LinkOperatorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitLinkOperator(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::LinkOperatorContext* LuaParser::linkOperator() {
  LinkOperatorContext *_localctx = _tracker.createInstance<LinkOperatorContext>(_ctx, getState());
  enterRule(_localctx, 74, LuaParser::RuleLinkOperator);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(400);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << LuaParser::T__0)
      | (1ULL << LuaParser::T__1)
      | (1ULL << LuaParser::T__2)
      | (1ULL << LuaParser::T__3)
      | (1ULL << LuaParser::T__4)
      | (1ULL << LuaParser::T__5)
      | (1ULL << LuaParser::T__6)
      | (1ULL << LuaParser::T__7)
      | (1ULL << LuaParser::T__8)
      | (1ULL << LuaParser::T__9)
      | (1ULL << LuaParser::T__10)
      | (1ULL << LuaParser::T__11)
      | (1ULL << LuaParser::T__12)
      | (1ULL << LuaParser::T__13)
      | (1ULL << LuaParser::T__14)
      | (1ULL << LuaParser::T__15)
      | (1ULL << LuaParser::T__16)
      | (1ULL << LuaParser::T__17)
      | (1ULL << LuaParser::T__18)
      | (1ULL << LuaParser::LT)
      | (1ULL << LuaParser::GT))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnaryOperatorContext ------------------------------------------------------------------

LuaParser::UnaryOperatorContext::UnaryOperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t LuaParser::UnaryOperatorContext::getRuleIndex() const {
  return LuaParser::RuleUnaryOperator;
}

antlrcpp::Any LuaParser::UnaryOperatorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitUnaryOperator(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::UnaryOperatorContext* LuaParser::unaryOperator() {
  UnaryOperatorContext *_localctx = _tracker.createInstance<UnaryOperatorContext>(_ctx, getState());
  enterRule(_localctx, 76, LuaParser::RuleUnaryOperator);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(402);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << LuaParser::T__8)
      | (1ULL << LuaParser::T__15)
      | (1ULL << LuaParser::T__19)
      | (1ULL << LuaParser::T__20))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NumberContext ------------------------------------------------------------------

LuaParser::NumberContext::NumberContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::NumberContext::INT() {
  return getToken(LuaParser::INT, 0);
}

tree::TerminalNode* LuaParser::NumberContext::HEX() {
  return getToken(LuaParser::HEX, 0);
}

tree::TerminalNode* LuaParser::NumberContext::FLOAT() {
  return getToken(LuaParser::FLOAT, 0);
}

tree::TerminalNode* LuaParser::NumberContext::HEX_FLOAT() {
  return getToken(LuaParser::HEX_FLOAT, 0);
}


size_t LuaParser::NumberContext::getRuleIndex() const {
  return LuaParser::RuleNumber;
}

antlrcpp::Any LuaParser::NumberContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitNumber(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::NumberContext* LuaParser::number() {
  NumberContext *_localctx = _tracker.createInstance<NumberContext>(_ctx, getState());
  enterRule(_localctx, 78, LuaParser::RuleNumber);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(404);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << LuaParser::INT)
      | (1ULL << LuaParser::HEX)
      | (1ULL << LuaParser::FLOAT)
      | (1ULL << LuaParser::HEX_FLOAT))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StringContext ------------------------------------------------------------------

LuaParser::StringContext::StringContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* LuaParser::StringContext::NORMALSTRING() {
  return getToken(LuaParser::NORMALSTRING, 0);
}

tree::TerminalNode* LuaParser::StringContext::CHARSTRING() {
  return getToken(LuaParser::CHARSTRING, 0);
}

tree::TerminalNode* LuaParser::StringContext::LONGSTRING() {
  return getToken(LuaParser::LONGSTRING, 0);
}


size_t LuaParser::StringContext::getRuleIndex() const {
  return LuaParser::RuleString;
}

antlrcpp::Any LuaParser::StringContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<LuaVisitor*>(visitor))
    return parserVisitor->visitString(this);
  else
    return visitor->visitChildren(this);
}

LuaParser::StringContext* LuaParser::string() {
  StringContext *_localctx = _tracker.createInstance<StringContext>(_ctx, getState());
  enterRule(_localctx, 80, LuaParser::RuleString);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(406);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << LuaParser::NORMALSTRING)
      | (1ULL << LuaParser::CHARSTRING)
      | (1ULL << LuaParser::LONGSTRING))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool LuaParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 23: return expSempred(dynamic_cast<ExpContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool LuaParser::expSempred(ExpContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

// Static vars and initialization.
std::vector<dfa::DFA> LuaParser::_decisionToDFA;
atn::PredictionContextCache LuaParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN LuaParser::_atn;
std::vector<uint16_t> LuaParser::_serializedATN;

std::vector<std::string> LuaParser::_ruleNames = {
  "chunk", "block", "stat", "varDecl", "functioncall", "gotoStat", "doStat", 
  "whileStat", "repeatStat", "ifStat", "forStat", "forInStat", "funcStat", 
  "localFuncStat", "localVarDecl", "retstat", "label", "funcname", "varlist", 
  "namelist", "attnamelist", "attrib", "explist", "exp", "prefixexp", "varOrExp", 
  "var", "varSuffix", "nameAndArgs", "args", "functiondef", "funcbody", 
  "parlist", "tableconstructor", "fieldlist", "field", "fieldsep", "linkOperator", 
  "unaryOperator", "number", "string"
};

std::vector<std::string> LuaParser::_literalNames = {
  "", "'and'", "'or'", "'..'", "'<='", "'>='", "'~='", "'=='", "'+'", "'-'", 
  "'*'", "'/'", "'%'", "'//'", "'&'", "'|'", "'~'", "'<<'", "'>>'", "'^'", 
  "'not'", "'#'", "'function'", "'return'", "'local'", "'true'", "'false'", 
  "'nil'", "'break'", "'do'", "'end'", "'goto'", "'while'", "'repeat'", 
  "'until'", "'for'", "'if'", "'then'", "'else'", "'elseif'", "'in'", "';'", 
  "','", "'.'", "'='", "':'", "'::'", "'...'", "'<'", "'>'", "'('", "')'", 
  "'{'", "'}'", "'['", "']'"
};

std::vector<std::string> LuaParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
  "", "", "", "", "FUNCTION", "RETURN", "LOCAL", "TRUE", "FALSE", "NIL", 
  "BREAK", "DO", "END", "GOTO", "WHILE", "REPEAT", "UNTIL", "FOR", "IF", 
  "THEN", "ELSE", "ELSEIF", "IN", "SEMI", "COMMA", "DOT", "EQL", "COLON", 
  "DCOLON", "ELLIPSIS", "LT", "GT", "LP", "RP", "LB", "RB", "LSB", "RSB", 
  "NAME", "NORMALSTRING", "CHARSTRING", "LONGSTRING", "INT", "HEX", "FLOAT", 
  "HEX_FLOAT", "COMMENT", "LINE_COMMENT", "WS", "SHEBANG"
};

dfa::Vocabulary LuaParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> LuaParser::_tokenNames;

LuaParser::Initializer::Initializer() {
	for (size_t i = 0; i < _symbolicNames.size(); ++i) {
		std::string name = _vocabulary.getLiteralName(i);
		if (name.empty()) {
			name = _vocabulary.getSymbolicName(i);
		}

		if (name.empty()) {
			_tokenNames.push_back("<INVALID>");
		} else {
      _tokenNames.push_back(name);
    }
	}

  _serializedATN = {
    0x3, 0x608b, 0xa72a, 0x8133, 0xb9ed, 0x417c, 0x3be7, 0x7786, 0x5964, 
    0x3, 0x45, 0x19b, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 
    0x9, 0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 
    0x4, 0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x4, 0xb, 
    0x9, 0xb, 0x4, 0xc, 0x9, 0xc, 0x4, 0xd, 0x9, 0xd, 0x4, 0xe, 0x9, 0xe, 
    0x4, 0xf, 0x9, 0xf, 0x4, 0x10, 0x9, 0x10, 0x4, 0x11, 0x9, 0x11, 0x4, 
    0x12, 0x9, 0x12, 0x4, 0x13, 0x9, 0x13, 0x4, 0x14, 0x9, 0x14, 0x4, 0x15, 
    0x9, 0x15, 0x4, 0x16, 0x9, 0x16, 0x4, 0x17, 0x9, 0x17, 0x4, 0x18, 0x9, 
    0x18, 0x4, 0x19, 0x9, 0x19, 0x4, 0x1a, 0x9, 0x1a, 0x4, 0x1b, 0x9, 0x1b, 
    0x4, 0x1c, 0x9, 0x1c, 0x4, 0x1d, 0x9, 0x1d, 0x4, 0x1e, 0x9, 0x1e, 0x4, 
    0x1f, 0x9, 0x1f, 0x4, 0x20, 0x9, 0x20, 0x4, 0x21, 0x9, 0x21, 0x4, 0x22, 
    0x9, 0x22, 0x4, 0x23, 0x9, 0x23, 0x4, 0x24, 0x9, 0x24, 0x4, 0x25, 0x9, 
    0x25, 0x4, 0x26, 0x9, 0x26, 0x4, 0x27, 0x9, 0x27, 0x4, 0x28, 0x9, 0x28, 
    0x4, 0x29, 0x9, 0x29, 0x4, 0x2a, 0x9, 0x2a, 0x3, 0x2, 0x3, 0x2, 0x3, 
    0x2, 0x3, 0x3, 0x7, 0x3, 0x59, 0xa, 0x3, 0xc, 0x3, 0xe, 0x3, 0x5c, 0xb, 
    0x3, 0x3, 0x3, 0x5, 0x3, 0x5f, 0xa, 0x3, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 
    0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 
    0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x5, 0x4, 0x70, 0xa, 
    0x4, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x5, 0x5, 0x76, 0xa, 0x5, 
    0x3, 0x6, 0x3, 0x6, 0x6, 0x6, 0x7a, 0xa, 0x6, 0xd, 0x6, 0xe, 0x6, 0x7b, 
    0x3, 0x6, 0x5, 0x6, 0x7f, 0xa, 0x6, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 
    0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 
    0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 
    0xa, 0x5, 0xa, 0x93, 0xa, 0xa, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 
    0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x7, 0xb, 0x9e, 0xa, 
    0xb, 0xc, 0xb, 0xe, 0xb, 0xa1, 0xb, 0xb, 0x3, 0xb, 0x3, 0xb, 0x5, 0xb, 
    0xa5, 0xa, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 
    0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x5, 0xc, 0xb1, 0xa, 0xc, 
    0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 
    0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xe, 0x3, 0xe, 
    0x3, 0xe, 0x3, 0xe, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 
    0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x5, 0x10, 0xcc, 0xa, 0x10, 
    0x3, 0x10, 0x5, 0x10, 0xcf, 0xa, 0x10, 0x3, 0x11, 0x3, 0x11, 0x5, 0x11, 
    0xd3, 0xa, 0x11, 0x3, 0x11, 0x5, 0x11, 0xd6, 0xa, 0x11, 0x3, 0x12, 0x3, 
    0x12, 0x3, 0x12, 0x3, 0x12, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x7, 0x13, 
    0xdf, 0xa, 0x13, 0xc, 0x13, 0xe, 0x13, 0xe2, 0xb, 0x13, 0x3, 0x13, 0x3, 
    0x13, 0x5, 0x13, 0xe6, 0xa, 0x13, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x7, 
    0x14, 0xeb, 0xa, 0x14, 0xc, 0x14, 0xe, 0x14, 0xee, 0xb, 0x14, 0x3, 0x15, 
    0x3, 0x15, 0x3, 0x15, 0x7, 0x15, 0xf3, 0xa, 0x15, 0xc, 0x15, 0xe, 0x15, 
    0xf6, 0xb, 0x15, 0x3, 0x16, 0x3, 0x16, 0x3, 0x16, 0x3, 0x16, 0x3, 0x16, 
    0x7, 0x16, 0xfd, 0xa, 0x16, 0xc, 0x16, 0xe, 0x16, 0x100, 0xb, 0x16, 
    0x3, 0x17, 0x3, 0x17, 0x3, 0x17, 0x5, 0x17, 0x105, 0xa, 0x17, 0x3, 0x18, 
    0x3, 0x18, 0x3, 0x18, 0x7, 0x18, 0x10a, 0xa, 0x18, 0xc, 0x18, 0xe, 0x18, 
    0x10d, 0xb, 0x18, 0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 
    0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x3, 
    0x19, 0x3, 0x19, 0x5, 0x19, 0x11c, 0xa, 0x19, 0x3, 0x19, 0x3, 0x19, 
    0x3, 0x19, 0x3, 0x19, 0x7, 0x19, 0x122, 0xa, 0x19, 0xc, 0x19, 0xe, 0x19, 
    0x125, 0xb, 0x19, 0x3, 0x1a, 0x3, 0x1a, 0x7, 0x1a, 0x129, 0xa, 0x1a, 
    0xc, 0x1a, 0xe, 0x1a, 0x12c, 0xb, 0x1a, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 
    0x3, 0x1b, 0x3, 0x1b, 0x5, 0x1b, 0x133, 0xa, 0x1b, 0x3, 0x1c, 0x3, 0x1c, 
    0x3, 0x1c, 0x3, 0x1c, 0x3, 0x1c, 0x3, 0x1c, 0x5, 0x1c, 0x13b, 0xa, 0x1c, 
    0x3, 0x1c, 0x7, 0x1c, 0x13e, 0xa, 0x1c, 0xc, 0x1c, 0xe, 0x1c, 0x141, 
    0xb, 0x1c, 0x3, 0x1d, 0x7, 0x1d, 0x144, 0xa, 0x1d, 0xc, 0x1d, 0xe, 0x1d, 
    0x147, 0xb, 0x1d, 0x3, 0x1d, 0x3, 0x1d, 0x3, 0x1d, 0x3, 0x1d, 0x3, 0x1d, 
    0x3, 0x1d, 0x5, 0x1d, 0x14f, 0xa, 0x1d, 0x3, 0x1e, 0x3, 0x1e, 0x5, 0x1e, 
    0x153, 0xa, 0x1e, 0x3, 0x1e, 0x3, 0x1e, 0x3, 0x1f, 0x3, 0x1f, 0x5, 0x1f, 
    0x159, 0xa, 0x1f, 0x3, 0x1f, 0x3, 0x1f, 0x3, 0x1f, 0x5, 0x1f, 0x15e, 
    0xa, 0x1f, 0x3, 0x20, 0x3, 0x20, 0x3, 0x20, 0x3, 0x21, 0x3, 0x21, 0x5, 
    0x21, 0x165, 0xa, 0x21, 0x3, 0x21, 0x3, 0x21, 0x3, 0x21, 0x3, 0x21, 
    0x3, 0x22, 0x3, 0x22, 0x3, 0x22, 0x5, 0x22, 0x16e, 0xa, 0x22, 0x3, 0x22, 
    0x5, 0x22, 0x171, 0xa, 0x22, 0x3, 0x23, 0x3, 0x23, 0x5, 0x23, 0x175, 
    0xa, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 
    0x24, 0x7, 0x24, 0x17d, 0xa, 0x24, 0xc, 0x24, 0xe, 0x24, 0x180, 0xb, 
    0x24, 0x3, 0x24, 0x5, 0x24, 0x183, 0xa, 0x24, 0x3, 0x25, 0x3, 0x25, 
    0x3, 0x25, 0x3, 0x25, 0x3, 0x25, 0x3, 0x25, 0x3, 0x25, 0x3, 0x25, 0x3, 
    0x25, 0x3, 0x25, 0x5, 0x25, 0x18f, 0xa, 0x25, 0x3, 0x26, 0x3, 0x26, 
    0x3, 0x27, 0x3, 0x27, 0x3, 0x28, 0x3, 0x28, 0x3, 0x29, 0x3, 0x29, 0x3, 
    0x2a, 0x3, 0x2a, 0x3, 0x2a, 0x2, 0x3, 0x30, 0x2b, 0x2, 0x4, 0x6, 0x8, 
    0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x20, 
    0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 
    0x3a, 0x3c, 0x3e, 0x40, 0x42, 0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e, 0x50, 
    0x52, 0x2, 0x7, 0x3, 0x2, 0x2b, 0x2c, 0x4, 0x2, 0x3, 0x15, 0x32, 0x33, 
    0x5, 0x2, 0xb, 0xb, 0x12, 0x12, 0x16, 0x17, 0x3, 0x2, 0x3e, 0x41, 0x3, 
    0x2, 0x3b, 0x3d, 0x2, 0x1af, 0x2, 0x54, 0x3, 0x2, 0x2, 0x2, 0x4, 0x5a, 
    0x3, 0x2, 0x2, 0x2, 0x6, 0x6f, 0x3, 0x2, 0x2, 0x2, 0x8, 0x71, 0x3, 0x2, 
    0x2, 0x2, 0xa, 0x77, 0x3, 0x2, 0x2, 0x2, 0xc, 0x80, 0x3, 0x2, 0x2, 0x2, 
    0xe, 0x83, 0x3, 0x2, 0x2, 0x2, 0x10, 0x87, 0x3, 0x2, 0x2, 0x2, 0x12, 
    0x8d, 0x3, 0x2, 0x2, 0x2, 0x14, 0x94, 0x3, 0x2, 0x2, 0x2, 0x16, 0xa8, 
    0x3, 0x2, 0x2, 0x2, 0x18, 0xb6, 0x3, 0x2, 0x2, 0x2, 0x1a, 0xbe, 0x3, 
    0x2, 0x2, 0x2, 0x1c, 0xc2, 0x3, 0x2, 0x2, 0x2, 0x1e, 0xc7, 0x3, 0x2, 
    0x2, 0x2, 0x20, 0xd0, 0x3, 0x2, 0x2, 0x2, 0x22, 0xd7, 0x3, 0x2, 0x2, 
    0x2, 0x24, 0xdb, 0x3, 0x2, 0x2, 0x2, 0x26, 0xe7, 0x3, 0x2, 0x2, 0x2, 
    0x28, 0xef, 0x3, 0x2, 0x2, 0x2, 0x2a, 0xf7, 0x3, 0x2, 0x2, 0x2, 0x2c, 
    0x104, 0x3, 0x2, 0x2, 0x2, 0x2e, 0x106, 0x3, 0x2, 0x2, 0x2, 0x30, 0x11b, 
    0x3, 0x2, 0x2, 0x2, 0x32, 0x126, 0x3, 0x2, 0x2, 0x2, 0x34, 0x132, 0x3, 
    0x2, 0x2, 0x2, 0x36, 0x13a, 0x3, 0x2, 0x2, 0x2, 0x38, 0x145, 0x3, 0x2, 
    0x2, 0x2, 0x3a, 0x152, 0x3, 0x2, 0x2, 0x2, 0x3c, 0x15d, 0x3, 0x2, 0x2, 
    0x2, 0x3e, 0x15f, 0x3, 0x2, 0x2, 0x2, 0x40, 0x162, 0x3, 0x2, 0x2, 0x2, 
    0x42, 0x170, 0x3, 0x2, 0x2, 0x2, 0x44, 0x172, 0x3, 0x2, 0x2, 0x2, 0x46, 
    0x178, 0x3, 0x2, 0x2, 0x2, 0x48, 0x18e, 0x3, 0x2, 0x2, 0x2, 0x4a, 0x190, 
    0x3, 0x2, 0x2, 0x2, 0x4c, 0x192, 0x3, 0x2, 0x2, 0x2, 0x4e, 0x194, 0x3, 
    0x2, 0x2, 0x2, 0x50, 0x196, 0x3, 0x2, 0x2, 0x2, 0x52, 0x198, 0x3, 0x2, 
    0x2, 0x2, 0x54, 0x55, 0x5, 0x4, 0x3, 0x2, 0x55, 0x56, 0x7, 0x2, 0x2, 
    0x3, 0x56, 0x3, 0x3, 0x2, 0x2, 0x2, 0x57, 0x59, 0x5, 0x6, 0x4, 0x2, 
    0x58, 0x57, 0x3, 0x2, 0x2, 0x2, 0x59, 0x5c, 0x3, 0x2, 0x2, 0x2, 0x5a, 
    0x58, 0x3, 0x2, 0x2, 0x2, 0x5a, 0x5b, 0x3, 0x2, 0x2, 0x2, 0x5b, 0x5e, 
    0x3, 0x2, 0x2, 0x2, 0x5c, 0x5a, 0x3, 0x2, 0x2, 0x2, 0x5d, 0x5f, 0x5, 
    0x20, 0x11, 0x2, 0x5e, 0x5d, 0x3, 0x2, 0x2, 0x2, 0x5e, 0x5f, 0x3, 0x2, 
    0x2, 0x2, 0x5f, 0x5, 0x3, 0x2, 0x2, 0x2, 0x60, 0x70, 0x7, 0x2b, 0x2, 
    0x2, 0x61, 0x70, 0x5, 0x8, 0x5, 0x2, 0x62, 0x70, 0x5, 0xa, 0x6, 0x2, 
    0x63, 0x70, 0x5, 0x22, 0x12, 0x2, 0x64, 0x70, 0x7, 0x1e, 0x2, 0x2, 0x65, 
    0x70, 0x5, 0xc, 0x7, 0x2, 0x66, 0x70, 0x5, 0xe, 0x8, 0x2, 0x67, 0x70, 
    0x5, 0x10, 0x9, 0x2, 0x68, 0x70, 0x5, 0x12, 0xa, 0x2, 0x69, 0x70, 0x5, 
    0x14, 0xb, 0x2, 0x6a, 0x70, 0x5, 0x16, 0xc, 0x2, 0x6b, 0x70, 0x5, 0x18, 
    0xd, 0x2, 0x6c, 0x70, 0x5, 0x1a, 0xe, 0x2, 0x6d, 0x70, 0x5, 0x1c, 0xf, 
    0x2, 0x6e, 0x70, 0x5, 0x1e, 0x10, 0x2, 0x6f, 0x60, 0x3, 0x2, 0x2, 0x2, 
    0x6f, 0x61, 0x3, 0x2, 0x2, 0x2, 0x6f, 0x62, 0x3, 0x2, 0x2, 0x2, 0x6f, 
    0x63, 0x3, 0x2, 0x2, 0x2, 0x6f, 0x64, 0x3, 0x2, 0x2, 0x2, 0x6f, 0x65, 
    0x3, 0x2, 0x2, 0x2, 0x6f, 0x66, 0x3, 0x2, 0x2, 0x2, 0x6f, 0x67, 0x3, 
    0x2, 0x2, 0x2, 0x6f, 0x68, 0x3, 0x2, 0x2, 0x2, 0x6f, 0x69, 0x3, 0x2, 
    0x2, 0x2, 0x6f, 0x6a, 0x3, 0x2, 0x2, 0x2, 0x6f, 0x6b, 0x3, 0x2, 0x2, 
    0x2, 0x6f, 0x6c, 0x3, 0x2, 0x2, 0x2, 0x6f, 0x6d, 0x3, 0x2, 0x2, 0x2, 
    0x6f, 0x6e, 0x3, 0x2, 0x2, 0x2, 0x70, 0x7, 0x3, 0x2, 0x2, 0x2, 0x71, 
    0x72, 0x5, 0x26, 0x14, 0x2, 0x72, 0x73, 0x7, 0x2e, 0x2, 0x2, 0x73, 0x75, 
    0x5, 0x2e, 0x18, 0x2, 0x74, 0x76, 0x7, 0x2b, 0x2, 0x2, 0x75, 0x74, 0x3, 
    0x2, 0x2, 0x2, 0x75, 0x76, 0x3, 0x2, 0x2, 0x2, 0x76, 0x9, 0x3, 0x2, 
    0x2, 0x2, 0x77, 0x79, 0x5, 0x34, 0x1b, 0x2, 0x78, 0x7a, 0x5, 0x3a, 0x1e, 
    0x2, 0x79, 0x78, 0x3, 0x2, 0x2, 0x2, 0x7a, 0x7b, 0x3, 0x2, 0x2, 0x2, 
    0x7b, 0x79, 0x3, 0x2, 0x2, 0x2, 0x7b, 0x7c, 0x3, 0x2, 0x2, 0x2, 0x7c, 
    0x7e, 0x3, 0x2, 0x2, 0x2, 0x7d, 0x7f, 0x7, 0x2b, 0x2, 0x2, 0x7e, 0x7d, 
    0x3, 0x2, 0x2, 0x2, 0x7e, 0x7f, 0x3, 0x2, 0x2, 0x2, 0x7f, 0xb, 0x3, 
    0x2, 0x2, 0x2, 0x80, 0x81, 0x7, 0x21, 0x2, 0x2, 0x81, 0x82, 0x7, 0x3a, 
    0x2, 0x2, 0x82, 0xd, 0x3, 0x2, 0x2, 0x2, 0x83, 0x84, 0x7, 0x1f, 0x2, 
    0x2, 0x84, 0x85, 0x5, 0x4, 0x3, 0x2, 0x85, 0x86, 0x7, 0x20, 0x2, 0x2, 
    0x86, 0xf, 0x3, 0x2, 0x2, 0x2, 0x87, 0x88, 0x7, 0x22, 0x2, 0x2, 0x88, 
    0x89, 0x5, 0x30, 0x19, 0x2, 0x89, 0x8a, 0x7, 0x1f, 0x2, 0x2, 0x8a, 0x8b, 
    0x5, 0x4, 0x3, 0x2, 0x8b, 0x8c, 0x7, 0x20, 0x2, 0x2, 0x8c, 0x11, 0x3, 
    0x2, 0x2, 0x2, 0x8d, 0x8e, 0x7, 0x23, 0x2, 0x2, 0x8e, 0x8f, 0x5, 0x4, 
    0x3, 0x2, 0x8f, 0x90, 0x7, 0x24, 0x2, 0x2, 0x90, 0x92, 0x5, 0x30, 0x19, 
    0x2, 0x91, 0x93, 0x7, 0x2b, 0x2, 0x2, 0x92, 0x91, 0x3, 0x2, 0x2, 0x2, 
    0x92, 0x93, 0x3, 0x2, 0x2, 0x2, 0x93, 0x13, 0x3, 0x2, 0x2, 0x2, 0x94, 
    0x95, 0x7, 0x26, 0x2, 0x2, 0x95, 0x96, 0x5, 0x30, 0x19, 0x2, 0x96, 0x97, 
    0x7, 0x27, 0x2, 0x2, 0x97, 0x9f, 0x5, 0x4, 0x3, 0x2, 0x98, 0x99, 0x7, 
    0x29, 0x2, 0x2, 0x99, 0x9a, 0x5, 0x30, 0x19, 0x2, 0x9a, 0x9b, 0x7, 0x27, 
    0x2, 0x2, 0x9b, 0x9c, 0x5, 0x4, 0x3, 0x2, 0x9c, 0x9e, 0x3, 0x2, 0x2, 
    0x2, 0x9d, 0x98, 0x3, 0x2, 0x2, 0x2, 0x9e, 0xa1, 0x3, 0x2, 0x2, 0x2, 
    0x9f, 0x9d, 0x3, 0x2, 0x2, 0x2, 0x9f, 0xa0, 0x3, 0x2, 0x2, 0x2, 0xa0, 
    0xa4, 0x3, 0x2, 0x2, 0x2, 0xa1, 0x9f, 0x3, 0x2, 0x2, 0x2, 0xa2, 0xa3, 
    0x7, 0x28, 0x2, 0x2, 0xa3, 0xa5, 0x5, 0x4, 0x3, 0x2, 0xa4, 0xa2, 0x3, 
    0x2, 0x2, 0x2, 0xa4, 0xa5, 0x3, 0x2, 0x2, 0x2, 0xa5, 0xa6, 0x3, 0x2, 
    0x2, 0x2, 0xa6, 0xa7, 0x7, 0x20, 0x2, 0x2, 0xa7, 0x15, 0x3, 0x2, 0x2, 
    0x2, 0xa8, 0xa9, 0x7, 0x25, 0x2, 0x2, 0xa9, 0xaa, 0x7, 0x3a, 0x2, 0x2, 
    0xaa, 0xab, 0x7, 0x2e, 0x2, 0x2, 0xab, 0xac, 0x5, 0x30, 0x19, 0x2, 0xac, 
    0xad, 0x7, 0x2c, 0x2, 0x2, 0xad, 0xb0, 0x5, 0x30, 0x19, 0x2, 0xae, 0xaf, 
    0x7, 0x2c, 0x2, 0x2, 0xaf, 0xb1, 0x5, 0x30, 0x19, 0x2, 0xb0, 0xae, 0x3, 
    0x2, 0x2, 0x2, 0xb0, 0xb1, 0x3, 0x2, 0x2, 0x2, 0xb1, 0xb2, 0x3, 0x2, 
    0x2, 0x2, 0xb2, 0xb3, 0x7, 0x1f, 0x2, 0x2, 0xb3, 0xb4, 0x5, 0x4, 0x3, 
    0x2, 0xb4, 0xb5, 0x7, 0x20, 0x2, 0x2, 0xb5, 0x17, 0x3, 0x2, 0x2, 0x2, 
    0xb6, 0xb7, 0x7, 0x25, 0x2, 0x2, 0xb7, 0xb8, 0x5, 0x28, 0x15, 0x2, 0xb8, 
    0xb9, 0x7, 0x2a, 0x2, 0x2, 0xb9, 0xba, 0x5, 0x2e, 0x18, 0x2, 0xba, 0xbb, 
    0x7, 0x1f, 0x2, 0x2, 0xbb, 0xbc, 0x5, 0x4, 0x3, 0x2, 0xbc, 0xbd, 0x7, 
    0x20, 0x2, 0x2, 0xbd, 0x19, 0x3, 0x2, 0x2, 0x2, 0xbe, 0xbf, 0x7, 0x18, 
    0x2, 0x2, 0xbf, 0xc0, 0x5, 0x24, 0x13, 0x2, 0xc0, 0xc1, 0x5, 0x40, 0x21, 
    0x2, 0xc1, 0x1b, 0x3, 0x2, 0x2, 0x2, 0xc2, 0xc3, 0x7, 0x1a, 0x2, 0x2, 
    0xc3, 0xc4, 0x7, 0x18, 0x2, 0x2, 0xc4, 0xc5, 0x7, 0x3a, 0x2, 0x2, 0xc5, 
    0xc6, 0x5, 0x40, 0x21, 0x2, 0xc6, 0x1d, 0x3, 0x2, 0x2, 0x2, 0xc7, 0xc8, 
    0x7, 0x1a, 0x2, 0x2, 0xc8, 0xcb, 0x5, 0x2a, 0x16, 0x2, 0xc9, 0xca, 0x7, 
    0x2e, 0x2, 0x2, 0xca, 0xcc, 0x5, 0x2e, 0x18, 0x2, 0xcb, 0xc9, 0x3, 0x2, 
    0x2, 0x2, 0xcb, 0xcc, 0x3, 0x2, 0x2, 0x2, 0xcc, 0xce, 0x3, 0x2, 0x2, 
    0x2, 0xcd, 0xcf, 0x7, 0x2b, 0x2, 0x2, 0xce, 0xcd, 0x3, 0x2, 0x2, 0x2, 
    0xce, 0xcf, 0x3, 0x2, 0x2, 0x2, 0xcf, 0x1f, 0x3, 0x2, 0x2, 0x2, 0xd0, 
    0xd2, 0x7, 0x19, 0x2, 0x2, 0xd1, 0xd3, 0x5, 0x2e, 0x18, 0x2, 0xd2, 0xd1, 
    0x3, 0x2, 0x2, 0x2, 0xd2, 0xd3, 0x3, 0x2, 0x2, 0x2, 0xd3, 0xd5, 0x3, 
    0x2, 0x2, 0x2, 0xd4, 0xd6, 0x7, 0x2b, 0x2, 0x2, 0xd5, 0xd4, 0x3, 0x2, 
    0x2, 0x2, 0xd5, 0xd6, 0x3, 0x2, 0x2, 0x2, 0xd6, 0x21, 0x3, 0x2, 0x2, 
    0x2, 0xd7, 0xd8, 0x7, 0x30, 0x2, 0x2, 0xd8, 0xd9, 0x7, 0x3a, 0x2, 0x2, 
    0xd9, 0xda, 0x7, 0x30, 0x2, 0x2, 0xda, 0x23, 0x3, 0x2, 0x2, 0x2, 0xdb, 
    0xe0, 0x7, 0x3a, 0x2, 0x2, 0xdc, 0xdd, 0x7, 0x2d, 0x2, 0x2, 0xdd, 0xdf, 
    0x7, 0x3a, 0x2, 0x2, 0xde, 0xdc, 0x3, 0x2, 0x2, 0x2, 0xdf, 0xe2, 0x3, 
    0x2, 0x2, 0x2, 0xe0, 0xde, 0x3, 0x2, 0x2, 0x2, 0xe0, 0xe1, 0x3, 0x2, 
    0x2, 0x2, 0xe1, 0xe5, 0x3, 0x2, 0x2, 0x2, 0xe2, 0xe0, 0x3, 0x2, 0x2, 
    0x2, 0xe3, 0xe4, 0x7, 0x2f, 0x2, 0x2, 0xe4, 0xe6, 0x7, 0x3a, 0x2, 0x2, 
    0xe5, 0xe3, 0x3, 0x2, 0x2, 0x2, 0xe5, 0xe6, 0x3, 0x2, 0x2, 0x2, 0xe6, 
    0x25, 0x3, 0x2, 0x2, 0x2, 0xe7, 0xec, 0x5, 0x36, 0x1c, 0x2, 0xe8, 0xe9, 
    0x7, 0x2c, 0x2, 0x2, 0xe9, 0xeb, 0x5, 0x36, 0x1c, 0x2, 0xea, 0xe8, 0x3, 
    0x2, 0x2, 0x2, 0xeb, 0xee, 0x3, 0x2, 0x2, 0x2, 0xec, 0xea, 0x3, 0x2, 
    0x2, 0x2, 0xec, 0xed, 0x3, 0x2, 0x2, 0x2, 0xed, 0x27, 0x3, 0x2, 0x2, 
    0x2, 0xee, 0xec, 0x3, 0x2, 0x2, 0x2, 0xef, 0xf4, 0x7, 0x3a, 0x2, 0x2, 
    0xf0, 0xf1, 0x7, 0x2c, 0x2, 0x2, 0xf1, 0xf3, 0x7, 0x3a, 0x2, 0x2, 0xf2, 
    0xf0, 0x3, 0x2, 0x2, 0x2, 0xf3, 0xf6, 0x3, 0x2, 0x2, 0x2, 0xf4, 0xf2, 
    0x3, 0x2, 0x2, 0x2, 0xf4, 0xf5, 0x3, 0x2, 0x2, 0x2, 0xf5, 0x29, 0x3, 
    0x2, 0x2, 0x2, 0xf6, 0xf4, 0x3, 0x2, 0x2, 0x2, 0xf7, 0xf8, 0x7, 0x3a, 
    0x2, 0x2, 0xf8, 0xfe, 0x5, 0x2c, 0x17, 0x2, 0xf9, 0xfa, 0x7, 0x2c, 0x2, 
    0x2, 0xfa, 0xfb, 0x7, 0x3a, 0x2, 0x2, 0xfb, 0xfd, 0x5, 0x2c, 0x17, 0x2, 
    0xfc, 0xf9, 0x3, 0x2, 0x2, 0x2, 0xfd, 0x100, 0x3, 0x2, 0x2, 0x2, 0xfe, 
    0xfc, 0x3, 0x2, 0x2, 0x2, 0xfe, 0xff, 0x3, 0x2, 0x2, 0x2, 0xff, 0x2b, 
    0x3, 0x2, 0x2, 0x2, 0x100, 0xfe, 0x3, 0x2, 0x2, 0x2, 0x101, 0x102, 0x7, 
    0x32, 0x2, 0x2, 0x102, 0x103, 0x7, 0x3a, 0x2, 0x2, 0x103, 0x105, 0x7, 
    0x33, 0x2, 0x2, 0x104, 0x101, 0x3, 0x2, 0x2, 0x2, 0x104, 0x105, 0x3, 
    0x2, 0x2, 0x2, 0x105, 0x2d, 0x3, 0x2, 0x2, 0x2, 0x106, 0x10b, 0x5, 0x30, 
    0x19, 0x2, 0x107, 0x108, 0x7, 0x2c, 0x2, 0x2, 0x108, 0x10a, 0x5, 0x30, 
    0x19, 0x2, 0x109, 0x107, 0x3, 0x2, 0x2, 0x2, 0x10a, 0x10d, 0x3, 0x2, 
    0x2, 0x2, 0x10b, 0x109, 0x3, 0x2, 0x2, 0x2, 0x10b, 0x10c, 0x3, 0x2, 
    0x2, 0x2, 0x10c, 0x2f, 0x3, 0x2, 0x2, 0x2, 0x10d, 0x10b, 0x3, 0x2, 0x2, 
    0x2, 0x10e, 0x10f, 0x8, 0x19, 0x1, 0x2, 0x10f, 0x11c, 0x7, 0x1d, 0x2, 
    0x2, 0x110, 0x11c, 0x7, 0x1c, 0x2, 0x2, 0x111, 0x11c, 0x7, 0x1b, 0x2, 
    0x2, 0x112, 0x11c, 0x5, 0x50, 0x29, 0x2, 0x113, 0x11c, 0x5, 0x52, 0x2a, 
    0x2, 0x114, 0x11c, 0x7, 0x31, 0x2, 0x2, 0x115, 0x11c, 0x5, 0x32, 0x1a, 
    0x2, 0x116, 0x11c, 0x5, 0x3e, 0x20, 0x2, 0x117, 0x11c, 0x5, 0x44, 0x23, 
    0x2, 0x118, 0x119, 0x5, 0x4e, 0x28, 0x2, 0x119, 0x11a, 0x5, 0x30, 0x19, 
    0x4, 0x11a, 0x11c, 0x3, 0x2, 0x2, 0x2, 0x11b, 0x10e, 0x3, 0x2, 0x2, 
    0x2, 0x11b, 0x110, 0x3, 0x2, 0x2, 0x2, 0x11b, 0x111, 0x3, 0x2, 0x2, 
    0x2, 0x11b, 0x112, 0x3, 0x2, 0x2, 0x2, 0x11b, 0x113, 0x3, 0x2, 0x2, 
    0x2, 0x11b, 0x114, 0x3, 0x2, 0x2, 0x2, 0x11b, 0x115, 0x3, 0x2, 0x2, 
    0x2, 0x11b, 0x116, 0x3, 0x2, 0x2, 0x2, 0x11b, 0x117, 0x3, 0x2, 0x2, 
    0x2, 0x11b, 0x118, 0x3, 0x2, 0x2, 0x2, 0x11c, 0x123, 0x3, 0x2, 0x2, 
    0x2, 0x11d, 0x11e, 0xc, 0x3, 0x2, 0x2, 0x11e, 0x11f, 0x5, 0x4c, 0x27, 
    0x2, 0x11f, 0x120, 0x5, 0x30, 0x19, 0x4, 0x120, 0x122, 0x3, 0x2, 0x2, 
    0x2, 0x121, 0x11d, 0x3, 0x2, 0x2, 0x2, 0x122, 0x125, 0x3, 0x2, 0x2, 
    0x2, 0x123, 0x121, 0x3, 0x2, 0x2, 0x2, 0x123, 0x124, 0x3, 0x2, 0x2, 
    0x2, 0x124, 0x31, 0x3, 0x2, 0x2, 0x2, 0x125, 0x123, 0x3, 0x2, 0x2, 0x2, 
    0x126, 0x12a, 0x5, 0x34, 0x1b, 0x2, 0x127, 0x129, 0x5, 0x3a, 0x1e, 0x2, 
    0x128, 0x127, 0x3, 0x2, 0x2, 0x2, 0x129, 0x12c, 0x3, 0x2, 0x2, 0x2, 
    0x12a, 0x128, 0x3, 0x2, 0x2, 0x2, 0x12a, 0x12b, 0x3, 0x2, 0x2, 0x2, 
    0x12b, 0x33, 0x3, 0x2, 0x2, 0x2, 0x12c, 0x12a, 0x3, 0x2, 0x2, 0x2, 0x12d, 
    0x133, 0x5, 0x36, 0x1c, 0x2, 0x12e, 0x12f, 0x7, 0x34, 0x2, 0x2, 0x12f, 
    0x130, 0x5, 0x30, 0x19, 0x2, 0x130, 0x131, 0x7, 0x35, 0x2, 0x2, 0x131, 
    0x133, 0x3, 0x2, 0x2, 0x2, 0x132, 0x12d, 0x3, 0x2, 0x2, 0x2, 0x132, 
    0x12e, 0x3, 0x2, 0x2, 0x2, 0x133, 0x35, 0x3, 0x2, 0x2, 0x2, 0x134, 0x13b, 
    0x7, 0x3a, 0x2, 0x2, 0x135, 0x136, 0x7, 0x34, 0x2, 0x2, 0x136, 0x137, 
    0x5, 0x30, 0x19, 0x2, 0x137, 0x138, 0x7, 0x35, 0x2, 0x2, 0x138, 0x139, 
    0x5, 0x38, 0x1d, 0x2, 0x139, 0x13b, 0x3, 0x2, 0x2, 0x2, 0x13a, 0x134, 
    0x3, 0x2, 0x2, 0x2, 0x13a, 0x135, 0x3, 0x2, 0x2, 0x2, 0x13b, 0x13f, 
    0x3, 0x2, 0x2, 0x2, 0x13c, 0x13e, 0x5, 0x38, 0x1d, 0x2, 0x13d, 0x13c, 
    0x3, 0x2, 0x2, 0x2, 0x13e, 0x141, 0x3, 0x2, 0x2, 0x2, 0x13f, 0x13d, 
    0x3, 0x2, 0x2, 0x2, 0x13f, 0x140, 0x3, 0x2, 0x2, 0x2, 0x140, 0x37, 0x3, 
    0x2, 0x2, 0x2, 0x141, 0x13f, 0x3, 0x2, 0x2, 0x2, 0x142, 0x144, 0x5, 
    0x3a, 0x1e, 0x2, 0x143, 0x142, 0x3, 0x2, 0x2, 0x2, 0x144, 0x147, 0x3, 
    0x2, 0x2, 0x2, 0x145, 0x143, 0x3, 0x2, 0x2, 0x2, 0x145, 0x146, 0x3, 
    0x2, 0x2, 0x2, 0x146, 0x14e, 0x3, 0x2, 0x2, 0x2, 0x147, 0x145, 0x3, 
    0x2, 0x2, 0x2, 0x148, 0x149, 0x7, 0x38, 0x2, 0x2, 0x149, 0x14a, 0x5, 
    0x30, 0x19, 0x2, 0x14a, 0x14b, 0x7, 0x39, 0x2, 0x2, 0x14b, 0x14f, 0x3, 
    0x2, 0x2, 0x2, 0x14c, 0x14d, 0x7, 0x2d, 0x2, 0x2, 0x14d, 0x14f, 0x7, 
    0x3a, 0x2, 0x2, 0x14e, 0x148, 0x3, 0x2, 0x2, 0x2, 0x14e, 0x14c, 0x3, 
    0x2, 0x2, 0x2, 0x14f, 0x39, 0x3, 0x2, 0x2, 0x2, 0x150, 0x151, 0x7, 0x2f, 
    0x2, 0x2, 0x151, 0x153, 0x7, 0x3a, 0x2, 0x2, 0x152, 0x150, 0x3, 0x2, 
    0x2, 0x2, 0x152, 0x153, 0x3, 0x2, 0x2, 0x2, 0x153, 0x154, 0x3, 0x2, 
    0x2, 0x2, 0x154, 0x155, 0x5, 0x3c, 0x1f, 0x2, 0x155, 0x3b, 0x3, 0x2, 
    0x2, 0x2, 0x156, 0x158, 0x7, 0x34, 0x2, 0x2, 0x157, 0x159, 0x5, 0x2e, 
    0x18, 0x2, 0x158, 0x157, 0x3, 0x2, 0x2, 0x2, 0x158, 0x159, 0x3, 0x2, 
    0x2, 0x2, 0x159, 0x15a, 0x3, 0x2, 0x2, 0x2, 0x15a, 0x15e, 0x7, 0x35, 
    0x2, 0x2, 0x15b, 0x15e, 0x5, 0x44, 0x23, 0x2, 0x15c, 0x15e, 0x5, 0x52, 
    0x2a, 0x2, 0x15d, 0x156, 0x3, 0x2, 0x2, 0x2, 0x15d, 0x15b, 0x3, 0x2, 
    0x2, 0x2, 0x15d, 0x15c, 0x3, 0x2, 0x2, 0x2, 0x15e, 0x3d, 0x3, 0x2, 0x2, 
    0x2, 0x15f, 0x160, 0x7, 0x18, 0x2, 0x2, 0x160, 0x161, 0x5, 0x40, 0x21, 
    0x2, 0x161, 0x3f, 0x3, 0x2, 0x2, 0x2, 0x162, 0x164, 0x7, 0x34, 0x2, 
    0x2, 0x163, 0x165, 0x5, 0x42, 0x22, 0x2, 0x164, 0x163, 0x3, 0x2, 0x2, 
    0x2, 0x164, 0x165, 0x3, 0x2, 0x2, 0x2, 0x165, 0x166, 0x3, 0x2, 0x2, 
    0x2, 0x166, 0x167, 0x7, 0x35, 0x2, 0x2, 0x167, 0x168, 0x5, 0x4, 0x3, 
    0x2, 0x168, 0x169, 0x7, 0x20, 0x2, 0x2, 0x169, 0x41, 0x3, 0x2, 0x2, 
    0x2, 0x16a, 0x16d, 0x5, 0x28, 0x15, 0x2, 0x16b, 0x16c, 0x7, 0x2c, 0x2, 
    0x2, 0x16c, 0x16e, 0x7, 0x31, 0x2, 0x2, 0x16d, 0x16b, 0x3, 0x2, 0x2, 
    0x2, 0x16d, 0x16e, 0x3, 0x2, 0x2, 0x2, 0x16e, 0x171, 0x3, 0x2, 0x2, 
    0x2, 0x16f, 0x171, 0x7, 0x31, 0x2, 0x2, 0x170, 0x16a, 0x3, 0x2, 0x2, 
    0x2, 0x170, 0x16f, 0x3, 0x2, 0x2, 0x2, 0x171, 0x43, 0x3, 0x2, 0x2, 0x2, 
    0x172, 0x174, 0x7, 0x36, 0x2, 0x2, 0x173, 0x175, 0x5, 0x46, 0x24, 0x2, 
    0x174, 0x173, 0x3, 0x2, 0x2, 0x2, 0x174, 0x175, 0x3, 0x2, 0x2, 0x2, 
    0x175, 0x176, 0x3, 0x2, 0x2, 0x2, 0x176, 0x177, 0x7, 0x37, 0x2, 0x2, 
    0x177, 0x45, 0x3, 0x2, 0x2, 0x2, 0x178, 0x17e, 0x5, 0x48, 0x25, 0x2, 
    0x179, 0x17a, 0x5, 0x4a, 0x26, 0x2, 0x17a, 0x17b, 0x5, 0x48, 0x25, 0x2, 
    0x17b, 0x17d, 0x3, 0x2, 0x2, 0x2, 0x17c, 0x179, 0x3, 0x2, 0x2, 0x2, 
    0x17d, 0x180, 0x3, 0x2, 0x2, 0x2, 0x17e, 0x17c, 0x3, 0x2, 0x2, 0x2, 
    0x17e, 0x17f, 0x3, 0x2, 0x2, 0x2, 0x17f, 0x182, 0x3, 0x2, 0x2, 0x2, 
    0x180, 0x17e, 0x3, 0x2, 0x2, 0x2, 0x181, 0x183, 0x5, 0x4a, 0x26, 0x2, 
    0x182, 0x181, 0x3, 0x2, 0x2, 0x2, 0x182, 0x183, 0x3, 0x2, 0x2, 0x2, 
    0x183, 0x47, 0x3, 0x2, 0x2, 0x2, 0x184, 0x185, 0x7, 0x38, 0x2, 0x2, 
    0x185, 0x186, 0x5, 0x30, 0x19, 0x2, 0x186, 0x187, 0x7, 0x39, 0x2, 0x2, 
    0x187, 0x188, 0x7, 0x2e, 0x2, 0x2, 0x188, 0x189, 0x5, 0x30, 0x19, 0x2, 
    0x189, 0x18f, 0x3, 0x2, 0x2, 0x2, 0x18a, 0x18b, 0x7, 0x3a, 0x2, 0x2, 
    0x18b, 0x18c, 0x7, 0x2e, 0x2, 0x2, 0x18c, 0x18f, 0x5, 0x30, 0x19, 0x2, 
    0x18d, 0x18f, 0x5, 0x30, 0x19, 0x2, 0x18e, 0x184, 0x3, 0x2, 0x2, 0x2, 
    0x18e, 0x18a, 0x3, 0x2, 0x2, 0x2, 0x18e, 0x18d, 0x3, 0x2, 0x2, 0x2, 
    0x18f, 0x49, 0x3, 0x2, 0x2, 0x2, 0x190, 0x191, 0x9, 0x2, 0x2, 0x2, 0x191, 
    0x4b, 0x3, 0x2, 0x2, 0x2, 0x192, 0x193, 0x9, 0x3, 0x2, 0x2, 0x193, 0x4d, 
    0x3, 0x2, 0x2, 0x2, 0x194, 0x195, 0x9, 0x4, 0x2, 0x2, 0x195, 0x4f, 0x3, 
    0x2, 0x2, 0x2, 0x196, 0x197, 0x9, 0x5, 0x2, 0x2, 0x197, 0x51, 0x3, 0x2, 
    0x2, 0x2, 0x198, 0x199, 0x9, 0x6, 0x2, 0x2, 0x199, 0x53, 0x3, 0x2, 0x2, 
    0x2, 0x29, 0x5a, 0x5e, 0x6f, 0x75, 0x7b, 0x7e, 0x92, 0x9f, 0xa4, 0xb0, 
    0xcb, 0xce, 0xd2, 0xd5, 0xe0, 0xe5, 0xec, 0xf4, 0xfe, 0x104, 0x10b, 
    0x11b, 0x123, 0x12a, 0x132, 0x13a, 0x13f, 0x145, 0x14e, 0x152, 0x158, 
    0x15d, 0x164, 0x16d, 0x170, 0x174, 0x17e, 0x182, 0x18e, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

LuaParser::Initializer LuaParser::_init;
