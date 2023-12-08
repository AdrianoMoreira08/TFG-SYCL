/*
 SYCL Academy (c)

 SYCL Academy is licensed under a Creative Commons
 Attribution-ShareAlike 4.0 International License.

 You should have received a copy of the license along with this
 work.  If not, see <http://creativecommons.org/licenses/by-sa/4.0/>.

 * SYCL Quick Reference
 * ~~~~~~~~~~~~~~~~~~~~
 *
 * // Get all available devices
 * auto devs = sycl::device::get_devices();
 *
 * // Construct a queue with a device
 * auto q = sycl::queue{my_device};
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

TEST_CASE("load_balancing", "load_balancing_source") {
  constexpr size_t data_size = 1024;
  constexpr float ratio = 0.5f;
  constexpr size_t data_size_first = ratio * data_size;
  constexpr size_t data_size_second = data_size - data_size_first;

  float a[data_size], b[data_size], r[data_size];
  for (int i = 0; i < data_size; ++i) {
    a[i] = static_cast<float>(i);
    b[i] = static_cast<float>(i);
    r[i] = 0.0f;
  }

  sycl::queue compute_queue_first{sycl::default_selector_v};
  sycl::queue compute_queue_second{compute_queue_first.get_context(),
                                   sycl::default_selector_v};
  
  { // Buffer scope
    sycl::buffer buf_a_first{a, sycl::range{data_size_first}};
    sycl::buffer buf_b_first{b, sycl::range{data_size_first}};
    sycl::buffer<float> buf_r_first{sycl::range{data_size_first}};
    buf_r_first.set_final_data(r);

    sycl::buffer buf_a_second{a + data_size_first, sycl::range{data_size_second}};
    sycl::buffer buf_b_second{b + data_size_first, sycl::range{data_size_second}};
    sycl::buffer<float> buf_r_second{sycl::range{data_size_second}};
    buf_r_second.set_final_data(r + data_size_first);

    compute_queue_first.submit([&](sycl::handler& handler) {
      sycl::accessor acc_a{buf_a_first, handler, sycl::read_only};
      sycl::accessor acc_b{buf_b_first, handler, sycl::read_only};
      sycl::accessor acc_r{buf_r_first, handler, sycl::write_only};
      handler.parallel_for(sycl::range{data_size_first}, [=](sycl::id<1> index){
        acc_r[index] = acc_a[index] + acc_b[index];
      });
    });

    compute_queue_second.submit([&](sycl::handler& handler) {
      sycl::accessor acc_a{buf_a_second, handler, sycl::read_only};
      sycl::accessor acc_b{buf_b_second, handler, sycl::read_only};
      sycl::accessor acc_r{buf_r_second, handler, sycl::write_only};
      handler.parallel_for(sycl::range{data_size_second}, [=](sycl::id<1> index){
        acc_r[index] = acc_a[index] + acc_b[index];
      });
    });
    compute_queue_first.wait_and_throw();
    compute_queue_second.wait_and_throw();
  }

  for (int i = 0; i < data_size; ++i) {
    REQUIRE(r[i] == static_cast<float>(i) * 2.0f);
  }
}
