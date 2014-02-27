#include <wdm.h>
#include <ntddk.h>

#include <excpt.h>
#include <ntdef.h>
#include <ntstatus.h>
#include <bugcodes.h>
#include <ntiologc.h>

#define IA32_SYSENTER_EIP 0x176

/* Function Prototypes */
NTSTATUS MyDriver_UnSupportedFunction(PDEVICE_OBJECT DeviceObject, PIRP Irp);
VOID MyDriver_Unload(PDRIVER_OBJECT  DriverObject);    
NTSTATUS DriverEntry(PDRIVER_OBJECT  pDriverObject, PUNICODE_STRING  pRegistryPath); 

/* Compile directives. */
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, MyDriver_Unload)
#pragma alloc_text(PAGE, MyDriver_UnSupportedFunction)


#pragma pack(1)
typedef struct _MSR {
  UINT32 value_low;
  UINT32 value_high;
} MSR, *PMSR;
#pragma pack()

#pragma pack(1)
typedef struct _THREADARG {
	UINT32 reg;
	UINT32 hookaddr;
} THREADARG, *PTHREADARG;

/* Global variable for storing old MSR address. */
UINT32 oldMSRAddressL = NULL;
UINT32 oldMSRAddressH = NULL;
UINT16 numActions = 1000;

/*
 * Get the value of 'reg' MSR register.
 */
MSR GetMSRAddress(UINT32 reg) {
    MSR msraddr;
	UINT32 lowvalue;
	UINT32 highvalue;
	
	/* get address of the IDT table */	
    __asm {
		push eax;
		push ecx;
		push edx;
        mov ecx, reg;
		rdmsr;
		mov lowvalue, eax;
		mov highvalue, edx;
		pop edx;
		pop ecx;
		pop eax;
    }
	msraddr.value_low = lowvalue;
	msraddr.value_high = highvalue;
	DbgPrint("Address of MSR entry %x is: %x.\r\n", reg, msraddr);
	
	/* store old MSR address in global variable, so we can use it later */
	oldMSRAddressL = msraddr.value_low;
	oldMSRAddressH = msraddr.value_high;
	
	return msraddr;
}

/*
 * Set the value of 'reg' MSR register.
 */
void SetMSRAddress(UINT32 reg, PMSR msr) {
	UINT32 lowvalue;
	UINT32 highvalue;
	lowvalue = msr->value_low;
	highvalue = msr->value_high;
	
	/* get address of the IDT table */	
    __asm {
		push eax;
		push ecx;
		push edx;
        mov ecx, reg;
		mov eax, lowvalue;
		mov edx, highvalue;
		wrmsr;
		pop edx;
		pop ecx;
		pop eax;
    }
	DbgPrint("Address of MSR entry %x is hooked: %x.\r\n", reg, msr->value_low);
}

/*
 * Function that can be called directly from assembly inside HookRoutine.
 */
void DebugPrint(UINT32 d) {
	if(numActions == 0) {
		DbgPrint("[*] Inside Hook Routine - dispatch %d called.\r\n", d);
		numActions = 1000;
	}
	else {
		numActions--;
	}
	return;
}

/*
 * Hook function. 
 */
__declspec(naked) HookRoutine() {
	__asm {
		pushad;
		pushfd;
		
		mov ecx, 0x23
		push 0x30
		pop fs
		mov ds, cx
		mov es, cx
		
		push eax;
		call DebugPrint;

		popfd;
		popad;
		
		jmp oldMSRAddressL;
	}
}

/*
 * Hook the MSR register by overwriting its value.
 */
void HookMSR(UINT32 reg, UINT32 hookaddr) {
	MSR msraddr;
	
	/* check if the MSR was already hooked */
	msraddr = GetMSRAddress(reg);
	if(msraddr.value_low == hookaddr) {
		DbgPrint("The MSR register %x already hooked.\r\n", reg);
	}
	else {
		DbgPrint("Hooking MSR register %x: %x --> %x.\r\n", reg, msraddr.value_low, hookaddr);
		msraddr.value_low = hookaddr;
		SetMSRAddress(reg, &msraddr);
	}
}

/*
 * Typedef accepting thread and affinity.
 */
typedef NTSTATUS (__stdcall * KeSetAffinityThread)(
	PKTHREAD thread,
	KAFFINITY affinity
);

/*
 * Hooks all processors in a system by using KeSetAffinityThread.
 */
void AllCPUs(PTHREADARG thread_arg) {
	KeSetAffinityThread KeSetAffinityThreadObj;
	UNICODE_STRING str;
	KAFFINITY processors;
	PKTHREAD thread;
	ULONG i = 0;
	KAFFINITY curCPU;
  
	processors = KeQueryActiveProcessors();
	thread     = KeGetCurrentThread();
	RtlInitUnicodeString(&str, L"KeSetAffinityThread");
	KeSetAffinityThreadObj = (KeSetAffinityThread)MmGetSystemRoutineAddress(&str);

	for(i = 0; i < 32; i++) {
		curCPU = processors & (1 << i);
		if(curCPU != 0) {
			DbgPrint("Logical processor 0x%x hooked\n", curCPU);
			//__asm { int 3 }
			KeSetAffinityThreadObj(thread, curCPU);
			DbgPrint("Thread Argument: reg : %x.\r\n", thread_arg->reg);
			DbgPrint("Thread Argument: hook address : %x.\r\n", thread_arg->hookaddr);
			HookMSR(thread_arg->reg, thread_arg->hookaddr);
		}
	}

	KeSetAffinityThreadObj(thread, processors);
	PsTerminateSystemThread(STATUS_SUCCESS);
}

