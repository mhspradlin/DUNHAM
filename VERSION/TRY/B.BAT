@echo off
echo compiling demo
bcc -mh demo.c globals.c motor.c tracking.c track2.c slew.c isr.c utils.c prefs.c | more
