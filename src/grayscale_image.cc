#include "grayscale_image.hpp"
#include "color.hpp"
#include <cmath>
#include <vector>
#include <fstream>

GrayscaleImage::GrayscaleImage(const ElevationDataset& dataset):
    height_(dataset.Height()),
    width_(dataset.Width()),
    image_(dataset.Height(), std::vector<Color>(dataset.Width(), Color(0, 0, 0))) {
  const auto& data = dataset.GetData();
  int max_ele = dataset.MaxEle();
  int min_ele = dataset.MinEle();
  for (size_t i = 0; i < height_; i++) {
    for (size_t j = 0; j < width_; j++) {
      int elevation_point = data.at(i).at(j);
      auto range = static_cast<float>(max_ele - min_ele);
      float ratio = (range > 0)
                        ? static_cast<float>(elevation_point - min_ele) / range
                        : 0.0F;
      int shade_of_gray = static_cast<int>(std::round(ratio * kMaxColorValue));
      Color point_color = Color(shade_of_gray, shade_of_gray, shade_of_gray);
      image_.at(i).at(j) = point_color;
    }
  }
}

GrayscaleImage::GrayscaleImage(const std::string& filename, size_t width, size_t height):
    GrayscaleImage(ElevationDataset(filename, width, height)) {
}

const Color& GrayscaleImage::ColorAt(size_t row, size_t col) const {
  return image_.at(row).at(col);
}

void GrayscaleImage::ToPpm(const std::string& name) const {
  std::ofstream output_file;
  output_file.open(name);
  if (output_file.is_open()) {
    output_file << "P3" << std::endl
                << width_ << " " << height_ << std::endl
                << kMaxColorValue << std::endl;
    for (size_t i = 0; i < height_; i++) {
      for (size_t j = 0; j < width_; j++) {
        Color c = ColorAt(i, j);
        output_file << c.Red() << " " << c.Green() << " " << c.Blue() << " ";
      }
      output_file << std::endl;
    }
    output_file << std::endl;
  } else {
    throw std::invalid_argument("File can't be opened.");
  }
}
