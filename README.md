# itan-compiler

## Why?
- I was bored, so I started learning how the steps of compilation works. This is the result of that.

## Should you use it?
- For what? This is an unnamed language, and has no functionality currently other than integer variables and equations, and basic printing / semantics passes
- It literally only exists for me to learn.
- It doesn't even handle errors in a user friendly way. It throws at the first error and calls it good.
  - The errors are also not very descriptive currently.
    - For example during parsing, instead of printing Unexpected token ( does not match expected qualifier ; it prints out the integer representation of the TokenType enum for each token.
      - This is because the tokentype enum is still not cemented, so it prints the integer index representation of the type in the enum.
      - A much nicer error message system is planned soon. It will not have advanced recovery systems, it will gather errors from the current phase of the first error, collect all following errors in that phase, then throw.
        - It will not advance to the following phases. And the parser I will try to implement functionality to protect from cascading errors, but no promises.

## Well, if you're still reading, I guess I may as well explain the semantics and syntax. This is subject to (and will often) change.

### Syntax
- Types: (not implemented fully, but are placeheld. More will come as functionality is added) **There is no overflow protection currently, but basic type checking exists. Overflow protection is planned**
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
  - ,
  - {
  - }
  - => (return)

### Statement vs. Expression
- In this language, a statement is a block of anything terminated by a ;
  - x = 7; int32 x = 5; x + 7 * 9 ^ (2 * 1); is three statements
    - x = 7 assigment statement
    - int32 x = 5 declaration statement
    - x + 7 * 9 ^ (2 * 1) expression statement
    - => 7; == "return int64(7)"
- On the other hand, an expression are the "functionalities" in the statement.
  - This can be: (subject to change)
    - an integer literal : 5;
    - an identifier : x; (returns the value currently stored in x)
    - an operand that will return the evaluation of its left and right.
    - cast or call nodes as they return values. (even void functions when functions are added, the calls will be expressions. Declarations will be statements.)

### Variables/Declarations
- A declaration is structured as (type) (identifier) = (expression);
  - Uninitialized declarations are currently not allowed in the language
  - If a case where it is necessary appears, I will allow it, though it will UB.
- Assignment is (identifier) = (expression);

- The assignment and declaration (expression)s can be full expressions, they are not limited to other variable identifiers or integer literals. They just cannot self-assign during declaration.

- This "language" is use if declared, not before or after.
  - As long as it is declared in scope it may be used.
  - This means that : x = 5; int32 x = 7; x; is valid and will evaluate to 7.
  - AND : x = 5; x; int32 x = 7; is ALSO valid and will evaluate to 5.
  - This is intentional, even though seemingly odd.
    - It's not intended as a "gimmick" that lets you have a temporary value before the declaration resets it or whatever- I just prefer the convenience.

## Architecture
- Currently, this language is interpreted.
  - It is using a double dispatch based visitor system to walk the AST during runtime.
  - Currently there are 5 visitors
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
    - TypeVisitor
      - As the name suggests, this visitor passes over the tree following declaration and semantic passes
      - It enforces type rules such as (More planned as new types are added):
        - Integer narrowing requiring explicit casts. (int32 x = 5;) will not compile as int literals are currently int64 by default.
          - As development progresses, int literals will likely become int32s, but I have not yet implemented the tagged union required to store them.
    - EvaluationVisitor
      - As the name suggests, this visitor evaluates the tree.
        - It evaluates every branch of the tree, even if the expressions are seperate.
          - for example: x + 5; x + 6; both will evaluate, and the order of values on the stack is right to left.
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
        - back in the * node, it pops right, then left. At this point, it is complete.

## Built in Functions:

- Currently only 1
  - print(expression);
    - This prints out the expression provided during the user. Takes exactly 1 input currently
  - casts are not technically functions in this lang, they construct a new variable in place with the proper widening/narrowing required
    - When types other than ints are added, I will also add relevant type conversion restrictions. 

## User Defined Functions (Not thoroughly tested- expect bugs)

- The architecture of a user defined function is quite simple
  - The only hard set rule currently is the "paired return rule"
    - This rule states if your function signature contains a return (=>), then your function body must ALSO contain a return.
      - If you wish to return nothing, do not include a return in the signature. Return statements in "void" functions still work, and incorrectly return whatever expression they contain to the stack, but that is later-me's problem. Now me is just happy this works at ALL.
     
-To call a user defined function, it is like calling a C++ function with modifications to how parameter defaulting works
  - In C++, to call a func with signature foo(int x, int y = 32) - foo(7) will set x = 7, y = 32.
  - Not in itan
    - in itan, your call must have as many expressions as parameters. To perform this same call, you must do foo(7,);
      - It is annoying, but has one AMAZING upside.
        - If the func signature was instead foo(int x = 7, int y) - C++ cannot default x in any instance. foo(32) does not set x = 7, y = 32.
        - BUT in itan, with foo(,32), it DOES. ABSOLUTE    CINEMA 
``` 
foo(int64 i = 5, int64 y = 7) => int64 {
  ...
  ...
  => i + j;
}
```
```
bar(int64 k = 12) {
  k = k + 12;
  =>;
  5 + 5;
}
```
- The 5 + 5; expression here is properly orphaned as unreachable code.
```
foo2(int64 L = 17) => int32 {
  => int32(L = 8);
}
```
- Return type is int32? must cast to int32 first.

## Planned 
- overflow protection

## Running an example test.it file.

### Example test file syntax

- ALL FILES MUST HAVE .it EXTENSIONS.
  - Comments are not currently supported, so I'll put it here- the third line DOES evaluate, but it doesn't assign anywhere so it's orphaned basically.
    - if you put print( ) around it, you can see it does evaluate.

```it
foo (int64 i = 5, int64 j = 7) => int64 {
    print(i);
    print(j);
}

print(foo());
print(foo(7));
print(foo(,6));
```
- Function calls can exist in global space, and honestly I don't know if this will be "fixed" to only allowing them and executing them in some "main" function or not any time soon, if ever
  - This may go beyond the intended scope of the project (user defined functions were well beyond the intended scope of a funny little math programming lang, but here I am 2 and a half weeks and like 40 hours of development time later successfully (I think) adding them.)
### Example test file execution and output

- **Note:** All files must have `.it` extensions. Comments are not currently supported.  
- To "compile" the file, first do "make" in the directory to generate the interpreter a.out file
  -  then, type "./a FILENAME"
    - Don't include the extension, as it is added by the compiler. JUST the filename. 

### Output of the example

Not-so-pretty printer
```plaintext
Program
    {
        Func foo
            Param i
                Number(5)
            Param j
                Number(7)
            {
                print:
                Variable i
                print:
                Variable j
                Return:
                    Operand 1
                        Variable i
                        Variable j
            }
        print:
        foo : params ->
        print:
        foo : params ->
            Number(7)
        print:
        foo : params ->
            ,
            Number(6)
    }
```
Evaluation-
```
5
7
12
7
7
14
5
6
11
```
