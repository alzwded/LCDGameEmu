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

There are also a number of predefined read-only registers, namely $left, $right, $up, $down, $fire, $alt, $start, $toggle, $upleft, $upright, $downleft, $downright . Virtually, these represent keys that may or may not be available to the user. By default they are mapped to `left`, `right`, `up`, `down` `1`, `2`, `0`, `9`, `Q`, `P`, `A`, `L` by default. In the future, they may also be mapped to a joystick or user configurable.

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

Roadmap
=======

| version    | what                                                  |
|------------|-------------------------------------------------------|
| 0.3-pre    | input simulator and parser/interpreter validation     |
| 0.4-pre    | initial GUI in view mode                              |
| 0.5        | GUI with input                                        |
| 0.6        | joystick support, remappable input                    |
| ~~0.7~~    | ~~example game that demoes what this turkey can do~~  |
| 0.9        | sound? TBD                                            |
| 1.0        | gold                                                  |
| 1.1        | Playable pre-bundled game (think DK or EGG)           |
| 2.0        | nameable sprites, macros and states (ids are annoying)|
| 3.0        | macros with parameters                                |
| 4.0        | multiplayer                                           |
| 5.0        | netplay                                               |

TODO
====

~~Input simulator~~
-------------------

Implement debug input simulator;

I imagine something like:
```
lcdgameemu --console-viewer --test=20 --input=1:start,5:left,6:fire,10:right,11:fire,20:right
```

In more detail:
* input simulator is enabled together with the console viewer (which becomes a console viewer/constroller)
* the console viewer prints the status of the machine at frame #N, the input that was active during that frame and the sprites active after that frame, as well as the state of the registers after the frame
* If you want to push multiple buttons, just add two separate entries for the same frame, e.g.: `...,42:left,42:fire,43:up,...`
* you can imagine that using this input simulator and a real thing might prove problematic, so I guess I should add a flag to disable all other possible viewers or something weird like that :-/

Building on windows
===================

You'll need a decent C compiler like mingw-gcc, make, bison and the SDL 1.2.15 library with the development headers.

How I do it
-----------

If you want to know how I build for windows, here's a list:

1. Grab a working version of MinGW and install it and add the bin directory to your PATH system variable.
2. Also, grab bison from the GnuWin32 project. That one should add itself to PATH.
3. Grab the SDL 1.2.15 vc development package and dump it in the project's root. The one I'd gotten had the include files dumped in its include directory, so if that's your case, rename the `include` sub-directory to `SDL`
4. Then, go to the project's root directory and do a `mingw32-make -f Makefile.win32`. The makefile's different because the pkg-config doesn't exist on windows from my knowledge, and bison doesn't support some switches which generate stuff that's really only useful for debugging and fanciness. Mostly fanciness.
5. To run it, you need SDL.dll in your PATH somewhere. The easiest method to get it running is to copy it next to lcdgameemu.exe

Protip
------

Installing cygwin with gcc, buildutils, make, bison and sdl is probably a better idea. I didn't do that because cygwin has a really weird license when it comes to distributing stuff built with it, which I don't understand too well. But it's okay if you grab cygwin and build the project yourself.

If you're going to do it on cygwin, then a simple `make` with the default `Makefile` will suffice.

Other compilers
---------------

Theoretically, you can build this with any somewhat modern day C compiler that's heard of C99. I think the only problem is, really, the `assert()` calls, but those errors should be silenced by adding a stub `assert.h` header to the project and putting `#define assert() /* NULL */` in it.

But it's likely that without a `make` compatible utility, you'd need to define a new project file. It's up to you. The code's pretty much standard C where the only C99 thing I'm aware of using are the `assert()`'s before variable deffinitions. But judging from the errors/warnings gcc in std C mode gives me when I use C99 features, those asserts are probably conformant. Don't quote me on that one.

Well, good luck to you if you plan to build this potato on windows. Let me know how it works out for you!

The Great Documentation Effort
==============================

Code is scarcely documented (i.e. using NoDOC technology). This will need to change.

All headers should have line-by-line comments, since everything in a header should be relevant, otherwise it should not be there.

All .c files hould have a module-wide description and have weird stuff documented for posterity.

