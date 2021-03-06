/** @file
  SPDM common library.
  It follows the SPDM Specification.

Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SpdmResponderLibInternal.h"

RETURN_STATUS
EFIAPI
SpdmReceiveSendData (
  IN     VOID                 *Context,
  IN     VOID                 *RequestBuffer,
  IN     UINTN                RequestBufferSize,
     OUT VOID                 *ResponseBuffer,
  IN OUT UINTN                *ResponseBufferSize
  )
{
  RETURN_STATUS            Status;
  SPDM_DEVICE_CONTEXT      *SpdmContext;

  SpdmContext = Context;

  Status = SpdmReceiveRequest (SpdmContext, RequestBufferSize, RequestBuffer);
  if (RETURN_ERROR(Status)) {
    return Status;
  }

  Status = SpdmSendResponse (SpdmContext, ResponseBufferSize, ResponseBuffer);
  if (RETURN_ERROR(Status)) {
    return Status;
  }
  return RETURN_SUCCESS;
}

RETURN_STATUS
EFIAPI
SpdmReceiveSendSessionData (
  IN     VOID                 *Context,
  IN     UINT8                SessionId,
  IN     VOID                 *RequestBuffer,
  IN     UINTN                RequestBufferSize,
     OUT VOID                 *ResponseBuffer,
  IN OUT UINTN                *ResponseBufferSize
  )
{
  RETURN_STATUS            Status;
  SPDM_DEVICE_CONTEXT      *SpdmContext;

  SpdmContext = Context;

  Status = SpdmReceiveRequestSession (SpdmContext, SessionId, RequestBufferSize, RequestBuffer);
  if (RETURN_ERROR(Status)) {
    return Status;
  }

  Status = SpdmSendResponseSession (SpdmContext, SessionId, ResponseBufferSize, ResponseBuffer);
  if (RETURN_ERROR(Status)) {
    return Status;
  }
  return RETURN_SUCCESS;
}
