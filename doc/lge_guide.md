Intro
=====

This guide proposes to teach you how to write an LGEScript game descriptor.

Basic format
============

Overall, the file contains sections that define sprites, states, ~~sounds~~ and macros. You can have any number of these sections in any order. All sections have a unique unsigned integer id.

Sprite Sections
---------------

These have the syntax:
```
.sprite ID X Y "file_name"
```
This defines a sprite using the bitmap located in your game's assets folder with the name `file_name` at coordinates `(X,Y)` known internally as sprite #ID.

Macro and state sections
------------------------

These have the same syntax:
```
.state ID @code .end
```
and
```
.macro ID @code .end
```

A state and a macro can have the same ID since they're stored in different places, but two states (or two macros) must have different id's.

The code contains actions to be executed upon activating a state.

Writing action code
===================

Blocks and Statements
---------------------

A statement is an atomic operation. A block is a series of operations separated by `&`.

They only difference between having a block and simply chaining statements together is that conditional statements are short-circuit evaluated, so a conditional returning false can prevent the execution of the following statements in the block.

Assignation and output
----------------------

Both use the `.set` instruction with different semantics.

To enable a sprite (set it to active) use
```
.set N
```
Where N is the sprite ID you want to light up.

To brown out a sprite, use
```
.reset N
```

You can only assign data to registers. There are 100 registers, identified as `$0`-`$99`. They are initially 0.

You do this with
```
.set <register> <value>
```

The `<value>` is the result of an expression.

Expressions
-----------

There are arithmetic, conditional, rng and state-checking conditional expressions.

Arithmetic and conditional statements can all be bundled together and used as each-other's left or right operands. State-checking instructions and the rng instruction are atomic and non-assoc.

### Arithmetic expressions

These are in polish prefix notation. Available operations are:

| full name | alt char | description                                     |
|:---------:|:--------:|-------------------------------------------------|
| `.sum`    | `+`      | `+ a b` -- sum of a and b |
| `.sub`    | `-`      | `- a b` -- a - b |
| `.mul`    | `*`      | `* a b` -- product of a and b |
| `.div`    | `/`      | `/ a b` -- a / b |
| `.mod`    |          | the remainder of a/b |

### Conditional expressions

Again, polish prefix

| op       | description                                     |
|:--------:|-------------------------------------------------|
| `=`      | equality |
| `!`      | inequality |
| `<`      | a < b |
| `>`      | a > b |
| `~`      | not a, unary negation |

### State checking

You have `.set` and `.reset` again, only these time they check instead of setting. The operands can be unsigned int's (sprites) or an input register bit.

### RNG

You can invoke the RNG with `%.N` where N is a sort-of distribution thing. Basically `%.4` will return true 40% of the time and `%.99` will return true 99% of the time.

Branching statement
-------------------

You only get one. The `.if` statement.

The syntax is:
```
.if condition ; actions .fi
```

`condition` is a conjunction of conditional expressions separated by `&` and are short-circuit eval'd. For example: `.set 117 & = $0 10 & ~ < $1 10`

`actions` is a sequence of blocks. I.e. everything that does something. These are executed only if `condition` is true.

Subroutines
-----------

You only have one kind: macros.

These are defined in their own `.macro` section.

You can call a subroutine with `.call N` where `N` is the macro's id.

These macros don't take parameters since the only form of storage are the 100 registers I've mentioned earlier.

Loops
-----

Loops are not explicitly part of the grammar.

However, the interpreter supports basic tail-recursion optimization (if you really don't know what that is, check is out on wikipedia).

In a nutshell, this means that if the last instruction in a subroutine is a call to another subroutine (`.macro`'s in LGEScript), there is no need to push the return address onto the stack since the return address will be the same as in the additional call.

This means that a macro with tail recursion will not cause a stack overflow. If you try this trick anywhere else in the body of a `.macro` with a sufficiantly large number of calls, the C stack will overflow and the program will crash.

Here's how to do it:
```
.macro 123 # loop
.call 124 # do some stuff you'd do in a loop
.if
    < $99 $98 ; # this condition can be anything. it's here to actually
                # stop the recursion. This is the 'c' in 'while(c)'
    .call 123
.fi
```

The above code will execute whatever macro 124 does as long as the value in register 99 is greater than the value in register 98. As of lcdgameemu 0.6.2, this will not cause a stack overflow.

Here's how to loop from 0 to 9 (without accomplishing anything):
```
.state 14
.set $0 0
.set $1 11
.call 1
.end

.macro 1
.set $0 + 1 $0
.if < $0 $1 ; .call 1 .fi
```

Here's something else you can do:
```
.macro 0
.if = $56 3 ; .transition 2 .fi # instantly stops interpretation and on the
                                # next clock, execution will start in state
                                # 2
.set $56 .mod * $33 $29 7
.call 0
```

Theoretically, trail recursion optimization should kick in even if you daisy chain a number of macros that all have the property of their last instruction being the recursive call. However, be careful, as tail call optimization only kicks in on the very last instruction of a `.macro`.

If you're asking, yes, tail-call loops look better in lisp like languages. But really now, loops do not have their place in what this languages wishes to accomplish. LGEScript should rather describe what switches would be activated on a circuit board and when and what happens when you do that. It doesn't want to be a high-level language.

State transitions
-----------------

You can transition between states with the `.transition N` statement. This halts evaluation of further statements until the next clock cycle, when execution will begin in the new state.

Transitioning to the current state has the result of a 'return' statement.

Input register bits
-------------------

You have the following:
```
$left $right $up $down $fire $alt $start $toggle $upleft $upright $downleft $downright
```

Comments
--------

Comments are preceded by a `#`. This has the effect of commenting the line starting from that position onward. Until the next line, that is.

You can only read them with `.set` and `.reset`. They correspond to the physical input.

Syntax highlight
================

This project contains a vim syntax file. You can either use vim to edit this kind of files, or write a syntax highlight config file for you favourite editor based on the vim one. Or based on the grammar in parser.y.

Future changes
==============

Until lcdgameemu v1.0, this syntax and interpretation of things might change. After that, it's unlikely.

The most probable change would be to use different keywords for setting and checking the state of sprites (i.e. introduce the .isset keyword).
