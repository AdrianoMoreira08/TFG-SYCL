/**
 * @brief Fits Image class that encapsulates CFITS mechanics.
 *
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */

#pragma once

#include <string>
#include <fitsio.h>

#include "morphology.h"

enum class OpeningMode {
  OPEN,
  OVERWRITE,
  CREATE
};

enum class PaddingType {
  MAX,
  MIN,
  CUSTOM
};

class StructuringElement;

/**
 * @brief Manages FITS images
 * - Asumes there is only one HDU and the data image is two-dimensional.
 * - Any changes on the image are only applied to the internal representation,
 *   the original file is not modified unless explicitly indicated using
 *   WriteToOriginalFile().
*/
class FitsImage {
 public:
  /**
   * @brief Opens a FITS file, creates one if it does not exist.
   * Empty file name is not allowed.
   * @param fits_file Pointer to the FITS file object.
   * @param mode The mode to open the FITS file (use the OpeningMode enum).
   * Default mode is OpeningMode::OPEN.
  */
  FitsImage(fitsfile* fits_file, OpeningMode mode);
  virtual ~FitsImage() { fits_close_file(fits_file_, &status_); }
  /**
   * @brief Copies the header information of another FitsImage into this one.
   * @param other_image The FitsImage to copy the header from.
  */
  void CopyHeaderFrom(FitsImage& other_image);
  // Returns the data type of the image. Types defined in the CFITSIO library.
  inline int GetDataType() { return data_type_; }
  // Returns the status of the last operation.
  inline int GetStatus() const { return status_; }
  // Returns the amount of columns including padding.
  inline long PaddedColumns() const { return padded_dimensions_[0]; }
  // Returns the amount of rows including padding.
  inline long PaddedRows() const { return padded_dimensions_[1]; }
  // Returns the amount of columns of the actual image.
  inline long Columns() const { return dimensions_[0]; }
  // Returns the amount of rows of the actual image.
  inline long Rows() const { return dimensions_[1]; }
  // Returns the amount of padding around the image.
  inline long Padding() const { return padding_; }
  // Returns the amount of pixels in the image.
  inline long TotalElements() const { return total_elements_; }
  // Returns the amount of pixels in the image (with padding).
  inline long PaddedTotalElements() const { return padded_total_elements_; }
  /**
   * @brief Reads the image from the original FITS file to the internal array.
   *  Overwrites the internal image.
   * @param padding Padding amount around the image.
   * @param padding_type Determines the padding value.
   * @param filling Padding value if padding_type is CUSTOM. Ignored otherwise.
   */
  virtual void Load(long padding = 0,
                    PaddingType padding_type = PaddingType::CUSTOM,
                    double filling = 0.0) = 0;
  // Writes the internal image to the FITS file.
  inline void WriteToOriginalFile() { WriteImageData(fits_file_); }
  /**
   * @brief Writes the internal image to a new FITS file.
   * @param file_name Output file name.
   */
  void WriteToFile(std::string file_name);
  // Calculates the median value of the original image.
  virtual double CalculateMedian() = 0;
  // Calculates the mean value of the original image.
  virtual double CalculateMean() = 0;
  /**
   * @brief Sets the morphology operation strategy.
   * @param operation The morphology operation to set.
   */
  inline void SetMorphology(Morphology* operation) { morphology_ = operation; }
  /**
   * @brief Applies the morphologic operation on the image using the structuring
   *  element.
   * @param sel Structuring Element to apply the operation with.
   */
  inline void ApplyMorphology(StructuringElement* sel) { morphology_->Operate(this, sel); }
 protected:
  /**
   * @brief Writes only the image data into the given FITS file.
   * @param fits_file FITS file pointer.
   */
  virtual void WriteImageData(fitsfile* fits_file) = 0;

  static constexpr int kAmountOfAxis = 2;

  fitsfile* fits_file_;
  Morphology* morphology_;
  int status_;
  int bitpix_;
  int data_type_;
  long dimensions_[kAmountOfAxis];
  long total_elements_;
  long padding_;
  long padded_dimensions_[kAmountOfAxis];
  long padded_total_elements_;
};



