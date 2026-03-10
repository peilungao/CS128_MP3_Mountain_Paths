#include "elevation_dataset.hpp"
#include <fstream>
#include <stdexcept>

ElevationDataset::ElevationDataset(const std::string& filename, size_t width, size_t height):
    width_(width),
    height_(height),
    data_(height_, std::vector<int>(width_, 0)) {
  std::ifstream input_file_stream{filename};
  if (!input_file_stream.is_open()) {
    throw std::runtime_error("File can't be read");
  }

  for (size_t i = 0; i < height_; i++) {
    for (size_t j = 0; j < width_; j++) {
      if (!(input_file_stream >> data_.at(i).at(j))) {
        throw std::runtime_error("Too few data points or formatted read failure.");
      }
      int current_data = data_.at(i).at(j);
      if (current_data > max_ele_) {
        max_ele_ = current_data;
      }
      if (current_data < min_ele_) {
        min_ele_ = current_data;
      }
    }
  }

  int extra_point = 0;
  if (input_file_stream >> extra_point) {
    throw std::runtime_error("More data points than required.");
  }
}
int ElevationDataset::DatumAt(size_t row, size_t col) const {
  return data_.at(row).at(col);
}
