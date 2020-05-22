HACKING
========

This file contains the details of the program's execution in stages.

## Arguments parsing and loading

 The user invokes the program as the following: `lua-format [options] <scripts>`

 At first, the program does is to parse the command line options and raise any errors if needed.

  * `options` are parsed by the 3rd party library [args](https://github.com/Taywee/args)

 Now the program reads the configuration files. There are a couple of _known_ locations the program will check for configuration files if **no** file is specified in the command line.
 In order of search:
    * Linux: `./.lua-format`, `$XDG_CONFIG_HOME/luaformatter/config.yaml`, `$HOME/.config/luaformatter/config.yaml`
    * Windows: `./.lua-format`
    * OSX: `./.lua-format`

 In case it doesn't find any suitable file it will fallback to the default values.

 The default values are hardcoded in the program (`Config.cpp`) and are used as a fallback whenever possible. For instance when fields are not specified in the user's configuration file.

 If the program finds the suitable file and has all the required permissions then the file and scripts are passed for formatting. Otherwise, raise the error if it doesn't have the required permissions.

 If all is well with arguments passing then it shows format success message else it will raise the logic error for the arguments passing.

## Configuration loading

 The program is hardcoded with a default value for every field of the configuration in `config.cpp`.

 The program uses the 3rd party library [yaml-cpp](https://github.com/jbeder/yaml-cpp) to load the user's configuration file and update the default value of the configuration.

 The value of each field is updated by picking the configuration field as key and value as their status.

 The function to update the configuration value and read the user configuration file is declared in `config.h`.

## Implementing formatting features

 At first, the program declares the variables, vectors, and functions to read and write the lines, columns along with operating with newline break in `SourceWriter.h`.

 The program uses the 3rd party library [antrl4](https://github.com/antlr/antlr4) for the token system and making the struct of various functions.

 The functions for implementing formatting features are declared along with variables and vectors to record the values while formatting in `FormatVisitor.h`.

 All the formatting features get executed in `FormatVisitor.cpp` according to the configuration values.

 In function `commentAfter`, the program checks the types of the tokens for different types of formatting and returns the string with comments between the attribute passed as ParseTreeNode and nextNode according to conditions.

 While in the `commentAfterNewLine`, the program returns the string with comments between two attributes of the node but with a line break. The line break condition is the newlineIndent value which is passed as the next node.

 Now for storing the lines, functions like `pushWriter` are defined by sourcewriter constructor with the value for initialColumn as 0 and set the volatile to true if the writers_ is not empty. While in `pushWriterWithColumn` all execution is the same except the initialColumn value for Soucewriter constructor is cur_columns value.

 When it comes to pop out the values stored in writers_, function `popWriter` is used. Also, after pop up the writers_, the writer is deleted to free up space.

 In the function `fastTestColumnlimit`, it returns a boolean value by comparing the columnLimit value with the configuration value. It checks the type of tokens starting from the firstnode till the last node and then checks the column’s value with column_limit and returns a boolean value.

 The program uses the `cur_column` for measuring the column length and returns the integer value and uses `cur_writer` to return the last element of the vector `writers_`.

 The program uses various types of indentation with tabs and spaces according to the configuration provided.
  * In `string FormatVisitor::indentWithAlign()` if the config value for **use_tab** is true then indent with a tab for indent_ times and with spaces for indenforAlign times and if false then indent with space for indent_+indentforAlign times and return the string.

  * While in ` string FormatVisitor:: indent()`, indent for indent_ times with tab if use_tab is true else indent with space and return the string.

 Now whenever indentation requires after the newline, the program uses certain functions to increase and decrease the indent.
  * In `incIndent()` indent_ value is increased by **indent_width** of the config value and in `decIndent()` it will decrease by the same value.
  * While in `incContinuationIndent()` and `decContinuationIndent()` indent_ value is increased and decreased by the value of the **continuation_indent_width** in the config respectively.

  * But for the `incIndentForAlign(int indent)` the value is passed which will be increased in the indentForAlign value and for decrement, it will be passed in `decIndentForAlign(int indent)`.

 In `visitChunk(LuaParser:: ChunkContext* ctx) `, it visits the block with chop_down_block as true then after checking the newline it returns a string with a newline if a newline is not there.

 In `visitBlock(LuaParser::BlockContext* ctx)` the function checks the semi and needComment for the StatContext in LuaParser for each stats. Then corresponding to their bool value if semi is false the function `visitStat` is executed and for needComment it checks the `previousWhiteSpace` with semi value and `lastcharWhiteSpace`. After that if the retstat for tht context is not null then execute function `visitretstat` else set the blockComment to commentAfter with context.

 In `visitStat(LuaParser::StatContext* ctx)` it checks the chop_down_block value and then applies an operator for indent in the cur_writer and execute `visitChildren` with the StatContext.

 In `buildFirstArgumentWS` function checks the args LP and front of the context then according to the value of **space_before_call** in config add the spaces.

 In `visitRetstat(LuaParser::RetstatContext* ctx)` it checks the chop_down_block, explist of the context and then execute the function `visitExplist`  with the explist of the retstatContext. Also, it checks the semi for the context and applies the operator with the text of the semiContext in the cur_writer.

 In `visitExplist(LuaParser::ExplistContext* ctx)` first it execute the `pushWriterwithcolumn`. Then the visitExp with the front exp of the context and pop the writer is executed. After that, it set the beyondlimit value by checking the explength with column_limit and cur_columns but if the lines in the cur_writer are greater than 1 and cur_column is greater than half of column_limit set the beyondlimit to true. Now for the beyond limit execute the corresponding functions and format the part else format by the different specified function call and indentation. Similarly, for every index of the context check the beyondlimit and then execute the feature of **alignargs** and corresponding to it set the indentation for newlines.

 In `visitExp(LuaParser::ExpContext* ctx)` there are various checkpoints such as
  * linkoperator, unaryoperator, prefixexp, functiondef, table constructor , string etc.
  * So if the **linkoperator** is not null then execute the visitexp itself for the exp[0] of the context and then check the config for the **break_after_operator** and implement the features according to the standard style by checking beyondlimit and with the column_limit and do the required indentation.
  * For the **unaryoperator** check the text is ‘not" then do the corresponding function otherwise check text for ‘-’ and do the commentAfter according to it.
  * For the **prefixexp** we call `visitprefixexp` for the context of prefixexp, for **functiondef** we call `visitfunctiondef` for the context of functiondef, for **tableconstructor** we call `visitTableconstructor` for the context of tableconstructor, for **string** we call `visitString` for the context of string and for rest apply the operator to the context of the text in cur_writer.

 In `visitString(LuaParser::StringContext* ctx)` the program implement **single_qoute_to_double_qoute** and **double_quote_to_single_quote** features. It starts with checking the normalstring of the context and sets the quote to ‘\’’ or ‘\"’ and later deals with switching to it. After that according to the config it replaces the single to double and vice-versa.

 In `visitPrefixexp(LuaParser::PrefixexpContext* ctx)` it start with pushing the false value to chainedMethodCallHasIncIndent_  and chainedMethodCallIsFirst_  and call the function `visitVarOrExp` with varOrExp context and set the string ws to return the value of the function buildFirstArguments with nameAndArgs of the context. The feature of **align_parameter** is implemented by checking the config value for it and executing the buildArguments function for the context of nameAndArgs. At last indent by checking the chainedMethodCallHasIncIndent_.

 The `visitVarOrExp(LuaParser::VarOrExpContext* ctx)` check the exp of the context and if it is not null then execute the visitExp with exp of the context otherwise call visitVar for the var of the context.

 In `visitVar(LuaParser::VarContext* ctx)` the function checks the size for the context of the varsuffix and then corresponding functions get executed. The function `visitVarSuffix` is executed for every index of the varsuffix context.

 In `visitVarSuffix(LuaParser::VarSuffixContext* ctx)` LSB and RSB is checked and corresponding functions get executed for the nameAndArgs.

 In `visitArgs(LuaParser::ArgsContext* ctx)`  the explist of the context is checked by the function to implement the **break_after_functioncall_lp** and **break_before_functioncall_rp** feature . For implementing this feature, the function check the beyondlimit and config value for the formatting and corresponding function get executed. The function `visitExplist` for the the context get executed after formatting of the break_after_functioncall_lp..

 In `visitFuncbody(LuaParser::FuncbodyContext* ctx)` , the function sets the beyondlimit by the value of the column_limit and the number of lines. In this features executed are **break_after_functiondef_lp** and  **break_before_functiondef_rp** . According to the config value of the features, the indentation for the lines and spaces is done in the functiondef.

 In `visitTableconstructor(LuaParser::TableconstructorContext* ctx)` check the fieldlist and implement the features like
  * **chop_down_kv_table**
  * **column_table_limit**
  * **break_after_table_lb**
  * **chop_down_table**
  * **extra_sep_at_table_end**
  * **table_sep**
  * **break_before_table_rp**
  * Above features are implemented by executing the functions according to the config value and add indentation to the corresponding lines along with adding newlines by checking beyondlimit.

 In `visitFieldlist(LuaParser::FieldlistContext* ctx)` features like **align_table_field** are implemented by executing the function `visitFieldSep` and checking the beyondlimit by the config value of the **column_table_limit** and **extra_sep_at_table_end**.

 In `bool FormatVisitor::needKeepBlockOneLine ` features like **keep_simple_control_block_one_line** are implemented by checking the blocktype along with config value and for **keep_simple_function_one_line** blocktype is compared to functionblock. And further, check the semi and needcomment by retstat of the context and return the boolean value.

## Script formatting

 Now the formatting setup of the program is done in `lua-format.cpp`. The **lua-format** string is defined to format the `string str` or `istream is` which returns the value to set the `_format` with attributes as input and config.

## Configuration and formatting testing

 Now at last the program does is to test all types of feature implementations.In `test_config.cpp` the program test the correct configuration implementation by setting the config value for each field while in `test_format_file.cpp` the program tests the expected formatting by passing a lua script and configuration file. Later check the formatted scripts with the desired script.
