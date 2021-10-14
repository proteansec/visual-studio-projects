Copyright (C) 2012-2014, Protean Security

All rights reserved.

The Visual Studio Projects repository contain the following projects described below!
* **dga**

    Code samples for domain generation algorithm (DGA) article.

* **createfile**

    A project that was used in the [The Sysenter Instruction and 0x2e Interrupt](http://www.proteansec.com/reverse-engineering/the-sysenter-instruction-and-0x2e-interrupt/) article.

* **createremotethread**

    A project that uses CreateRemoteThread function to inject a DLL into chosen process's address space.


* **dllinject**

    The DLL that can be injected into some process so it can hook the IAT import table.

* **dlltest**
   
    This is a sample DLL that was used in the article accessible here: [Api Hooking Detours](http://www.proteansec.com/exploit-development/api-hooking-detours/).

* **flags**

    A very simple program for reverse engineering where you can see the difference between creating an object on stack versus the heap.

* **helloworld**

    This is a "Hello World" project that uses a software interrupt to instruct a debugger to pause execution just before the printf statement is reached. Basically it uses the __asm { int 0x3 }; code block.

* **hookidt**

	Windows kernel driver that hooks IDT descriptors and runs arbitrary function when the chosen interrupt is invoked.

* **hookmsr**

	Windows kernel driver that hooks MSR entries and runs arbitrary function when the sysenter instruction is used for system calls.

* **hookssdt**

	Windows kernel driver that hooks SSDT system calls, which is able to hook arbitrary system calls regardless of whether they're being called with "int 0x2e" or sysenter.
	
* **isdebuggerpresent**

    A project that uses IsDebuggerPresent function that can detect whether a debugger is being used to debug the current program. There are two articles where this project is relevant:
    [Anti-Debugging: Detecting System Debugger](http://www.proteansec.com/reverse-engineering/anti-debugging-detecting-system-debugger/)

* **kerneldebug**
  
    A simple project that uses the NtQueryInformationProcess function to get information from the process.

* **linearsweep**
  
    This program was used in the following article [Linear Sweep vs Recursive Disassembling Algorithm](http://www.proteansec.com/reverse-engineering/linear-sweep-vs-recursive-disassembling-algorithm/)

* **setwindowshookex**

    A project that uses SetWindowsHookEx function to inject a DLL into chosen process's address space.


