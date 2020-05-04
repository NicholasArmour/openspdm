/**
@file
UEFI OS based application.

Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SpdmRequesterTest.h"

VOID                         *mSpdmContext;
SOCKET                       mSocket;

BOOLEAN
CommunicatePlatformData (
  IN SOCKET           Socket,
  IN UINT32           Command,
  IN UINT8            *SendBuffer,
  IN UINTN            BytesToSend,
  OUT UINT32          *Response,
  IN OUT UINTN        *BytesToReceive,
  OUT UINT8           *ReceiveBuffer
  )
{
  BOOLEAN Result;

  Result = SendPlatformData (Socket, Command, SendBuffer, BytesToSend);
  if (!Result) {
    printf ("SendPlatformData Error - %x\n", WSAGetLastError());
    return Result;
  }

  Result = ReceivePlatformData (Socket, Response, ReceiveBuffer, BytesToReceive);
  if (!Result) {
    printf ("ReceivePlatformData Error - %x\n", WSAGetLastError());
    return Result;
  }
  return Result;
}

/**
  Send a SPDM request command to a device.

  @param  This                         Indicates a pointer to the calling context.
  @param  RequestSize                  Size in bytes of the request data buffer.
  @param  Request                      A pointer to a destination buffer to store the request.
                                       The caller is responsible for having
                                       either implicit or explicit ownership of the buffer.
  @param  Timeout                      The timeout, in 100ns units, to use for the execution
                                       of the request. A Timeout value of 0
                                       means that this function will wait indefinitely for the
                                       request to execute. If Timeout is greater
                                       than zero, then this function will return RETURN_TIMEOUT if the
                                       time required to execute the request is greater
                                       than Timeout.
                                       
  @retval RETURN_SUCCESS                  The SPDM request is sent successfully.
  @retval RETURN_DEVICE_ERROR             A device error occurs when the SPDM request is sent to the device.
  @retval RETURN_INVALID_PARAMETER        The Request is NULL or the RequestSize is zero.
  @retval RETURN_TIMEOUT                  A timeout occurred while waiting for the SPDM request
                                          to execute.
**/
RETURN_STATUS
EFIAPI
SpdmClientSendRequest (
  IN     EDKII_SPDM_IO_PROTOCOL  *This,
  IN     UINTN                   RequestSize,
  IN     VOID                    *Request,
  IN     UINT64                  Timeout
  )
{
  BOOLEAN Result;

  Result = SendPlatformData (mSocket, SOCKET_SPDM_COMMAND_NORMAL, Request, (UINT32)RequestSize);
  if (!Result) {
    printf ("SendPlatformData Error - %x\n", WSAGetLastError());
    return RETURN_DEVICE_ERROR;
  }
  return RETURN_SUCCESS;
}

/**
  Receive a SPDM response from a device.

  @param  This                         Indicates a pointer to the calling context.
  @param  ResponseSize                 Size in bytes of the response data buffer.
  @param  Response                     A pointer to a destination buffer to store the response.
                                       The caller is responsible for having
                                       either implicit or explicit ownership of the buffer.
  @param  Timeout                      The timeout, in 100ns units, to use for the execution
                                       of the response. A Timeout value of 0
                                       means that this function will wait indefinitely for the
                                       response to execute. If Timeout is greater
                                       than zero, then this function will return RETURN_TIMEOUT if the
                                       time required to execute the response is greater
                                       than Timeout.
                                       
  @retval RETURN_SUCCESS                  The SPDM response is received successfully.
  @retval RETURN_DEVICE_ERROR             A device error occurs when the SPDM response is received from the device.
  @retval RETURN_INVALID_PARAMETER        The Reponse is NULL, ResponseSize is NULL or
                                          the *RequestSize is zero.
  @retval RETURN_TIMEOUT                  A timeout occurred while waiting for the SPDM response
                                          to execute.
**/
RETURN_STATUS
EFIAPI
SpdmClientReceiveResponse (
  IN     EDKII_SPDM_IO_PROTOCOL  *This,
  IN OUT UINTN                   *ResponseSize,
  IN OUT VOID                    *Response,
  IN     UINT64                  Timeout
  )
{
  BOOLEAN Result;
  UINT32  Command;

  Result = ReceivePlatformData (mSocket, &Command, Response, ResponseSize);
  if (!Result) {
    printf ("ReceivePlatformData Error - %x\n", WSAGetLastError());
    return RETURN_DEVICE_ERROR;
  }
  if (Command != SOCKET_SPDM_COMMAND_NORMAL) {
    printf ("ReceivePlatformData Command Error - %x\n", Command);
    return RETURN_DEVICE_ERROR;
  }
  return RETURN_SUCCESS;
}

