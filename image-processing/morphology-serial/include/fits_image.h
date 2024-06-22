/**
 * @brief Fits Image class that encapsulates CFITS mechanics.
 *
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */

#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <fitsio.h>

// Opening modes
enum OpeningMode {
  OPEN,
  OVERWRITE,
  CREATE
};

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
   * @param file_name The name of the FITS file.
   * @param mode The mode to open the FITS file (use the OpeningMode enum).
   * Default mode is OpeningMode::OPEN.
  */
  FitsImage(std::string file_name, int mode = OpeningMode::OPEN);
  ~FitsImage();
  /**
   * @brief Copies the header information of another FitsImage into this one.
   * @param other_image The FitsImage to copy the header from.
  */
  void CopyHeaderFrom(FitsImage& other_image);
  /**
   * @brief Converts the internal numeric representation to binary integers data
   * using the median as the threshold.
   * @param threshold The threshold to convert the data to binary.
  */
  void ConvertToBinary(double threshold);
  /**
   * @brief Creates a Morphology class compatible version of the FITS data.
   * @return A stringstream with the FITS data.
  */
  std::stringstream GetMorphologyData();
  void SetMorphologyData(std::stringstream& data);
  // Returns the status of the last operation.
  inline int GetStatus() const { return status_; }
  // Reads the image from the original FITS file. Overwrites the internal image.
  void ReloadImage();
  // Writes the internal image to the FITS file.
  void WriteToOriginalFile();
  // Writes the internal image to a new FITS file.
  void WriteToFile(std::string file_name);
  // Calculates the median value of the image.
  double CalculateMedian();
  // Calculates the mean value of the image.
  double CalculateMean();
 private:
  // Checks if the internal file of the FITS image exists.
  bool FileExists();

  static constexpr int kAmountOfAxis = 2;

  std::string file_name_;
  fitsfile *fits_file_;
  int status_;
  long dimensions_[kAmountOfAxis];
  long total_elements_;
  double* image_data_;
};

