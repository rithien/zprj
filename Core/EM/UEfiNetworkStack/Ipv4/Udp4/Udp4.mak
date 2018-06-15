#//**********************************************************************
#//**********************************************************************
#//**                                                                  **
#//**        (C)Copyright 1985-2008, American Megatrends, Inc.         **
#//**                                                                  **
#//**                       All Rights Reserved.                       **
#//**                                                                  **
#//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
#//**                                                                  **
#//**                       Phone: (770)-246-8600                      **
#//**                                                                  **
#//**********************************************************************
#//**********************************************************************

#**********************************************************************
# $Header: /Alaska/BIN/Modules/Network/UEFINetworkStack II/IPV4/Udp4/Udp4.mak 2     5/01/12 10:24a Hari $
#
# Revision: $
#
# $Date: 5/01/12 10:24a $
#**********************************************************************
# Revision History
# ----------------
# 
#**********************************************************************
#<AMI_FHDR_START>
#
# Name:	Udp6.mak
#
# Description:	
#
#<AMI_FHDR_END>
#**********************************************************************
all : Udp4

Udp4 : $(BUILD_DIR)\Udp4.ffs

!IF "$(x64_BUILD)"=="1"
$(BUILD_DIR)\Udp4.ffs : $(Udp4_DIR)\Udp4Dxe.efi
!ELSE
$(BUILD_DIR)\Udp4.ffs : $(Udp4_DIR)\Udp4DxeIa32.efi
!ENDIF
	$(MAKE) /f Core\FFS.mak \
	BUILD_DIR=$(BUILD_DIR) \
	GUID=10EE5462-B207-4a4f-ABD8-CB522ECAA3A4\
	TYPE=EFI_FV_FILETYPE_DRIVER \
	PEFILE=$** FFSFILE=$@ COMPRESS=1 NAME=$(**B)
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2004, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************