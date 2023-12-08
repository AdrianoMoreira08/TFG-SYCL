/*
 SYCL Academy (c)

 SYCL Academy is licensed under a Creative Commons
 Attribution-ShareAlike 4.0 International License.

 You should have received a copy of the license along with this
 work.  If not, see <http://creativecommons.org/licenses/by-sa/4.0/>.
 
 * SYCL Quick Reference
 * ~~~~~~~~~~~~~~~~~~~~
 *
 * // Include SYCL header
 * #include <sycl/sycl.hpp>
 *
 * // Default construct a queue
 * auto q = sycl::queue{};
 *
 * // Allocate device memory
 * auto * devPtr = sycl::malloc_device<int>(mycount, q);
 *
 * // Memcpy
 * q.memcpy(dst, src, sizeof(T)*n).wait();
 * // (dst and src are pointers)
 *
 * // Free memory
 * sycl::free(ptr, q);
 *
 * // Construct a buffer of size n associated with ptr
 * auto buf = sycl::buffer{ptr, sycl::range{n}};
 *
 * // Submit a kernel
 * q.submit([&](sycl::handler &cgh) {
 *    cgh.single_task([=](){
 *      // Some kernel code
 *      });
 * }).wait();
 *    
 * // Construct an accessor for buf 
 * // (must be done within command group)
 *     auto acc = sycl::accessor{buf, cgh};
 *     auto acc = sycl::accessor{buf, cgh, sycl::read_only};
 *     auto acc = sycl::accessor{buf, cgh, sycl::write_only};
 *     auto acc = sycl::accessor{buf, cgh, sycl::no_init};
 *
*/

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <sycl/sycl.hpp>

// TEST_CASE("scalar_add_usm", "scalar_add_source") {

//   int a = 18, b = 24, r = 0;

//   // Task: Compute a+b on the SYCL device using USM
//   r = a + b;

//   REQUIRE(r == 42);
// }

TEST_CASE("scalar_add_buff_acc", "scalar_add_source") {
  // Task: Compute a+b on the SYCL device using the buffer
  // accessor memory model
  // r = a + b;

  int a = 18, b = 24, r = 0;
  sycl::queue accelerator_queue;
  { // Buffer scope
    sycl::buffer summand_a{&a, sycl::range(1)};
    sycl::buffer summand_b{&b, sycl::range(1)};
    sycl::buffer result_r{&r, sycl::range(1)};


    accelerator_queue.submit([&](sycl::handler& handler){
      sycl::accessor in_a{summand_a, handler, sycl::read_only};
      sycl::accessor in_b{summand_b, handler, sycl::read_only};
      sycl::accessor out_r{result_r, handler, sycl::write_only, sycl::no_init};

      handler.single_task([=](){
        out_r[0] = in_a[0] + in_b[0];
      });
    }).wait();
  }
  // sycl::host_accessor access_result_r{result_r, sycl::read_only};
  // r = access_result_r[0];

  REQUIRE(r == 42);
}
