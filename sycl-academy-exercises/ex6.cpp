/*
 SYCL Academy (c)

 SYCL Academy is licensed under a Creative Commons
 Attribution-ShareAlike 4.0 International License.

 You should have received a copy of the license along with this
 work.  If not, see <http://creativecommons.org/licenses/by-sa/4.0/>.

 * SYCL Quick Reference
 * ~~~~~~~~~~~~~~~~~~~~
 *
 * // Default construct a queue
 * auto q = sycl::queue{};
 *
 * // Declare a buffer pointing to ptr
 * auto buf = sycl::buffer{ptr, sycl::range{n}};
 *
 * // Submit work to the queue
 * q.submit([&](sycl::handler &cgh) {
 *   // COMMAND GROUP
 * });
 *
 * // Within the command group you can
 * //    1. Declare an accessor to a buffer
 *          auto read_write_acc = sycl::accessor{buf, cgh};
 *          auto read_acc = sycl::accessor{buf, cgh, sycl::read_only};
 *          auto write_acc = sycl::accessor{buf, cgh, sycl::write_only};
 *          auto no_init_acc = sycl::accessor{buf, cgh, sycl::no_init};
 * //    2. Enqueue a single task:
 *          cgh.single_task<class mykernel>([=]() {
 *              // Do something
 *          });
 * //    3. Enqueue a parallel for:
 *          cgh.parallel_for<class mykernel>(sycl::range{n}, [=](sycl::id<1> i) {
 *              // Do something
 *          });
 *
*/

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <sycl/sycl.hpp>

TEST_CASE("vector_add", "vector_add_solution") {
  constexpr size_t dataSize = 1024;

  float a[dataSize], b[dataSize], r[dataSize];
  for (int i = 0; i < dataSize; ++i) {
    a[i] = static_cast<float>(i);
    b[i] = static_cast<float>(i);
    r[i] = 0.0f;
  }

  // Task: Compute r[i] = a[i] + b[i] in parallel on the SYCL device

  sycl::buffer bufA{a, sycl::range(dataSize)};
  sycl::buffer bufB{b, sycl::range(dataSize)};
  sycl::buffer bufR{r, sycl::range(dataSize)};

  sycl::queue computeQueue;

  auto sumTask = computeQueue.submit([&](sycl::handler& handler){
    sycl::accessor accA{bufA, handler, sycl::read_only};
    sycl::accessor accB{bufB, handler, sycl::read_only};
    sycl::accessor accR{bufR, handler, sycl::write_only};

    handler.parallel_for(sycl::range(dataSize), [=](sycl::id<1> index){
      accR[index] = accA[index] + accB[index];
    });
  });
  
  /**
   * Ways of retrieving data from device to host:
   *  1. Get the buffer out of scope, which results in a copy device -> host
   *  2. Use host_accessor to the buffer
   *  3. Request an update: update_host
   */

  // Omitting first method.
  // ...

  // Second method:
  // sycl::host_accessor accR{bufR, sycl::read_only};
  // for (size_t index{0}; index < dataSize; ++index) {
  //   r[index] = accR[index];
  // }

  // Third method
  // computeQueue.submit([&](sycl::handler& handler){
  //   sycl::accessor accR{bufR, handler};
  //   handler.update_host(accR);
  // });
  
  // Simplified solution using update_host
  sycl::accessor accR{bufR};
  computeQueue.update_host(accR);
  
  computeQueue.wait();
  
  
  for (int i = 0; i < dataSize; ++i) {
    REQUIRE(r[i] == static_cast<float>(i) * 2.0f);
  }
}
