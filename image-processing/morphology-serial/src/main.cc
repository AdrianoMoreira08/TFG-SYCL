/**
 * @brief This program performs morphological operations on a binary
 * image using a structuring element.
 *
 * @author: Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 * @author: Stefan Hernandez (Original author)
 *
 * @note: Based on the works of Stefan Hernandez
 * (https://github.com/StefanHernandez/Morphology)
 */

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <chrono>

#include "../include/morphology.h"
#include "../include/fits_image.h"
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
    }
  } else if (argc < 5 || argc > 6) {
    std::cerr << Text::kUsage << std::endl;
    return 1;
  }
  std::string image_file_name{argv[1]};
  std::string stucturing_element_file_name{argv[2]};
  std::string output_file_name{argv[3]};
  std::string operation_argument{argv[4]};
  if (operation_argument.size() != 1) {
    std::cerr << Text::kInvalidOperation << std::endl;
    return 1;
  }
  char operation = operation_argument[0];
  std::string threshold_type_argument{argc == 6 ? argv[5] : "m"};
  if (threshold_type_argument.size() != 1) {
    std::cerr << Text::kInvalidThreshold << std::endl;
    return 1;
  }
  char threshold_type = threshold_type_argument[0];
  std::ifstream structuring_element_file(stucturing_element_file_name);
  FitsImage image(image_file_name, OpeningMode::OPEN);
  double threshold;
  switch (threshold_type) {
    case 'm':
    case 'M':
      threshold = image.CalculateMedian();
      break;
    case 'a':
    case 'A':
      threshold = image.CalculateMean();
      break;
    default:
      std::cerr << Text::kInvalidThreshold << std::endl;
      return 1;
  }
  image.Load(threshold);
  std::stringstream morphology_data{image.GetMorphologyData()};
  Morphology morphology(morphology_data, structuring_element_file);
  std::stringstream result_erosion_data;
  auto start_time = std::chrono::steady_clock::now();
  switch (operation) {
    case 'd':
    case 'D':
      morphology.Dilation(result_erosion_data);
      break;
    case 'e':
    case 'E':
      morphology.Erosion(result_erosion_data);
      break;
    case 'o':
    case 'O':
      morphology.Opening(result_erosion_data);
      break;
    case 'c':
    case 'C':
      morphology.Closing(result_erosion_data);
      break;
    default:
      std::cerr << Text::kInvalidOperation << std::endl;
      break;
  }
  auto end_time = std::chrono::steady_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
  std::cout << "Operation execution time: " << NanosecondsToSeconds(time) << " (s)\n";
  FitsImage result_image(output_file_name, OpeningMode::OVERWRITE);
  result_image.CopyHeaderFrom(image);
  result_image.SetMorphologyData(result_erosion_data);
  result_image.WriteToOriginalFile();
  
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

