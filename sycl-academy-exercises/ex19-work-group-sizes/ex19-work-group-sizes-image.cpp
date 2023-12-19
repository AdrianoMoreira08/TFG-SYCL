/*
 SYCL Academy (c)

 SYCL Academy is licensed under a Creative Commons
 Attribution-ShareAlike 4.0 International License.

 You should have received a copy of the license along with this
 work.  If not, see <http://creativecommons.org/licenses/by-sa/4.0/>.
*/

#include <sycl/sycl.hpp>

#include <benchmark.h>
#include <image_conv.h>

class image_convolution;

inline constexpr util::filter_type filterType = util::filter_type::blur;
inline constexpr int filterWidth = 11;
inline constexpr int halo = filterWidth / 2;

bool is_number(const std::string& text) {
  for (const auto& character: text) {
    if (!std::isdigit(character)) {
      return false;
    }
  }
  return true;
}

/**
 * Note from the student:
 *  This is an attempt to refactor the code using texture memory using unsampled_image.
 *  However, this code was not tested since the current working machine does
 *  not have aspect::image.
*/

int main(int argc, char* argv[]) {
  size_t kDimesion0{8};
  size_t kDimesion1{32};

  if (argc >= 3) {
    if (!is_number(argv[1]) && !is_number(argv[2])) {
      std::cerr << "Error: Local range dimensions must be numbers." << std::endl;
      return 1;
    }
    kDimesion0 = std::stoul(argv[1]);
    kDimesion1 = std::stoul(argv[2]);
  }
  
  const char* inputImageFile =
      "../Code_Exercises/Images/dogs.png";
  const char* outputImageFile =
      "../Code_Exercises/Images/blurred_dogs_ex18.png";

  auto inputImage = util::read_image(inputImageFile, halo);

  auto outputImage = util::allocate_image(
      inputImage.width(), inputImage.height(), inputImage.channels());

  auto filter = util::generate_filter(util::filter_type::blur, filterWidth);

  try {
    // No GPU on current machine
    sycl::queue myQueue{/*sycl::gpu_selector_v*/};
    // std::cout << "Running on "
    //           << myQueue.get_device().get_info<sycl::info::device::name>()
    //           << "\n";

    auto inputImgWidth = inputImage.width();
    auto inputImgHeight = inputImage.height();
    auto channels = inputImage.channels();
    auto filterWidth = filter.width();
    auto halo = filter.half_width();
    auto doubleHalo = halo * 2;

    auto globalRange = sycl::range(inputImgWidth, inputImgHeight);
    auto localRange = sycl::range(kDimesion0, kDimesion1);
    auto ndRange = sycl::nd_range(globalRange, localRange);

    const std::string kBenchmarkMessage{"Work-group size: " +
        std::to_string(kDimesion0) + " x " + std::to_string(kDimesion1)};

    auto inBufRange =
        sycl::range(inputImgHeight + doubleHalo, inputImgWidth + doubleHalo) *
        sycl::range(1, channels);
    auto outBufRange =
        sycl::range(inputImgHeight, inputImgWidth) * sycl::range(1, channels);
    auto filterRange = filterWidth * sycl::range(1, channels);
    sycl::range tileRange = localRange + sycl::range(doubleHalo, doubleHalo);

    {
      auto inBuf = sycl::buffer{inputImage.data(), inBufRange};
      auto outBuf = sycl::buffer<float, 2>{outBufRange};
      auto filterBuf = sycl::buffer{filter.data(), filterRange};
      outBuf.set_final_data(outputImage.data());

      // Buffer data type reinterpretation for data vectorization
      const sycl::range kInVecBufRange{inBufRange[0], inBufRange[1] / channels};
      const sycl::range kOutVecBufRange{outBufRange[0], outBufRange[1] / channels};
      const sycl::range kFiletrVecBufRange{filterRange[0], filterRange[1] / channels};

      sycl::unsampled_image<2> inVecBuf(inputImage.data(),
                                        sycl::image_format::r32g32b32a32_sfloat,
                                        kInVecBufRange);
      sycl::unsampled_image<2> outVecBuf(sycl::image_format::r32g32b32a32_sfloat,
                                         kOutVecBufRange);
      sycl::unsampled_image<2> filterBufVec(filter.data(),
                                            sycl::image_format::r32g32b32a32_sfloat,
                                            kFiletrVecBufRange);
      util::benchmark(
          [&]() {
            myQueue.submit([&](sycl::handler& cgh) {
              sycl::unsampled_image_accessor<sycl::float4, 2, 
                                             sycl::access::mode::read,
                                             sycl::image_target::device> inputAcc{inVecBuf, cgh};
              sycl::unsampled_image_accessor<sycl::float4, 2, 
                                             sycl::access::mode::write,
                                             sycl::image_target::device> outputAcc{outVecBuf, cgh};
              sycl::unsampled_image_accessor<sycl::float4, 2, 
                                             sycl::access::mode::read,
                                             sycl::image_target::device> filterAcc{filterBufVec, cgh};
              
              auto tile = sycl::local_accessor<sycl::float4, 2>(tileRange, cgh);

              cgh.parallel_for<image_convolution>(
                  ndRange, [=](sycl::nd_item<2> item) {
                    auto globalId = item.get_global_id();
                    sycl::int2 globalIdInt{globalId[0], globalId[1]};
                    auto localId = item.get_local_id();

                    auto groupOffset = item.get_group().get_group_id() * localRange;
                    sycl::int2 groupOffsetInt{groupOffset[0], groupOffset[1]};

                    for (size_t row{localId[0]};
                        row < tileRange[0]; row += localRange[0]) {
                      for (size_t column{localId[1]};
                          column < tileRange[1]; column += localRange[1]) {
                        tile[row][column] = inputAcc.read(groupOffsetInt + sycl::int2{row, column});
                      }
                    }
                    sycl::group_barrier(item.get_group());

                    sycl::float4 sum{0.0f, 0.0f, 0.0f, 0.0f};

                    for (int r = 0; r < filterWidth; ++r) {
                      for (int c = 0; c < filterWidth; ++c) {
                        sum += tile[localId[0] + r][localId[1] + c] * filterAcc.read(sycl::int2{r, c});
                      }
                    }
                    outputAcc.write(globalIdInt, sum);
                  });
            });

            myQueue.wait_and_throw();
          },
          100, kBenchmarkMessage);
    }
  } catch (sycl::exception e) {
    std::cout << "Exception caught: " << e.what() << std::endl;
  }

  util::write_image(outputImage, outputImageFile);
}