All files should have something like a standard header:
/*
<license>
*/
/* <filename>
// 
// <description>
// 
// <implementation dependencies, e.g. SDL>
*/

Static functions should have a minimum of documentation.

Implementation (i.e. defined-in-a-.c-file-) data structs should have the same line-by-line documentation as a header data struct would have. They are essentially the same, but these are probably better to be masked from outside use (for sanity's sake)

The overall architecture should be well documented in a ARCH.md guide or something like that.

Using a joystick/gamepad
========================

If you are a programmer who's worked with joysticks in SDL before, then it should be easy. If you're not, skip to the next sub-section.

You have `--list-joysticks` which lists the devices SDL detects. Then, `--use-joystick`, `--joystick-use-hat` and `--joystick-x/yaxis` tell the main input loop which device/hat/axis to use for this role. Buttons are remapped like keys, so something like `--joystick-map-buttons=fire:0,alt:1,start:9,toggle:8` where the keys are the (internal) key names and the numbers are the button ids.

Mapping your gamepad like a pro
-------------------------------

If you aren't a programmer or haven't had any contact with coding for input devices, then these switches probably leave you confused and your gamepad ina non-working state. But fear not, LCDGameEmu's builtin debugging messages will help you figure out what you're doing.

Open up something like notepad and start writing.

### Step1

Figuring out which is your joystick and what to pass to `--use-joystick`. This one's easy, run `lcdgameemu --list-joysticks`. Identify the one you want to use, and note the number in the left column. (e.g. the first one's 0, the 3rd one's 2)

Write in your open text editor (i.e. notepad) `--use-joystick=0` (or the number corresponding to the joystick you want to use, if you have multiple ones connected.

### Step2

Run `lcdgameemu -d2`. The example game should be good enough. Now, pick a button on your controller that you want to use for e.g. FIRE. Now spamm it.

Now look at the console. There should be a lot of output like
```
11:47:33 sending input:                     020
11:47:33 joystick button input              1
11:47:33 sending input:                     020
11:47:33 joystick button input              1
11:47:33 sending input:                     020
11:47:33 joystick button input              1
11:47:33 sending input:                     020
```
The important part there is "joystick button input.............1". In my case, I kept spamming a button which is identified as "1". So I write it down in my open notepad window as `--joystick-map-buttons=fire:1`.

Repeat for the alt, start and toggle buttons and you should have something like `--joystick-map-buttons=fire:1,alt:0,start:4,toggle:9`.

### Step3

Now it's time for your dpads or analogue sticks. Now, certain non-high-end gamepads behave very oddly, but we'll sort it out.

Basically do the same as for the button mapping, but this time spam your dpad or analogue stick left-to-right. Now, output may vary between:

1)
```
11:51:13 joystick axis input (axis/value)   2
+        0
11:51:13 joystick axis input (axis/value)   2
+        -32768
11:51:13 joystick axis input (axis/value)   2
+        0
11:51:13 joystick axis input (axis/value)   2
+        32767
```
This means the gamepad's digital d-pad thinks it's an analogue stick. No worries. This output tells you what analogue value the Nth axis is sending. In my case it was axis 2. If I repeat the same spamming processes for "up/down", I'll get axis 3. So update your notepad window with `--joystick-xaxis=2 --joystick-yaxis=3`

2)
```
11:51:13 joystick hat input                 0
+        0
11:51:13 joystick hat input                 0
+        4
```
In this case it means the dpad identifies itself as a POV hat. So write down `--joystick-use-hat=0`.

### Putting it all together

Now you can paste everything you're written down together as command line arguments to lcdgameemu. E.g.:
```
lcdgameemu --load=my_super_game.lge --use-joystick=1 --joystick-map-buttons=fire:1,alt:0,start:4,toggle:9 --joystick-xaxis=2 --joystick-yaxis=3 --joystick-use-hat=0
```
And now your joystick/gamepad should be working.

### Why isn't there a GUI for this?

It's planned for version 19.5 . You can check the current version with
```
lcdgameemu --version
```
It will probably say something like `lcdgameemu 0.6` or `lcdgameemu 0.6-win32`
