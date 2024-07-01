/**
 * @brief Fits Image class that encapsulates CFITS mechanics.
 *
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */


#include "../include/fits_image.h"

#include <fstream>
#include <bits/stdc++.h>


FitsImage::FitsImage(std::string file_name, int mode):
    file_name_{file_name}, status_{0}, image_data_{nullptr} {
  if (file_name.empty()) {
    throw std::invalid_argument("Empty file name is not allowed.");
  }
  bool file_exists{FileExists()};
  switch (mode) {
    case OpeningMode::OPEN: {
      if (!file_exists) {
        throw std::invalid_argument("The FITS file does not exist.");
      }
      fits_open_file(&fits_file_, file_name.c_str(), READWRITE, &status_);
      int data_type, real_amount_of_axis;
      fits_get_img_param(fits_file_, kAmountOfAxis, &data_type, &real_amount_of_axis,
        dimensions_, &status_);
      if (kAmountOfAxis != real_amount_of_axis) {
        throw std::runtime_error("The image is not two-dimensional.");
      }
      total_elements_ = dimensions_[0] * dimensions_[1];
      break;
    } case OpeningMode::CREATE: {
      if (file_exists) {
        throw std::invalid_argument("The FITS file already exists.");
      }
      fits_create_file(&fits_file_, file_name.c_str(), &status_);
      dimensions_[0] = 0;
      dimensions_[1] = 0;
      total_elements_ = 0;
      break;
    } case OpeningMode::OVERWRITE: {
      file_name = "!" + file_name;
      fits_create_file(&fits_file_, file_name.c_str(), &status_);
      dimensions_[0] = 0;
      dimensions_[1] = 0;
      total_elements_ = 0;
      break;
    } default: {
      throw std::invalid_argument("Invalid opening mode.");
    }
  }
}

FitsImage::~FitsImage() {
  fits_close_file(fits_file_, &status_);
  delete[] image_data_;
}

void FitsImage::CopyHeaderFrom(FitsImage& other_image) {
  fits_copy_header(other_image.fits_file_, fits_file_, &status_);
}

std::stringstream FitsImage::GetMorphologyData() {
  std::stringstream data;
  const char kSmallestValue{'0'};
  const char kBiggestValue{'1'};
  data << dimensions_[1] << " " << dimensions_[0] << " "
       << kSmallestValue << " " << kBiggestValue << std::endl;
  for (long row = dimensions_[1] - 1; row >= 0; --row) {
    for (long column = 0; column < dimensions_[0]; ++column) {
      data << static_cast<unsigned>(image_data_[row * dimensions_[0] + column]) << " ";
    }
    data << std::endl;
  }
  return data;
}

void FitsImage::SetMorphologyData(std::stringstream& data) {
  std::string auxiliar;
  data >> auxiliar;
  dimensions_[1] = stol(auxiliar);
  data >> auxiliar;
  dimensions_[0] = stol(auxiliar);
  data >> auxiliar; // Minimum value
  data >> auxiliar; // Maximum value
  total_elements_ = dimensions_[0] * dimensions_[1];
  delete[] image_data_;
  image_data_ = new unsigned char[total_elements_];
  fits_resize_img(fits_file_, BYTE_IMG, kAmountOfAxis, dimensions_, &status_);
  for (long row = dimensions_[1] - 1; row >= 0; --row) {
    for (long column = 0; column < dimensions_[0]; ++column) {
      data >> auxiliar;
      image_data_[row * dimensions_[0] + column] =
        static_cast<unsigned char>(std::stoul(auxiliar));
    }
  }
}

void FitsImage::Load(double threshold) {
  delete[] image_data_;
  double* data = new double[total_elements_];
  fits_read_img(fits_file_, TDOUBLE, 1, total_elements_, nullptr, data,
    nullptr, &status_);
  image_data_ = new unsigned char[total_elements_];
  for (long index = 0; index < total_elements_; ++index) {
    image_data_[index] = data[index] > threshold ? 1 : 0;
  }
  delete[] data;
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

double FitsImage::CalculateMedian() {
  double* data = new double[total_elements_];
  fits_read_img(fits_file_, TDOUBLE, 1, total_elements_, nullptr, data,
    nullptr, &status_);
  std::sort(data, data + total_elements_);
  double median;
  if (total_elements_ % 2 == 0) {
    median = (data[total_elements_ / 2 - 1] + data[total_elements_ / 2]) / 2;
  } else {
    median = data[total_elements_ / 2];
  }
  delete[] data;
  return median;
}

double FitsImage::CalculateMean() {
  double* data = new double[total_elements_];
  fits_read_img(fits_file_, TDOUBLE, 1, total_elements_, nullptr, data,
    nullptr, &status_);
  double sum{0};
  for (long i = 0; i < total_elements_; ++i) {
    sum += data[i];
  }
  delete[] data;
  return sum / total_elements_;
}

// Private methods below

bool FitsImage::FileExists() {
  std::ifstream file(file_name_);
  bool exists{file.good()};
  file.close();
  return exists;
}

void FitsImage::WriteImageData(fitsfile* fits_file) {
  fits_resize_img(fits_file, BYTE_IMG, kAmountOfAxis, dimensions_, &status_);
  fits_write_img(fits_file, TBYTE, 1, total_elements_, image_data_, &status_);
}
