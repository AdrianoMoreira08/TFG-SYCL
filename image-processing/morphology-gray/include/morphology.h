/**
 * @brief Morphology abstract class that serves as a base for morphology
 *  operation derived classes.
 * 
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */

#pragma once

class FitsImage;
class StructuringElement;

/**
 * @brief Represents a morphology operation.
 */
class Morphology {
 public:
  Morphology() {}
  virtual ~Morphology() = 0;
  /**
   * @brief Performs the morphologic operation on the image with the structuring
   *  element.
   * @param image FITS image to transform.
   * @param sel Structuring element for the operation.
   */
  virtual void Operate(FitsImage* fits_image, StructuringElement* operation_sel) = 0;
};
