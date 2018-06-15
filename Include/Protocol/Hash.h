//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
// $Header: /Alaska/BIN/Core/Include/Protocol/Hash.h 3     5/13/11 4:35p Artems $
//
// $Revision: 3 $
//
// $Date: 5/13/11 4:35p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/BIN/Core/Include/Protocol/Hash.h $
// 
// 3     5/13/11 4:35p Artems
// AMI coding style compliance added
// 
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:	<Hash.h>
//
// Description:	Hash protocol header file
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef __EFI_HASH_PROTOCOL_H__
#define __EFI_HASH_PROTOCOL_H__
#ifdef __cplusplus
extern "C" {
#endif

#define EFI_HASH_SERVICE_BINDING_PROTOCOL_GUID \
  { 0x42881c98, 0xa4f3, 0x44b0, 0xa3, 0x9d, 0xdf, 0xa1, 0x86, 0x67, 0xd8, 0xcd }
  
#define EFI_HASH_PROTOCOL_GUID \
  { 0xc5184932, 0xdba5, 0x46db, 0xa5, 0xba, 0xcc, 0x0b, 0xda, 0x9c, 0x14, 0x35 }

#define EFI_HASH_ALGORITHM_SHA1_GUID \
  { 0x2ae9d80f, 0x3fb2, 0x4095, 0xb7, 0xb1, 0xe9, 0x31, 0x57, 0xb9, 0x46, 0xb6 }

#define EFI_HASH_ALGORITHM_SHA224_GUID \
  { 0x8df01a06, 0x9bd5, 0x4bf7, 0xb0, 0x21, 0xdb, 0x4f, 0xd9, 0xcc, 0xf4, 0x5b } 

#define EFI_HASH_ALGORITHM_SHA256_GUID \
  { 0x51aa59de, 0xfdf2, 0x4ea3, 0xbc, 0x63, 0x87, 0x5f, 0xb7, 0x84, 0x2e, 0xe9 } 

#define EFI_HASH_ALGORITHM_SHA384_GUID \
  { 0xefa96432, 0xde33, 0x4dd2, 0xae, 0xe6, 0x32, 0x8c, 0x33, 0xdf, 0x77, 0x7a } 

#define EFI_HASH_ALGORITHM_SHA512_GUID \
  { 0xcaa4381e, 0x750c, 0x4770, 0xb8, 0x70, 0x7a, 0x23, 0xb4, 0xe4, 0x21, 0x30 }

#define EFI_HASH_ALGORTIHM_MD5_GUID \
  { 0xaf7c79c, 0x65b5, 0x4319, 0xb0, 0xae, 0x44, 0xec, 0x48, 0x4e, 0x4a, 0xd7 }

typedef struct _EFI_HASH_PROTOCOL EFI_HASH_PROTOCOL;

typedef UINT8  EFI_MD5_HASH[16];
typedef UINT8  EFI_SHA1_HASH[20];
typedef UINT8  EFI_SHA224_HASH[28];
typedef UINT8  EFI_SHA256_HASH[32];
typedef UINT8  EFI_SHA384_HASH[48];
typedef UINT8  EFI_SHA512_HASH[64];

typedef union {
    EFI_MD5_HASH     *Md5Hash;
    EFI_SHA1_HASH    *Sha1Hash;
    EFI_SHA224_HASH  *Sha224Hash;
    EFI_SHA256_HASH  *Sha256Hash;
    EFI_SHA384_HASH  *Sha384Hash;
    EFI_SHA512_HASH  *Sha512Hash;
} EFI_HASH_OUTPUT;

typedef
EFI_STATUS
(EFIAPI *EFI_HASH_GET_HASH_SIZE) (
    IN  CONST EFI_HASH_PROTOCOL     *This,
    IN  CONST EFI_GUID              *HashAlgorithm,
    OUT UINTN                       *HashSize
);      

typedef
EFI_STATUS
(EFIAPI *EFI_HASH_HASH) (
    IN CONST EFI_HASH_PROTOCOL      *This,
    IN CONST EFI_GUID               *HashAlgorithm,
    IN BOOLEAN                      Extend,
    IN CONST UINT8                  *Message,
    IN UINT64                       MessageSize,
    IN OUT EFI_HASH_OUTPUT          *Hash
);    

struct _EFI_HASH_PROTOCOL {
    EFI_HASH_GET_HASH_SIZE GetHashSize;
    EFI_HASH_HASH          Hash;
};

GUID_VARIABLE_DECLARATION(gEfiHashServiceBindingProtocolGuid, EFI_HASH_SERVICE_BINDING_PROTOCOL_GUID);
GUID_VARIABLE_DECLARATION(gEfiHashProtocolGuid, EFI_HASH_PROTOCOL_GUID);
GUID_VARIABLE_DECLARATION(gEfiHashAlgorithmSha1Guid, EFI_HASH_ALGORITHM_SHA1_GUID);
GUID_VARIABLE_DECLARATION(gEfiHashAlgorithmSha224Guid, EFI_HASH_ALGORITHM_SHA224_GUID);
GUID_VARIABLE_DECLARATION(gEfiHashAlgorithmSha256Guid, EFI_HASH_ALGORITHM_SHA256_GUID);
GUID_VARIABLE_DECLARATION(gEfiHashAlgorithmSha384Guid, EFI_HASH_ALGORITHM_SHA384_GUID);
GUID_VARIABLE_DECLARATION(gEfiHashAlgorithmSha512Guid, EFI_HASH_ALGORITHM_SHA512_GUID);
GUID_VARIABLE_DECLARATION(gEfiHashAlgorithmMD5Guid, EFI_HASH_ALGORTIHM_MD5_GUID);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************