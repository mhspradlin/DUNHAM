@echo off
echo compiling demo
bcc -mh -v demo.c globals.c motor.c tracking.c slew.c isr.c utils.c prefs.c | more
