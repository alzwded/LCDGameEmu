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
* controls (joystick input left, everything else done)
* cmd line switch to set the clock cycle time
* rewrite cmdargs.c HandleParameters since it's of a lesser quality than I remember
* audio

Testing
=======

```
lcdgameemu --load example/test-game.lge
```
loads the test game. This game doesn't actually do anything.

You can code your own little games in LGEScript since the engine is pretty much stable.

The console viewer and automated input can be useful in debugging your own little games. You can activate these with --console-viewer and --input=<frame>:key,...

There's also the --test=N parameter which runs a certain number of cycles and then exits.

Have fun!

Release notes thing
===================

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
