/*
 BSD License
 
 Copyright (c) 2013, Kazunori Sakamoto Copyright (c) 2016, Alexander Alexeev All rights reserved.
 

 Redistribution and use in source and binary forms, with or without modification, are permitted
 provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the documentation and/or other
 materials provided with the distribution. 3. Neither the NAME of Rainer Schuster nor the NAMEs of
 its contributors may be used to endorse or promote products derived from this software without
 specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 This grammar file derived from:
 
 Lua 5.3 Reference Manual http://www.lua.org/manual/5.3/manual.html
 
 Lua 5.2 Reference Manual http://www.lua.org/manual/5.2/manual.html
 
 Lua 5.1 grammar written by Nicolai Mainiero http://www.antlr3.org/grammar/1178608849736/Lua.g
 

 Tested by Kazunori Sakamoto with Test suite for Lua 5.2 (http://www.lua.org/tests/5.2/)
 
 Tested by Alexander Alexeev with Test suite for Lua 5.3
 http://www.lua.org/tests/lua-5.3.2-tests.tar.gz
 */

grammar Lua;

chunk: block EOF;

block: stat* retstat?;

stat:
	SEMI
	| varDecl
	| functioncall
	| label
	| BREAK
	| gotoStat
	| doStat
	| whileStat
	| repeatStat
	| ifStat
	| forStat
	| forInStat
	| funcStat
	| localFuncStat
	| localVarDecl;

varDecl: varlist EQL explist SEMI?;

functioncall: varOrExp nameAndArgs+ SEMI?;

gotoStat: GOTO NAME;

doStat: DO block END;

whileStat: WHILE exp DO block END;

repeatStat: REPEAT block UNTIL exp SEMI?;

ifStat:
	IF exp THEN block (ELSEIF exp THEN block)* (ELSE block)? END;

forStat: FOR NAME EQL exp COMMA exp (COMMA exp)? DO block END;

forInStat: FOR namelist IN explist DO block END;

funcStat: FUNCTION funcname funcbody;

localFuncStat: LOCAL FUNCTION NAME funcbody;

localVarDecl: LOCAL namelist (EQL explist)? SEMI?;

retstat: RETURN explist? SEMI?;

label: DCOLON NAME DCOLON;

funcname: NAME (DOT NAME)* (COLON NAME)?;

varlist: var (COMMA var)*;

namelist: NAME (COMMA NAME)*;

explist: exp (COMMA exp)*;

exp:
	NIL
	| FALSE
	| TRUE
	| number
	| string
	| ELLIPSIS
	| prefixexp
	| functiondef
	| tableconstructor
	| unaryOperator exp
	| exp linkOperator exp;

prefixexp: varOrExp nameAndArgs*;

varOrExp: var | LP exp RP;

var: (NAME | LP exp RP varSuffix) varSuffix*;

varSuffix: nameAndArgs* (LSB exp RSB | DOT NAME);

nameAndArgs: (COLON NAME)? args;

args: LP explist? RP | tableconstructor | string;

functiondef: FUNCTION funcbody;

funcbody: LP parlist? RP block END;

parlist: namelist (COMMA ELLIPSIS)? | ELLIPSIS;

tableconstructor: LB fieldlist? RB;

fieldlist: field (fieldsep field)* fieldsep?;

field: LSB exp RSB EQL exp | NAME EQL exp | exp;

fieldsep: COMMA | SEMI;

linkOperator:
	'and'
	| 'or'
	| '..'
	| '<'
	| '>'
	| '<='
	| '>='
	| '~='
	| '=='
	| '+'
	| '-'
	| '*'
	| '/'
	| '%'
	| '//'
	| '&'
	| '|'
	| '~'
	| '<<'
	| '>>'
	| '^';

unaryOperator: 'not' | '#' | '-' | '~';

number: INT | HEX | FLOAT | HEX_FLOAT;

string: NORMALSTRING | CHARSTRING | LONGSTRING;

// LEXER

//keyword
FUNCTION: 'function';
RETURN: 'return';
LOCAL: 'local';
TRUE: 'true';
FALSE: 'false';
NIL: 'nil';
BREAK: 'break';
DO: 'do';
END: 'end';
GOTO: 'goto';
WHILE: 'while';
REPEAT: 'repeat';
UNTIL: 'until';
FOR: 'for';
IF: 'if';
THEN: 'then';
ELSE: 'else';
ELSEIF: 'elseif';
IN: 'in';

SEMI: ';';
COMMA: ',';
DOT: '.';
EQL: '=';
COLON: ':';
DCOLON: '::';
ELLIPSIS: '...';
LP: '(';
RP: ')';
LB: '{';
RB: '}';
LSB: '[';
RSB: ']';

NAME: [a-zA-Z_][a-zA-Z_0-9]*;

NORMALSTRING: '"' ( EscapeSequence | ~('\\' | '"'))* '"';

CHARSTRING: '\'' ( EscapeSequence | ~('\'' | '\\'))* '\'';

LONGSTRING: '[' NESTED_STR ']';

fragment NESTED_STR: '=' NESTED_STR '=' | '[' .*? ']';

INT: Digit+;

HEX: '0' [xX] HexDigit+;

FLOAT:
	Digit+ '.' Digit* ExponentPart?
	| '.' Digit+ ExponentPart?
	| Digit+ ExponentPart;

HEX_FLOAT:
	'0' [xX] HexDigit+ '.' HexDigit* HexExponentPart?
	| '0' [xX] '.' HexDigit+ HexExponentPart?
	| '0' [xX] HexDigit+ HexExponentPart;

fragment ExponentPart: [eE] [+-]? Digit+;

fragment HexExponentPart: [pP] [+-]? Digit+;

fragment EscapeSequence:
	'\\' [abfnrtvz"'\\]
	| '\\' '\r'? '\n'
	| DecimalEscape
	| HexEscape
	| UtfEscape;

fragment DecimalEscape:
	'\\' Digit
	| '\\' Digit Digit
	| '\\' [0-2] Digit Digit;

fragment HexEscape: '\\' 'x' HexDigit HexDigit;
fragment UtfEscape: '\\' 'u{' HexDigit+ '}';
fragment Digit: [0-9];
fragment HexDigit: [0-9a-fA-F];
COMMENT: '--[' NESTED_STR ']' -> channel(1);

LINE_COMMENT:
	'--' (
		// --
		| '[' '='* // --[==
		| '[' '='* ~('=' | '[' | '\r' | '\n') ~('\r' | '\n')* // --[==AA
		| ~('[' | '\r' | '\n') ~('\r' | '\n')* // --AAA
	) ('\r\n' | '\r' | '\n' | EOF) -> channel(1);

WS: [ \t\u000C\r\n]+ -> channel(1);
SHEBANG: '#' '!' ~('\n' | '\r')* -> channel(1);