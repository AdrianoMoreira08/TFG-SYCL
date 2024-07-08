/**
 * @brief Structuring Element class that encapsulates a binary structuring element.
 *
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */

#include "../include/templated_structuring_element.h"

#include <fitsio.h>

#include "../include/fits_utils.h"

StructuringElement* NewStructuringElement(const std::string& file_name,
                                          int data_type) {
  StructuringElement* sel;
  switch (data_type) {
    case TBYTE: {
      sel = new TemplatedStructuringElement<unsigned char>(file_name, data_type);
      break;
    } case TSHORT: {
      sel = new TemplatedStructuringElement<short>(file_name, data_type);
      break;
    } case TLONG: {
      sel = new TemplatedStructuringElement<long>(file_name, data_type);
      break;
    } case TLONGLONG: {
      sel = new TemplatedStructuringElement<long long>(file_name, data_type);
      break;
    } case TFLOAT: {
      sel = new TemplatedStructuringElement<float>(file_name, data_type);
      break;
    } case TDOUBLE: {
      sel = new TemplatedStructuringElement<double>(file_name, data_type);
      break;
    } default: {
      throw std::invalid_argument("Image data type unsupported.");
      break;
    }
  }
  return sel;
}