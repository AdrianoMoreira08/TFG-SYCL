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
 * //             i: Without dependent events
 *                    cgh.parallel_for<class mykernel>(sycl::range{n}, 
 *                    [=](sycl::id<1> i) { // Do something });
 * //             ii: With dependent events
 *                    cgh.parallel_for<class mykernel>(sycl::range{n}, 
 *                    {event1, event2}, [=](sycl::id<1> i) { 
 *                        // Do something
 *                      });

*/

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <sycl/sycl.hpp>

// Task: Run these kernels on the SYCL device, respecting the dependencies
// as shown in the README
TEST_CASE("in_order_queue_usm", "in_order_queue_source") {
  auto AsyncHandler = [](sycl::exception_list exceptions) {
    for (auto& exception: exceptions) {
      try {
        std::rethrow_exception(exception);
      } catch (sycl::exception syclException) {
        std::cerr << "SYCL asynchronous exception has occurred.\n" <<
                     syclException.what() << std::endl;
      } catch (...) {
        std::cerr << "Unknown asynchronous exception has occurred." << std::endl;
      }
    }
    std::terminate();
  };
  try {
    constexpr size_t data_size = 1024;
    sycl::queue compute_queue{
      sycl::aspect_selector(sycl::aspect::usm_device_allocations),
      AsyncHandler,
      {sycl::property::queue::in_order{}}};

    int* in_a{sycl::malloc_device<int>(data_size, compute_queue)};
    int* in_b{sycl::malloc_device<int>(data_size, compute_queue)};
    int* in_c{sycl::malloc_device<int>(data_size, compute_queue)};
    int* out{sycl::malloc_device<int>(data_size, compute_queue)};

    compute_queue.parallel_for(sycl::range{data_size}, [=](sycl::id<1> index) {
      in_a[index] = static_cast<float>(index);
      in_b[index] = static_cast<float>(index);
      in_c[index] = static_cast<float>(index);
      out[index] = 0.0f;
    });
    compute_queue.parallel_for(sycl::range{data_size}, [=](sycl::id<1> index) {
      in_a[index] = in_a[index] * 2.0f;
    });
    compute_queue.parallel_for(sycl::range{data_size}, [=](sycl::id<1> index) {
      in_b[index] += in_a[index];
    });
    compute_queue.parallel_for(sycl::range{data_size}, [=](sycl::id<1> index) {
      in_c[index] -= in_a[index];
    });
    compute_queue.parallel_for(sycl::range{data_size}, [=](sycl::id<1> index) {
      out[index] = in_b[index] + in_c[index];
    });

    compute_queue.wait_and_throw();

    compute_queue.submit([&](sycl::handler& handler) {
      handler.host_task([=](){
        sycl::free(in_a, compute_queue);
        sycl::free(in_b, compute_queue);
        sycl::free(in_c, compute_queue);
      });
    });

    for (int i = 0; i < data_size; ++i) {
      REQUIRE(out[i] == i * 2.0f);
    }

    sycl::free(out, compute_queue);
  } catch (sycl::exception& e) {
    std::cerr << "SYCL synchronous exception has occurred.\n" <<
                 e.what() << std::endl;
  } catch (std::exception& e) {
    std::cerr << "Standard exception has occurred.\n" <<
                 e.what() << std::endl;
  }
}

// Task: Run these kernels on the SYCL device, respecting the dependencies
// as shown in the README
TEST_CASE("in_order_queue_buffer_acc", "in_order_queue_source") {
  auto AsyncHandler = [](sycl::exception_list exceptions) {
    for (auto& exception: exceptions) {
      try {
        std::rethrow_exception(exception);
      } catch (sycl::exception syclException) {
        std::cerr << "SYCL asynchronous exception has occurred.\n" <<
                     syclException.what() << std::endl;
      } catch (...) {
        std::cerr << "Unknown asynchronous exception has occurred." << std::endl;
      }
    }
    std::terminate();
  };
  try {
    constexpr size_t data_size{1024};
    
    int in_a[data_size], in_b[data_size], in_c[data_size], out[data_size];
    for (int i = 0; i < data_size; ++i) {
      in_a[i] = static_cast<float>(i);
      in_b[i] = static_cast<float>(i);
      in_c[i] = static_cast<float>(i);
      out[i] = 0.0f;
    }

    sycl::queue compute_queue{AsyncHandler, {sycl::property::queue::in_order{}}};
    sycl::buffer buff_a{in_a, sycl::range{data_size}};
    sycl::buffer buff_b{in_b, sycl::range{data_size}};
    sycl::buffer buff_c{in_c, sycl::range{data_size}};
    sycl::buffer buff_out{out, sycl::range{data_size}};

    compute_queue.submit([&](sycl::handler& handler) {
      sycl::accessor acc_a{buff_a, handler, sycl::read_write};
      handler.parallel_for(sycl::range{data_size}, [=](sycl::id<1> index){
        acc_a[index] = acc_a[index] * 2.0f;
      });
    });
    compute_queue.submit([&](sycl::handler& handler) {
      sycl::accessor acc_b{buff_b, handler, sycl::read_write};
      sycl::accessor acc_a{buff_a, handler, sycl::read_only};
      handler.parallel_for(sycl::range{data_size}, [=](sycl::id<1> index){
        acc_b[index] += acc_a[index];
      });
    });
    compute_queue.submit([&](sycl::handler& handler) {
      sycl::accessor acc_c{buff_c, handler, sycl::read_write};
      sycl::accessor acc_a{buff_a, handler, sycl::read_only};
      handler.parallel_for(sycl::range{data_size}, [=](sycl::id<1> index){
        acc_c[index] -= acc_a[index];
      });
    });
    auto kernel_d = compute_queue.submit([&](sycl::handler& handler) {
      sycl::accessor acc_out{buff_out, handler, sycl::write_only};
      sycl::accessor acc_b{buff_b, handler, sycl::read_only};
      sycl::accessor acc_c{buff_c, handler, sycl::read_only};
      handler.parallel_for(sycl::range{data_size}, [=](sycl::id<1> index){
        acc_out[index] = acc_b[index] + acc_c[index];
      });
    });
    sycl::accessor acc_out{buff_out};
    compute_queue.update_host(acc_out).wait_and_throw();
    
    for (int i = 0; i < data_size; ++i) {
      REQUIRE(out[i] == i * 2.0f);
    }
  } catch (sycl::exception& e) {
    std::cerr << "SYCL synchronous exception has occurred.\n" <<
                 e.what() << std::endl;
  } catch (std::exception& e) {
    std::cerr << "Standard exception has occurred.\n" <<
                 e.what() << std::endl;
  }
}

