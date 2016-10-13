/**
 * @file
 * Defines message handling functions.
 */

/*
Purpose: (Contains functions that can be used for error handling within the Chup
          Comm System.)
*/

#ifndef MESSAGE_HANDLER_HH
#define MESSAGE_HANDLER_HH

// System includes
#include <cstdarg>

/**
 * Contains free functions pertaining to handling messages of
 * various levels of severity.
 */
namespace MessageHandler {

  /**
   * Terminate due to some fatal error.
   * Note that this is a variable arguments function ala printf.
   * The format parameter specifies the format. Subsequent arguments
   * specify the content.
   * @param file      The source file in which the error occurred,
   *                  typically \_\_FILE\_\_
   * @param line      The line number at which the error occurred,
   *                  typically \_\_LINE\_\_
   * @param msg_code  Message ID
   * @param format    Message format
   */
  void fail (
     const char * file,
     unsigned int line,
     const char * msg_code,
     const char * format,
     ...);

  /**
   * Indicate that a serious but non-fatal error has occurred.
   * Note that this is a variable arguments function ala printf.
   * The format parameter specifies the format. Subsequent arguments
   * specify the content.
   * @param file      The source file in which the error occurred,
   *                  typically \_\_FILE\_\_
   * @param line      The line number at which the error occurred,
   *                  typically \_\_LINE\_\_
   * @param msg_code  Message ID
   * @param format    Message format
   */
  void error (
     const char * file,
     unsigned int line,
     const char * msg_code,
     const char * format,
     ...);

  /**
   * Indicate that a lesser problem has occurred.
   * Note that this is a variable arguments function ala printf.
   * The format parameter specifies the format. Subsequent arguments
   * specify the content.
   * @param file      The source file in which the error occurred,
   *                  typically \_\_FILE\_\_
   * @param line      The line number at which the error occurred,
   *                  typically \_\_LINE\_\_
   * @param msg_code  Message ID
   * @param format    Message format
   */
  void warn (
     const char * file,
     unsigned int line,
     const char * msg_code,
     const char * format,
     ...);
}

#endif