/**
  Return Secure Messaging type.

  @param  This                         Indicates a pointer to the calling context.
  @param  SecureMessagingType          Secure messaging type.
                                       
  @retval RETURN_SUCCESS                  The secure messaging type is returned successfully.
  @retval RETURN_INVALID_PARAMETER        The SecureMessagingType is NULL.
**/
RETURN_STATUS
EFIAPI
SpdmClientGetSecureMessagingType (
  IN     EDKII_SPDM_IO_PROTOCOL              *This,
     OUT UINTN                               *SecureMessagingTypeCount,
     OUT EDKII_SPDM_IO_SECURE_MESSAGING_TYPE **SecureMessagingType
  )
{
  *SecureMessagingTypeCount = 1;
  *SecureMessagingType = (VOID *)malloc (sizeof(EDKII_SPDM_IO_SECURE_MESSAGING_TYPE));
  **SecureMessagingType = EdkiiSpdmIoSecureMessagingTypeSocket;
  return RETURN_UNSUPPORTED;
}

/**
  Send a SPDM request command to a device.

  The request is a data blob to send to the messaging device directly,
  including session ID, length, encrypted message and MAC.
  The caller need use GetSecureMessagingType() to decide the format of the message.

  @param  This                         Indicates a pointer to the calling context.
  @param  RequestSize                  Size in bytes of the request data buffer.
  @param  Request                      A pointer to a destination buffer to store the request.
                                       The caller is responsible for having
                                       either implicit or explicit ownership of the buffer.
  @param  Timeout                      The timeout, in 100ns units, to use for the execution
                                       of the request. A Timeout value of 0
                                       means that this function will wait indefinitely for the
                                       request to execute. If Timeout is greater
                                       than zero, then this function will return RETURN_TIMEOUT if the
                                       time required to execute the request is greater
                                       than Timeout.
                                       
  @retval RETURN_SUCCESS                  The SPDM request is sent successfully.
  @retval RETURN_DEVICE_ERROR             A device error occurs when the SPDM request is sent to the device.
  @retval RETURN_INVALID_PARAMETER        The Request is NULL or the RequestSize is zero.
  @retval RETURN_TIMEOUT                  A timeout occurred while waiting for the SPDM request
                                          to execute.
**/
RETURN_STATUS
EFIAPI
SpdmClientSecureSendRequest (
  IN     EDKII_SPDM_IO_PROTOCOL                 *This,
  IN     EDKII_SPDM_IO_SECURE_MESSAGING_TYPE    SecureMessagingType,
  IN     UINT8                                  SessionId,
  IN     UINTN                                  RequestSize,
  IN     VOID                                   *Request,
  IN     UINT64                                 Timeout
  )
{
  BOOLEAN Result;

  Result = SendPlatformData (mSocket, MAKE_COMMAND(SOCKET_SPDM_COMMAND_SECURE, SessionId), Request, (UINT32)RequestSize);
  if (!Result) {
    printf ("SendPlatformData Error - %x\n", WSAGetLastError());
    return RETURN_DEVICE_ERROR;
  }
  return RETURN_SUCCESS;
}

/**
  Receive a SPDM response from a device.

  The response is a data blob received from the messaging device directly,
  including session ID, length, encrypted message and MAC.
  The caller need use GetSecureMessagingType() to decide the format of the message.

  @param  This                         Indicates a pointer to the calling context.
  @param  ResponseSize                 Size in bytes of the response data buffer.
  @param  Response                     A pointer to a destination buffer to store the response.
                                       The caller is responsible for having
                                       either implicit or explicit ownership of the buffer.
  @param  Timeout                      The timeout, in 100ns units, to use for the execution
                                       of the response. A Timeout value of 0
                                       means that this function will wait indefinitely for the
                                       response to execute. If Timeout is greater
                                       than zero, then this function will return RETURN_TIMEOUT if the
                                       time required to execute the response is greater
                                       than Timeout.
                                       
  @retval RETURN_SUCCESS                  The SPDM response is received successfully.
  @retval RETURN_DEVICE_ERROR             A device error occurs when the SPDM response is received from the device.
  @retval RETURN_INVALID_PARAMETER        The Reponse is NULL, ResponseSize is NULL or
                                          the *RequestSize is zero.
  @retval RETURN_TIMEOUT                  A timeout occurred while waiting for the SPDM response
                                          to execute.
**/
RETURN_STATUS
EFIAPI
SpdmClientSecureReceiveResponse (
  IN     EDKII_SPDM_IO_PROTOCOL                 *This,
  IN     EDKII_SPDM_IO_SECURE_MESSAGING_TYPE    SecureMessagingType,
  IN     UINT8                                  SessionId,
  IN OUT UINTN                                  *ResponseSize,
  IN OUT VOID                                   *Response,
  IN     UINT64                                 Timeout
  )
{
  BOOLEAN Result;
  UINT32  Command;

  Command = MAKE_COMMAND(SOCKET_SPDM_COMMAND_SECURE, SessionId);
  Result = ReceivePlatformData (mSocket, &Command, Response, ResponseSize);
  if (!Result) {
    printf ("ReceivePlatformData Error - %x\n", WSAGetLastError());
    return RETURN_DEVICE_ERROR;
  }
  if (GET_COMMAND_SESSION_ID(Command) != SessionId) {
    printf ("ReceivePlatformData Command Error - %x\n", Command);
    return RETURN_DEVICE_ERROR;
  }
  if (GET_COMMAND_OPCODE(Command) != SOCKET_SPDM_COMMAND_SECURE) {
    printf ("ReceivePlatformData Command Error - %x\n", Command);
    return RETURN_DEVICE_ERROR;
  }
  return RETURN_SUCCESS;
}