/*
 * Takes care of hooking arbitrary MSR 'reg' with 'hookaddr'.
 */
void HookAll(UINT32 reg, UINT32 hookaddr) {
	/* local variables */
	HANDLE thread;
	OBJECT_ATTRIBUTES attrs;
	PKTHREAD pkthread;
	LARGE_INTEGER timeout;
	THREADARG thread_arg;
	NTSTATUS access_status;
   
	/* initializes the object attributes */
	InitializeObjectAttributes(&attrs, NULL, 0, NULL, NULL);

	/* create a new thread by passing it thread_arg as parameter */
	thread_arg.reg = reg;
	thread_arg.hookaddr = hookaddr;
	PsCreateSystemThread(&thread, THREAD_ALL_ACCESS, &attrs, NULL, NULL, (PKSTART_ROUTINE)AllCPUs, (PVOID)&thread_arg);
   
	/* check access rights of the thread */
	access_status = ObReferenceObjectByHandle(thread, THREAD_ALL_ACCESS, NULL, KernelMode, &pkthread, NULL);
	if(access_status == STATUS_SUCCESS) {
		timeout.QuadPart = 500;
		while(KeWaitForSingleObject(pkthread, Executive, KernelMode, FALSE, &timeout) != STATUS_SUCCESS) { }
	}
	else if(access_status == STATUS_OBJECT_TYPE_MISMATCH) {
		DbgPrint("[ObReferenceObjectByHandle] Wrong object type for the specified object.\r\n");
	}
	else if(access_status == STATUS_ACCESS_DENIED) {
		DbgPrint("[ObReferenceObjectByHandle] Access to the object cannot be granted.\r\n");
	}
	else if(access_status == STATUS_INVALID_HANDLE) {
		DbgPrint("[ObReferenceObjectByHandle] The handle is not valid.\r\n");
	}
	else {
		DbgPrint("[ObReferenceObjectByHandle] Unknown problem occurred.\r\n");
	}
	
	/* close the thread handle */
	ZwClose(thread);
}




/*
 * DriverEntry: entry point for drivers.
 */
NTSTATUS DriverEntry(PDRIVER_OBJECT  pDriverObject, PUNICODE_STRING  pRegistryPath) {
    NTSTATUS NtStatus = STATUS_SUCCESS;
    unsigned int uiIndex = 0;
    PDEVICE_OBJECT pDeviceObject = NULL;
    UNICODE_STRING usDriverName, usDosDeviceName;

    DbgPrint("DriverEntry Called \r\n");

    RtlInitUnicodeString(&usDriverName, L"\\Device\\MyDriver");
    RtlInitUnicodeString(&usDosDeviceName, L"\\DosDevices\\MyDriver"); 

    NtStatus = IoCreateDevice(pDriverObject, 0, &usDriverName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);

    if(NtStatus == STATUS_SUCCESS) {
        /* MajorFunction: is a list of function pointers for entry points into the driver. */
        for(uiIndex = 0; uiIndex < IRP_MJ_MAXIMUM_FUNCTION; uiIndex++)
             pDriverObject->MajorFunction[uiIndex] = MyDriver_UnSupportedFunction;

			 
		/* DriverUnload is required to be able to dynamically unload the driver. */
        pDriverObject->DriverUnload =  MyDriver_Unload; 
        pDeviceObject->Flags |= 0;
		pDeviceObject->Flags &= (~DO_DEVICE_INITIALIZING);
		
        /* Create a Symbolic Link to the device. MyDriver -> \Device\MyDriver */
        IoCreateSymbolicLink(&usDosDeviceName, &usDriverName);
		
		/* hook IDT */
		//HookMSR(IA32_SYSENTER_EIP, (UINT32)HookRoutine);
		HookAll(IA32_SYSENTER_EIP, (UINT32)HookRoutine);
    }

    return NtStatus;
}


 /*
  * MyDriver_Unload: called when the driver is unloaded.
  */
VOID MyDriver_Unload(PDRIVER_OBJECT  DriverObject) {
	/* local variables */
	UNICODE_STRING usDosDeviceName;

	/* restore the hook */
	if(oldMSRAddressL != NULL || oldMSRAddressH != NULL) {
		HookMSR(IA32_SYSENTER_EIP, (UINT32)oldMSRAddressL);
	}

	/* delete the driver */
    DbgPrint("MyDriver_Unload Called \r\n");
    RtlInitUnicodeString(&usDosDeviceName, L"\\DosDevices\\MyDriver");
    IoDeleteSymbolicLink(&usDosDeviceName);
    IoDeleteDevice(DriverObject->DeviceObject);
}


/*
 * MyDriver_UnSupportedFunction: called when a major function is issued that isn't supported.
 */
NTSTATUS MyDriver_UnSupportedFunction(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    NTSTATUS NtStatus = STATUS_NOT_SUPPORTED;
    DbgPrint("MyDriver_UnSupportedFunction Called \r\n");

    return NtStatus;
}


