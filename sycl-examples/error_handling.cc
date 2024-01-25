/**
 * Universidad de La Laguna. Escuela Superior de Ingenierı́a y Tecnologı́a
 * Grado en Ingenierı́a Informática
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 * @brief Handling of synchronous and asynchronous errors example.
 */

#include <sycl/sycl.hpp>
#include <iostream>


int main() {
  auto AsyncHandler = [](sycl::exception_list exceptions) {
    for (auto& exception: exceptions) {
      std::rethrow_exception(exception);
    }
  };
  
  try {
    sycl::queue queue{AsyncHandler};
    // Potentially exception-prone code
    // ...
  } catch (sycl::exception& e) {
    std::cerr << "SYCL exception has occurred.\n"
              << e.what() << std::endl;
  }
}