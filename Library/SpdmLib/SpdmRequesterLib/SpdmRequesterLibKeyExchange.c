/** @file
  SPDM common library.
  It follows the SPDM Specification.

Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SpdmRequesterLibInternal.h"

#pragma pack(1)

typedef struct {
  SPDM_MESSAGE_HEADER  Header;
  UINT32               DHE_Named_Group;
  UINT8                RandomData[SPDM_RANDOM_DATA_SIZE];
  UINT8                ExchangeData[MAX_DHE_KEY_SIZE];
} SPDM_KEY_EXCHANGE_REQUEST_MINE;

typedef struct {
  SPDM_MESSAGE_HEADER  Header;
  UINT16               Length;
  UINT8                MutAuthRequested;
  UINT8                Reserved;
  UINT8                RandomData[SPDM_RANDOM_DATA_SIZE];
  UINT8                ExchangeData[MAX_DHE_KEY_SIZE];
  UINT8                MeasurementSummaryHash[MAX_HASH_SIZE];
  UINT8                Signature[MAX_ASYM_KEY_SIZE];
  UINT8                VerifyData[MAX_HASH_SIZE];
} SPDM_KEY_EXCHANGE_RESPONSE_MAX;

#pragma pack()

BOOLEAN
SpdmRequesterVerifyKeyExchangeSignature (
  IN SPDM_DEVICE_CONTEXT          *SpdmContext,
  IN SPDM_SESSION_INFO            *SessionInfo,
  IN VOID                         *SignData,
  IN INTN                         SignDataSize
  )
{
  HASH_ALL                                  HashFunc;
  UINTN                                     HashSize;
  UINT8                                     HashData[MAX_HASH_SIZE];
  BOOLEAN                                   Result;
  UINT8                                     *CertBuffer;
  UINTN                                     CertBufferSize;
  UINT8                                     CertBufferHash[MAX_HASH_SIZE];
  VOID                                      *Context;
  ASYM_GET_PUBLIC_KEY_FROM_X509             GetPublicKeyFromX509Func;
  ASYM_FREE                                 FreeFunc;
  ASYM_VERIFY                               VerifyFunc;
  LARGE_MANAGED_BUFFER                      THCurr = {MAX_SPDM_MESSAGE_BUFFER_SIZE};

  HashFunc = GetSpdmHashFunc (SpdmContext);
  ASSERT(HashFunc != NULL);
  HashSize = GetSpdmHashSize (SpdmContext);

  if ((SpdmContext->LocalContext.SpdmCertChainVarBuffer == NULL) || (SpdmContext->LocalContext.SpdmCertChainVarBufferSize == 0)) {
    return FALSE;
  }
  CertBuffer = (UINT8 *)SpdmContext->LocalContext.SpdmCertChainVarBuffer + sizeof(SPDM_CERT_CHAIN) + HashSize;
  CertBufferSize = SpdmContext->LocalContext.SpdmCertChainVarBufferSize - (sizeof(SPDM_CERT_CHAIN) + HashSize);
  HashFunc (CertBuffer, CertBufferSize, CertBufferHash);

  DEBUG((DEBUG_INFO, "MessageA Data :\n"));
  InternalDumpHex (GetManagedBuffer(&SpdmContext->Transcript.MessageA), GetManagedBufferSize(&SpdmContext->Transcript.MessageA));

  DEBUG((DEBUG_INFO, "THMessageCt Data :\n"));
  InternalDumpHex (CertBuffer, CertBufferSize);

  DEBUG((DEBUG_INFO, "MessageK Data :\n"));
  InternalDumpHex (GetManagedBuffer(&SessionInfo->SessionTranscript.MessageK), GetManagedBufferSize(&SessionInfo->SessionTranscript.MessageK));

  AppendManagedBuffer (&THCurr, GetManagedBuffer(&SpdmContext->Transcript.MessageA), GetManagedBufferSize(&SpdmContext->Transcript.MessageA));
  AppendManagedBuffer (&THCurr, CertBufferHash, HashSize);
  AppendManagedBuffer (&THCurr, GetManagedBuffer(&SessionInfo->SessionTranscript.MessageK), GetManagedBufferSize(&SessionInfo->SessionTranscript.MessageK));

  HashFunc (GetManagedBuffer(&THCurr), GetManagedBufferSize(&THCurr), HashData);
  DEBUG((DEBUG_INFO, "THCurr Hash - "));
  InternalDumpData (HashData, HashSize);
  DEBUG((DEBUG_INFO, "\n"));

  GetPublicKeyFromX509Func = GetSpdmAsymGetPublicKeyFromX509 (SpdmContext);
  FreeFunc = GetSpdmAsymFree (SpdmContext);
  VerifyFunc = GetSpdmAsymVerify (SpdmContext);
  Result = GetPublicKeyFromX509Func (CertBuffer, CertBufferSize, &Context);
  if (!Result) {
    return FALSE;
  }

  Result = VerifyFunc (
             Context,
             HashData,
             HashSize,
             SignData,
             SignDataSize
             );
  FreeFunc (Context);
  if (!Result) {
    DEBUG((DEBUG_INFO, "!!! VerifyKeyExchangeSignature - FAIL !!!\n"));
    return FALSE;
  }
  DEBUG((DEBUG_INFO, "!!! VerifyKeyExchangeSignature - PASS !!!\n"));

  return TRUE;
}

BOOLEAN
SpdmRequesterVerifyKeyExchangeHmac (
  IN     SPDM_DEVICE_CONTEXT  *SpdmContext,
  IN     SPDM_SESSION_INFO    *SessionInfo,
  IN     VOID                 *HmacData,
  IN     UINTN                HmacDataSize
  )
{
  HASH_ALL                                  HashFunc;
  HMAC_ALL                                  HmacFunc;
  UINTN                                     HashSize;
  UINT8                                     CalcHmacData[MAX_HASH_SIZE];
  UINT8                                     *CertBuffer;
  UINTN                                     CertBufferSize;
  UINT8                                     CertBufferHash[MAX_HASH_SIZE];
  LARGE_MANAGED_BUFFER                      THCurr = {MAX_SPDM_MESSAGE_BUFFER_SIZE};

  HmacFunc = GetSpdmHmacFunc (SpdmContext);
  HashFunc = GetSpdmHashFunc (SpdmContext);
  HashSize = GetSpdmHashSize (SpdmContext);
  ASSERT(HashSize == HmacDataSize);

  if ((SpdmContext->LocalContext.SpdmCertChainVarBuffer == NULL) || (SpdmContext->LocalContext.SpdmCertChainVarBufferSize == 0)) {
    return FALSE;
  }
  CertBuffer = (UINT8 *)SpdmContext->LocalContext.SpdmCertChainVarBuffer + sizeof(SPDM_CERT_CHAIN) + HashSize;
  CertBufferSize = SpdmContext->LocalContext.SpdmCertChainVarBufferSize - (sizeof(SPDM_CERT_CHAIN) + HashSize);
  HashFunc (CertBuffer, CertBufferSize, CertBufferHash);

  DEBUG((DEBUG_INFO, "MessageA Data :\n"));
  InternalDumpHex (GetManagedBuffer(&SpdmContext->Transcript.MessageA), GetManagedBufferSize(&SpdmContext->Transcript.MessageA));

  DEBUG((DEBUG_INFO, "THMessageCt Data :\n"));
  InternalDumpHex (CertBuffer, CertBufferSize);

  DEBUG((DEBUG_INFO, "MessageK Data :\n"));
  InternalDumpHex (GetManagedBuffer(&SessionInfo->SessionTranscript.MessageK), GetManagedBufferSize(&SessionInfo->SessionTranscript.MessageK));

  AppendManagedBuffer (&THCurr, GetManagedBuffer(&SpdmContext->Transcript.MessageA), GetManagedBufferSize(&SpdmContext->Transcript.MessageA));
  AppendManagedBuffer (&THCurr, CertBufferHash, HashSize);
  AppendManagedBuffer (&THCurr, GetManagedBuffer(&SessionInfo->SessionTranscript.MessageK), GetManagedBufferSize(&SessionInfo->SessionTranscript.MessageK));

  ASSERT(SessionInfo->HashSize != 0);
  HmacFunc (GetManagedBuffer(&THCurr), GetManagedBufferSize(&THCurr), SessionInfo->HandshakeSecret.ResponseHandshakeSecret, SessionInfo->HashSize, CalcHmacData);
  DEBUG((DEBUG_INFO, "THCurr Hmac - "));
  InternalDumpData (CalcHmacData, HashSize);
  DEBUG((DEBUG_INFO, "\n"));

  if (CompareMem (CalcHmacData, HmacData, HashSize) != 0) {
    DEBUG((DEBUG_INFO, "!!! VerifyKeyExchangeHmac - FAIL !!!\n"));
    return FALSE;
  }
  DEBUG((DEBUG_INFO, "!!! VerifyKeyExchangeHmac - PASS !!!\n"));

  return TRUE;
}

/**
  This function executes SPDM key change.
  
  @param[in]  SpdmContext            The SPDM context for the device.
  @param[out] DeviceSecurityState    The Device Security state associated with the device.
**/
RETURN_STATUS
SpdmSendReceiveKeyExchange (
  IN     SPDM_DEVICE_CONTEXT  *SpdmContext,
  IN     UINT8                MeasurementHashType,
  IN     UINT8                SlotNum,
     OUT UINT8                *HeartbeatPeriod,
     OUT UINT8                *SessionId,
     OUT VOID                 *MeasurementHash
  )
{
  BOOLEAN                                   Result;
  RETURN_STATUS                             Status;
  SPDM_KEY_EXCHANGE_REQUEST_MINE            SpdmRequest;
  UINTN                                     SpdmRequestSize;
  SPDM_KEY_EXCHANGE_RESPONSE_MAX            SpdmResponse;
  UINTN                                     SpdmResponseSize;
  UINT32                                    DHEKeySize;
  UINT32                                    HashSize;
  UINT32                                    SignatureSize;
  UINT32                                    HmacSize;
  UINT8                                     *Ptr;
  VOID                                      *MeasurementSummaryHash;
  UINT8                                     *Signature;
  UINT8                                     *VerifyData;
  VOID                                      *DHEContext;
  UINT8                                     FinalKey[MAX_DHE_KEY_SIZE];
  UINTN                                     FinalKeySize;
  SPDM_SESSION_INFO                         *SessionInfo;

  if ((SpdmContext->ConnectionInfo.Capability.Flags & SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_KEY_EX_CAP) == 0) {
    return RETURN_DEVICE_ERROR;
  }

  SpdmContext->ErrorState = SPDM_STATUS_ERROR_DEVICE_NO_CAPABILITIES;

  SpdmRequest.Header.SPDMVersion = SPDM_MESSAGE_VERSION_11;
  SpdmRequest.Header.RequestResponseCode = SPDM_KEY_EXCHANGE;
  SpdmRequest.Header.Param1 = MeasurementHashType;
  SpdmRequest.Header.Param2 = SlotNum;
  SpdmRequest.DHE_Named_Group = SpdmContext->ConnectionInfo.Algorithm.DHENamedGroup;
  GetRandomNumber (SPDM_RANDOM_DATA_SIZE, SpdmRequest.RandomData);
  DEBUG((DEBUG_INFO, "ClientRandomData (0x%x) - ", SPDM_RANDOM_DATA_SIZE));
  InternalDumpData (SpdmRequest.RandomData, SPDM_RANDOM_DATA_SIZE);
  DEBUG((DEBUG_INFO, "\n"));

  DHEKeySize = GetSpdmDHEKeySize (SpdmContext);
  GenerateDHESelfKey (SpdmContext, DHEKeySize, SpdmRequest.ExchangeData, &DHEContext);
  DEBUG((DEBUG_INFO, "ClientKey (0x%x):\n", DHEKeySize));
  InternalDumpHex (SpdmRequest.ExchangeData, DHEKeySize);
  SpdmRequestSize = sizeof(SPDM_KEY_EXCHANGE_REQUEST) + DHEKeySize;
  Status = SpdmSendRequest (SpdmContext, SpdmRequestSize, &SpdmRequest);
  if (RETURN_ERROR(Status)) {
    return RETURN_DEVICE_ERROR;
  }

  SpdmResponseSize = sizeof(SpdmResponse);
  ZeroMem (&SpdmResponse, sizeof(SpdmResponse));
  Status = SpdmReceiveResponse (SpdmContext, &SpdmResponseSize, &SpdmResponse);
  if (RETURN_ERROR(Status)) {
    return RETURN_DEVICE_ERROR;
  }
  if (SpdmResponseSize < sizeof(SPDM_KEY_EXCHANGE_RESPONSE)) {
    return RETURN_DEVICE_ERROR;
  }
  if (SpdmResponseSize > sizeof(SpdmResponse)) {
    return RETURN_DEVICE_ERROR;
  }
  if (SpdmResponse.Header.RequestResponseCode != SPDM_KEY_EXCHANGE_RSP) {
    return RETURN_DEVICE_ERROR;
  }

  if (HeartbeatPeriod != NULL) {
    *HeartbeatPeriod = SpdmResponse.Header.Param1;
  }
  *SessionId = SpdmResponse.Header.Param2;
  SessionInfo = SpdmAssignSessionId (SpdmContext, *SessionId);
  SessionInfo->UsePsk = FALSE;

  //
  // Cache session data
  //
  AppendManagedBuffer (&SessionInfo->SessionTranscript.MessageK, &SpdmRequest, SpdmRequestSize);
  // Need remove HMAC.
  SignatureSize = GetSpdmAsymSize (SpdmContext);
  HashSize = GetSpdmHashSize (SpdmContext);
  HmacSize = GetSpdmHashSize (SpdmContext);
  if (SpdmResponseSize != sizeof(SPDM_KEY_EXCHANGE_RESPONSE) +
                          DHEKeySize +
                          HashSize +
                          SignatureSize +
                          HmacSize) {
    SpdmFreeSessionId (SpdmContext, *SessionId);
    return RETURN_DEVICE_ERROR;
  }

  AppendManagedBuffer (&SessionInfo->SessionTranscript.MessageK, &SpdmResponse, SpdmResponseSize - SignatureSize - HmacSize);

  DEBUG((DEBUG_INFO, "ServerRandomData (0x%x) - ", SPDM_RANDOM_DATA_SIZE));
  InternalDumpData (SpdmResponse.RandomData, SPDM_RANDOM_DATA_SIZE);
  DEBUG((DEBUG_INFO, "\n"));

  DEBUG((DEBUG_INFO, "ServerKey (0x%x):\n", DHEKeySize));
  InternalDumpHex (SpdmResponse.ExchangeData, DHEKeySize);

  Ptr = SpdmResponse.ExchangeData;
  Ptr += DHEKeySize;
  
  MeasurementSummaryHash = Ptr;
  DEBUG((DEBUG_INFO, "MeasurementSummaryHash (0x%x) - ", HashSize));
  InternalDumpData (MeasurementSummaryHash, HashSize);
  DEBUG((DEBUG_INFO, "\n"));

  Ptr += HashSize;

  Signature = Ptr;
  DEBUG((DEBUG_INFO, "Signature (0x%x):\n", SignatureSize));
  InternalDumpHex (Signature, SignatureSize);
  Ptr += SignatureSize;  
  Result = SpdmRequesterVerifyKeyExchangeSignature (SpdmContext, SessionInfo, Signature, SignatureSize);
  if (!Result) {
    SpdmFreeSessionId (SpdmContext, *SessionId);
    SpdmContext->ErrorState = SPDM_STATUS_ERROR_KEY_EXCHANGE_FAILURE;
    return RETURN_SECURITY_VIOLATION;
  }

  AppendManagedBuffer (&SessionInfo->SessionTranscript.MessageK, (UINT8 *)&SpdmResponse + SpdmResponseSize - SignatureSize - HmacSize, SignatureSize);

  //
  // Fill data to calc Secret for HMAC verification
  //
  FinalKeySize = sizeof(FinalKey);
  ComputeDHEFinalKey (SpdmContext, DHEContext, DHEKeySize, SpdmResponse.ExchangeData, &FinalKeySize, FinalKey);
  DEBUG((DEBUG_INFO, "FinalKey (0x%x):\n", FinalKeySize));
  InternalDumpHex (FinalKey, FinalKeySize);

  ASSERT (FinalKeySize <= sizeof(SessionInfo->HandshakeSecret.DheSecret));
  SessionInfo->DheKeySize = FinalKeySize;
  CopyMem (SessionInfo->HandshakeSecret.DheSecret, FinalKey, FinalKeySize);

  Status = SpdmGenerateSessionHandshakeKey (SpdmContext, *SessionId, TRUE);
  if (RETURN_ERROR(Status)) {
    SpdmFreeSessionId (SpdmContext, *SessionId);
    SpdmContext->ErrorState = SPDM_STATUS_ERROR_KEY_EXCHANGE_FAILURE;
    return Status;
  }

  VerifyData = Ptr;
  DEBUG((DEBUG_INFO, "VerifyData (0x%x):\n", HmacSize));
  InternalDumpHex (VerifyData, HmacSize);
  Result = SpdmRequesterVerifyKeyExchangeHmac (SpdmContext, SessionInfo, VerifyData, HmacSize);
  if (!Result) {
    SpdmFreeSessionId (SpdmContext, *SessionId);
    SpdmContext->ErrorState = SPDM_STATUS_ERROR_KEY_EXCHANGE_FAILURE;
    return RETURN_SECURITY_VIOLATION;
  }

  if (MeasurementHash != NULL) {
    CopyMem (MeasurementHash, MeasurementSummaryHash, HashSize);
  }
  SessionInfo->MutAuthRequested = SpdmResponse.MutAuthRequested;

  SessionInfo->SessionState = SpdmStateHandshaking;
  SpdmContext->ErrorState = SPDM_STATUS_SUCCESS;
  
  return RETURN_SUCCESS;
}

