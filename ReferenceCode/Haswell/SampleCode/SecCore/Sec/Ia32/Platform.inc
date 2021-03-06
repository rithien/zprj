;@file
;  Platform Specific Definitions
;
;@copyright
;  Copyright (c)  2011 - 2012 Intel Corporation. All rights reserved
;  This software and associated documentation (if any) is furnished
;  under a license and may only be used or copied in accordance
;  with the terms of the license. Except as permitted by such
;  license, no part of this software or documentation may be
;  reproduced, stored in a retrieval system, or transmitted in any
;  form or by any means without the express written consent of
;  Intel Corporation.
;
;  This file contains an 'Intel Peripheral Driver' and uniquely
;  identified as "Intel Reference Module" and is
;  licensed for Intel CPUs and chipsets under the terms of your
;  license agreement with Intel or your vendor.  This file may
;  be modified by the user, subject to additional terms of the
;  license agreement
;

;(AMI_CHG-)INCLUDE FlashMap.inc

; Set "MINIMUM_BOOT_SUPPORT" flag allows BIOS boot as minimum feature in SEC phase.
MINIMUM_BOOT_SUPPORT          EQU 0               ; ="0", Normal Boot;
                                                  ; ="1", Minimum Feature Boot
; "RESET_IN_SEC" flag allows BIOS doing RESET in SEC phase
RESET_IN_SEC                  EQU 0               ; ="0", RESET occurs in OemIohInit.c
                                                  ; ="1", RESET occurs in SEC phase

EARLY_MICROCODE_SUPPORT       EQU 1
DETERMINISTIC_BSP_SUPPORT     EQU 0
DEBUG                         EQU 1

;
; IO port to access the upper 128-byte of RTC RAM
;
RTC_UPPER_INDEX               EQU 072h
RTC_UPPER_DATA                EQU 073h

;
; Offset of data stored in the upper 128-byte of RTC RAM.
;
CMOS_CPU_BSP_SELECT           EQU 010h            ; BspSelection
CMOS_CPU_UP_MODE              EQU 011h            ; UpBootSelection

;
; Cpu Ratio and Vid stored in the upper 128-byte of RTC RAM.
; 
CMOS_CPU_RATIO_OFFSET         EQU 012h            ; ProcessorFlexibleRatio
CMOS_CPU_CORE_HT_OFFSET       EQU 013h            ; ProcessorHyperThreadingEnable & EnableCoresInSbsp & EnableCoresInNbsp

;
; CPU Feature
;
CMOS_CPU_BIST_OFFSET          EQU 015h            ; ProcessorBistEnable
CMOS_CPU_VMX_OFFSET           EQU 016h            ; ProcessorVmxEnable

;
; Port80 Selection
;
CMOS_PCH_PORT80_OFFSET        EQU 017h            ; PchPort80Route

;
;Flash layout map
;
PEICODE_REGION_BASE_ADDRESS   EQU FLASH_BASE
PEICODE_REGION_SIZE           EQU FLASH_SIZE
PEICODE_REGION_SIZE_MASK      EQU (NOT (PEICODE_REGION_SIZE - 1))

BIOS_REGION_UPDATABLE_STATUS  EQU 0058h            ; Offset
;----------------------------------------------------------------------------------------
; "Merlin" support used equates
;----------------------------------------------------------------------------------------
MAGIC_ADDRESS_IN_SEG          EQU  0FFF0h
MAGIC_SEG                     EQU  0F000h

;
; -- Equates for CAR initialization
;    TileSize (must be a power of 2)
;
; Define the tile size
; The tile size and tile placement are critical to ensuring that no data loss occurs
; See BWG - chapter "Determining Tile Size"
;
TILE_SIZE                     EQU 000000000h

;
; See BWG - chapter "Determining Cacheable Code Region Base Addresses and Ranges".
;
; Now FvRecovery is 6 blocks, so it is seperated into 2 parts to set MTRR:
; 1. base address = FFFA0000, length = 0x20000
; 2. base address = FFFC0000, length = 0x40000
; 
; *** NOTE: If FvRecovery size changes, this code needs to be changed accordingly.
;           Possible enhancement is to dynamically accomodate size changes.
;

;(AMI_CHG)>
;-CODE_REGION_BASE_ADDRESS_PART1      EQU FLASH_REGION_FV_RECOVERY_BASE
;-CODE_REGION_SIZE_PART1              EQU (TILE_SIZE + (128*1024))

MIN_CODE_REGION_SIZE EQU 40000h
MIN_CODE_REGION_SIZE_MASK EQU (NOT (MIN_CODE_REGION_SIZE - 1))                      
CODE_REGION_BASE_ADDRESS_PART1      EQU MKF_CODE_CACHE_BASE_ADDRESS AND 0ffff0000h
IF MKF_CODE_CACHE_SIZE lt 100000h
    CODE_REGION_SIZE_PART1              EQU 100000h   
ELSE
    CODE_REGION_SIZE_PART1              EQU MKF_CODE_CACHE_SIZE   
ENDIF
    
