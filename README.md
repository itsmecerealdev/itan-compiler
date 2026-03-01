# itan-compiler

## Why?
- I was bored, so I started learning how the steps of compilation works. This is the result of that.

## Should you use it?
- For what? This is an unnamed language, and has no functionality currently other than reading single line inputs (for now) that are integer variables and equations
- It literally only exists for me to learn.
- It doesn't even handle errors in a user friendly way. It throws at the first error and calls it good.

## Well, if you're still reading, I guess I may as well explain the semantics and syntax. This is subject to (and will often) change.

### Syntax
- Types: (not implemented fully, but are placeheld. More will come as functionality is added) **There is no type-checking or any of that currently. It's planned soon.**
  - int32
  - int64
- Symbols (anything outside of this list will throw an error during lexing.)
  - \+
  - \-
  - \*
  - /
  - ( )
  - ;
  - =

### Statement vs. Expression
- In this language, a statement is a block of anything terminated by a ;
  - x = 7; int32 x = 5; x + 7 * 9 ^ (2 * 1); is three statements
    - x = 7 assigment statement
    - int32 x = 5 declaration statement
    - x + 7 * 9 ^ (2 * 1) expression statement
- On the other hand, an expression are the "functionalities" in the statement.
  - This can be: (subject to change)
    - an integer literal : 5;
    - an identifier : x; (returns the value curreently stored in x)
    - an operand that will return the evaluation of its left and right.

### Variables/Declarations
- A declaration is structured as (type) (identifier) = (expression);
  - Uninitialized declarations are currently not allowed in the language
  - If a case where it is necessary appears, I will allow it, though it will UB.
- Assignment is (identifier) = (expression);

- The assignment and declaration (expression)s can be full expressions, they are not limited to other variable identifiers or integer literals. They just cannot self-assign during declaration.

- This "language" is use if declared, not before or after.
  - As long as it is declared in scope (not implemented yet, but soon) it may be used.
  - This means that : x = 5; int32 x = 7; x; is valid and will evaluate to 7.
  - AND : x = 5; x; int32 x = 7; is ALSO valid and will evaluate to 5.
  - This is intentional, even though seemingly odd.
    - It's not intended as a "gimmick" that lets you have a temporary value before the declaration resets it or whatever- I just prefer the convenience.

## Architecture
- Currently, this language is interpreted.
  - It is using a double dispatch based visitor system to walk the AST during runtime.
  - Currently there are 4 visitors
    - PrintVisitor (unPretty Printing)
    - DeclarationVisitor
      - This is the first of the (currently) two pass semantics validation structure
        - It stores declarations in a symbol table
        - It also checks for double declaration in scope
    - SemanticsVisitor
      - This is the second pass of the semantics validation structure
      - It is verifying that any variable used in scope is declared.
      - It is also verifying no self assignment in declarations.
        - int32x = x + 5; is an error. If there is ever SOMEHOW a reason in the future, this may become UB in the future instead.
    - EvaluationVisitor
      - As the name suggests, this visitor evaluates the tree. Currently, it only evaluates the last expression statement it encounters
        - This will change, as upcoming changes include {} scoping and functions.
      - It walks the tree, pushing values to a stack, and popping them to evaluate the current operation in the tree.
      - For example:
            *
          /   \
        4      +
             /  \
            3    7
          - it first visits the operand node *
            - it then visits the left node, and pushes 5 to the stack
            - it then visits the right node
              - it then visits the left node of the + operand node, making the stack 5, 3
              - it then visits the right node, making the stack 5, 3, 7
            - back in the + node, it pops right then left, leaving just 5 in the stack, and pushes the result of 5 + 7, so the stack is 4, 12
        - back in the * node, it pops right, then left, and will output the result of 4 * 12, so 48.
          - It outputs the result instead of storing it in a stack or symbol table because scoping/multi function evaluation is unsupported, and the * node is the last occuring root expression statement.
          - The evaluation visitor does still evaluate the other statements propery and store the returns in the stack- but it only prints out the final one for testing purposes as no printing keyword or functionality currently exists in my language.

## Planned 
- Functions/scopes using {} 
- Type checking + widening/narrowing rules
- overflow protection
- A new keyword for printing to screen.
