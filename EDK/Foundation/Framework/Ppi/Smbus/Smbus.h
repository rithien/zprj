/*++

Copyright (c) 1999 - 2006, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.


Module Name:

  Smbus.h
    
Abstract:

  Smbus PPI as defined in EFI 2.0

--*/

#ifndef _PEI_SMBUS_PPI_H
#define _PEI_SMBUS_PPI_H

#include "EfiSmbus.h"

#define PEI_SMBUS_PPI_GUID \
  { \
    0xabd42895, 0x78cf, 0x4872, 0x84, 0x44, 0x1b, 0x5c, 0x18, 0xb, 0xfb, 0xda \
  }

EFI_FORWARD_DECLARATION (PEI_SMBUS_PPI);

typedef
EFI_STATUS
(EFIAPI *PEI_SMBUS_PPI_EXECUTE_OPERATION) (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN PEI_SMBUS_PPI                  * This,
  IN      EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress,
  IN      EFI_SMBUS_DEVICE_COMMAND  Command,
  IN      EFI_SMBUS_OPERATION       Operation,
  IN      BOOLEAN                   PecCheck,
  IN OUT  UINTN                     *Length,
  IN OUT  VOID                      *Buffer
  );

//;;## ...AMI_OVERRIDE... Support PI1.x typedef struct {
//;;## ...AMI_OVERRIDE... Support PI1.x   UINT32  VendorSpecificId;
//;;## ...AMI_OVERRIDE... Support PI1.x   UINT16  SubsystemDeviceId;
//;;## ...AMI_OVERRIDE... Support PI1.x   UINT16  SubsystemVendorId;
//;;## ...AMI_OVERRIDE... Support PI1.x   UINT16  Interface;
//;;## ...AMI_OVERRIDE... Support PI1.x   UINT16  DeviceId;
//;;## ...AMI_OVERRIDE... Support PI1.x   UINT16  VendorId;
//;;## ...AMI_OVERRIDE... Support PI1.x   UINT8   VendorRevision;
//;;## ...AMI_OVERRIDE... Support PI1.x   UINT8   DeviceCapabilities;
//;;## ...AMI_OVERRIDE... Support PI1.x } EFI_SMBUS_UDID;

typedef
EFI_STATUS
(EFIAPI *PEI_SMBUS_NOTIFY_FUNCTION) (
  IN      EFI_PEI_SERVICES              **PeiServices,
  IN PEI_SMBUS_PPI                      * SmbusPpi,
  IN      EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress,
  IN      UINTN                         Data
  );

//
// If ArpAll is TRUE, SmbusUdid/SlaveAddress is Optional.
// If FALSE, ArpDevice will enum SmbusUdid and the address will be at SlaveAddress
//
typedef
EFI_STATUS
(EFIAPI *PEI_SMBUS_PPI_ARP_DEVICE) (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN PEI_SMBUS_PPI                  * This,
  IN      BOOLEAN                   ArpAll,
  IN      EFI_SMBUS_UDID            * SmbusUdid, OPTIONAL
  IN OUT  EFI_SMBUS_DEVICE_ADDRESS  * SlaveAddress OPTIONAL
  );

//;;## ...AMI_OVERRIDE... Support PI1.x typedef struct {
//;;## ...AMI_OVERRIDE... Support PI1.x   EFI_SMBUS_DEVICE_ADDRESS  SmbusDeviceAddress;
//;;## ...AMI_OVERRIDE... Support PI1.x   EFI_SMBUS_UDID            SmbusDeviceUdid;
//;;## ...AMI_OVERRIDE... Support PI1.x } EFI_SMBUS_DEVICE_MAP;

typedef
EFI_STATUS
(EFIAPI *PEI_SMBUS_PPI_GET_ARP_MAP) (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN PEI_SMBUS_PPI                  * This,
  IN OUT  UINTN                     *Length,
  IN OUT  EFI_SMBUS_DEVICE_MAP      **SmbusDeviceMap
  );

typedef
EFI_STATUS
(EFIAPI *PEI_SMBUS_PPI_NOTIFY) (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN PEI_SMBUS_PPI                  * This,
  IN      EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress,
  IN      UINTN                     Data,
  IN      PEI_SMBUS_NOTIFY_FUNCTION NotifyFunction
  );

typedef struct _PEI_SMBUS_PPI {
  PEI_SMBUS_PPI_EXECUTE_OPERATION Execute;
  PEI_SMBUS_PPI_ARP_DEVICE        ArpDevice;
  PEI_SMBUS_PPI_GET_ARP_MAP       GetArpMap;
  PEI_SMBUS_PPI_NOTIFY            Notify;
} PEI_SMBUS_PPI;

extern EFI_GUID gPeiSmbusPpiGuid;

#endif
