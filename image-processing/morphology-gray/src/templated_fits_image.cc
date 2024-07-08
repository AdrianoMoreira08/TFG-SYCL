/**
 * @brief Fits Image class that encapsulates CFITS mechanics.
 *
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */


#include "../include/templated_fits_image.h"

#include <stdexcept>
#include <iostream>

#include "../include/fits_utils.h"

FitsImage* NewFitsImage(std::string file_name,
                        OpeningMode mode,
                        int creation_bitpix) {
  if (file_name.empty()) {
    throw std::invalid_argument("Empty file name is not allowed.");
  }
  bool file_exists{FitsUtils::FileExists(file_name)};
  fitsfile* fits_file;
  int status{0};
  constexpr int kAmountOfAxis{2};
  long dimensions[kAmountOfAxis] = {0, 0};
  int bitpix;
  switch (mode) {
    case OpeningMode::OPEN: {
      if (!file_exists) {
        throw std::invalid_argument("The FITS file does not exist.");
      }
      fits_open_file(&fits_file, file_name.c_str(), READWRITE, &status);
      int real_amount_of_axis;
      fits_get_img_param(fits_file, kAmountOfAxis, &bitpix, &real_amount_of_axis,
        dimensions, &status);
      break;
    } case OpeningMode::CREATE: {
      if (file_exists) {
        throw std::invalid_argument("The FITS file already exists.");
      }
      fits_create_file(&fits_file, file_name.c_str(), &status);
      fits_create_img(fits_file, creation_bitpix, kAmountOfAxis, dimensions, &status);
      bitpix = creation_bitpix;
      break;
    } case OpeningMode::OVERWRITE: {
      file_name = "!" + file_name;
      fits_create_file(&fits_file, file_name.c_str(), &status);
      fits_create_img(fits_file, creation_bitpix, kAmountOfAxis, dimensions, &status);
      bitpix = creation_bitpix;
      break;
    } default: {
      throw std::invalid_argument("Invalid opening mode.");
    }
  }
  if (status != 0) {
    throw std::invalid_argument("Templated FITS image creation failed.");
  }
  FitsImage* fits_image;
  switch (bitpix) {
    case BYTE_IMG: {
      fits_image = new TemplatedFitsImage<unsigned char>(fits_file, mode);
      break;
    } case SHORT_IMG: {
      fits_image = new TemplatedFitsImage<short>(fits_file, mode);
      break;
    } case LONG_IMG: {
      fits_image = new TemplatedFitsImage<long>(fits_file, mode);
      break;
    } case LONGLONG_IMG: {
      fits_image = new TemplatedFitsImage<long long>(fits_file, mode);
      break;
    } case FLOAT_IMG: {
      fits_image = new TemplatedFitsImage<float>(fits_file, mode);
      break;
    } case DOUBLE_IMG: {
      fits_image = new TemplatedFitsImage<double>(fits_file, mode);
      break;
    } default: {
      throw std::invalid_argument("Image pixel size unsupported.");
      break;
    }
  }
  return fits_image;
}