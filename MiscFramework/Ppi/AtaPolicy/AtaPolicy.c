/*++
  This file contains 'Framework Code' and is licensed as such   
  under the terms of your license agreement with Intel or your  
  vendor.  This file may not be modified, except as allowed by  
  additional terms of your license agreement.                   
--*/
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  AtaPolicy.c

Abstract:

  ATA Policy PPI GUID as defined in EFI 2.0

--*/

#include "Tiano.h"
#include "Pei.h"
#include EFI_PPI_DEFINITION (AtaPolicy)

EFI_GUID  gPeiAtaPolicyPpiGuid = PEI_ATA_POLICY_PPI_GUID;

EFI_GUID_STRING(&gPeiAtaPolicyPpiGuid, "AtaPolicy", "ATA Policy PPI");
