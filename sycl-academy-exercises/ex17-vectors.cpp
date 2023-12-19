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


#include <benchmark.h>
#include <image_conv.h>

class image_convolution;

inline constexpr util::filter_type filterType = util::filter_type::blur;
inline constexpr int filterWidth = 11;
inline constexpr int halo = filterWidth / 2;

TEST_CASE("image_convolution_vectorized", "vectors_source") {
  const char* inputImageFile =
      "../Code_Exercises/Images/dogs.png";
  const char* outputImageFile =
      "../Code_Exercises/Images/blurred_dogs_ex17.png";

  auto inputImage = util::read_image(inputImageFile, halo);

  auto outputImage = util::allocate_image(
      inputImage.width(), inputImage.height(), inputImage.channels());

  auto filter = util::generate_filter(util::filter_type::blur, filterWidth);

  try {
    // No GPU on current machine
    sycl::queue myQueue{/*sycl::gpu_selector_v*/};

    std::cout << "Running on "
              << myQueue.get_device().get_info<sycl::info::device::name>()
              << "\n";

    auto inputImgWidth = inputImage.width();
    auto inputImgHeight = inputImage.height();
    auto channels = inputImage.channels();
    auto filterWidth = filter.width();
    auto halo = filter.half_width();

    auto globalRange = sycl::range(inputImgWidth, inputImgHeight);
    auto localRange = sycl::range(1, 32);
    auto ndRange = sycl::nd_range(globalRange, localRange);

    auto inBufRange =
        sycl::range(inputImgHeight + (halo * 2), inputImgWidth + (halo * 2)) *
        sycl::range(1, channels);
    auto outBufRange =
        sycl::range(inputImgHeight, inputImgWidth) * sycl::range(1, channels);
    auto filterRange = filterWidth * sycl::range(1, channels);

    {
      auto inBuf = sycl::buffer{inputImage.data(), inBufRange};
      auto outBuf = sycl::buffer<float, 2>{outBufRange};
      auto filterBuf = sycl::buffer{filter.data(), filterRange};
      outBuf.set_final_data(outputImage.data());

      // Buffer data type reinterpretation for data vectorization
      const sycl::range kInVecBufRange{inBufRange[0], inBufRange[1] / channels};
      const sycl::range kOutVecBufRange{outBufRange[0], outBufRange[1] / channels};
      const sycl::range kFiletrVecBufRange{filterRange[0], filterRange[1] / channels};
      auto inVecBuf = inBuf.reinterpret<sycl::float4>(kInVecBufRange);
      auto outVecBuf = outBuf.reinterpret<sycl::float4>(kOutVecBufRange);
      auto filterBufVec = filterBuf.reinterpret<sycl::float4>(kFiletrVecBufRange);

      util::benchmark(
          [&]() {
            myQueue.submit([&](sycl::handler& cgh) {
              sycl::accessor inputAcc{inVecBuf, cgh, sycl::read_only};
              sycl::accessor outputAcc{outVecBuf, cgh, sycl::write_only};
              sycl::accessor filterAcc{filterBufVec, cgh, sycl::read_only};

              cgh.parallel_for<image_convolution>(
                  ndRange, [=](sycl::nd_item<2> item) {
                    auto globalId = item.get_global_id();
                    // Slower, why?
                    // globalId = sycl::id{globalId[1], globalId[0]};

                    auto haloOffset = sycl::id(halo, halo);
                    auto src = (globalId + haloOffset);
                    auto dest = globalId;

                    sycl::float4 sum{0.0f, 0.0f, 0.0f, 0.0f};

                    for (int r = 0; r < filterWidth; ++r) {
                      for (int c = 0; c < filterWidth; ++c) {
                        auto srcOffset =
                            sycl::id(src[0] + (r - halo),
                                     src[1] + ((c - halo)));
                        auto filterOffset = sycl::id(r, c);
                        
                        sum += inputAcc[srcOffset] * filterAcc[filterOffset];
                      }
                    }
                    outputAcc[dest] = sum;
                  });
            });

            myQueue.wait_and_throw();
          },
          100, "image convolution (coalesced)");
    }
  } catch (sycl::exception e) {
    std::cout << "Exception caught: " << e.what() << std::endl;
  }

  util::write_image(outputImage, outputImageFile);
  REQUIRE(true);
}
