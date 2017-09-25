XL320 Arduino Libraries
=======================

- **examples/serial-brige**: this is the sketch to use the host library
- [librairies/XL320](libraries/XL320/README.md): this is the arduino driver for XL320 servos
- [librairies/XLCMD](libraries/XLCMD/README.md): this is the lib and sketch to get a XL320 control terminal

Install in Arduino IDE
----------------------

Just find the install dir of your arduino IDE and copy XL320 libs and examples in the same folders.

```bash
ls arduino-1.8.4/

# arduino  arduino-builder  examples  hardware  install.sh  java  lib  libraries  reference  revisions.txt  tools  tools-builder  uninstall.sh

cp -rvf patch-to/woo-xl320/arduino/* arduino-1.8.4/
```
