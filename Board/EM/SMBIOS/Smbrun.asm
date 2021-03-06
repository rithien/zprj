	TITLE	SMBRUN.ASM -- SMBIOS RUNTIME PnP FUNCTIONS 5Xh

;****************************************************************************
;****************************************************************************
;**                                                                        **
;**           (C)Copyright 1985-2009, American Megatrends, Inc.            **
;**                                                                        **
;**                          All Rights Reserved.                          **
;**                                                                        **
;**            5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093           **
;**                                                                        **
;**                          Phone: (770)-246-8600                         **
;**                                                                        **
;****************************************************************************
;****************************************************************************

;****************************************************************************
; $Header: /Alaska/BIN/Modules/SMBIOS/Smbrun.asm 6     6/02/09 3:49p Davidd $
;
; $Revision: 6 $
;
; $Date: 6/02/09 3:49p $
;
;****************************************************************************
; Revision History
; ----------------
; $Log: /Alaska/BIN/Modules/SMBIOS/Smbrun.asm $
; 
; 6     6/02/09 3:49p Davidd
; Updated AMI headers and code clean up (EIP 22180)
; 
; 5     5/19/09 10:53a Davidd
; Changes done for PnP 54 function support (EIP 19358).
; 
; 4     1/22/08 4:17p Olegi
; Modifications for 16-bit PM calls.
; 
; 2     12/31/07 12:53p Olegi
; Modifications for 16-bit protected mode interface.
; 
; 1     12/26/07 5:08p Olegi
; File moved from Board to Core component.
; 
; 3     9/06/07 4:23p Vyacheslava
; Added support for GPNV PnP functions: 55h, 56h, 57h
; The main procedures are in SMIGPNV module
; 
; 1     9/06/07 3:31p Vyacheslava
; Added support for PnP functions: 55h, 56h, 57h
; 
; 2     8/09/07 4:10p Olegi
; Support for functions 53 and 54.
; 
; 1     8/03/07 4:50p Olegi
; SMBIOS PnP 16-bit functions - initial check-in; functions 50h, 51h and
; 52h are implemented.
; 
;****************************************************************************

.586p

;<AMI_FHDR_START>
;---------------------------------------------------------------------------
;
; Name:		Smbrun.asm
;
; Description:	SMBIOS runtime PnP functions 5Xh
;
;---------------------------------------------------------------------------
;<AMI_FHDR_END>

INCLUDE	Token.equ

OEM16_CSEG SEGMENT PARA PUBLIC 'CODE' USE16
        ASSUME cs:OEM16_CSEG, ds:OEM16_CSEG

