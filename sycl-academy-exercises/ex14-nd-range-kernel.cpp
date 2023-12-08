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
 * // Construct an in-order queue
 * auto q = sycl::queue{sycl::default_selector_v, 
 *        {sycl::property::queue::in_order{}}};
 *
 * // Declare a buffer pointing to ptr
 * auto buf = sycl::buffer{ptr, sycl::range{n}};
 *
 * // Do a USM memcpy
 * auto event = q.memcpy(dst_ptr, src_ptr, sizeof(T)*n);
 * // Do a USM memcpy with dependent events
 * auto event = q.memcpy(dst_ptr, src_ptr, sizeof(T)*n, {event1, event2});
 *
 * // Wait on an event
 * event.wait();
 *
 * // Wait on a queue
 * q.wait();
 *
 * // Submit work to the queue
 * auto event = q.submit([&](sycl::handler &cgh) {
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
 * //             i: With range:
 *                    cgh.parallel_for<class mykernel>(sycl::range{n}, 
 *                    [=](sycl::id<1> i) { // Do something });
 * //             ii: With nd_range:
 *                    cgh.parallel_for<class mykernel>(sycl::nd_range{
 *                        globalRange, localRange}, [=](sycl::nd_item<1> i) { 
 *                        // Do something
 *                      });
*/

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <sycl/sycl.hpp>

// Task: parallelise the vector add kernel using nd_range
TEST_CASE("nd_range_kernel", "nd_range_kernel_source") {
  constexpr size_t data_size{1024};
  const sycl::range data_range_size{data_size};

  int a[data_size], b[data_size], r[data_size];
  for (int i = 0; i < data_size; ++i) {
    a[i] = i;
    b[i] = i;
    r[i] = 0;
  }

  sycl::queue compute_queue;
  { // Buffer scope
    sycl::buffer buff_a{a, data_range_size};
    sycl::buffer buff_b{b, data_range_size};
    sycl::buffer<int> buff_r{data_range_size};
    buff_a.set_final_data(nullptr);
    buff_b.set_final_data(nullptr);
    buff_r.set_final_data(r);

    compute_queue.submit([&](sycl::handler& handler){
      sycl::accessor acc_a{buff_a, handler, sycl::read_only};
      sycl::accessor acc_b{buff_b, handler, sycl::read_only};
      sycl::accessor acc_r{buff_r, handler, sycl::write_only};

      // Experiment 1: Same single-dimensional global and local range
      // handler.parallel_for(sycl::nd_range{data_range_size, data_range_size},
      // [=](sycl::nd_item<1> item){
      //   auto id{item.get_global_id()};
      //   acc_r[id] = acc_a[id] + acc_b[id];
      // });

      // Experiment 2: Rectangular shaped nd-range with 4 work-groups
      const size_t fraction_factor{16};
      const size_t amount_of_groups{4};
      const size_t global_fraction{data_size / fraction_factor};
      const size_t group_fraction{global_fraction / amount_of_groups};
      sycl::range global_range{global_fraction, fraction_factor};
      sycl::range group_range{group_fraction, fraction_factor};
      handler.parallel_for(sycl::nd_range{global_range, group_range},
      [=](sycl::nd_item<2> item){
        auto id{item.get_global_linear_id()};
        acc_r[id] = acc_a[id] + acc_b[id];
      });
    }).wait_and_throw();
  }
  
  for (int i = 0; i < data_size; ++i) {
    REQUIRE(r[i] == i * 2);
  }
}
