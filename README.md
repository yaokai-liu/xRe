
# xRe

A better regular expression.

## Grammar

### Grammar Structures

#### Escape

An escape is `\ ` concat with a xRe meta character or a unicode number.
xRe views escapes as plain characters, according to the [escape table](#escape_table).

#### Plain Text (Sequence)
Plain text(sequence) is strings have no xRe meta symbols. 
xRe will do no operation for them at parse stage. 

Match a sequence is to match every character once in order, even exists duplicates.

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
2. **Variables**: Variables are those identifiers consistent with only lower letters, digits or underscore but not start with digits.
   1. Using `=` can assign an xRe structure to a variable in an xRe string.
   2. Using `@` can call the structure of it in match program in an xRe string.
   3. Using `#` can get its order number in variable array, only can be using in expressions.
   4. Using `$` can get the last matched result of it in an expression, present as a plain text.
   
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


### xRe Attributes

xRe structures can have some extern attributes, them are parts of xRe grammar. 
XRe attributes was lead by `~`, bracketed with `<` and `>` and normally separated by `;`.

xRe attributes determinate some special behaviors of parsing and matching.

#### Only Parse

Character `!` is an attribute prefix. It means the xRe structure string it prefixed will only parse to an xRe structure,
but will do no matching.

#### Inverse

Character `^` is an attribute prefix. It means the xRe structure it prefixed will match its inverse text.

| xRe structure | inverse behavior                                                   |
|:--------------|:-------------------------------------------------------------------|
| plain text    | every character is not matched in plain text, but cast same length |
| set           | every option is not matched, but cast same length                  |
| group         | not matched the xRe string in it                                   |
| union         | not matched any xRe string in its options                          |
| count         | count of matched structure is not in the range                     |
| switch        | same expression but not matched the xRe string in it               |
| `@` variable  | not matched the xRe string of it                                   |
| `$` variable  | not matched the plain text xRe string of it                        |

#### Other Special Attributes

Character `~` suffix an xRe structure with an expression. Only matched successfully when expression is true, else failed.

## Internal Object

xRe grammar structures will be interpreted as xRe objects.
Every kind of structure except escape will be interpreted called same name object.

Specially, a group object will be view as a variable object with special variable name.

## Parser

An xRe parser is defined with a context of variables and a program interpreting xRe string to xRe objects.
Different parser has different context. 
Different xRe objects processed by the same parser will share a common context. 


### Escape Table

### Operator Table

