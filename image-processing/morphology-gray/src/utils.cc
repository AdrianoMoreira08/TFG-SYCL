/**
 * @brief Various Uilities.
 *
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */

#include "../include/utils.h"

#include <fitsio.h>

#ifdef USE_SYCL
  #include "../include/erode_sycl.h"
#else
  #include "../include/erode.h"
#endif

#include "../include/fits_image.h"

Morphology* GetMorphologyOperation(std::string operation, int data_type) {
  if (operation.size() > 1) {
    throw std::invalid_argument("Morphology operation not supported.");
  }
  Morphology* operation_function;
  switch (operation[0]) {
    case 'e': {
      operation_function = NewErode(data_type);
      break;
    } default: {
      throw std::invalid_argument("Morphology operation not supported.");
      break;
    }
  }
  return operation_function;
}

PaddingType GetFillingType(std::string operation) {
  if (operation.size() > 1) {
    throw std::invalid_argument("Morphology operation not supported.");
  }
  PaddingType filling;
  switch (operation[0]) {
    case 'e': { // Erosion
      filling = PaddingType::MAX;
      break;
    } default: {
      throw std::invalid_argument("Morphology operation not supported.");
      break;
    }
  }
  return filling;
}
