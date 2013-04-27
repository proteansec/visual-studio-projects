Copyright (C) 2012-2013, Dejan Lukan

All rights reserved.

The Visual Studio Projects contain the projects described below.

* **createfile**

  A project that was used in the [The Sysenter Instruction and 0x2e Interrupt](http://resources.infosecinstitute.com/the-sysenter-instruction-and-0x2e-interrupt/) article.

* **createremotethread**

    A project that uses CreateRemoteThread function to inject a DLL into chosen process's address space.


* **dllinject**

    The DLL that can be injected into some process so it can hook the IAT import table.

* **dlltest**
   
    This is a sample DLL that was used in the article accessible here: [Api Hooking DetoursInfosec](http://resources.infosecinstitute.com/api-hooking-detours/).

* **flags**

    A very simple program for reverse engineering where you can see the difference between creating an object on stack versus the heap.

* **helloworld**

    This is a "Hello World" project that uses a software interrupt to instruct a debugger to pause execution just before the printf statement is reached. Basically it uses the __asm { int 0x3 }; code block.

* **isdebuggerpresent**

    A project that uses IsDebuggerPresent function that can detect whether a debugger is being used to debug the current program. There are two articles where this project is relevant:
    [Anti-Debugging: Detecting System Debugger](http://resources.infosecinstitute.com/anti-debugging-detecting-system-debugger/)

* **kerneldebug**
  
    A simple project that uses the NtQueryInformationProcess function to get information from the process.

* **linearsweep**
  
    This program was used in the following article [Linear Sweep vs Recursive Disassembling Algorithm](http://resources.infosecinstitute.com/linear-sweep-vs-recursive-disassembling-algorithm/)

* **setwindowshookex**

    A project that uses SetWindowsHookEx function to inject a DLL into chosen process's address space.


