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
 * // Declare a buffer using host ptr
 * auto buf = sycl::buffer{ptr, sycl::range{n}, 
 *                 {sycl::property::buffer::use_host_ptr{}}};
 *
 * // Declare a buffer relating to no host memory
 * auto buf = sycl::buffer{sycl::range{n}};
 *
 * // Set final data of a buffer
 * buf.set_final_data(host_ptr);
 *
 * // Set final data of a buffer to nullptr
 * buf.set_final_data(nullptr);
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
 * //    2. Enqueue a parallel for:
 *          cgh.parallel_for<class mykernel>(sycl::range{n}, [=](sycl::id<1> i) {
 *              // Do something
 *          });
 *
*/

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <sycl/sycl.hpp>

// Task: run these kernels on a SYCL device, minimising the memory
// transfers between the host and device
TEST_CASE("temporary_data_buffer_accessor", "temporary_data_source") {
  constexpr size_t data_size = 1024;

  float in[data_size], out[data_size], tmp[data_size];
  for (int i = 0; i < data_size; ++i) {
    in[i] = static_cast<float>(i);
    tmp[i] = 0.0f;
    out[i] = 0.0f;
  }
  sycl::queue compute_queue;
  { // Buffer scope
    sycl::buffer buf_in{in, sycl::range{data_size}};
    buf_in.set_final_data(nullptr);
    sycl::buffer<float> buf_tmp{sycl::range{data_size}};
    sycl::buffer<float> buf_out{sycl::range{data_size}};
    buf_out.set_final_data(out);

    compute_queue.submit([&](sycl::handler& handler){
      sycl::accessor acc_tmp{buf_tmp, handler, sycl::write_only};
      sycl::accessor acc_in{buf_in, handler, sycl::read_only};
      handler.parallel_for(sycl::range{data_size}, [=](sycl::id<1> index){
        acc_tmp[index] = acc_in[index] * 8.0f;
      });
    });

    compute_queue.submit([&](sycl::handler& handler){
      sycl::accessor acc_out{buf_out, handler, sycl::write_only};
      sycl::accessor acc_tmp{buf_tmp, handler, sycl::read_only};
      handler.parallel_for(sycl::range{data_size}, [=](sycl::id<1> index){
        acc_out[index] = acc_tmp[index] / 2.0f;
      });
    });
    compute_queue.wait_and_throw();
  }
  for (int i = 0; i < data_size; ++i) {
    REQUIRE(out[i] == i * 4.0f);
  }
}

// Task: run these kernels on a SYCL device, minimising the memory
// transfers between the host and device
TEST_CASE("temporary_data_usm", "temporary_data_source") {
  constexpr size_t data_size = 1024;
  constexpr size_t data_size_bytes = sizeof(float) * data_size;
  float in[data_size], out[data_size];
  for (int i = 0; i < data_size; ++i) {
    in[i] = static_cast<float>(i);
  }

  sycl::queue compute_queue{
    sycl::aspect_selector(sycl::aspect::usm_device_allocations)};
  float* device_in{sycl::malloc_device<float>(data_size, compute_queue)};
  float* device_out{sycl::malloc_device<float>(data_size, compute_queue)};
  float* tmp{sycl::malloc_device<float>(data_size, compute_queue)};

  auto initial_copy = compute_queue.memcpy(device_in, in, data_size_bytes);
  auto kernel_a = compute_queue
  .parallel_for(sycl::range{data_size}, initial_copy, [=](sycl::id<1> index){
    tmp[index] = device_in[index] * 8.0f;
  });
  auto kernel_b = compute_queue
  .parallel_for(sycl::range{data_size}, kernel_a, [=](sycl::id<1> index){
    device_out[index] = tmp[index] / 2.0f;
  });
  auto copy_back = compute_queue.memcpy(out, device_out, data_size_bytes, kernel_b);
  compute_queue.wait_and_throw();
  compute_queue.submit([&](sycl::handler& handler){
    handler.depends_on(copy_back);
    handler.host_task([=]{
      sycl::free(device_out, compute_queue);
      sycl::free(device_in, compute_queue);
      sycl::free(tmp, compute_queue);
    });
  });

  for (int i = 0; i < data_size; ++i) {
    REQUIRE(out[i] == i * 4.0f);
  }
}