
# xRe

A better regular expression.

## Grammar

### Grammar Structures

#### Plain Text (Sequence)
Plain text(sequence) is strings have no xRe meta symbols. 
xRe will do no operation for them at parse stage. 

Match a sequence is to match every character once in order, even exists duplicates.

#### Escape

An escape is `\ ` concat with a xRe meta character or a unicode number.
xRe views escapes as plain characters, according to the [escape table](#escape_table).

#### Set

A set is characters bracketed with `[` and `]`. 
These characters include plain text characters, and `-`. 
But `-` must be not at start or end of set characters, also not divided with only one character.

Match a set is to match any none `-` character in the set's characters once, but include characters between
the two characters concat with `-`.

#### Group

A group is an xRe string bracketed with `(` and `)`. 

Match a group is match the xRe string in it.

#### Union

A union is some xRe strings divided with `|`. 

Match a union is try to match any xRe string of the union once, and take the longest result.

#### Count

A count is some expressions bracketed with `{` and `}` but divided with `,`. 
Number of expressions is 0 to 3. 
First expression is the lower bound(included), and the second is the upper bound(included).
The third expression is the step length of match, and its value must be a positive integer.
If the number of expressions is 0, then the count range is 0 to infinity, and step is 1.

Count will not match. But it will let the match program repeat the last struct before it at least its
lower bound and at most its upper bound, in times of step length.

#### Switch

A switch is an expression and an xRe string bracketed with `<` and `>` but divided with `?` in order.

If value of the expression is not a zero, the match program will match the xRe 
or match a part of union according to the value if the xRe string is a union.

#### Expression

An expression is special strings which not to match, but will be calculated value with match program context.

And there are some different expressions:
1. **Const Expressions**: Const expression means can be computed and replaced with their value before match stage.
2. **Variables**: Variables are those identifiers with lower letters, digits or underscore but not start with digits.
   1. Using `=` can assign an xRe structure to a variable in an xRe string.
   2. Using `@` can call the structure of it in match program in an xRe string.
   3. Using `#` can get its order number in variable array in an expression.
   4. Using `$` can get the last matched result of it in an expression.
   
   When assign or call a variable, the variable must be bracketed with `<` and `>`.
3. **Functions**: Functions are those identifiers that stored in match program and can be called.
    The name of functions must be all upper latter, digits or underscore but not start with digits.
    Argument(s) of functions are some expression(s) bracketed with `(` and `)` after the name,
    and if more than one should be divided with `,`.
4. **Normal Expressions**: other functions is normal expressions. Most of them have at least one operator.
    Operators could be list in the [operator table](#operator_table).

#### xRe Macros

xRe macros are those strings to replace with there value in an xRe string before match stage.

All xRe macros are:

| Macros | Value   |
|:-------|:--------|
| `*`    | `{}`    |
| `+`    | `{1,}`  |
| `?`    | `{0,1}` |


### Escape Table

### Operator Table

