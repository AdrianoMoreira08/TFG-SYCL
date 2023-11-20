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
  float a[dataSize], b[dataSize], r[dataSize];
  for (int i = 0; i < dataSize; ++i) {
    a[i] = static_cast<float>(i);
    b[i] = static_cast<float>(i);
    r[i] = 0.0f;
  }
  
  auto devicePointerA{sycl::malloc_device<float>(dataSize, computeQueue)};
  auto devicePointerB{sycl::malloc_device<float>(dataSize, computeQueue)};
  auto devicePointerR{sycl::malloc_device<float>(dataSize, computeQueue)};

  const size_t kCopySize{sizeof(float) * dataSize};

  computeQueue.memcpy(devicePointerA, &a, kCopySize);
  computeQueue.memcpy(devicePointerB, &b, kCopySize);
  computeQueue.memcpy(devicePointerR, &r, kCopySize);

  computeQueue.wait();

  computeQueue.parallel_for(sycl::range{dataSize}, [=](sycl::id<1> index){
    devicePointerR[index] = devicePointerA[index] + devicePointerB[index];
  });

  sycl::free(devicePointerA, computeQueue);
  sycl::free(devicePointerB, computeQueue);

  computeQueue.memcpy(&r, devicePointerR, kCopySize).wait();

  sycl::free(devicePointerR, computeQueue);

  for (int i = 0; i < dataSize; ++i) {
    REQUIRE(r[i] == i * 2);
  }
}
