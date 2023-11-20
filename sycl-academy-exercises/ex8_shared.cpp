/*
 SYCL Academy (c)

 SYCL Academy is licensed under a Creative Commons
 Attribution-ShareAlike 4.0 International License.

 You should have received a copy of the license along with this
 work.  If not, see <http://creativecommons.org/licenses/by-sa/4.0/>.
*/

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <sycl/sycl.hpp>

// Task: Allocate the arrays in USM, and compute r[i] = a[i] + b[i] on the SYCL device

TEST_CASE("usm_vector_add", "usm_vector_add_source") {
  constexpr size_t dataSize = 1024;
  sycl::queue computeQueue;

  float* a{sycl::malloc_shared<float>(dataSize, computeQueue)};
  float* b{sycl::malloc_shared<float>(dataSize, computeQueue)};
  float* r{sycl::malloc_shared<float>(dataSize, computeQueue)};

  auto initialTask = computeQueue.parallel_for(dataSize, [=](sycl::id<1> index){
    a[index] = static_cast<float>(index);
    b[index] = static_cast<float>(index);
    r[index] = 0.0f;
  });

  computeQueue.submit([&](sycl::handler& handler){
    handler.depends_on(initialTask);
    handler.parallel_for(dataSize, [=](sycl::id<1> index){
      r[index] = a[index] + b[index];
    });
  });

  computeQueue.wait();

  sycl::free(a, computeQueue);
  sycl::free(b, computeQueue);

  for (int i = 0; i < dataSize; ++i) {
    REQUIRE(r[i] == i * 2);
  }
  
  sycl::free(r, computeQueue);
}
