/**
 * @brief Various Uilities.
 *
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */

#pragma once

#include <string>
#include <map>
#include <functional>

namespace Text {
  const std::string kUsage{
    "Usage: ./morphology <fits_file> <se_file> <output_file> <operation> [threshold_type]\n"
    "Type './morphology -h' for help."
  };
  const std::string kHelp{
    "Usage: ./morphology <fits_file> <se_file> <output_file> <operation> [threshold_type]\n"
    "Performs morphological operations on a binary image using a structuring element.\n"
    "Arguments:\n"
    "  <fits_file>      - The input FITS file.\n"
    "  <se_file>        - The structuring element file.\n"
    "  <output_file>    - The output FITS file to be created.\n"
    "  <operation>      - The morphological operation to perform (single letter).\n"
    "      Options: (e)rosion, (d)ilation, (o)pening, (c)losing.\n"
    "  [threshold_type]  - The threshold to convert the data to binary (optional).\n"
    "      Options: (m)edian, (a)verage. Default is median."
  };
  const std::string kInvalidOperation{
    "Invalid operation. Use one of the following: (e)rosion, (d)ilation, (o)pening, (c)losing."
  };
  const std::string kInvalidThreshold{
    "Invalid threshold type. Use one of the following: (m)edian, (a)verage."
  };
}
