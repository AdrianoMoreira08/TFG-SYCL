/**
 * @brief Structuring Element class that reads a structuring element from a file
 * and provides basic access to its data.
 * 
 * The structuring element only allows 0s and 1s.
 *
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */

#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "structuring_element.h"

/**
 * @brief Represents a structuring element that can be read from a file.
 * Grants access to the data and dimensions of the structuring element.
 * The structuring element only allows 0s and 1s.
 */
template<typename T>
class TemplatedStructuringElement: public StructuringElement {
 public:
  /**
   * @brief Opens a struturing element file and loads its contents.
   * @param file_name The name of the structuring element file.
   * @note The file must have the follwing structure.
   *  First line: `number_of_rows` `number_of_columns`.
   *  Second line: `center_row` `center_column`.
   *  The next lines of the file are the rows of the SE, where the cell values are
   *  sparated by at least one space. The structuring element only allows 0s and 1s.
   */
  TemplatedStructuringElement(const std::string& file_name, int data_type):
    StructuringElement{} {
    if (file_name.empty()) {
      throw std::invalid_argument("The SE file does not exist.");
    }
    std::ifstream file{file_name};
    std::string line_raw;
    // First line
    getline(file, line_raw);
    std::stringstream line{line_raw};
    std::string element{""};
    line >> element;
    rows_ = std::stol(element);
    line >> element;
    columns_ = std::stol(element);
    // Second line
    getline(file, line_raw);
    line.clear();
    line.str(line_raw);
    line >> element;
    center_row_ = std::stol(element);
    line >> element;
    center_column_ = std::stol(element);
    total_elements_ = rows_ * columns_;
    data_ = new T[total_elements_];
    const T kZeroValue{static_cast<T>(0)};
    const T kOneValue{static_cast<T>(1)};
    // Actual SE lines
    for (long row{rows_ - 1}; row >= 0 ; --row) {
      getline(file, line_raw);
      line.clear();
      line.str(line_raw);
      long data_row{row * columns_};
      for (long column{0}; column < columns_; ++column) {
        line >> element;
        T element_value{static_cast<T>(std::stod(element))};
        if (element_value == kOneValue || element_value == kZeroValue) {
          data_[data_row + column] = element_value;
        } else {
          throw std::runtime_error("Wrong file format.");
        }
      }
    }
  }
  ~TemplatedStructuringElement() override { delete[] data_; };
  inline T* GetData() { return data_; };
 private:
  T* data_;
};

/**
 * @brief Creates a Structuring element instance from a file using dynamic
 *  memory. Is the user's responsibility to free the memory.
 * @param file_name The name of the structuring element file.
 * @param data_type The type of data of the items in the SE.
 *  Uses CFITSIO data type enum.
 * @returns A TemplatedFitsImage object as its base class poiner.
 */
StructuringElement* NewStructuringElement(const std::string& file_name,
                                          int data_type);
