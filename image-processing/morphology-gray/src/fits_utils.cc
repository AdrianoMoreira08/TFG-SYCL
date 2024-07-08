/**
 * @brief Various utilities created for FITS related classes
 * 
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */

#include "../include/fits_utils.h"

#include <fitsio.h>
#include <fstream>
#include <string>

bool FitsUtils::FileExists(const std::string& file_name) {
  std::ifstream file(file_name);
  bool exists{file.good()};
  file.close();
  return exists;
}

int FitsUtils::GetDataType(int bitpix) {
  switch (bitpix) {
    case BYTE_IMG: {
      return TBYTE;
      break;
    } case SHORT_IMG: {
      return TSHORT;
      break;
    } case LONG_IMG: {
      return TLONG;
      break;
    } case LONGLONG_IMG: {
      return TLONGLONG;
      break;
    } case FLOAT_IMG: {
      return TFLOAT;
      break;
    } case DOUBLE_IMG: {
      return TDOUBLE;
      break;
    } default: {
      throw std::invalid_argument("Image pixel size (bitpix) unsupported.");
      break;
    }
  }
}