;------------------------------------------------------------------------------
; Run Time Function Return Codes (from Core8's RT.EQU)
;------------------------------------------------------------------------------
RT_NO_ERROR                     equ 00h
RT_INVALID_FUNC                 equ 81h
RT_NVR_READ_ERROR               equ 82h
RT_CMOS_READ_ERROR              equ 82h
RT_PCI_BAD_VENDOR_ID            equ 83h
RT_PCI_DEV_NOT_FOUND            equ 86h
RT_PCI_BAD_REG_ADD              equ 87h
RT_PCI_SET_FAILED               equ 88h
RT_PCI_BUF_TOO_SMALL            equ 89h

RT_PNP_UNSUPPORTED              equ 82h
RT_PNP_INVALID_NODE             equ 83h
RT_PNP_BAD_PARAMETER            equ 84h
RT_PNP_SET_FAILED               equ 85h
RT_PNP_USE_ESCD                 equ 8Dh

RT_ESCD_READ_ERROR              equ 55h
RT_ESCD_INVALID                 equ 56h

RT_DMI_SUCCESS                  equ 00h
RT_DMI_UNKNOWN_FUNCTION         equ 81h
RT_DMI_FUNCTION_NOT_SUPPORTED   equ 82h
RT_DMI_INVALID_HANDLE           equ 83h
RT_DMI_BAD_PARAMETER            equ 84h
RT_DMI_INVALID_SUBFUNCTION      equ 85h
RT_DMI_NO_CHANGE                equ 86h
  RT_DMI_NO_EVENTS_PENDING      equ 86h
RT_DMI_ADD_STRUCTURE_FAILED     equ 87h
                                ; 88h-8Ch..not defined
RT_DMI_READ_ONLY                equ 8Dh
                                ; 8Eh-8Fh..not defined
RT_DMI_LOCK_NOT_SUPPORTED       equ 90h
RT_DMI_CURENTLY_LOCKED          equ 91h
RT_DMI_INVALID_LOCK             equ 92h

SMBIOS_PNP_FUNC50_DMIEDIT_STRUC	struc
  dataIgnoredByDmiFn50  DB 24 DUP (?)
  dDmiBiosRevision      DD ?    ; BYTE
  dDmiNumStructures     DD ?    ; WORD
  dDmiStructureSize     DD ?    ; WORD
  dDmiStorageBase       DD ?    ; DWORD
  dDmiStorageSize       DD ?    ; WORD
SMBIOS_PNP_FUNC50_DMIEDIT_STRUC	ends

SMBIOS_PNP_FUNC51_DMIEDIT_STRUC	struc
  wFunction             DW ?    ; 51h
  dStructure            DD ?    ; UINT16*
  dDmiStructureBuffer   DD ?    ; UINT32*
  wDmiSelector          DW ?
  dBiosSelector         DW ?
SMBIOS_PNP_FUNC51_DMIEDIT_STRUC	ends

DMIHDR_STRUC    STRUCT
  bType         BYTE ?
  bLength       BYTE ?
  wHandle       WORD ?
DMIHDR_STRUC    ENDS

;----------------------------------------------------------------------------
;	STRUCTURE OF DATA BUFFER IN SMBIOS FUNCTION 53H
;----------------------------------------------------------------------------
SMBIOSFun53BufferSTRUC  STRUC
    bChangeStatus       BYTE ?
    bChangeType         BYTE ?
    wChangeHandle       WORD ?
    bChangeReserved     BYTE 12 DUP (?)
SMBIOSFun53BufferSTRUC  ENDS

;----------------------------------------------------------------------------
;	EQUATES USED IN SMBIOS FUNCTION 53H
;----------------------------------------------------------------------------
; equates used in SMBIOS Change Status
SMBIOS_NO_CHANGE                    EQU 00h ; 00h..SMBIOS No Change
SMBIOS_OTHER_CHANGE                 EQU 01h ; 01h..SMBIOS Other Change
SMBIOS_UNKNOWN_CHANGE               EQU 02h ; 02h..SMBIOS Unknown Change
SMBIOS_SINGLE_STRUCTURE_AFFECTED    EQU 03h ; 03h..SMBIOS Single Structure Affected
SMBIOS_MULTIPLE_STRUCTURE_AFFECTED  EQU 04h ; 04h..SMBIOS Multiple Structure Affected

; equates used in SMBIOS Change Type
SMBIOS_ONE_MORE_STRUCTURE_CHANGED   EQU 00000001b ; Bit-0 = 1, One/More Structure was changed
SMBIOS_ONE_MORE_STRUCTURE_ADDED     EQU 00000010b ; Bit-1 = 1, One/More Structure was added
                                                  ; Bit7-2.....Reserved

;<AMI_FHDR_START>
;---------------------------------------------------------------------------
;
; Name:         SmbiosPnpFunctions
;
; Description:  SMBIOS PnP functions 50..57
;
; Output:       None
;
;---------------------------------------------------------------------------
;<AMI_FHDR_END>

SmbiosPnpFunctions PROC FAR PUBLIC
        pushf
        push    fs
        push    bx
        push    dx
        push    si
        push    bp

        call    $+3                     ; Push curent IP
        pop     si		        ; Get current IP in SI
        sub     si, $
        inc     si

        mov     bx, 0FF4Ch
        mov     bx, cs:[bx]             ; OFFSET RUN_CSEG:Legacy16Data
        mov     bx, cs:[bx+8]           ; OFFSET RUN_CSEG:smiflash_table
        mov     dx, WORD PTR cs:[bx+12] ; SMI port IO address
        
        mov     bx, [ebp+00h]           ; get the function number
        sub     bx, 50h
        mov     ax, RT_INVALID_FUNC     ; Return code for unknown func
    
;-IF NOT MKF_SMBIOS_DATA_STRUCTURES_BELOW_TOM
        cmp     bx, (smbios_func_table_end - smbios_func_table_start) / 2
        jae     SHORT srfe_00           ; invalid function
        
        add     si, OFFSET cs:smbios_func_table_start
        shl     bx, 1
        add     si, bx
        call    si                      ; Call proper PnP function

srfe_00:
        pop     bp
        pop     si
        pop     dx
        pop     bx
        pop     fs
        popf

rt_pnp_exit:
        pop     bx
        popf
        pop     ebp
        ret

SmbiosPnpFunctions ENDP

;----------------------------------------------------------------------------
;   SMBIOS_FUNC_TABLE (SHORT is the key word needed to assure 2 bytes per jmp)
;----------------------------------------------------------------------------
smbios_func_table_start LABEL   BYTE
        jmp SHORT func_50
        jmp SHORT func_51
        jmp SHORT func_52
        jmp SHORT func_53
        jmp SHORT func_54
        jmp SHORT func_55
        jmp SHORT func_56
        jmp SHORT func_57
smbios_func_table_end	LABEL	BYTE

func_50:    jmp     rt_get_smbios_info
func_51:    jmp     rt_get_smbios_struc
func_52:    jmp     rt_set_smbios_struc
func_53:    jmp     rt_get_smbios_struc_change_info
func_54:    jmp     rt_smbios_control
func_55:    jmp     rt_smbios_get_gpnv
func_56:    jmp     rt_smbios_read_gpnv
func_57:    jmp     rt_smbios_write_gpnv

;<AMI_FHDR_START>
;---------------------------------------------------------------------------
;
; Name:         rt_generate_sw_smi
;
; Description:  Generate SW SMI
;
; Input:        AL = PnP function number
;               DX = SMI Port
;
; Output:       None
;
;---------------------------------------------------------------------------
;<AMI_FHDR_END>
rt_generate_sw_smi      PROC NEAR
        out     dx, al
        movzx   bx, al
        ret
rt_generate_sw_smi      ENDP

;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure:	SMBIOS_Func_50h
; Procedure:	RT_GET_SMBIOS_INFO
;
; Description:	This function returns information about the SMBIOS extensions
;		such as	the number of structures present and the size of the
;		largest one. This function is presently called only during RUNTIME.
;
; Input:	[EBP+00] = Function number (50h)
;		[EBP+02] = BYTE FAR *DmiBiosVersion
;		[EBP+06] = WORD FAR *NumStructures
;		[EBP+0A] = WORD FAR *StructureSize
;		[EBP+0E] = DWORD FAR *DMIStorageBase
;		[EBP+12] = WORD FAR *DMIStorageSize
;		[EBP+16] = WORD BiosSelector
;               DX = SMI IO port address
;
; Output:	AX	= Zero if successful
;			non-zero return code if non successful
;		[EBP+02] = BYTE FAR *DmiBiosVersion filled in with the version
;			of the DMI BIOS spec that this code supports.
;		[EBP+06] = WORD FAR *NumStructures filled with total number of
;			DMI structures that are present in the system.
;		[EBP+0A] = WORD FAR *StructureSize filled with the size in
;			bytes of the largest DMI structure.
;		[EBP+0E] = DWORD FAR *DMIStorageBase filled with the absolute
;			32Bit address of any memory-mapped structure
;		[EBP+12] = WORD FAR *DMIStorageSize filled with the buffer
;			size needed in Func 52h/54h.
;
; Modified:	Nothing
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

rt_get_smbios_info      PROC NEAR

        push    ebx
        push    eax
        push    si
        push    edi
        push    ds

        ; EBX = SS<<4+SP-sizeof(SMBIOS_PNP_FUNC50_DMIEDIT_STRUC)
        mov     ebx, 0
        mov     bx, ss
        shl     ebx, 4
        movzx   edi, sp
        sub     di, size SMBIOS_PNP_FUNC50_DMIEDIT_STRUC
        add     ebx, edi

        mov     sp, di

        mov     al, 50h
        call    rt_generate_sw_smi

        lds	si, DWORD PTR [ebp+02h]
        mov     eax, ss:(SMBIOS_PNP_FUNC50_DMIEDIT_STRUC PTR [di]).dDmiBiosRevision
        mov     BYTE PTR [si], al

        lds	si, DWORD PTR [ebp+06h]
        mov     eax, ss:(SMBIOS_PNP_FUNC50_DMIEDIT_STRUC PTR [di]).dDmiNumStructures
        mov     WORD PTR [si], ax

        lds	si, DWORD PTR [ebp+0Ah]
        mov     eax, ss:(SMBIOS_PNP_FUNC50_DMIEDIT_STRUC PTR [di]).dDmiStructureSize
        mov     WORD PTR [si], ax

        lds	si, DWORD PTR [ebp+0Eh]
        mov     eax, ss:(SMBIOS_PNP_FUNC50_DMIEDIT_STRUC PTR [di]).dDmiStorageBase
        mov     DWORD PTR [si], eax

        lds	si, DWORD PTR [ebp+12h]
        mov     eax, ss:(SMBIOS_PNP_FUNC50_DMIEDIT_STRUC PTR [di]).dDmiStorageSize
        mov     WORD PTR [si], ax

        add     sp, size SMBIOS_PNP_FUNC50_DMIEDIT_STRUC
        pop     ds
        pop     edi
        pop     si
        pop     eax
        mov     ax, bx
        pop     ebx

        ret
        
rt_get_smbios_info	ENDP


;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure:	SMBIOS_Func_51h
; Procedure:	RT_GET_SMBIOS_STRUC
;
; Description:  This function copies one DMI structure into the buffer
;		provided by the caller. The caller's DMI structure number is
;		then updated with next DMI structure number (or FFFF if no
;		more DMI structures are present).
;
; Input:	[EBP+00] = Function number (51h)
;		[EBP+02] = WORD FAR *StructureNum
;		[EBP+06] = WORD FAR *DmiStructureBuffer
;		[EBP+0A] = WORD DmiSelector
;		[EBP+0C] = WORD BiosSelector
;               DX = SMI IO port address
;
; Output:	AX	= Zero if success,  non-zero return code if non successful
;		[EBP+02] = WORD FAR *StructureNum updated to contain the next DMI
;			structure number or FFFF if no more structures exist
;		[EBP+06] = WORD FAR *DmiStructureBuffer pointer to buffer that is
;			filled in with the requested DMI structure.
;
; Modified:	AX
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

rt_get_smbios_struc	PROC NEAR
        push    ebx
        push    eax
;
; convert *StructureNum and *DmiStructureBuffer from SEG:OFS format
; to 32-bit addresses
;
        push    [ebp+2]       ; UINT16 *StructureNum
        push    [ebp+4]
        push    [ebp+6]       ; UINT8 *DmiStructureBuffer
        push    [ebp+8]

        mov     eax, 0

; prepare EBX with 32-bit addresses of *StructureNum and *DmiStructureBuffer
        mov     ebx, 0
        mov     bx, [ebp+4]
        shl     ebx, 4
        mov     ax, [ebp+2]
        add     ebx, eax
        mov     [ebp+2], ebx

        mov     ebx, 0
        mov     bx, [ebp+8]
        shl     ebx, 4
        mov     ax, [ebp+6]
        add     ebx, eax
        mov     [ebp+6], ebx

; prepare EBX with 32-bit address out of DS:EBP
        mov     ebx, 0
        mov     bx, ss
        shl     ebx, 4
        add     ebx, ebp
        mov     al, 51h
        call    rt_generate_sw_smi

        pop     [ebp+8]
        pop     [ebp+6]
        pop     [ebp+4]
        pop     [ebp+2]

        pop     eax
        mov     ax, bx
        pop     ebx

        ret

rt_get_smbios_struc     ENDP


;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure:    SMBIOS_Func_52h
; Procedure:    RT_SET_SMBIOS_STRUC
;
; Description:  This function sets the SMBIOS structure identified by the
;               type and possibly handle, found in the SMBIOS structure
;               header in the given buffer.
;
; Input:        [EBP+00] = Function number (52h)
;               [EBP+02] = WORD FAR *DmiDataBuffer
;               [EBP+06] = WORD FAR *DmiWorkBuffer
;               [EBP+0A] = WORD Control
;               [EBP+0C] = WORD DmiSelector
;               [EBP+0E] = WORD BiosSelector
;
; Output:   AX       = Zero if successful
;           non-zero return code if unsuccessful
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

rt_set_smbios_struc	PROC NEAR
        push    ebx
        push    eax
        push    si
        push    di
        push    ds

        lds     di, DWORD PTR [ebp+02h] ; DS:SI = ptr to given data buffer

        push    [ebp+2]

        mov     eax, 0
        mov     ebx, 0
        mov     bx, [ebp+4]
        shl     ebx, 4
        mov     ax, [ebp+2]
        add     ebx, eax
        mov     [ebp+2], ebx
        
; prepare EBX with 32-bit address out of DS:EBP
        mov     ebx, 0
        mov     bx, ss
        shl     ebx, 4
        add     ebx, ebp
        mov     al, 52h
        call    rt_generate_sw_smi

        pop     [ebp+2]

; Update smbios_change_status

        ; Get the offset of SMBIOS_CHANGE_STRUC data
        call    get_smbios_change_struc

        call    SMB_cs_read_x_write_ram

        mov     cs:(SMBIOS_CHANGE_STRUC PTR [si]).smbios_change_status, SMBIOS_SINGLE_STRUCTURE_AFFECTED
        mov     cs:(SMBIOS_CHANGE_STRUC PTR [si]).smbios_change_type, SMBIOS_ONE_MORE_STRUCTURE_CHANGED
        mov     ax, ds:WORD PTR (DMIHDR_STRUC PTR [di]).wHandle; AX = handle# of structure being changed
        mov     cs:(SMBIOS_CHANGE_STRUC PTR [si]).smbios_change_handle, ax	; changed handle

        call    SMB_cs_read_ram_write_rom

        pop     ds
        pop     di
        pop     si
        pop     eax
        mov     ax, bx
        pop     ebx
        ret
rt_set_smbios_struc	ENDP


;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure:    SMBIOS_Func_53h
; Procedure:    RT_GET_SMBIOS_STRUC_CHANGE_INFO
;
; Description:  This function returns the information about what type of
;               SMBIOS structure-change occurred.
;
; Input:        [EBP+00] = Function number (53h)
;               [EBP+02] = WORD FAR *DmiChangeStructure
;               [EBP+06] = WORD DmiSelector
;               [EBP+08] = WORD BiosSelector
;
; Output:       AX = Zero if successful
;               Non-zero return code if unsuccessful
;
; Modified:     Nothing
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

rt_get_smbios_struc_change_info	PROC	NEAR

        push    es
        push    ds
        push    gs
        push    fs
    
        mov     gs, WORD PTR [ebp+08h]  ; GS = BIOS Selector
        mov     fs, WORD PTR [ebp+06h]  ; FS = GPNV Selector
    
        call    SMB_cs_read_x_write_ram

        push    si
        push    di

        les     di, DWORD PTR [ebp+02h]	; ES:DI = ptr to DMI Change Structure buffer

; Get the offset of SMBIOS_CHANGE_STRUC data
        call    get_smbios_change_struc

        xor     ax, ax
        xchg    ah, cs:(SMBIOS_CHANGE_STRUC PTR [si]).smbios_change_status; AH = SMBIOS Change Status
        xchg    al, cs:(SMBIOS_CHANGE_STRUC PTR [si]).smbios_change_type; AL = SMBIOS Change Type
        mov     es:BYTE PTR (SMBIOSFun53BufferSTRUC PTR [di]).bChangeStatus, ah
        mov     es:BYTE PTR (SMBIOSFun53BufferSTRUC PTR [di]).bChangeType, al
        push    ax
        cmp     ah, SMBIOS_MULTIPLE_STRUCTURE_AFFECTED
        mov     ax, 0000h		; DO NOT USE xor ax, ax !!!!!!!!!!!!!!!!
                                ; Flags has the information of prev CMP
        xchg    ax, cs:(SMBIOS_CHANGE_STRUC PTR [si]).smbios_change_handle; AX = handle# of changed structure
        jnz     SHORT rgssci_00 ; single structure affected
        xor     ax, ax          ; make handle# 00 for multiple structures change
    
rgssci_00:
        mov     es:WORD PTR (SMBIOSFun53BufferSTRUC PTR [di]).wChangeHandle, ax
        pop     ax
        pop     si
        pop     di
    
        call    SMB_cs_read_ram_write_rom
    
        cmp     ah, SMBIOS_NO_CHANGE    ; any change in status ?
        mov     ax, RT_DMI_SUCCESS      ; successful
        jnz     SHORT rgssci_01         ; change in status
        mov     ax, RT_DMI_NO_CHANGE    ; no change in status,  return with error code
    
rgssci_01:
        pop     fs
        pop     gs
        pop     ds
        pop     es
        ret

rt_get_smbios_struc_change_info ENDP

SMBIOS_CHANGE_STRUC STRUCT
    smbios_change_handle    DW ?
    smbios_change_status    DB ?
    smbios_change_type      DB ?
SMBIOS_CHANGE_STRUC ENDS

get_smbios_change_struc:
    pushf
    cli
    call    l_01    ; stores IP
    mov     si, sp
    mov     si, ss:[si-2]
    popf            ; restore IF
    add     si, 11  ; cs:si points to smbios_change_handle
l_01:
    ret
smbios_change_handle    DW ?
smbios_change_status    DB ?
smbios_change_type      DB ?

;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure:    SMB_cs_read_x_write_ram
;
; Description:  This function makes the code segment Write Only.
;
; Input:        None
;
; Notes:        This function needs to ported or modified accordingly.
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

SMB_cs_read_x_write_ram PROC NEAR PUBLIC
        push    eax
        push    dx
        mov     dx, 0CF8h
        mov     eax, 80000094h
        out     dx, eax
        mov     dx, 0CFCh
        in      eax, dx
        or      eax, 00020000h  ; 0/0/0/96/Bit1, enable E8000 for writing
        out     dx, eax
        pop     dx
        pop     eax
        ret
SMB_cs_read_x_write_ram ENDP


;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure:    SMB_cs_read_ram_write_rom
;
; Description:  This function makes the code segment Read Only.
;
; Input:        None
;
; Notes:        This function needs to ported or modified accordingly.
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

SMB_cs_read_ram_write_rom PROC NEAR PUBLIC
        push    eax
        push    dx
        mov     dx, 0CF8h
        mov     eax, 80000094h
        out     dx, eax
        mov     dx, 0CFCh
        in      eax, dx
        and     eax, 0FFFDFFFFh  ; 0/0/0/96/Bit1, make E8000 read-only
        out     dx, eax
        pop     dx
        pop     eax
        ret
SMB_cs_read_ram_write_rom ENDP


;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure:    SMBIOS_Func_54h
; Procedure:    RT_SMBIOS_CONTROL
;
; Description:  This function returns the information about what type of
;               SMBIOS structure-change occurred.
;
; Input:        [EBP+00] = Function number (54h)
;               [EBP+02] = WORD SubFunction - Defines the specific control operation
;               [EBP+04] = VOID FAR *Data   - Input/output data buffer, SubFunction specific
;               [EBP+08] = BYTE Control     - Conditions for setting the structure
;               [EBP+09] = WORD DmiSelector - SMBIOS data read/write selector
;               [EBP+11] = WORD BiosSelector- PnP BIOS readable/writeable selector
;
; Output:       AX = Zero if successful
;               Non-zero return code if unsuccessful
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

rt_smbios_control   PROC NEAR
; Do the porting here as needed
;-    mov     ax, RT_PNP_UNSUPPORTED  ;Return code for unsupported func
; (debx+03062009)>
    mov     al, 30h            ; SmiGpnv
    call    rt_generate_sw_smi
; <(debx+03062009)
    ret

rt_smbios_control   ENDP

;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure:    SMBIOS_Func_55h
;
; Description:  Required for GPNV support. This function returns information 
;               about a General Purpose NonVolatile (GPNV) area. The Handle 
;               argument is a pointer to a number that identifies which GPNV's
;               information is requested, a value of 0 accesses the first 
;               (or only) area.
;
; Input:        [EBP+00] DW PnP BIOS Function 55h
;               [EBP+02] DW *Handle - Identifies which GPNV to access
;               [EBP+06] DW *MinGPNVRWSize - Minimum buffer size in bytes for GPNV access
;               [EBP+0A] DW *GPNVSize - Size allocated for GPNV within the R/W Block
;               [EBP+0E] DD *NVStorageBase - 32-bit physical base address for...
;                                     ... mem. mapped nonvolatile storage media 
;               [EBP+12] DW BiosSelector - PnP BIOS readable/writable selector
;
; Output:       If successful - DMI_SUCCESS If an Error (Bit 7 set) or a 
;               Warning occurred the Error Code will be returned in AX, the 
;               FLAGS and all other registers will be preserved
;
; Notes:        On return:
;
;               *Handle is updated either with the handle of the next GPNV 
;               area or, if there are no more areas, 0FFFFh. GPNV handles 
;               are assigned sequentially by the system, from 0 to the total 
;               number of areas (minus 1).
;
;               *MinGPNVRW Size is updated with the minimum size, in bytes, 
;               of any buffer used to access this GPNV area. For a Flash 
;               based GPNV area, this would be the size of the Flash block 
;               containing the actual GPNV.
;
;               *GPNVSize is updated with the size, in bytes, of this GPNV 
;               area (which is less than or equal to the MinGPNVRWSize value).
;
;               *NVStorageBase is updated with the paragraph-aligned, 32-bit 
;               absolute physical base address of this GPNV. If non-zero, 
;               this value allows the caller to construct a 16-bit data 
;               segment descriptor with a limit of MinGPNVRWSize and 
;               read/write access. If the value is 0, protected-mode mapping 
;               is not required for this GPNV.
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

rt_smbios_get_gpnv PROC NEAR PUBLIC
    mov     al, 30h            ; SmiGpnv
    call    rt_generate_sw_smi
    ret
rt_smbios_get_gpnv ENDP

;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure:    SMBIOS_Func_56h
;
; Description:  Required for GPNV support. This function is used to read an 
;               entire GPNV area into the buffer specified by GPNVBuffer. It 
;               is the responsibility of the caller to ensure that GPNVBuffer 
;               is large enough to store the entire GPNV storage block - this 
;               buffer must be at least the MinGPNVRWSize returned by Function 
;               55h - Get GPNV Information. The Handle argument identifies the 
;               specific GPNV to be read. On a successful read of a GPNV area, 
;               that GPNV area will be placed in the GPNVBuffer beginning at 
;               offset 0. The protected-mode selector GPNVSelector has base 
;               equal to NVStorageBase and limit of at least MinGPNVRWSize - 
;               so long as the NVStorageBase value returned from Function 55h 
;               was non-zero.
;
; Input:        [EBP+00] DW PnP BIOS Function 56h
;               [EBP+02] DW Handle - Identifies which GPNV is to be read
;               [EBP+04] DW *GPNVBuffer - Address of buffer in which to return GPNV
;               [EBP+08] DW *GPNVLock - Lock value
;               [EBP+0C] DW GPNVSelector - Selector for GPNV Storage
;               [EBP+0E] DW BiosSelector - PnP BIOS readable/writable selector
;
; Output:       If the GPNV lock is supported and the lock set request succeeds, 
;               the caller's GPNVLock is set to the value of the current lock 
;               and the function returns DMI_SUCCESS.
;
; Notes:        If the GPNV request fails, one of the following values is 
;               returned:
;                   - DMI_LOCK_NOT_SUPPORTED
;                   - DMI_INVALID_LOCK
;                   - DMI_CURRENTLY_LOCKED
;
;               For return status codes DMI_SUCCESS, DMI_LOCK_NOT_SUPPORTED 
;               and DMI_CURRENTLY_LOCKED, the GPNV Read function returns the 
;               current contents of the GPNV associated with Handle as the 
;               first GPNVSize bytes within GPNVBuffer, starting at offset 0. 
;               If a lock request fails with DMI_CURRENTLY_LOCKED status, 
;               the caller's GPNVLock will be set to the value of the current 
;               lock.
;               Passing a GPNVLock value of -1 to the GPNV Read causes the 
;               GPNVLock value to be ignored - in this case the underlying 
;               logic makes no attempt to store a lock value for comparison 
;               with lock values passed into GPNV Write. Any value provided 
;               for GPNVLock besides -1 is accepted as a valid value for a 
;               lock request.
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

rt_smbios_read_gpnv PROC NEAR PUBLIC
    mov     al, 30h            ; SmiGpnv
    call    rt_generate_sw_smi
    ret
rt_smbios_read_gpnv ENDP

;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure:    SMBIOS_Func_57h
;
; Description:  Required for GPNV support. This function is used to write an 
;               entire GPNV from the GPNVBuffer into the nonvolatile storage 
;               area. The Handle argument identifies the specific GPNV to be 
;               written. The protectedmode selector GPNVSelector has base equal 
;               to NVStorageBase and limit of at least MinGPNVRWSize - so long 
;               as the NVStorageBase value returned from Get GPNV Information 
;               was non-zero. The caller should first call Read GPNV Data 
;               (with a lock) to get the current area contents, modify the 
;               data, and pass it into this function - this ensures that the 
;               GPNVBuffer which is written contains a complete definition for 
;               the entire GPNV area. If the BIOS uses some form of block erase 
;               device, the caller must also allocate enough buffer space for 
;               the BIOS to store all data from the part during the reprogramming 
;               operation, not just the data of interest. 
;               The data to be written to the GPNV selected by Handle must 
;               reside as the first GPNVSize bytes of the GPNVBuffer. 
;               Note: The remaining (MinGPNVRWSize-GPNVSize) bytes of the 
;               GPNVBuffer area are used as a scratch-area by the BIOS call 
;               in processing the write request; the contents of that area of 
;               the buffer are destroyed by this function call.
;               The GPNVLock provides a mechanism for cooperative use of the 
;               GPNV, and is set during a GPNV Read (Function 56h). If the 
;               input GPNVLock value is -1 the caller requests a forced write 
;               to the GPNV area, ignoring any outstanding GPNVLock. If the 
;               caller is not doing a forced write, the value passed in GPNVLock 
;               to the GPNV Write must be the same value as that (set and) 
;               returned by a previous GPNV Read (Function 56h). 
;
; Input:        [EBP+00] - PnP BIOS Function 57h 
;               [EBP+02] - Handle - Identifies which GPNV is to be written 
;               [EBP+04] - *GPNVBuffer - Address of buffer containing complete GPNV to write
;               [EBP+08] - GPNVLock - Lock value 
;               [EBP+0A] - GPNVSelector - Selector for GPNV Storage 
;               [EBP+0C] - BiosSelector - PnP BIOS readable/writable selector 
;
; Output:       The GPNV Write function returns a value of DMI_ LOCK_NOT_SUPPORTED 
;               when a GPNVLock value other than -1 is specified and locking is 
;               not supported. A return status of DMI_ CURRENTLY_LOCKED indicates 
;               that the call has failed due to an outstanding lock on the GPNV 
;               area which does not match the caller's GPNVLock value. Any 
;               outstanding GPNVLock value (which was set by a previous Error! 
;               Reference source not found.) gets cleared on a successful 
;               write of the GPNV.
; Notes:        
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

rt_smbios_write_gpnv PROC NEAR PUBLIC
    mov     al, 30h            ; SmiGpnv
    call    rt_generate_sw_smi
    ret
rt_smbios_write_gpnv ENDP


OEM16_CSEG ENDS

END
;----------------------------------------------------------------------------
;****************************************************************************
;****************************************************************************
;**                                                                        **
;**           (C)Copyright 1985-2009, American Megatrends, Inc.            **
;**                                                                        **
;**                          All Rights Reserved.                          **
;**                                                                        **
;**            5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093           **
;**                                                                        **
;**                          Phone: (770)-246-8600                         **
;**                                                                        **
;****************************************************************************
;****************************************************************************
