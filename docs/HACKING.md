HACKING
========
This file contains the details of the program's execution in stages.
## Arguments parsing and loading
 The user invokes the program as the following: `lua-format [options] <scripts>`
 The `options` are parsed by the 3rd party library [args](https://github.com/Taywee/args).  It is the first task in `main()`.  
 The program checks for the configuration files right after parsing the command-line arguments. There are a couple of _known_ locations the program will check for configuration files if **no** file is specified in the command line.  
 In order of search:
   * Linux: `./.lua-format`, `$XDG_CONFIG_HOME/luaformatter/config.yaml`, `$HOME/.config/luaformatter/config.yaml`
   * Windows: `./.lua-format`
   * OSX: `./.lua-format`

 In case it doesn't find any suitable file it will check the command line for configuration values and then fall back to the default values.  
 The program reads the configuration through the command line by checking the flag. To set the configuration for boolean arguments, it uses the `no-` prefix for falsifiable options.  
 The program will give the top priority to the configuration values given in the command-line, then to the configuration files, and finally to the hard-coded default values.  
 If the program finds the suitable file and has all the required permissions then the file and scripts are passed for formatting. Otherwise, it will raise an error.  
## Configuration loading
The default configuration values are hard-coded in `Config.cpp` and are used as a fallback whenever needed. For instance when fields are not specified in the user's configuration file.  
The program uses the 3rd party library [yaml-cpp](https://github.com/jbeder/yaml-cpp) to load the user's configuration file and update the default values of the configuration.  
The value of each field is updated by picking the configuration field as key and value as their status. After that values are passed through their corresponding validators for validation. In the case of validation error in configuration values, the program terminates and throws the error message.  
The configuration values are validated by
   * **validate_integer** : Validates the configuration field that required positive integers
   * **validate_integer_zero** : Validated the configuration field that required non-negative integers
   * **validate_boolean** : Validated the configuration field that required boolean value
   * **validate_character** : Validated the configuration field that required character value
   * **validate_quote** : Validated the conflict values of single_quote and double quote configuration
   * **validate_use_tab** : Validate the conflict values of the use_tab with tab_width configuration
   * **validate_tab_width** : Validate the integral values and conflict of tab_width configuration  

The function to update the configuration value and read the user configuration file is declared in `config.h`.  
## Formatting the scripts
 The Lua script of the user is passed through the lualexer and luaParser to get tokenstream and chunkcontext. The program uses the 3rd party library [antrl4](https://github.com/antlr/antlr4) for the string tokenization and AST construction. The program is using a very specific design pattern of ANTRL4 called "Visitor Pattern" to traverse the AST just like it's described in the `Lua.g4`. AST traversal starts with the function `visitChunk(LuaParser::ChunkContext* ctx)` and then it calls the function `visitBlock`. In visitBlock function, the program checks for the semi to call the function `visitStat`, and after that, it checks retstat then function `visitRetstat` is called. In visitStat, `visitChidren` is called for the context. In `visitRepeatStat`, the function gets the token `REPEAT block UNTIL exp` and process them recursively. In `visitWhileStat` the function gets the token `WHILE exp DO block END` and process them while exp holds.  
   
 In function visitRetstat, `visitExplist` is called after checking the explist of the context. Then in function visiExplist, `visitExp` is called for various exp of the context after checking various conditions and do the formatting of the **align_args** feature. In visitExp, for linkOperator, the function visitExp is called itself for the context and does the formatting of the **break_after_operator** feature. For unaryOperator, the function visitExp is called itself for the front of the exp context.While for prefixexp the function `visitPrefixexp` is called, for tableconstructor `visitTableconstructor` is called, for functiondef `visitFunctiondef` is called and for string of the context `visitString` is called in which the formatting of **single_quote_to_double_quote** and **double_quote_to_single_quote** is done.  
   
 In visitPrefixexp, `visitVarOrExp` is called for the varOrExp of the context and after checking the config value of **align_parameter** `buildArguments` is called where the formatting is done for the **spaces_before_call** feature.In visitVarOrExp, it checks the exp of the context then `visitExp` is called else `visitVar` is called and in which `visitVarSuffix` is called. After that in visitVarSuffix, `visitNameAndArgs` is called and later on after checking the exp of the context `visitNextNameAndArgs` is called which again called the visitNameAndArgs. In visitNameAndArgs, `visitArgs` is called for the context which checks the LP and called `visitExplist` else `visitChildren`. In visitArgs, formatting for the break after or before functioncall lp or rp is done.  
   
 In visitFunctiondef, `visitFuncbody` is called for the context and do formatting for the break after or before functiondef lp or rp and checks the parlist then it called the function `visitParlist` in which `visitNamelist` is called.In visitNamelist, formatting of **chop_down_parameter** is done along with checking the feature **align_args** and **break_after_functiondef_lp**.After that in functiondef, `visitBlockAndComment` is called for the function block which checks the config of the keep function or controlblock in one line and called the function `visitBlock`.  
   
 In visitTableconstructor, it checks the fieldlist of the context and then do the formatting for the feature **chop_down_kv_table** and **chop_down_table** and call the function `vistfieldlist` in which `vistField` and `visitFieldSep` is called in which formatting of **table_sep** is done. In visitTableconstructor the formatting for the break after or before table lb or rb is done. While the formatting for **column_table_limit** and **extra_sep_at_table_end** is done in visitfieldlist as well as in visitTableconstructor.  
   
 The formatting for the indentation is done while changing the line in every function by config value of the features  
  * indent_width
  * use_tab
  * tab_width
  * continuation_indent_width

 For most of the functions, the program repeatedly checks the line indentation and config value of the column limit, and also it needs to do comment re-insertion as ANTRL deals with the tokenvector. The comment re-insertion is done because the program is redirecting the comment tokens to another channel. So these are not part of the AST. After doing the formatting in most of the `visit` functions the program loads the context in the vector.  
## Testing the program
 The unit and regressions tests are implemented in the directory `test` directory.  
 It uses the 3rd party library [Catch2](https://github.com/catchorg/Catch2) as the test library.  
 For regression testing, we isolate known bugs reported in the past.  
 The directory `testdata` contains the scripts used for the unit testing suite and regression testing suite.  
 The directory `testconfig` contains test configurations files for unit tests.  
The directory `cpp17` contains a small test done by CMake to ensure that the compiler supports the `filesystem` C++ 17 library.  
