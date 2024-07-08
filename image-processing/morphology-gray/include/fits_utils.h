/**
 * @brief Various utilities created for FITS related classes
 * 
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */

#pragma once

#include <string>

namespace FitsUtils {
  /**
   * @brief Checks if the given file exists.
   * @param file_name File to check.
   * @returns True if it exists, false otherwise.
   */
  bool FileExists(const std::string& file_name);

  /**
   * @brief Calculates the ceiling of the quotient obtained by dividing the
   *  operands.
   * @param dividend Dividend of the operation.
   * @param divisor Divisor of the operation.
   * @returns The ceiling of the quotient.
   */
  inline int DivisionCeiling(int dividend, int divisor) {
    return (dividend + divisor - 1) / divisor;
  }
  /**
   * @brief Transforms the bitpix into the data type equivalent.
   *  Uses CFITSIO enums.
   * @param bitpix Bitpix value to transform the value from.
   * @returns The data type equivalent.
   */
  int GetDataType(int bitpix);
}
