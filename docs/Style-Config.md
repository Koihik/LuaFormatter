# Style config file

## format with style config file

`lua-format {your_source_file} -c {your_config_file}`

## style config file format

```yaml
key: "string"
key2: 123
key3: true
# comment
```

## Style options

### column_limit

type: int, default: 80

The column limit of one line.

### indent_width

type: int, default: 4

The number of spaces used for indentation.

```lua
-- indent_width: 2
function x()
  print(1)
end

-- indent_width: 4
function x()
	print(1)
end
```

### use_tab

type: bool, default: false

Use tab for indent.

```lua
-- use_tab: false
function x()
  print(1)
end

-- use_tab: true
function x()
	print(1)
end
```


### continuation_indent_width

type: int, default: 4

Indent width for continuations line.

```lua
-- continuation_indent_width: 2
local xxx, yyy = 
  111, 222

-- continuation_indent_width: 4
local xxx, yyy = 
    111, 222
```

### keep_simple_control_block_one_line

type: bool, default: true

Allow format simple control block(e.g. if, while, for, ...) to one line.

```lua
-- keep_simple_block_one_line: true
if cond then xx() end

-- keep_simple_block_one_line: false
if cond then
    xx()
end
```

### keep_simple_function_one_line

type: bool, default: true

Allow format simple function to one line.

```lua
-- keep_simple_block_one_line: true
function x() print(1) end

-- keep_simple_block_one_line: false
function x()
    print(1)
end
```
### align_args

type: bool, default: true

Align arguements of function call if there is a line break.
If false, use `continuation_indent_width` to indentation.

```lua
-- align_args: true
xxxxxxx(qqqqqqqqq,
        wwwwww)

-- align_args: false
xxxxxxx(qqqqqqqqq,
    wwwwww)
```

### break_after_functioncall_lp

type: bool, default: false

Break after '(' of function call if columns greater than `column_limit`.

```lua
-- break_after_functioncall_lp: true
xxxxxxx(
    qqqqqqqqq,
    wwwwww)

-- break_after_functioncall_lp: false
xxxxxxx(qqqqqqqqq,
        wwwwww)
```

### break_before_functioncall_rp

type: bool, default: false

Break before ')' of function call if columns greater than `column_limit`.

```lua
-- break_before_functioncall_rp: true
xxxxxxx(
    qqqqqqqqq,
    wwwwww
)

-- break_before_functioncall_rp: false
xxxxxxx(
    qqqqqqqqq,
    wwwwww)
```

### align_parameter

type: bool, default: true

Align parameter of function define if there is a line break.
if false, use `continuation_indent_width` to indentation.

```lua
-- align_parameter: true
function xxx(aaa, bbb
             ccc, ddd)

-- align_parameter: false
function xxx(aaa, bbb
    ccc, ddd)
```

### chop_down_parameter

type: bool, default: false

Chop down all parameters if the function declaration doesn’t fit on a line.

```lua
-- chop_down_parameter: true
function xxx(aaa, 
             bbb,
             ccc,
             ddd)

-- chop_down_parameter: false
function xxx(aaa, bbb
             ccc, ddd)
```

### break_after_functiondef_lp

type: bool, default: false

Break after '(' of function define if columns greater than `column_limit`.

```lua
-- break_after_functiondef_lp: true
function xxx(
    aaa, bbb, ccc,
    ddd)

-- break_after_functiondef_lp: false
function xxx(aaa,bbb
             ccc,ddd)
```

### break_before_functiondef_rp

type: bool, default: false

Break before ')' of function define if columns greater than `column_limit`.

```lua
-- break_before_functiondef_rp: true
function xxx(
    aaa, bbb, ccc,
    ddd
)

-- break_before_functiondef_rp: false
function xxx(
    aaa, bbb, ccc,
    ddd)
```

### align_table_field

type: bool, default: true

Align fields of table if there is a line break.
if false, use `indent_width` to indentation.

```lua
-- align_table_field: true
x = {111, 222,
     333, 444}

-- align_table_field: false
x = {111, 222,
  333, 444}
```

### break_after_table_lb

type: bool, default: true

Break after '{' of table if columns greater than `column_limit`.

```lua
-- break_after_table_lb: true
x = {
  111, 222,
  333, 444}

-- break_after_table_lb: false
x = {111, 222,
     333, 444}
```

### break_before_table_rb

type: bool, default: true

Break before '}' of table if columns greater than `column_limit`.

```lua
-- break_before_table_rb: true
x = {
  111, 222,
  333, 444
}

-- break_before_table_rb: false
x = {
  111, 222,
  333, 444}
```

### chop_down_table

type: bool, default: false

Chop down any table.

```lua
-- chop_down_table: true
x = {
    v1,
    v2,
    v3,
    v4
}

-- chop_down_table: false
x = {
    v1, v2,
    v3, v4
}
```

### chop_down_kv_table

type: bool, default: true

Chop down table if table contains key.

```lua
-- chop_down_kv_table: true
x = {
    k1 = v1,
    k2 = v2,
    k3 = v3,
    k4 = v4
}

-- chop_down_kv_table: false
x = {
    k1 = v1, k2 = v2,
    k3 = v3, k4 = v4
}
```

### table_sep

type: str, default: ','

Define character to separate table fields.

```lua
-- table_sep: ','
x = {
    k1 = v1,
    k2 = v2,
    k3 = v3
}

-- table_sep: ';'
x = {
    k1 = v1;
    k2 = v2;
    k3 = v3
}
```

### extra_sep_at_table_end

type: bool, default: false

Add a extra field separator after last field unless the table is in a single line.

```lua
-- extra_sep_at_table_end: true
x = {
    k1 = v1,
    k2 = v2,
    k3 = v3,
}

-- extra_sep_at_table_end: false
x = {
    k1 = v1,
    k2 = v2,
    k3 = v3
}
```

### break_after_operator

type: bool, default: true

Put break after operators if columns greater than `column_limit`.
If false, put break before operators.

```lua
-- break_after_operator: true
x = 11111 + 11111 +
    11111 + 11111 +
    11111

-- break_after_operator: false
x = 11111 + 11111
    + 11111 + 11111
    + 11111
```

### single_quote_to_double_quote

type: bool, default: false

Transform string literals to use double quote.

```lua
-- original
local foo = 'a'
local foo = '"'
local bar = 'don\'t'
local foo = '\''
local foobar = '\\\\\''

-- transformed
local foo = "a"
local foo = "\""
local bar = "don't"
local foo = "'"
local foo = "\\\\'"
```

### double_quote_to_single_quote

type: bool, default: false

Transform string literals to use single quote.

```lua
-- original
local foo = "a"
local foo = "'"
local bar = "don't"
local bar = "\""
local foobar = "\\\\\""

-- transformed
local foo = 'a'
local foo = '\''
local bar = 'don\'t'
local bar = '"'
local foobar = '\\\\"'
```

### spaces_before_call

type: int, default: 1

Inserts a space on function calls with parantheses omitted.

```lua
-- spaces_before_call: 1
require "foo"
f "a" "b"

-- spaces_before_call: 0
require"foo"
f"a" "b"
```
