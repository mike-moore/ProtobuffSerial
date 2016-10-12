///////////////////////////////////////////////////////////////
///  This file defines a class that is used for performing
///  32 bit CRC checksums.
///  
///  Call with CommCrc32::crc32(data, size, 0);
///
/// @author
///         $Author: Mike Moore $
///
/// Contact: mike.moore@so.engineering
///
/// Created on: Saturday June 18 2016
///////////////////////////////////////////////////////////////
#ifndef COMM_CRC_32_HH
#define COMM_CRC_32_HH

#include <stdint.h>
#include <stdlib.h>

class CommCrc32 {
  public:
    //////////////////////////////////////////////////////////
    /// Look-up tables for Polynomial = 0xEDB88320
    //////////////////////////////////////////////////////////
    static uint32_t crc32Lookup[16];
    //////////////////////////////////////////////////////////
    /// @brief Compute CRC32 (standard algorithm)
    //////////////////////////////////////////////////////////
    static uint32_t crc32(const void* data, size_t length, uint32_t previousCrc32);
};

#endif  // COMM_CRC_32_HH
