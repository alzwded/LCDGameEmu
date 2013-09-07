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
* ~~gtk front end~~ I meant SDL front-end (viewer, partially, ongoing)
* ~~sprite graph state machine~~ done
* ~~storage format~~ done
* ~~loader~~ done
* controls (scripted input done, ongoing)
* cmd line switch to set the clock cycle time

Testing
=======

Since it doesn't really do anything right now because the input is not implemented, you can test it with
```
lcdgameemu -C --input=6:right,10:left,12:right
```
to actually see it do something :-D
