/**
 * @brief This program performs morphological operations on a FITS
 * image using a structuring element.
 *
 * @author: Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 */

#include <string>
#include <iostream>
#include <chrono>
#include <algorithm>

#include "../include/templated_fits_image.h"
#include "../include/templated_structuring_element.h"
#include "../include/utils.h"

/**
 * @brief Protected main function that can throw exceptions.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return The status of the program.
*/
int ProtectedMain(int argc, char* argv[]) {
  if (argc == 2) {
    std::string argument{argv[1]};
    if (argument == "-h" || argument == "--help") {
      std::cout << Text::kHelp << std::endl;
      return 0;
    } else {
      std::cerr << Text::kUsage << std::endl;
      return 1;
    }
  } else if (argc != 5) {
    std::cerr << Text::kUsage << std::endl;
    return 1;
  }
  
  std::string image_file_name{argv[1]};
  std::string sel_file_name{argv[2]};
  std::string output_file_name{argv[3]};
  std::string operation_input{argv[4]};

  auto start_program_time = std::chrono::steady_clock::now();
  
  FitsImage* image = NewFitsImage(image_file_name);
  const int kDataType{image->GetDataType()};
  StructuringElement* sel = NewStructuringElement(sel_file_name, kDataType);
  Morphology* operation = GetMorphologyOperation(operation_input, kDataType);
  const long kPadding{std::max(sel->Rows(), sel->Columns())};
  image->Load(kPadding, GetFillingType(operation_input));
  image->SetMorphology(operation);
  auto start_operation_time = std::chrono::steady_clock::now();
  image->ApplyMorphology(sel);
  auto end_operation_time = std::chrono::steady_clock::now();
  image->WriteToFile(output_file_name);

  delete image;
  delete sel;
  delete operation;

  auto end_program_time = std::chrono::steady_clock::now();

  auto operation_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
    end_operation_time - start_operation_time).count();
  auto program_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
    end_program_time - start_program_time).count();
  std::cout << "Program execution time: "
            << NanosecondsToSeconds(program_time) << " (s)" << std::endl;
  std::cout << "Operation execution time: "
            << NanosecondsToSeconds(operation_time) << " (s)" << std::endl;

  return 0;
}

int main(int argc, char* argv[]) {
  try {
    return ProtectedMain(argc, argv);
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return 1;
}

