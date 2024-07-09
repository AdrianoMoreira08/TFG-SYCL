/**
 * @brief Erode class which implements the morphological erosion operation.
 *  
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */

#ifdef USE_SYCL
  #include "../include/erode_sycl.h"
#else
  #include "../include/erode.h"
#endif

Morphology* NewErode(int data_type) {
  Morphology* operation;
  switch (data_type) {
    case TBYTE: {
      operation = new Erode<unsigned char>();
      break;
    } case TSHORT: {
      operation = new Erode<short>();
      break;
    } case TLONG: {
      operation = new Erode<long>();
      break;
    } case TLONGLONG: {
      operation = new Erode<long long>();
      break;
    } case TFLOAT: {
      operation = new Erode<float>();
      break;
    } case TDOUBLE: {
      operation = new Erode<double>();
      break;
    } default: {
      throw std::invalid_argument("Image pixel size unsupported.");
      break;
    }
  }
  return operation;
}
