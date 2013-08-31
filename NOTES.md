Organization of a game project
==============================

directory layout:

| file                | description                                    |
|---------------------|------------------------------------------------|
| game.lge            | sprite descriptor and state graph (lgescript)  |
| game.assets/bg.png  | background                                     |
| game.assets/*.png   | sprites                                        |

general:  
* state transitions happen on a 100ms clock

game.lge:
* the game descriptor
* contains definitions of sprites, sounds (not implemented, TODO v2), states and macros
* refer to LGEScript

bg.png:  
* the backdrop that will be displayed throughout the game
* size of the game's window determined by size of bg.png

*.png:  
* sprite data. They should contain transparency information (png can do that)

Launching a game
================

```
lcdgameemu --load path/to/game.lge
```

In the future there will be a number of options you can pass to the exe. To see them all you can lcdgameemu --usage .

Right now I can tell you that there is the --debug-level option which sets the amount of stuff the exe prints out, the --load option which loads a game and --help option which is the same as --usage .

game.lge Grammar
================

The language is called LGEScript. It's a hybrid declarative/iterative/state-machiney scripting language. Wrap your head around THAT!

c.f. with parser.y the grammar section.

EXAMPLE FILE
============

c.f. examples/test-game.lge

LGEScript
=========

The game logic is programmed using the internal scripting language, LGEScript.

It's a very simple and light-hearted imperative and state-machine based language.

States are defined in .state sections. They all have a unique id which will identify them.

Transitions between states are handled by .transition statements in the code section. When a transition is encountered, execution of the currnet state is halted and on the next clock cycle execution will begin in the state the transition was made towards.

There are a total of 100 internal registers you can use to store temporary data. They are BYTE-sized. The are accessed using the $N syntax. The number 100 is completely arbitrary and in the future it may be configurable.

There are also a number of predefined read-only registers, namely $left, $right, $up, $down, $fire, $alt, $start, $toggle, $upleft, $upright, $downleft, $downright . Virtually, these represent keys that may or may not be available to the user. By default they are mapped to <left>, <right>, <up>, <down> 1, 2, 0, 9, Q, P, A, L by default. In the future, they may also be mapped to a joystick or user configurable.

Register data is written using the .set instruction. For example, .set $1 0 assigns the int value 0 to register 1. The lvalue needs to be writable. The left value can be a literal constant, another register, the state of a sprite, or an expression.

Data is read with comparisson operators: = (EQ), ! (NE), < (LT), > (GT), ~ (NOT). The state of a sprite is read using .set <number> or .reset <number>.

Conditional statements are formed with the syntax: .if <condition> ; <action> .fi

Condition needs to be an expression that returns a value. Multiple conditions can be converged (i.e. AND-ed) using &, for example .if .set 1 & = $5 42 ; ... .fi .

Actions can be any arbitrary code block. A block is formed by chaining instructions with &. So a full if would be .if .set 1 & = $5 42 ; .set $5 0 ; .transition 2 .fi

Sprites are either active or inactive. Their value is read and written using .set N and .reset N . A sprite is identified by its unique int id. There is also the special operation .reset .all which disables all sprites.

Arithmetic operations are in polish notation (everything is in polish notation if you think about it). The valid operations are:

| char | alternative name | description    |
|------|------------------|----------------|
| +    | .sum             | addition       |
| -    | .sub             | substraction   |
| *    | .mul             | multiplication |
| /    | .div             | int division   |
|      | .mod             | modulo         |

The operands can be other expressions or literal constants. There is no need for parentheses because of the polish notation.

Use of whitespace is completely liberal, but, for the moment, because the lexer is written by hand, there needs to exist at least one piece of whitespace between tokens. (i.e. even though I'd love it to work with .if.set3&=$7 8;.transition9.fi , it doesn't. Also, history's taught us completely ignoring whitespace (like FORTRAN77) can lead to loss of human life because of a typo) <-- pretentious

If you use the same chunk of code multiple times, you can store it in something we call a macro. This is defined in a .macro section, it has a unique int id and code. You can then call this macro using the .call N syntax. This will move execution to that macro and upon return execution will continue from where it left of.

There is no notion of local/global/thread_local variables here. All you have to work with are the 100 r/w registers and the states of sprites you'd defined. So macros do not take parameters nor do the return values (not explicity, at least).

Comments are delimited by # and EOL. # this is a comment
This is not.

The would have been no point to introduce the notion of "variables" or "stacks" since in that case, someone would've been able to actually implement decent quicky-games on this thing. That is not the point. The point is to force people to create emulations of bad chineso-british LCD games (like Nintendo's Game&Watch games, only bad and done in a hurry with 0 funding or research).

For more info on the scripting language, examine the test-game.lge closely.

Register machine description
============================

This describes the emulated machine your code runs on.

Registers
---------

$0-$99: 100 general purpose BYTE sized unsigned registers. This means that normally you should be able to store values from 0..2^8-1. They are accessed using $N from code.

Instruction pointer: Internal register. This is literally a code_t* that stores the currently executing instruction. After a clock cycle / transition, this is set to the first instruction to be executed. It is set to the first instruction of the current state upon a clock pulse.

Stack pointer: Internal register. This is used to store the return information upon entering a .call . It is theoretically limitless. It is implemented as a vector, so it grows as it is needed. Initially it is 2 and grows exponentially afterwards. It's an infinite stack for all inherent purposes. It is initially blank. It is empty before the execution of code on the clock posedge. (because if it wouldn't be, you would most likely have halted the emulated CPU with an infinite loop or something)

State pointer: Internal register. It points to the currently active state. On the clock posedge is sets the IP to the first instruction of state pointed to by this. It is modified using the .transition instruction in code.

Sprite register: Internal register. It is a register whose width is that of the number of sprites you have defined. Active bits cause sprites to be lit (i.e. made visible) on the display. This can be modified and/or queried using the .set and .reset logical statements.

Input register: Internal register. Individual bits are connected to the various outside input buttons. The values can be querried using the special $<name> registers. Even though they're only bits, they appear as wider registeres. You should query them something like: .if ~ $left ; or .if ~ ~ $right ;

Display
-------

The sprites you defined will appear on-screen with their top-left corner at the coordinates you define. Transparency is handled.

A sprite is displayed if its bit (hence the unique int ids of the sprites in an .lge file) is set. These bits are set using the .set or .reset statements.

There is an internal path-name translator that makes sure a bitmap is only loaded once even if it's referenced multiple times.

Clock
-----

The clock visible to the outside world is the internal one that runs at 100/200ms intervals. State transitions happen at this interval. Upon a transition, the code assigned to that transition will trigger. The internal code interpretation is done "instantaneuously", i.e. at the host's CPU speed. This emulated the hard-wired behaviour many of these things seem to have.

On the communication between the GUI and the BackEnd
====================================================

BackEnd
-------

The entity BackEnd will contain:
* the emulated machine data instance (as described above)
* the interpreter
* the BackEndGameData
* a callback that updates the internal state: set_input(int reg, BOOL state)
* a callback to trigger a clock impulse to start the interpreter for one cycle: onclock()
* query functions:
    - get_max_num_sprites(int& nsprites)
    - get_active_sprites(int& sprites[], int& nsprites), memory should be preallocated
    - get_active_sound(int& id) // not implemented, TODO v2
* an init method: init_machine(THEGAME)

### BackEndGameData

The BackEndGameData contains, at a minimum of:
* the code assigned to each state, stored in an associative array or state_id - state_code
* the stored macros in an AssocArray
* the current state (state pointer)
* the macro call stack
* query methods:
    - code_t* get_state_code(state_id)
    - code_t* get_macro_code(macro_id)
    - state_id get_current_state()
    - stack_t* get_call_stack()

### The interpreter

Interface:
* start()
* (-) call_macro(macro_id)

GUI
---

The entity GUI will contain:
* the main rendering window
* an associative array between sprite unique ids and the graphic they represent
* host input handling
* sound engine (not implemented, TODO v2)
* an init procedure: init_gui(game_t* THEGAME)
* a 100ms clock that triggers the BackEnd's interpreter