CODE_REGION_SIZE_MASK_PART1         EQU (NOT (CODE_REGION_SIZE_PART1 - 1))   

IF MKF_CODE_CACHE_PART2_BASE
;-CODE_REGION_BASE_ADDRESS_PART2      EQU CODE_REGION_BASE_ADDRESS_PART1 + CODE_REGION_SIZE_PART1
;-CODE_REGION_SIZE_PART2              EQU (TILE_SIZE + (256*1024))                   
CODE_REGION_BASE_ADDRESS_PART2      EQU MKF_CODE_CACHE_PART2_BASE
CODE_REGION_SIZE_PART2              EQU MKF_CODE_CACHE_PART2_SIZE                  
CODE_REGION_SIZE_MASK_PART2         EQU (NOT (CODE_REGION_SIZE_PART2 - 1))    
ENDIF

IF MKF_WDB_REGION_BASE_ADDRESS
;-WDB_REGION_BASE_ADDRESS             EQU 040000000h
;-WDB_REGION_SIZE                     EQU 01000h
WDB_REGION_BASE_ADDRESS             EQU MKF_WDB_REGION_BASE_ADDRESS
WDB_REGION_SIZE                     EQU MKF_WDB_REGION_BASE_SIZE
WDB_REGION_SIZE_MASK                EQU (NOT (WDB_REGION_SIZE - 1))
ENDIF
;<(AMI_CHG)

;
; See BWG - chapter "Determining Data Stack Base Address and Range"
;
;(AMI_CHG)>
;-;DATA_STACK_BASE_ADDRESS       EQU (CODE_REGION_BASE_ADDRESS - TILE_SIZE - (16*1024 * 1024))
;-DATA_STACK_BASE_ADDRESS       EQU 0FFB00000h
;-DATA_STACK_SIZE               EQU (64*1024)       ; 10000h
DATA_STACK_BASE_ADDRESS       EQU MKF_CAR_BASE_ADDRESS
DATA_STACK_SIZE               EQU MKF_CAR_TOTAL_SIZE
DATA_STACK_SIZE_MASK          EQU (NOT (DATA_STACK_SIZE - 1))
TEMPORARY_RAM_BASE_ADDRESS    EQU DATA_STACK_BASE_ADDRESS
TEMPORARY_RAM_SIZE            EQU DATA_STACK_SIZE   
;<(AMI_CHG)

;
; Cache init and test values
; These are inverted to flip each bit at least once
;
CACHE_INIT_VALUE              EQU 0A5A5A5A5h
CACHE_TEST_VALUE              EQU (NOT CACHE_INIT_VALUE)

PEI_CORE_ENTRY_BASE           EQU 0FFFFFFE0h
FV_MAIN_BASE                  EQU 0FFFFFFFCh

MAX_NR_BUS                    EQU 0FFh
MAX_NR_CPU_SOCKETS            EQU 2               ; DP example, MP may have 4 or more

;
; Support EDK1117 build - Sample BASE Address and Size insteads of PcdGet() 
;
MICROCODE_FV_BASE_ADDRESS     EQU 0FFF20000h      ; PcdGet32 (PcdFlashMicrocodeFvBase)
MICROCODE_FV_SIZE             EQU 40000h          ; PcdGet32 (PcdFlashMicrocodeFvSize)
CODE_CACHE_BASE_ADDRESS       EQU 0FFF80000h      ; PcdGet32 (PcdNemCodeCacheBase)
CODE_CACHE_SIZE               EQU 80000h          ; PcdGet32 (PcdNemCodeCacheSize)
FLASH_AREA_BASE_ADDRESS       EQU 0FF800000h      ; PcdGet32 (PcdFlashAreaBaseAddress)
;(AMI_CHG)>
;TEMPORARY_RAM_BASE_ADDRESS    EQU 0FEF00000h      ; PcdGet32 (PcdTemporaryRamBase)
;TEMPORARY_RAM_SIZE            EQU 2000h           ; PcdGet32 (PcdTemporaryRamSize)
;<(AMI_CHG)
PCIEXPRESS_BASE_ADDRESS       EQU 0E0000000h      ; PcdGet64 (PcdPciExpressBaseAddress)

BIT0                          EQU 01h
BIT1                          EQU 02h
BIT2                          EQU 04h
BIT3                          EQU 08h
BIT4                          EQU 10h
BIT5                          EQU 20h
BIT6                          EQU 40h
BIT7                          EQU 80h
BIT8                          EQU 100h
BIT9                          EQU 200h
BIT10                         EQU 400h
BIT11                         EQU 800h
BIT12                         EQU 1000h
BIT13                         EQU 2000h
BIT14                         EQU 4000h
BIT15                         EQU 8000h
BIT16                         EQU 10000h
BIT17                         EQU 20000h
BIT18                         EQU 40000h
BIT19                         EQU 80000h
BIT23                         EQU 0800000h
BIT31			                    EQU 080000000h
; Bit definition in MM1
BadCMOSDetected               EQU (BIT0 shl 17)
BSPApicIDSaveStart            EQU 24
