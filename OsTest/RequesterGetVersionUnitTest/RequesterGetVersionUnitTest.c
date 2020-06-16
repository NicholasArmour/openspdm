#include "RequesterGetVersionUnitTest.h"



char __VERIFIER_nondet_char(void){

    return  (char)0x2c;
}

RETURN_STATUS
EFIAPI
SpdmClientSendRequest (
  IN     SPDM_IO_PROTOCOL        *This,
  IN     UINTN                   RequestSize,
  IN     VOID                    *Request,
  IN     UINT64                  Timeout
  )
  {
      return 0;
  }





#pragma pack(1)
typedef struct {
  SPDM_MESSAGE_HEADER  Header;
  UINT8                Reserved;
  UINT8                VersionNumberEntryCount;
  SPDM_VERSION_NUMBER  VersionNumberEntry[MAX_SPDM_VERSION_COUNT];
} SPDM_VERSION_RESPONSE_MAX;
#pragma pack()



RETURN_STATUS
EFIAPI
SpdmClientReceiveResponse (
  IN     SPDM_IO_PROTOCOL        *This,
  IN OUT UINTN                   *ResponseSize,
  IN OUT VOID                    *Response,
  IN     UINT64                  Timeout
  )
  {
    SPDM_VERSION_RESPONSE_MAX *SpdmResponse = (SPDM_VERSION_RESPONSE_MAX*)Response;
    *ResponseSize = sizeof(SPDM_VERSION_RESPONSE_MAX);
    ZeroMem(SpdmResponse, *ResponseSize);

    SpdmResponse->Header.SPDMVersion = SPDM_MESSAGE_VERSION_10;
    SpdmResponse->Header.RequestResponseCode = SPDM_VERSION;
    SpdmResponse->Header.Param1 = 0;
    SpdmResponse->Header.Param2 = 0;
    SpdmResponse->VersionNumberEntryCount = __VERIFIER_nondet_char();
    SpdmResponse->VersionNumberEntry[0].MajorVersion = 1;
    SpdmResponse->VersionNumberEntry[0].MinorVersion = 0;
    SpdmResponse->VersionNumberEntry[1].MajorVersion = 1;
    SpdmResponse->VersionNumberEntry[1].MinorVersion = 1;


    return 0;
  }


SPDM_IO_PROTOCOL mSpdmProtocol = {
    SpdmClientSendRequest,
    SpdmClientReceiveResponse,
    NULL,
    NULL,
    NULL
};
int main()
{

    SPDM_DEVICE_CONTEXT *SpdmContext;
    UINT8 VersionCount;
    SPDM_VERSION_NUMBER VersionNumberEntries[2];
    
    // Init SpdmContext
    SpdmContext = malloc(SpdmGetContextSize());
    SpdmInitContext(SpdmContext);
    SpdmContext->SpdmIo = &mSpdmProtocol;


    VersionCount = MAX_SPDM_VERSION_COUNT;
    SpdmGetVersion(SpdmContext, &VersionCount, &VersionNumberEntries);
    return 0;
}