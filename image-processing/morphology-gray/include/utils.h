/**
 * @brief Various Uilities.
 *
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */

#pragma once

#include <string>

class Morphology;
enum class PaddingType;

namespace Text {
  const std::string kUsage{
    "Usage: ./morphology <fits_file> <se_file> <output_file> <operation>\n"
    "Type './morphology -h' for help."
  };
  const std::string kHelp{
    "Usage: ./morphology <fits_file> <se_file> <output_file> <operation>\n"
    "Performs morphological operations on a binary image using a structuring element.\n"
    "Arguments:\n"
    "  <fits_file>      - The input FITS file.\n"
    "  <se_file>        - The structuring element file.\n"
    "  <output_file>    - The output FITS file to be created.\n"
    "  <operation>      - The morphological operation to perform (single letter).\n"
    "      Options: (e)rosion, (d)ilation."
  };
  const std::string kInvalidOperation{
    "Invalid operation. Use one of the following: (e)rosion, (d)ilation, (o)pening, (c)losing."
  };
  const std::string kInvalidThreshold{
    "Invalid threshold type. Use one of the following: (m)edian, (a)verage."
  };
}

inline double NanosecondsToSeconds(int64_t time) { return time * 1e-9; }

/**
 * @brief Creates the corresponding Morphology operation.
 *  Throws an exception if the operation does not exist.
 * @param operation User's input for the operation.
 * @returns The morphology operation as its base class pointer.
 */
Morphology* GetMorphologyOperation(std::string operation, int data_type);

/**
 * @brief Returns the filling type depending on the morphology operation.
 * @param operation User's input for the operation.
 * @returns The filling value type.
 */
PaddingType GetFillingType(std::string operation);

