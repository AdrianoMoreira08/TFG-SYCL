/**
 * @brief Fits Image class that encapsulates CFITS mechanics.
 *
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */

#pragma once

#include <string>
#include <iostream>
#include <fitsio.h>
#include <algorithm>
#include <limits>

#include "fits_image.h"

/**
 * @brief Manages FITS images
 * - Asumes there is only one HDU and the data image is two-dimensional.
 * - Any changes on the image are only applied to the internal representation,
 *   the original file is not modified unless explicitly indicated using
 *   WriteToOriginalFile().
*/
template<typename T>
class TemplatedFitsImage: public FitsImage {
 public:
  TemplatedFitsImage(fitsfile* fits_file, OpeningMode mode):
      FitsImage{fits_file, mode}, image_data_{nullptr} {}
  ~TemplatedFitsImage() override { delete[] image_data_; }
  /**
   * @brief Copies the header information of another FitsImage into this one.
   * @param other_image The FitsImage to copy the header from.
  */
  void CopyHeaderFrom(TemplatedFitsImage& other_image) {
    fits_copy_header(other_image.fits_file_, fits_file_, &status_);
  }
  // Gives a pointer to the actual data of the image
  inline T* GetData() { return image_data_; }
  /**
   * @brief Reads the image from the original FITS file to the internal array.
   *  Overwrites the internal image.
   * @param padding Padding amount around the image.
   * @param padding_type Type of the padding value.
   * @param filling Padding value if padding_type is CUSTOM, ignored otherwise.
   */
  void Load(long padding = 0,
            PaddingType padding_type = PaddingType::CUSTOM,
            double filling = 0) override {
    delete[] image_data_;
    padding_ = padding;
    const long twice_padding{2 * padding};
    padded_total_elements_ = 1;
    for (unsigned dimension{0}; dimension < kAmountOfAxis; ++dimension) {
      padded_dimensions_[dimension] = twice_padding + dimensions_[dimension];
      padded_total_elements_ *= padded_dimensions_[dimension];
    }
    image_data_ = new T[padded_total_elements_];
    T data_filling = GetFilling(padding_type, filling);
    std::fill(image_data_, image_data_ + padded_total_elements_, data_filling);
    long first_element{1};
    T* image_data_pointer{image_data_ + padding_ * padded_dimensions_[0] + padding_};
    T* original_row = new T[dimensions_[0]];
    for (int row{0};
        row < dimensions_[1];
        image_data_pointer += padded_dimensions_[0],
        first_element += dimensions_[0],
        ++row) {
      fits_read_img(fits_file_, data_type_, first_element, dimensions_[0], nullptr,
                    original_row, nullptr, &status_);
      for (long index{0}; index < dimensions_[0]; ++index) {
        image_data_pointer[index] = original_row[index];
      }
    }
  }
  // Calculates the median value of the original image.
  double CalculateMedian() override {
    T* data = new T[total_elements_];
    fits_read_img(fits_file_, data_type_, 1, total_elements_, nullptr, data,
      nullptr, &status_);
    std::sort(data, data + total_elements_);
    double median;
    if (total_elements_ % 2 == 0) {
      median = (data[total_elements_ / 2 - 1] + data[total_elements_ / 2]) / 2.0;
    } else {
      median = data[total_elements_ / 2];
    }
    delete[] data;
    return median;
  }
  // Calculates the mean value of the original image.
  double CalculateMean() override {
    T* data = new T[total_elements_];
    fits_read_img(fits_file_, data_type_, 1, total_elements_, nullptr, data,
      nullptr, &status_);
    double sum{0};
    for (long i = 0; i < total_elements_; ++i) {
      sum += data[i];
    }
    delete[] data;
    return sum / static_cast<double>(total_elements_);
  }
 private:
  /**
   * @brief Writes only the image data into the given FITS file.
   * @param fits_file FITS file pointer.
   */
  void WriteImageData(fitsfile* fits_file) override {
    long first_element{1};
    T* image_data_pointer{image_data_ + padding_ * padded_dimensions_[0] + padding_};
    for (int row{0};
        row < dimensions_[1];
        image_data_pointer += padded_dimensions_[0],
        first_element += dimensions_[0],
        ++row) {
      fits_write_img(fits_file, data_type_, first_element, dimensions_[0], image_data_pointer, &status_);
    }
  }
  /**
   * @brief Calculates the padding value.
   * @param padding_type Type of the padding value.
   * @param filling Padding value if padding_type is CUSTOM.
   * @returns The padding value.
   */
  T GetFilling(PaddingType padding_type, double filling) {
    T padding_value;
    switch (padding_type) {
      case PaddingType::MAX: {
        padding_value = std::numeric_limits<T>::max();
        break;
      } case PaddingType::MIN: {
        padding_value = std::numeric_limits<T>::min();
        if (padding_value > 0) {
          padding_value = static_cast<T>(-std::numeric_limits<T>::max());
        }
        break;
      } case PaddingType::CUSTOM: {
        padding_value = static_cast<T>(filling);
        break;
      } default: {
        throw std::invalid_argument("Unknown padding type.");
        break;
      }
    }
    return padding_value;
  }

  T* image_data_;
};

/**
 * @brief Creates a FITS image instance from a FITS file using dynamic memory.
 *  Is the user's responsibility to free the memory.
 *  Guarantees that the created object has an actual file associated.
 * @param file_name The name of the FITS file.
 * @param mode The mode to open the FITS file (use the OpeningMode enum).
 *  Default mode is OpeningMode::OPEN.
 * @param creation_type The type of the FITS image to create if the mode is
 *  OpeningMode::CREATE, ignored otherwise. Default is TFLOAT.
 * @returns A TemplatedFitsImage object as its base class poiner.
 */
FitsImage* NewFitsImage(std::string file_name,
                        OpeningMode mode = OpeningMode::OPEN,
                        int creation_bitpix = FLOAT_IMG);

