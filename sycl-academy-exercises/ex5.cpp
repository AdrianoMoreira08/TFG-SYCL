/*
 SYCL Academy (c)

 SYCL Academy is licensed under a Creative Commons
 Attribution-ShareAlike 4.0 International License.

 You should have received a copy of the license along with this
 work.  If not, see <http://creativecommons.org/licenses/by-sa/4.0/>.
 *
 * SYCL Quick Reference
 * ~~~~~~~~~~~~~~~~~~~~
 *
 * // Make a child class of sycl::device_selector
 * class my_functor_selector : public sycl::device_selector {
 *   // Overload operator() for sycl::device.
 *   int operator()(const sycl::device& dev) const override {
 *   ...
 *   }
 * }
 * ...
 * auto q = sycl::queue{my_functor_selector{}};
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * // Or use a function selector
 * int my_function_selector(const sycl::device &d) {
 *  ...
 * }
 * ...
 * auto q = sycl::queue{my_function_selector};
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * // Or use a lambda selector
 * auto my_lambda_selector = [](const sycl::device &d) {
 *  ...
 * };
 * ...
 * auto q = sycl::queue{my_lambda_selector};
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * // Query a device for some things:
 * std::string vendor = dev.get_info<sycl::info::device::vendor>();
 * std::string dev_name = dev.get_info<sycl::info::device::name>();
 * std::string dev_driver_ver = dev.get_info<sycl::info::device::driver_version>();
 *
 *
*/

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <sycl/sycl.hpp>

int IntelSelector(const sycl::device& dev) {
  if (dev.is_gpu() &&
      dev.get_info<sycl::info::device::vendor>().find("Intel") != std::string::npos) {
    return 1;
  }
  return 0;
}

TEST_CASE("intel_gpu_device_selector", "device_selectors_solution") {
  int a = 18, b = 24, r = 0;

  try {
    // Task: add a device selector to create this queue with an Intel GPU
    auto defaultQueue = sycl::queue{IntelSelector};

    const auto myDevice{defaultQueue.get_device()};
    std::cout << myDevice.get_info<sycl::info::device::name>() << std::endl <<
                 myDevice.get_info<sycl::info::device::vendor>() << std::endl <<
                 myDevice.get_platform().get_info<sycl::info::platform::name>() << std::endl <<
                 myDevice.has(sycl::aspect::accelerator) << std::endl;

    {
      auto bufA = sycl::buffer{&a, sycl::range{1}};
      auto bufB = sycl::buffer{&b, sycl::range{1}};
      auto bufR = sycl::buffer{&r, sycl::range{1}};

      defaultQueue
          .submit([&](sycl::handler& cgh) {
            auto accA = sycl::accessor{bufA, cgh, sycl::read_only};
            auto accB = sycl::accessor{bufB, cgh, sycl::read_only};
            auto accR = sycl::accessor{bufR, cgh, sycl::write_only};
            // Scalar add
            cgh.single_task([=]() { accR[0] = accA[0] + accB[0]; });
          })
          .wait();
    }

    defaultQueue.throw_asynchronous();
  } catch (const sycl::exception& e) {
    std::cout << "Exception caught: " << e.what() << std::endl;
  }

  REQUIRE(r == 42);
}
