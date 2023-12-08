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
 * // Do a USM malloc_device
 * auto ptr = sycl::malloc_device<T>(n, q);
 *
 * // Do a USM memcpy
 * q.memcpy(dst_ptr, src_ptr, sizeof(T)*n);
 *
 * // Wait on a queue
 * q.wait();
 *
 * // Submit work to the queue
 * q.submit([&](sycl::handler &cgh) {
 *   // COMMAND GROUP
 * });
 *
 *
 * // Within the command group you can
 * //    1. Declare an accessor to a buffer
 *          auto read_write_acc = sycl::accessor{buf, cgh};
 *          auto read_acc = sycl::accessor{buf, cgh, sycl::read_only};
 *          auto write_acc = sycl::accessor{buf, cgh, sycl::write_only};
 *          auto no_init_acc = sycl::accessor{buf, cgh, sycl::no_init};
 * //    2. Enqueue a parallel for:
 *              cgh.parallel_for<class mykernel>(sycl::range{n}, 
 *                    [=](sycl::id<1> i) { // Do something });
 *
 *
*/

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <sycl/sycl.hpp>

TEST_CASE("synchronization_usm", "synchronization_source") {
  sycl::queue accelerator_queue;
  int* a{sycl::malloc_host<int>(1, accelerator_queue)};
  int* b{sycl::malloc_host<int>(1, accelerator_queue)};
  int* r{sycl::malloc_device<int>(1, accelerator_queue)};
  *a = 18;
  *b = 24;

  auto sum = accelerator_queue.single_task([=](){
    r[0] = a[0] + b[0];
  });

  int result;
  auto copy = accelerator_queue.memcpy(&result, r, sizeof(int), sum);

  accelerator_queue.wait_and_throw();

  accelerator_queue.submit([&](sycl::handler& handler) {
    handler.depends_on(copy);
    handler.host_task([=](){
      sycl::free(a, accelerator_queue);
      sycl::free(b, accelerator_queue);
      sycl::free(r, accelerator_queue);
    });
  });

  // REQUIRE(true); // Original require statement
  REQUIRE(result == 42);
}

TEST_CASE("synchronization_buffer_acc", "synchronization_source") {
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
    }).wait_and_throw();
  }
  // REQUIRE(true); // Original require statement
  REQUIRE(r == 42);
}
