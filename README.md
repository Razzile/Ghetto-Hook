# Ghetto Hook
![](https://pbs.twimg.com/media/CsEWAA_WAAAVi-4.jpg)

A PoC library that aims to showcase function hooking on multiple architectures with minimal patching
to executable code (a maximum of 1 instruction patch needed, although it can be achieved with none).

Ghetto Hook works by installing a breakpoint at the specified address and handling the exception it raises,
redirecting the 'PC' register to a target function in the process.

I have already achieved this in another project but the code is messy and dependent on other parts of the
project so I have decided to create a separate repo dedicated to ghetto hook.

It is so named because of the unorthodox nature of the hooking method and because of the pretty bad code; this is my first decent-sized project in pure C.

---
For an example of the method ghetto hook uses in action right now, please see my other project [Liberation](https://github.com/Razzile/Liberation/tree/unfinished)
