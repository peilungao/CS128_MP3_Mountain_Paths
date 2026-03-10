#include <iostream>
#include <exception>

#include "elevation_dataset.hpp"
#include "grayscale_image.hpp"
#include "path_image.hpp"

int main() {
  try {
    const std::string kTestInputFile =
        "./example-data/ex_input_data/prompt_5w_2h.dat";
    constexpr size_t kTestInputFileWidth = 5;
    constexpr size_t kTestInputFileHeight = 2;

    ElevationDataset ed(
        kTestInputFile, kTestInputFileWidth, kTestInputFileHeight);
    GrayscaleImage gi2(ed);
    gi2.ToPpm("./test.ppm");
    PathImage pi(gi2, ed);
    pi.ToPpm("./PathImage.ppm");
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return 1;
  }

  return 0;
}
