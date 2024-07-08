/**
 * @brief Fits Image class that encapsulates CFITS mechanics.
 *
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */


#include "../include/fits_image.h"

#include <fstream>
#include <bits/stdc++.h>

#include "../include/fits_utils.h"

FitsImage::FitsImage(fitsfile* fits_file, OpeningMode mode):
  fits_file_{fits_file}, status_{0} {
  switch (mode) {
    case OpeningMode::OPEN: {
      int real_amount_of_axis{0};
      fits_get_img_param(fits_file_, kAmountOfAxis, &bitpix_, &real_amount_of_axis,
        dimensions_, &status_);
      if (kAmountOfAxis != real_amount_of_axis) {
        throw std::runtime_error("The image is not two-dimensional.");
      }
      total_elements_ = dimensions_[0] * dimensions_[1];
      break;
    } case OpeningMode::CREATE: {
    } case OpeningMode::OVERWRITE: {
      dimensions_[0] = 0;
      dimensions_[1] = 0;
      total_elements_ = 0;
      break;
    } default: {
      throw std::invalid_argument("Invalid opening mode.");
    }
  }
  data_type_ = FitsUtils::GetDataType(bitpix_);
}

void FitsImage::CopyHeaderFrom(FitsImage& other_image) {
  fits_copy_header(other_image.fits_file_, fits_file_, &status_);
}

void FitsImage::WriteToFile(std::string file_name) {
  fitsfile* new_file;
  // Adds a '!' to the file name to overwrite it.
  file_name = std::string("!") + file_name;
  fits_create_file(&new_file, file_name.c_str(), &status_);
  fits_copy_header(fits_file_, new_file, &status_);
  WriteImageData(new_file);
  fits_close_file(new_file, &status_);
}

