/**
 * @brief Structuring Element class that reads a structuring element from a file
 * and provides basic access to its data.
 * 
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */

#pragma once

/**
 * @brief Represents a structuring element that can be read from a file.
 * Grants access to the data and dimensions of the structuring element.
 */
class StructuringElement {
 public:
  StructuringElement() {}
  virtual ~StructuringElement() = 0;
  // Returns the data type of the image. Types defined in the CFITSIO library.
  inline int GetDataType() { return data_type_; }
  inline long Rows() const { return rows_; };
  inline long Columns() const { return columns_; }
  inline long CenterRow() const { return center_row_; }
  inline long CenterColumn() const { return center_column_; }
 protected:
  int data_type_;
  long rows_;
  long columns_;
  long center_row_;
  long center_column_;
  long total_elements_;
};


