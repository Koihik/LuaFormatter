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
	| exp linkOperator exp
	| unaryOperator exp;

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
SHEBANG: '#' '!' ~('\n' | '\r')* -> channel(HIDDEN);