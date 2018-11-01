//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: /Alaska/BIN/Chipset/Intel/SouthBridge/LynxPoint/Intel Pch SB Chipset/SleepSmi/SleepSmi.c 4     9/26/12 3:59a Victortu $
//
// $Revision: 4 $
//
// $Date: 9/26/12 3:59a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/BIN/Chipset/Intel/SouthBridge/LynxPoint/Intel Pch SB Chipset/SleepSmi/SleepSmi.c $
// 
// 4     9/26/12 3:59a Victortu
// [TAG]           None
// [Category]      Improvement
// [Description]   Update for PCH LP GPIO compatible.
// [Files]         SB.sdl, SB.H, AcpiModeEnable.c, AcpiModeEnable.sdl,
// SBDxe.c, SBGeneric.c, SBPEI.c, SBSMI.c, SleepSmi.c,
// SmiHandlerPorting.c, SmiHandlerPorting2.c
// 
// 3     7/27/12 6:17a Victortu
// [TAG]           None
// [Category]      Improvement
// [Description]   Update to support ULT Platform.
// [Files]         SB.H, SB.mak, SB.sdl, SB.sd, SBSetup.c,
// AcpiModeEnable.c, SBDxe.c, SBPEI.c, SBSMI.c, SleepSmi.c,
// SmiHandlerPorting.c, SmiHandlerPorting2.c, SBPPI.h, Pch.sdl
// 
// 2     6/13/12 11:29p Victortu
// [TAG]           None
// [Category]      Improvement
// [Description]   Program AfterG3 bit depend the setup question in
// S3/S4/S5.
// [Files]         SleepSmi.c
// 
// 1     2/08/12 8:30a Yurenlai
// Intel Lynx Point/SB eChipset initially releases.
// 
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:        SleepSmi.C
//
// Description: Provide functions to register and handle Sleep SMI
//              functionality.  
//
//<AMI_FHDR_END>
//*************************************************************************

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------

#include <Token.h>
#include <Setup.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)&&(CORE_COMBINED_VERSION >= 0x4028B)
#include <Protocol\SmmBase2.h>
#include <Protocol\SmmSxDispatch2.h>
#else
#include <Protocol\SmmBase.h>
#include <Protocol\SmmSxDispatch.h>
#endif
#include <PchAccess.h>

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)&&(CORE_COMBINED_VERSION >= 0x4028B)
#define AMI_SMM_SX_DISPATCH_PROTOCOL EFI_SMM_SX_DISPATCH2_PROTOCOL
#define AMI_SMM_SX_DISPATCH_CONTEXT  EFI_SMM_SX_REGISTER_CONTEXT
#define SMM_CHILD_DISPATCH_SUCCESS   EFI_SUCCESS
#else
#define AMI_SMM_SX_DISPATCH_PROTOCOL EFI_SMM_SX_DISPATCH_PROTOCOL
#define AMI_SMM_SX_DISPATCH_CONTEXT  EFI_SMM_SX_DISPATCH_CONTEXT
#define SMM_CHILD_DISPATCH_SUCCESS
#endif

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

BOOLEAN gIsLastState    = FALSE;
BOOLEAN gPchWakeOnLan   = FALSE;
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)&&(CORE_COMBINED_VERSION >= 0x4028B)
EFI_SMM_BASE2_PROTOCOL  *gSmmBase2;
#endif

// GUID Definition(s)

EFI_GUID gThisFileGuid = \
   {0x6298fe18, 0xd5ef, 0x42b7, 0xbb, 0xc, 0x29, 0x53, 0x28, 0x3f, 0x57, 0x4};
    // {6298FE18-D5EF-42b7-BB0C-2953283F5704}

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)
// DW01_Custom_Support_RTC_Wake >>
typedef struct {
  UINT8    AaeonRTCWakeupTimeHour ;
  UINT8    AaeonRTCWakeupTimeMinute ;
  UINT8    AaeonRTCWakeupTimeSecond ;
  UINT8    AaeonRTCWakeupDateDay;
} SMMData ;

SMMData gSmmData ;

