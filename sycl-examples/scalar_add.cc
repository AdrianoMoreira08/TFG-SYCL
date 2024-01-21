/**
 * Universidad de La Laguna. Escuela Superior de Ingenierı́a y Tecnologı́a
 * Grado en Ingenierı́a Informática
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 * @brief Simple scalar add example.
 */

#include <sycl/sycl.hpp>
#include <iostream>

int main() {
  int summand_a{1}, summand_b{2}, result{0};
  sycl::queue queue;
  { // Buffer scope
    sycl::buffer buffer_a{&summand_a, sycl::range(1)};
    sycl::buffer buffer_b{&summand_b, sycl::range(1)};
    sycl::buffer buffer_r{&result, sycl::range(1)};

    queue.submit([&](sycl::handler& handler){
      sycl::accessor in_a{buffer_a, handler};
      sycl::accessor in_b{buffer_b, handler};
      sycl::accessor out_r{buffer_r, handler};

      handler.single_task([=](){
        out_r[0] = in_a[0] + in_b[0];
      });
    }).wait();
  }
  std::cout << result << std::endl;
}
