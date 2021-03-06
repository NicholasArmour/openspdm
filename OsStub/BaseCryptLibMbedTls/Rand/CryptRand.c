/** @file
  Pseudorandom Number Generator Wrapper Implementation over OpenSSL.

Copyright (c) 2010 - 2013, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "InternalCryptLib.h"

int rand ();

/**
  Sets up the seed value for the pseudorandom number generator.

  This function sets up the seed value for the pseudorandom number generator.
  If Seed is not NULL, then the seed passed in is used.
  If Seed is NULL, then default seed is used.

  @param[in]  Seed      Pointer to seed value.
                        If NULL, default seed is used.
  @param[in]  SeedSize  Size of seed value.
                        If Seed is NULL, this parameter is ignored.

  @retval TRUE   Pseudorandom number generator has enough entropy for random generation.
  @retval FALSE  Pseudorandom number generator does not have enough entropy for random generation.

**/
BOOLEAN
EFIAPI
RandomSeed (
  IN  CONST  UINT8  *Seed  OPTIONAL,
  IN  UINTN         SeedSize
  )
{
  // TBD
  return TRUE;
}

/**
  Generates a pseudorandom byte stream of the specified size.

  If Output is NULL, then return FALSE.

  @param[out]  Output  Pointer to buffer to receive random value.
  @param[in]   Size    Size of random bytes to generate.

  @retval TRUE   Pseudorandom byte stream generated successfully.
  @retval FALSE  Pseudorandom number generator fails to generate due to lack of entropy.

**/
BOOLEAN
EFIAPI
RandomBytes (
  OUT  UINT8  *Output,
  IN   UINTN  Size
  )
{
  UINTN Count = Size/4;
  UINTN Final = Size%4;
  UINTN Index = 0;
  UINT32 Data32;

  for (Index = 0; Index < Count; Index++) {
    *(UINT32 *)(Output + Index * 4) = rand();
  }
  if (Final != 0) {
    Data32 = rand();
    CopyMem (Output + Count * 4, &Data32, Final);
  }
  return TRUE;
}

int myrand( void *rng_state, unsigned char *output, size_t len )
{
  RandomBytes (output, len);

  return 0;
}
