/**
 * Universidad de La Laguna. Escuela Superior de Ingenierı́a y Tecnologı́a
 * Grado en Ingenierı́a Informática
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 * @brief Use of bulit-in CPU selector example.
 */

#include <sycl/sycl.hpp>
#include <iostream>

int main() {
  sycl::queue queue{sycl::cpu_selector_v};
  std::cout << "Device: "
            << queue.get_device().get_info<sycl::info::device::name>()
            << std::endl;
}
