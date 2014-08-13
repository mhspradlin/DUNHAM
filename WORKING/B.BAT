@echo off
echo compiling demo
bcc -mh demo.c globals.c motor.c tracking.c slew.c prefs.c isr.c utils.c | more
