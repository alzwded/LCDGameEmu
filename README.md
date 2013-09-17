LCDGameEmu
==========

emulator of bad LCD games with hi-res graphics

Resources
=========

`NOTES.md` contains my notes on this project

the `doc` folder contains more (or less) coherent info about how to use lcdgameemu

the `tools` folder contains the vim syntax file for LGEScript (LGEScript is described in `NOTES.md` and `lge_guide.md`

TODO
====

* ~~specs~~ done
* ~~sprite object~~ done
* ~~gtk front end~~ ~~I meant SDL front-end~~ done
* ~~sprite graph state machine~~ done
* ~~storage format~~ done
* ~~loader~~ done
* ~~controls (almost done, ability to remap joystick hats/axes/buttons left)~~ done
* cmd line switch to set the clock cycle time
* rewrite cmdargs.c HandleParameters since it's of a lesser quality than I remember
* audio
* The Great Documentation Effort
* put the path to the currently loaded game in the titlebar

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

Testing
=======

```
lcdgameemu --load example/test-game.lge
```
loads the test game. This game doesn't actually do anything.

You can code your own little games in LGEScript since the engine is pretty much stable.

The console viewer and automated input can be useful in debugging your own little games. You can activate these with `--console-viewer` and `--input=<frame>:key,...`

There's also the `--test=N` parameter which runs a certain number of cycles and then exits.

Have fun!

Release notes thing
===================

0.6.3
-----

* add indexed addressing for registers. Syntax is `.offset <base> <index>`, where base can be a register (accessed directly or with another offset) and index is an arithmetic expression;
* updated the vim syntax file
* update lge_guide.md

0.6.2
-----

* basic tail ~~recursion~~ call optimization in interpreter

0.6
---

* joystick input

0.5.1
-----

* fix stupid crashes. My bad...

0.5-win32
---------

* initial win32 release

0.5
---

* Initial release
* working parser & interpreter, gui and keyboard input. You can pretty much begin using it as intended