EDKII_SPDM_IO_PROTOCOL       mSpdmProtocol = {
  SpdmClientSendRequest,
  SpdmClientReceiveResponse,
  SpdmClientGetSecureMessagingType,
  SpdmClientSecureSendRequest,
  SpdmClientSecureReceiveResponse,
};

VOID
SpdmClientInit (
  VOID
  )
{
  VOID                         *SpdmContext;
  RETURN_STATUS                Status;
  BOOLEAN                      Res;
  VOID                         *Data;
  UINTN                        DataSize;
  EDKII_SPDM_DATA_PARAMETER    Parameter;
  UINT8                        Data8;
  UINT16                       Data16;
  UINT32                       Data32;

  mSpdmContext = malloc (SpdmGetContextSize());
  SpdmContext = mSpdmContext;
  SpdmInitContext (SpdmContext);
  SpdmRegisterSpdmIo (SpdmContext, &mSpdmProtocol);

  Res = ReadPublicCertificateChain (&Data, &DataSize);
  if (Res) {
    ZeroMem (&Parameter, sizeof(Parameter));
    Parameter.Location = EdkiiSpdmDataLocationLocal;
    SpdmSetData (SpdmContext, EdkiiSpdmPeerPublicCertChains, &Parameter, Data, DataSize);
    // Do not free it.
  }

  Data8 = 0;
  ZeroMem (&Parameter, sizeof(Parameter));
  Parameter.Location = EdkiiSpdmDataLocationLocal;
  SpdmSetData (SpdmContext, EdkiiSpdmDataCapabilityCTExponent, &Parameter, &Data8, sizeof(Data8));

  Data32 = SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CERT_CAP |
           SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CHAL_CAP |
           SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MEAS_CAP_NO_SIG |
           SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MEAS_CAP_SIG |
           SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_ENCRYPT_CAP |
           SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MAC_CAP |
           SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_KEY_EX_CAP;
  SpdmSetData (SpdmContext, EdkiiSpdmDataCapabilityFlags, &Parameter, &Data32, sizeof(Data32));

  Data32 = SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSASSA_2048;
  SpdmSetData (SpdmContext, EdkiiSpdmDataBaseAsymAlgo, &Parameter, &Data32, sizeof(Data32));
  Data32 = SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_256;
  SpdmSetData (SpdmContext, EdkiiSpdmDataBaseHashAlgo, &Parameter, &Data32, sizeof(Data32));
  Data16 = SPDM_ALGORITHMS_DHE_NAMED_GROUP_FFDHE_2048;
  SpdmSetData (SpdmContext, EdkiiSpdmDataDHENamedGroup, &Parameter, &Data16, sizeof(Data16));
  Data16 = SPDM_ALGORITHMS_KEY_SCHEDULE_HMAC_HASH;
  SpdmSetData (SpdmContext, EdkiiSpdmDataAEADCipherSuite, &Parameter, &Data16, sizeof(Data16));
  Data16 = SPDM_ALGORITHMS_KEY_SCHEDULE_HMAC_HASH;
  SpdmSetData (SpdmContext, EdkiiSpdmDataKeySchedule, &Parameter, &Data16, sizeof(Data16));

  Status = SpdmInitConnection (SpdmContext);
  if (RETURN_ERROR(Status)) {
    printf ("SpdmInitConnection - 0x%x\n", (UINT32)Status);
  }

  return ;
}