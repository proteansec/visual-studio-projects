#include <wdm.h>

/* Function Prototypes */
NTSTATUS MyDriver_UnSupportedFunction(PDEVICE_OBJECT DeviceObject, PIRP Irp);
VOID MyDriver_Unload(PDRIVER_OBJECT  DriverObject);    
NTSTATUS DriverEntry(PDRIVER_OBJECT  pDriverObject, PUNICODE_STRING  pRegistryPath); 

/* Compile directives. */
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, MyDriver_Unload)
#pragma alloc_text(PAGE, MyDriver_UnSupportedFunction)


#pragma pack(1)
typedef struct _DESC {
  UINT16 offset00;
  UINT16 segsel;
  CHAR unused:5;
  CHAR zeros:3;
  CHAR type:5;
  CHAR DPL:2;
  CHAR P:1;
  UINT16 offset16;
} DESC, *PDESC;
#pragma pack()

#pragma pack(1)
typedef struct _IDTR {
	UINT16 bytes;
	UINT32 addr;
} IDTR;
#pragma pack()



/*
 * Get the address of IDT table.
 */
IDTR GetIDTAddress() {
    IDTR idtraddr;
	
	/* get address of the IDT table */	
    __asm {
        cli;
		sidt idtraddr;
		sti;
    }
	DbgPrint("Address of IDT table is: %x.\r\n", idtraddr.addr);
	
	return idtraddr;
}


/*
 * Get the address of the service descriptor.
 */
PDESC GetDescriptorAddress(UINT16 service) {
	/* allocate local variables */
    IDTR idtraddr;
	PDESC descaddr;
	
	idtraddr = GetIDTAddress();
	
	/* get address of the interrupt entry we would like to hook */
	descaddr = idtraddr.addr + service * 0x8;
	DbgPrint("Address of IDT Entry is: %x.\r\n", descaddr);
	
	/* print some statistics */
	DbgPrint("DESC->offset00 : %x\r\n", descaddr->offset00); 
	DbgPrint("DESC->segsel   : %x\r\n", descaddr->segsel);
	DbgPrint("DESC->type     : %x\r\n", descaddr->type); 
	DbgPrint("DESC->DPL      : %x\r\n", descaddr->DPL); 
	DbgPrint("DESC->P        : %x\r\n", descaddr->P); 
	DbgPrint("DESC->offset16 : %x\r\n", descaddr->offset16); 
	
	return descaddr;
}

/*
 * Get the ISR address.
 */
UINT32 GetISRAddress(UINT16 service) {
	PDESC descaddr;
	UINT32 israddr;

	descaddr  = GetDescriptorAddress(service);
	
	/* calculate address of ISR from offset00 and offset16 */
	israddr = descaddr->offset16;
	israddr = israddr << 16;
	israddr += descaddr->offset00;
	DbgPrint("Address of the ISR is: %x.\r\n", israddr);
	
	return israddr;
}

/*
 * Hook function. 
 */
__declspec(naked) HookRoutine() {
	DbgPrint("Hook Routine called.\r\n");
}

/*
 * Hook the interrupt descriptor by overwriting its ISR pointer.
 */
void HookISR(UINT16 service, UINT32 hookaddr) {
	UINT32 israddr;
	UINT16 hookaddr_low;
	UINT16 hookaddr_high;
	PDESC descaddr;
	
	/* check if the ISR was already hooked */
	israddr = GetISRAddress(service);
	if(israddr == hookaddr) {
		DbgPrint("The service %x already hooked.\r\n", service);
	}
	else {
		DbgPrint("Hooking interrupt %x: ISR %x --> %x.\r\n", service, israddr, hookaddr);
		descaddr  = GetDescriptorAddress(service);
		DbgPrint("Hook Address: %x\r\n", hookaddr);
		hookaddr_low = (UINT16)hookaddr;
		hookaddr = hookaddr >> 16;
		hookaddr_high = (UINT16)hookaddr;
		DbgPrint("Hook Address Lower: %x\r\n", hookaddr_low);
		DbgPrint("Hook Address Higher: %x\r\n", hookaddr_high);
	}
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
		GetISRAddress(0x2e);
		HookISR(0x2e, (UINT32)HookRoutine);
    }

    return NtStatus;
}


 /*
  * MyDriver_Unload: called when the driver is unloaded.
  */
VOID MyDriver_Unload(PDRIVER_OBJECT  DriverObject) {    
    UNICODE_STRING usDosDeviceName;
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


