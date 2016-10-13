/**
 * @file
 * Defines message handling functions.
 */

/*
Purpose: (Contains functions that can be used for error handling within the Chupacabra
          comm system.)
*/

// System includes
#include <cstdarg>
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "MessageHandler.h"


namespace MessageHandler {

    /**
     * Generate a string that identifies the message source.
     * @param file      The source file in which the error occurred
     * @param line      The line number at which the error occurred
     * @return Generated string.
     */
    static std::string message_pos (
       const char * file,
       unsigned int line)
    {
       std::stringstream ssbuf;
       ssbuf << file << " line " << line;
       return ssbuf.str();
    }


    /**
     * Generate the message body.
     * @param format    printf-style format string
     * @param args      Variable arguments, as a va_list
     * @return Generated message
     */
    static std::string va_message (
       const char * format,
       va_list args)
    {
       char buffer[2048];
       std::vsnprintf (buffer, sizeof(buffer), format, args);
       return buffer;
    }


    /**
     * Generate the message.
     * @param level     Severity level
     * @param prefix    Message prefix
     * @param file      The source file in which the error occurred
     * @param line      The line number at which the error occurred
     * @param msg_code  Message code
     * @param message   Message body
     */
    static void generate_message (
       int level,
       const char * prefix,
       const char * file,
       unsigned int line,
       const char * msg_code,
       const std::string & message)
    {
       std::string where = message_pos (file, line);

       std::cerr << prefix << " " << msg_code
                 << " at " << where << ":\n"
                 << message << "\n";
    }


    void fail (
       const char * file,
       unsigned int line,
       const char * id,
       const char * format,
       ...)
    {
       std::string message_body;
       va_list args;
       va_start (args, format);
       message_body = va_message (format, args);
       va_end (args);

       std::string where = message_pos (file, line);
       std::cerr << "Fatal error " << id
                 << " at " << where << ":\n"
                 << message_body << "\n";
       throw std::runtime_error("Fatal error ");
    }

    void error (
       const char * file,
       unsigned int line,
       const char * msg_code,
       const char * format,
       ...)
    {
       std::string message_body;
       va_list args;
       va_start (args, format);
       message_body = va_message (format, args);
       va_end (args);

       generate_message (
          0, "Error", file, line, msg_code, message_body);
    }


    void warn (
       const char * file,
       unsigned int line,
       const char * msg_code,
       const char * format,
       ...)
    {
       std::string message_body;
       va_list args;
       va_start (args, format);
       message_body = va_message (format, args);
       va_end (args);

       generate_message (
          0, "Warning", file, line, msg_code, message_body);
    }

}
