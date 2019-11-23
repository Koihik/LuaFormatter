
// Generated from Lua.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"




class  LuaLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, FUNCTION = 22, RETURN = 23, LOCAL = 24, TRUE = 25, FALSE = 26, 
    NIL = 27, BREAK = 28, DO = 29, END = 30, GOTO = 31, WHILE = 32, REPEAT = 33, 
    UNTIL = 34, FOR = 35, IF = 36, THEN = 37, ELSE = 38, ELSEIF = 39, IN = 40, 
    SEMI = 41, COMMA = 42, DOT = 43, EQL = 44, COLON = 45, DCOLON = 46, 
    ELLIPSIS = 47, LT = 48, GT = 49, LP = 50, RP = 51, LB = 52, RB = 53, 
    LSB = 54, RSB = 55, NAME = 56, NORMALSTRING = 57, CHARSTRING = 58, LONGSTRING = 59, 
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

