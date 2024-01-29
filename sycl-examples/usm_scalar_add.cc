/**
 * Universidad de La Laguna. Escuela Superior de Ingenierı́a y Tecnologı́a
 * Grado en Ingenierı́a Informática
 * @author Adriano dos Santos Moreira <alu0101436784@ull.edu.es>
 * @brief Simple scalar add example using USM.
 */

#include <sycl/sycl.hpp>

int main() {
  const size_t kDataSize{1024};
  sycl::queue queue;

  float* summand_a{sycl::malloc_shared<float>(kDataSize, queue)};
  float* summand_b{sycl::malloc_shared<float>(kDataSize, queue)};
  float* result{sycl::malloc_shared<float>(kDataSize, queue)};

  auto initialization_task = queue.parallel_for(kDataSize, [=](sycl::id<1> index){
    summand_a[index] = static_cast<float>(index);
    summand_b[index] = static_cast<float>(index);
    result[index] = 0.0f;
  });

  queue.submit([&](sycl::handler& handler){
    handler.depends_on(initialization_task);
    handler.parallel_for(kDataSize, [=](sycl::id<1> index){
      result[index] = summand_a[index] + summand_b[index];
    });
  }).wait();


  sycl::free(summand_a, queue);
  sycl::free(summand_b, queue);
  sycl::free(result, queue);
}