EFI_GUID	gSetupGuid = SETUP_GUID;
SETUP_DATA	SetupData;
//---------------------------------------------------------------------------
UINT8 DecToBCD(UINT8 Dec)
{
    UINT8 FirstDigit = Dec % 10;
    UINT8 SecondDigit = Dec / 10;

    return (SecondDigit << 4) + FirstDigit;
}
UINT8 BCDToDec(UINT8 BCD)
{
    UINT8 FirstDigit = BCD & 0xf;
    UINT8 SecondDigit = BCD >> 4;;
    
    return SecondDigit * 10  + FirstDigit;
}
UINT8 ReadRtcIndex(IN UINT8 Index)
{
    UINT8 volatile Value;

    // Check if Data Time is valid
    if(Index <= 9) do {
    IoWrite8(0x70, 0x0A | 0x80);
    Value = IoRead8(0x71);        
    } while (Value & 0x80); 

    IoWrite8(0x70, Index | 0x80);
    // Read register
    Value = IoRead8(0x71);               
    if (Index <= 9) Value = BCDToDec(Value);    
    return (UINT8)Value;
}
void WriteRtcIndex(IN UINT8 Index, IN UINT8 Value)
{
    IoWrite8(0x70,Index | 0x80);
      if (Index <= 9 ) Value = DecToBCD(Value);
    // Write Register
    IoWrite8(0x71,Value);               
}
void SetWakeupTime (
    IN EFI_TIME     *Time
)
{
    UINT8 Value;

    WriteRtcIndex(5,Time->Hour);
    WriteRtcIndex(3,Time->Minute);
    WriteRtcIndex(1,Time->Second);
    Value = ReadRtcIndex(0x0D) & 0xC0;
    WriteRtcIndex(0x0D,(Value|DecToBCD(Time->Day)));

    //Set Enable
    Value = ReadRtcIndex(0xB);
    Value |= 1 << 5;
    WriteRtcIndex(0xB,Value);
}
VOID RTCWakeFunc(){
	UINT32 i = 0;
	EFI_TIME Time;
    
	Time.Hour = gSmmData.AaeonRTCWakeupTimeHour;
	Time.Minute = gSmmData.AaeonRTCWakeupTimeMinute;
	Time.Second = gSmmData.AaeonRTCWakeupTimeSecond;
	Time.Day = gSmmData.AaeonRTCWakeupDateDay;

	SetWakeupTime(&Time);            
	//Clear RTC PM1 status
	IoWrite16(PM_BASE_ADDRESS , ( IoRead16(PM_BASE_ADDRESS) | (1 << 10) ));
	//set RTC_EN bit to wake up from the alarm
	IoWrite32(PM_BASE_ADDRESS, ( IoRead32(PM_BASE_ADDRESS) | (1 << 26) ));
}
// DW01_Custom_Support_RTC_Wake <<

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   ProgramAfterG3Bit
//
// Description: This function will set AfterG3 bit depend the setup question.
//
// Input:       None
//
// Output:      None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID ProgramAfterG3Bit(VOID)
{
  if (gIsLastState) SET_PCI8_SB(SB_REG_GEN_PMCON_3, 1); // 0xA4
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   S1SleepSmiOccurred
//
// Description: This function will be called by EfiSmmSxDispatch when a Sleep
//              SMI occurs and the sleep state is S1.
//
// Input:       DispatchHandle   - SMI dispatcher handle
//              *DispatchContext - Pointer to the dispatch context
//
// Output:      Nothing
//
// Notes:       This function does not need to put the system to sleep.  This is
//              handled by PutToSleep.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)&&(CORE_COMBINED_VERSION >= 0x4028B)
EFI_STATUS S1SleepSmiOccurred (
	IN EFI_HANDLE       DispatchHandle,
	IN CONST VOID       *DispatchContext OPTIONAL,
	IN OUT VOID         *CommBuffer OPTIONAL,
	IN OUT UINTN        *CommBufferSize OPTIONAL )
#else
VOID S1SleepSmiOccurred (
    IN EFI_HANDLE                   DispatchHandle,
    IN EFI_SMM_SX_DISPATCH_CONTEXT  *DispatchContext )
#endif
{
    // Porting required if any workaround is needed.
    EnablePs2KeyboardMousePme();    
    return SMM_CHILD_DISPATCH_SUCCESS;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   S3SleepSmiOccurred
//
// Description: This function will be called by EfiSmmSxDispatch when a Sleep
//              SMI occurs and the sleep state is S3.
//
// Input:       DispatchHandle   - SMI dispatcher handle
//              *DispatchContext - Pointer to the dispatch context
//
// Output:      Nothing
//
// Notes:       This function does not need to put the system to sleep.  This is
//              handled by PutToSleep.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)&&(CORE_COMBINED_VERSION >= 0x4028B)
EFI_STATUS S3SleepSmiOccurred (
	IN EFI_HANDLE       DispatchHandle,
	IN CONST VOID       *DispatchContext OPTIONAL,
	IN OUT VOID         *CommBuffer OPTIONAL,
	IN OUT UINTN        *CommBufferSize OPTIONAL )
#else
VOID S3SleepSmiOccurred (
    IN EFI_HANDLE                   DispatchHandle,
    IN EFI_SMM_SX_DISPATCH_CONTEXT  *DispatchContext )
#endif
{
    BOOLEAN IsGpioLocked;
    UINT16  LpcDeviceId;

    LpcDeviceId = READ_PCI16_SB(R_PCH_LPC_DEVICE_ID);

    IsGpioLocked = (READ_PCI8_SB(SB_REG_GC) & BIT00)? TRUE:FALSE;

    if (IS_PCH_LPT_LPC_DEVICE_ID_MOBILE (LpcDeviceId)) {

      // Reset GPIO Lockdown Enable (GLE)
      if (IsGpioLocked)
      RESET_PCI8_SB(SB_REG_GC, BIT00);

      // Set GPIO 60 to low (S3 power)
    if (GetPchSeries() == PchLp) {
      RESET_IO32( GPIO_BASE_ADDRESS + (GP_IOREG_GP_GPN_CFG1 + GP_GPIO_CONFIG_SIZE*60), ~BIT31); // 0x38
    } else {
      RESET_IO32( GPIO_BASE_ADDRESS + GP_IOREG_GP_LVL2, ~0xEFFFFFFF); // 0x38
    }

      // Set GPIO Lockdown Enable (GLE)
      if (IsGpioLocked)
        SET_PCI8_SB(SB_REG_GC, BIT00);
    }

    ProgramAfterG3Bit();
    EnablePs2KeyboardMousePme();

    return SMM_CHILD_DISPATCH_SUCCESS;
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   S4SleepSmiOccurred
//
// Description: This function will be called by EfiSmmSxDispatch when a Sleep
//              SMI occurs and the sleep state is S4.
//
// Input:       DispatchHandle   - SMI dispatcher handle
//              *DispatchContext - Pointer to the dispatch context
//
// Output:      Nothing
//
// Notes:       This function does not need to put the system to sleep.  This is
//              handled by PutToSleep.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)&&(CORE_COMBINED_VERSION >= 0x4028B)
EFI_STATUS S4SleepSmiOccurred (
	IN EFI_HANDLE       DispatchHandle,
	IN CONST VOID       *DispatchContext OPTIONAL,
	IN OUT VOID         *CommBuffer OPTIONAL,
	IN OUT UINTN        *CommBufferSize OPTIONAL )
#else
VOID S4SleepSmiOccurred (
    IN EFI_HANDLE                   DispatchHandle,
    IN EFI_SMM_SX_DISPATCH_CONTEXT  *DispatchContext )
#endif
{
    ClearMeWakeSts();

    ProgramAfterG3Bit();
    EnablePs2KeyboardMousePme();

    return SMM_CHILD_DISPATCH_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   S5SleepSmiOccurred
//
// Description: This function will be called by EfiSmmSxDispatch when a Sleep
//              SMI occurs and the sleep state is S1.
//
// Input:       DispatchHandle   - SMI dispatcher handle
//              *DispatchContext - Pointer to the dispatch context
//
// Output:      Nothing
//
// Notes:       This function does not need to put the system to sleep.  This is
//              handled by PutToSleep.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)&&(CORE_COMBINED_VERSION >= 0x4028B)
EFI_STATUS S5SleepSmiOccurred (
	IN EFI_HANDLE       DispatchHandle,
	IN CONST VOID       *DispatchContext OPTIONAL,
	IN OUT VOID         *CommBuffer OPTIONAL,
	IN OUT UINTN        *CommBufferSize OPTIONAL )
#else
VOID S5SleepSmiOccurred (
    IN EFI_HANDLE                   DispatchHandle,
    IN EFI_SMM_SX_DISPATCH_CONTEXT  *DispatchContext )
#endif
{

    ClearMeWakeSts();
    SBLib_BeforeShutdown();

    if (gPchWakeOnLan) Enable_GbE_PME();

// DW01_Custom_Support_RTC_Wake >>
    {
	    EFI_STATUS Status = EFI_SUCCESS;
	    UINT8	Value;

    	//Disable RTC alarm and clear RTC PM1 status
    	Value = ReadRtcIndex(0xB);
    	Value &= ~((UINT8)1 << 5);
    	WriteRtcIndex(0xB,Value);
    	//Clear Alarm Flag (AF) by reading RTC Reg C
    	Value = ReadRtcIndex(0xC);
    	IoWrite16(PM_BASE_ADDRESS , ( IoRead16(PM_BASE_ADDRESS) | (1 << 10) ));

    	if(!EFI_ERROR(Status)){
    		//if(gSetupData.FixedWakeOnRTCS5 == 1){
    		if(SetupData.AaeonWakeOnRtc == 1){
    			gSmmData.AaeonRTCWakeupTimeHour = SetupData.AaeonRTCWakeupTimeHour ;
    			gSmmData.AaeonRTCWakeupTimeMinute = SetupData.AaeonRTCWakeupTimeMinute ;
    			gSmmData.AaeonRTCWakeupTimeSecond = SetupData.AaeonRTCWakeupTimeSecond ;
    			gSmmData.AaeonRTCWakeupDateDay = SetupData.AaeonRTCWakeupTimeDay;
    		}

    		//if(gSetupData.DynamicWakeOnRTCS5 == 1){
    		if(SetupData.AaeonWakeOnRtc == 2){
    			gSmmData.AaeonRTCWakeupTimeHour = ReadRtcIndex(4);
    			gSmmData.AaeonRTCWakeupTimeMinute = ReadRtcIndex(2);
    			gSmmData.AaeonRTCWakeupTimeSecond = ReadRtcIndex(0);
    			gSmmData.AaeonRTCWakeupTimeMinute += SetupData.AaeonRTCWakeupTimeMinuteIncrease;
    			if  (gSmmData.AaeonRTCWakeupTimeMinute >= 60)
    			{
    				gSmmData.AaeonRTCWakeupTimeMinute = 0;
    				++gSmmData.AaeonRTCWakeupTimeHour;
    				if (gSmmData.AaeonRTCWakeupTimeHour == 24)
    					gSmmData.AaeonRTCWakeupTimeHour = 0;
    			}
    		}
    	}

    	if(SetupData.AaeonWakeOnRtc == 1 || SetupData.AaeonWakeOnRtc == 2)
    		RTCWakeFunc();
    }
// DW01_Custom_Support_RTC_Wake <<

    // Program AfterG3 bit depend the setup question.
    ProgramAfterG3Bit();

    return SMM_CHILD_DISPATCH_SUCCESS;
}

//----------------------------------------------------------------------------
//
// Procedure:   InSmmFunction
//
// Description: Install Sleep SMI Handlers for south bridge.
//
// Input:       ImageHandle  - Image handle
//              *SystemTable - Pointer to the system table
//
// Output:      EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS InSmmFunction (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    EFI_STATUS                      Status;
    EFI_HANDLE                      hS1Smi;
    EFI_HANDLE                      hS3Smi;
    EFI_HANDLE                      hS4Smi;
    EFI_HANDLE                      hS5Smi;
    AMI_SMM_SX_DISPATCH_PROTOCOL    *SxDispatch;
    AMI_SMM_SX_DISPATCH_CONTEXT     S1DispatchContext = {SxS1, SxEntry};
    AMI_SMM_SX_DISPATCH_CONTEXT     S3DispatchContext = {SxS3, SxEntry};
    AMI_SMM_SX_DISPATCH_CONTEXT     S4DispatchContext = {SxS4, SxEntry};
    AMI_SMM_SX_DISPATCH_CONTEXT     S5DispatchContext = {SxS5, SxEntry};
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)&&(CORE_COMBINED_VERSION >= 0x4028B)
    EFI_SMM_SYSTEM_TABLE2           *pSmst2;
#endif

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)&&(CORE_COMBINED_VERSION >= 0x4028B)
    Status = InitAmiSmmLib( ImageHandle, SystemTable );
    if (EFI_ERROR(Status)) return Status;

    // We are in SMM, retrieve the pointer to SMM System Table
    Status = gSmmBase2->GetSmstLocation( gSmmBase2, &pSmst2);
    if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;

    Status = pSmst2->SmmLocateProtocol( &gEfiSmmSxDispatch2ProtocolGuid , \
                                       NULL, \
                                       &SxDispatch );
    TRACE((TRACE_ALWAYS, "Smm Locate Protocol gEfiSmmSxDispatch2ProtocolGuid, Status = %r\n",Status));

#else
    Status = pBS->LocateProtocol( &gEfiSmmSxDispatchProtocolGuid , \
                                  NULL, \
                                  &SxDispatch );
#endif
    if (EFI_ERROR(Status)) return Status;

    // Register Sleep SMI Handlers
    Status = SxDispatch->Register( SxDispatch, \
                                   S1SleepSmiOccurred, \
                                   &S1DispatchContext, \
                                   &hS1Smi );
    if (EFI_ERROR(Status)) return Status;

    Status = SxDispatch->Register( SxDispatch, \
                                   S3SleepSmiOccurred, \
                                   &S3DispatchContext, \
                                   &hS3Smi );
    if (EFI_ERROR(Status)) return Status;

    Status = SxDispatch->Register( SxDispatch, \
                                   S4SleepSmiOccurred, \
                                   &S4DispatchContext, \
                                   &hS4Smi );
    if (EFI_ERROR(Status)) return Status;

    Status = SxDispatch->Register( SxDispatch, \
                                   S5SleepSmiOccurred, \
                                   &S5DispatchContext, \
                                   &hS5Smi );
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   InitSleepSmi
//
// Description: This function Registers Sleep SMI functionality.
//
// Input:       ImageHandle - Handle for this FFS image
//              *SystemTable- Pointer to the system table
//
// Output:      EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS InitSleepSmi (
    IN EFI_HANDLE                   ImageHandle,
    IN EFI_SYSTEM_TABLE             *SystemTable )
{
    EFI_STATUS                      Status;
    SB_SETUP_DATA                   *SbSetupData = NULL;
    UINTN                           VariableSize = sizeof(SB_SETUP_DATA);

    InitAmiLib( ImageHandle, SystemTable );

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)&&(CORE_COMBINED_VERSION >= 0x4028B)
    Status = SystemTable->BootServices->LocateProtocol( \
                                                &gEfiSmmBase2ProtocolGuid, \
                                                NULL, \
                                                &gSmmBase2 );
    ASSERT_EFI_ERROR(Status);
#endif

    // Porting Required
    // Put the Setup Vairable to SMM if needed.
    Status = pBS->AllocatePool( EfiBootServicesData, \
                                VariableSize, \
                                &SbSetupData );
    ASSERT_EFI_ERROR(Status);

    GetSbSetupData( pRS, SbSetupData, FALSE );

    gIsLastState = (SbSetupData->LastState == 2) ? TRUE : FALSE;
    gPchWakeOnLan = (SbSetupData->PchWakeOnLan == 1) ? TRUE : FALSE;
    Status = pBS->FreePool( SbSetupData );

// DW01_Custom_Support_RTC_Wake >>
    VariableSize = sizeof(SETUP_DATA);
    Status = pRS->GetVariable( L"Setup", \
                                       &gSetupGuid, \
                                       NULL, \
                                       &VariableSize, \
                                       &SetupData );

// DW01_Custom_Support_RTC_Wake <<
    // Porting End

    return InitSmmHandler( ImageHandle, SystemTable, InSmmFunction, NULL );
}


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
