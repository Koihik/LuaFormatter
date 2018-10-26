
// Generated from Lua.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"




class  LuaLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, T__21 = 22, T__22 = 23, FUNCTION = 24, RETURN = 25, LOCAL = 26, 
    TRUE = 27, FALSE = 28, NIL = 29, BREAK = 30, DO = 31, END = 32, GOTO = 33, 
    WHILE = 34, REPEAT = 35, UNTIL = 36, FOR = 37, IF = 38, THEN = 39, ELSE = 40, 
    ELSEIF = 41, IN = 42, SEMI = 43, COMMA = 44, DOT = 45, EQL = 46, COLON = 47, 
    DCOLON = 48, ELLIPSIS = 49, LP = 50, RP = 51, LB = 52, RB = 53, LSB = 54, 
    RSB = 55, NAME = 56, NORMALSTRING = 57, CHARSTRING = 58, LONGSTRING = 59, 
    INT = 60, HEX = 61, FLOAT = 62, HEX_FLOAT = 63, COMMENT = 64, LINE_COMMENT = 65, 
    WS = 66, SHEBANG = 67
  };

  LuaLexer(antlr4::CharStream *input);
  ~LuaLexer();

  virtual std::string getGrammarFileName() const override;
  virtual const std::vector<std::string>& getRuleNames() const override;

  virtual const std::vector<std::string>& getChannelNames() const override;
  virtual const std::vector<std::string>& getModeNames() const override;
  virtual const std::vector<std::string>& getTokenNames() const override; // deprecated, use vocabulary instead
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;

  virtual const std::vector<uint16_t> getSerializedATN() const override;
  virtual const antlr4::atn::ATN& getATN() const override;

private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;
  static std::vector<std::string> _channelNames;
  static std::vector<std::string> _modeNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

