
# xRe

A better regular expression.

## Grammar

### Grammar Structures

#### Escape

An escape is `\ ` concat with a xRe meta character or a unicode number.
xRe views escapes as plain characters, according to the [escape table](#escape-table).

#### Plain Text (Sequence)
Plain text(`sequence`) is strings have no xRe meta symbols. 
xRe will do no operation for them at parse stage. 

Match a sequence is to match every character once in order, even exists duplicates.

#### Set

A `set` is characters bracketed with `[` and `]`. 
These characters include plain text characters, and `-`. 
But `-` must be not at start or end of set characters, also not divided with only one character.

Match a set is to match any none `-` character in the set's characters once, but include characters between
the two characters concat with `-`.

#### Group

A `group` is an xRe string bracketed with `(` and `)`. 

Match a group is match the xRe string in it.

#### Union

A `union` is some xRe strings divided by `|`. 

Match a union is try to match any xRe string of the union once, and take the longest result.

#### Count

A `count` is some expressions bracketed with `{` and `}` but divided with `,`. 
Number of expressions is 0 to 3. 
First expression is the lower bound(included), and the second is the upper bound(included).
The third expression is the step length of match, and its value must be a positive integer.
If the number of expressions is 0, then the count range is 0 to infinity, and step is 1.

Count will not match. But it will let the match program repeat the last struct before it at least its
lower bound and at most its upper bound, in times of step length.

#### Switch

A `switch` is an expression and an xRe string bracketed with `<` and `>` but divided with `?` in order.

If value of the expression is not a zero, the match program will match the xRe 
or match a part of union according to the value if the xRe string is a union.

#### Expression

An `expression` is special strings which not to match, but will be calculated value with match program context.

And there are some different expressions:
1. **Const Expressions**: Const expression means can be computed and replaced with their value before match stage.
2. **Labels**: Labels are those identifiers consistent with only lower letters, digits or underscore, and must start with lower letters.
   1. Using `=` to give an xRe structure a label, only can be using in non-expression and non-set structures.
   2. Using `@` to apply the structure of called label in match program, only can be using in non-expression and non-set structures.
   3. Using `#` to apply a copy of structure of the called label, only can be using in expressions.
   4. Using `$` to apply the last matching result of a label, treat as a plain text structure,
      only can be using in non-expression and non-set structures.

   When assign or call a label, the label must be bracketed with `<` and `>`, and can be suffixed with `:` followed some attributes.

   More details see [label](#label).
3. **Functions**: Functions are those identifiers that stored in match program and can be called.
    The name of functions must be all UPPER letter, digits and underscore, and must start with UPPER letters.
    Argument(s) of functions are some expression(s) bracketed with `(` and `)` after the name,
    and if more than one should be divided with `,`.
4. **Operator Expressions**: other expressions are operator expressions. normally them have at least one operator.
    Operators will be list in the [operator table](#operator-table).

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
xRe attributes tell parser and match programming how to process the suffixed structure.
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
| `@` label     | as the structure the label refers                                  |
| `$` label     | as the plain text structure                                        |

#### Other Special Attributes

The expression after the character "~" will be used as a suffix attribute to the xRe structure. 
The match succeeds only if the expression is true, otherwise the match fails.

## Internal Objects

xRe grammar structures will be interpreted as xRe objects.
Every kind of structure except escape will be interpreted called same name object.

Specially, every `group` object will be assigned a special label name.

## Parser

An xRe parser consists of a program that parse xRe strings and a set of label contexts.

Different parsers have different contexts.

Different xRe objects generated by the same parser will share a common context.

But group labels of different xRe strings will not be shared.

### Escape Table

### Operator Table

## Expression

### Label


