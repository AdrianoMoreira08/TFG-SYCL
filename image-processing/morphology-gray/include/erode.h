/**
 * @brief Erode class which implements the morphological erosion operation.
 * 
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */

#pragma once

#include "morphology.h"

#include <limits>
#include <algorithm>

#include "templated_fits_image.h"
#include "templated_structuring_element.h"

/**
 * @brief Performs a morphological erosion operation.
 */
template<typename T>
class Erode: public Morphology {
 public:
  Erode() {}
  ~Erode() override {}
  /**
   * @brief Performs a morphological erosion on the image with the structuring
   *  element.
   * @param image FITS image to transform.
   * @param sel Structuring element for the operation.
   */
  void Operate(FitsImage* fits_image, StructuringElement* operation_sel) override {
    TemplatedFitsImage<T>& image =
      *dynamic_cast<TemplatedFitsImage<T>*>(fits_image);
    TemplatedStructuringElement<T>& sel =
      *dynamic_cast<TemplatedStructuringElement<T>*>(operation_sel);
    T* image_data = image.GetData();
    T* image_data_copy = new T[image.PaddedTotalElements()];
    std::copy(image_data, image_data + image.PaddedTotalElements(), image_data_copy);
    T* sel_data = sel.GetData();
    long origin = image.Padding() * image.PaddedColumns() + image.Padding();
    for (long row{0}; row < image.Rows(); ++row) {
      long image_row = origin + row * image.PaddedColumns();
      for (long column{0}; column < image.Columns(); ++column) {
        long pixel_index = image_row + column;
        long local_origin = pixel_index -
                            sel.CenterRow() * image.PaddedColumns() -
                            sel.CenterColumn();
        T minimum = std::numeric_limits<T>::max();
        for (long local_row{0}; local_row < sel.Rows(); ++local_row) {
          long local_image_row = local_origin + local_row * image.PaddedColumns();
          long sel_row = local_row * sel.Columns();
          for (long local_column{0}; local_column < sel.Columns(); ++local_column) {
            long local_pixel = local_image_row + local_column;
            if (sel_data[sel_row + local_column] == 1 && 
                image_data_copy[local_pixel] <= minimum) {
              minimum = image_data_copy[local_pixel];
            }
          }
        }
        image_data[pixel_index] = minimum;
      }
    }
    delete[] image_data_copy;
  }
};

/**
 * @brief Creates a Erode instance using dynamic memory. Is the user's
 *  responsibility to free the memory.
 * @param data_type The type of data it operates with.
 *  Uses CFITSIO data type enum.
 * @returns An Erode object as its base class poiner.
 */
Morphology* NewErode(int data_type);
