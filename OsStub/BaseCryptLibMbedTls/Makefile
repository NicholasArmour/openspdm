## @file
#  SPDM library.
#
#  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

#
# Platform Macro Definition
#

!INCLUDE $(WORKSPACE)\MakeFile.Flags

#
# Module Macro Definition
#
MODULE_NAME = BaseCryptLibMbedTls

#
# Build Directory Macro Definition
#
BUILD_DIR = $(WORKSPACE)\Build
BIN_DIR = $(BUILD_DIR)\$(TARGET)_$(TOOLCHAIN)\$(ARCH)
OUTPUT_DIR = $(BIN_DIR)\OsStub\$(MODULE_NAME)

SOURCE_DIR = $(WORKSPACE)\OsStub\$(MODULE_NAME)

CC_FLAGS = $(CC_FLAGS) /wd4090 /wd4018

#
# Build Macro
#
SOURCE_FILES =  \
	$(SOURCE_DIR)\Cipher\CryptAeadAesCcm.c \
	$(SOURCE_DIR)\Cipher\CryptAeadAesGcm.c \
	$(SOURCE_DIR)\Cipher\CryptAeadChaCha20Poly1305.c \
	$(SOURCE_DIR)\Hash\CryptSha256.c \
	$(SOURCE_DIR)\Hash\CryptSha512.c \
	$(SOURCE_DIR)\Hmac\CryptHmacSha256.c \
	$(SOURCE_DIR)\Kdf\CryptHkdf.c \
	$(SOURCE_DIR)\Mac\CryptCmacAes.c \
	$(SOURCE_DIR)\Mac\CryptGmacAes.c \
	$(SOURCE_DIR)\Pem\CryptPem.c \
	$(SOURCE_DIR)\Pk\CryptEc.c \
	$(SOURCE_DIR)\Pk\CryptDh.c \
	$(SOURCE_DIR)\Pk\CryptRsaBasic.c \
	$(SOURCE_DIR)\Pk\CryptRsaExt.c \
	$(SOURCE_DIR)\Pk\CryptX509.c \
	$(SOURCE_DIR)\Rand\CryptRand.c \
	$(SOURCE_DIR)\SysCall\BaseMemAllocation.c \
	$(SOURCE_DIR)\SysCall\CrtWrapperHost.c \
	$(SOURCE_DIR)\SysCall\TimerWrapperHost.c \


OBJECT_FILES =  \
    $(OUTPUT_DIR)\CryptAeadAesCcm.obj \
    $(OUTPUT_DIR)\CryptAeadAesGcm.obj \
    $(OUTPUT_DIR)\CryptAeadChaCha20Poly1305.obj \
    $(OUTPUT_DIR)\CryptSha256.obj \
    $(OUTPUT_DIR)\CryptSha512.obj \
    $(OUTPUT_DIR)\CryptHmacSha256.obj \
    $(OUTPUT_DIR)\CryptHkdf.obj \
    $(OUTPUT_DIR)\CryptCmacAes.obj \
    $(OUTPUT_DIR)\CryptGmacAes.obj \
    $(OUTPUT_DIR)\CryptPem.obj \
    $(OUTPUT_DIR)\CryptDh.obj \
    $(OUTPUT_DIR)\CryptEc.obj \
    $(OUTPUT_DIR)\CryptRsaBasic.obj \
    $(OUTPUT_DIR)\CryptRsaExt.obj \
    $(OUTPUT_DIR)\CryptX509.obj \
    $(OUTPUT_DIR)\CryptRand.obj \
    $(OUTPUT_DIR)\BaseMemAllocation.obj \
    $(OUTPUT_DIR)\CrtWrapperHost.obj \
    $(OUTPUT_DIR)\TimerWrapperHost.obj \

INC =  \
    /I$(SOURCE_DIR)\
    /I$(WORKSPACE)\Include \
    /I$(WORKSPACE)\Include\Hal \
    /I$(WORKSPACE)\Include\Hal\$(ARCH) \
    /I$(WORKSPACE)\OsStub\Include \
    /I$(WORKSPACE)\OsStub\MbedTlsLib\mbedtls\include

#
# Overridable Target Macro Definitions
#
INIT_TARGET = init
CODA_TARGET = $(OUTPUT_DIR)\$(MODULE_NAME).lib

#
# Default target, which will build dependent libraries in addition to source files
#

all: mbuild

#
# ModuleTarget
#

mbuild: $(INIT_TARGET) $(CODA_TARGET)

#
# Initialization target: print build information and create necessary directories
#
init:
	-@if not exist $(OUTPUT_DIR) $(MD) $(OUTPUT_DIR)

#
# Individual Object Build Targets
#
$(OBJECT_FILES):  $(SOURCE_FILES)
	"$(CC)" /Fo$(OUTPUT_DIR)\.\ $(CC_FLAGS) $(INC) $(SOURCE_FILES)

$(OUTPUT_DIR)\$(MODULE_NAME).lib : $(OBJECT_FILES)
	"$(SLINK)" $(SLINK_FLAGS) $(OBJECT_FILES) /OUT:$@

#
# clean all intermediate files
#
clean:
	-@if exist $(OUTPUT_DIR) $(RD) $(OUTPUT_DIR)
	$(RM) *.pdb *.idb > NUL 2>&1


