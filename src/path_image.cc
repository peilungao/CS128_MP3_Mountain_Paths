#include "path_image.hpp"
#include <fstream>

PathImage::PathImage(const GrayscaleImage& image, const ElevationDataset& dataset):
    height_(dataset.Height()),
    width_(dataset.Width()),
    // paths_(),
    dataset_(dataset),
    image_(image),
    path_image_(image.GetImage()) {
  for (size_t i = 0; i < height_; i++) {
    paths_.push_back(CalculatePath(i));
  }
  row_of_smallest_change_ = FindSmallestEleChange();
}

Path PathImage::CalculatePath(size_t starting_row) {
  size_t current_row = starting_row;
  // std::vector<size_t> path;
  Path path(width_, current_row);
  path.SetLoc(0, starting_row);
  for (size_t j = 0; j + 1 < width_; ++j) {
    int middle = dataset_.DatumAt(current_row, j + 1);
    int top = INT32_MAX;
    int bottom = INT32_MAX;
    if (current_row > 0) {
      top = dataset_.DatumAt(current_row - 1, j + 1);
    }
    if (current_row + 1 < height_) {
      bottom = dataset_.DatumAt(current_row + 1, j + 1);
    }

    int current_elevation = dataset_.DatumAt(current_row, j);

    unsigned int choice = CompareOptions(current_elevation, middle, top, bottom);

    if (choice == 2) {
      current_row--;
    } else if (choice == 3) {
      current_row++;
    }
    int next_elevation = dataset_.DatumAt(current_row, j + 1);
    path.SetLoc(j + 1, current_row);
    unsigned int elevation_change = std::abs(current_elevation - next_elevation);
    path.IncEleChange(elevation_change);
  }
  return path;
}

/*
 * @return one of {1,2,3}
 * returns the minimum change of one of o1,o2,o3, tie-breaking: o2 > o1 > o3
 */
unsigned int CompareOptions(int current, int o1, int o2, int o3) {
  unsigned int d1 = std::abs(current - o1);
  unsigned int d2 = std::abs(current - o2);
  unsigned int d3 = std::abs(current - o3);

  unsigned min_ele_change = std::min({d1, d2, d3});
  // Tie-break order required by spec: forward, then down, then up.
  if (min_ele_change == d1) {
    return 1;
  }
  if (min_ele_change == d3) {
    return 3;
  }
  return 2;
}

size_t PathImage::FindSmallestEleChange() {
  size_t row_of_smallest_change = 0;
  unsigned int smallest_ele_change = paths_[0].EleChange();
  for (size_t i = 1; i < paths_.size(); i++) {
    Path path = paths_[i];
    unsigned this_ele_change = path.EleChange();
    if (this_ele_change < smallest_ele_change) {
      smallest_ele_change = path.EleChange();
      row_of_smallest_change = i;
    }
  }
  return row_of_smallest_change;
}

void PathImage::ToPpm(const std::string& name) const {
  std::ofstream output_file;
  output_file.open(name);
  if (output_file.is_open()) {
    output_file << "P3" << std::endl
                << width_ << " " << height_ << std::endl
                << kMaxColorValue << std::endl;
    for (size_t i = 0; i < height_; i++) {
      for (size_t j = 0; j < width_; j++) {
        bool in_a_path = IsInAPath(i, j);
        bool in_smallest_change_path = IsInSmallestPath(i, j);
        Color c;
        if (in_smallest_change_path) {
          c = kSmallestResistancePath;
        } else if (in_a_path) {
          c = kGeneralPath;
        } else {
          c = image_.ColorAt(i, j);
        }
        output_file << c.Red() << " " << c.Green() << " " << c.Blue() << " ";
      }
      output_file << std::endl;
    }
    output_file << std::endl;
  } else {
    throw std::invalid_argument("File can't be opened.");
  }
}

bool PathImage::IsInAPath(size_t row, size_t col) const {
  // bool in_a_path = false;
  for (const auto& path : paths_) {
    if (path.GetRowFromCol(col) == row) {
      return true;
    }
  }
  return false;
}

bool PathImage::IsInSmallestPath(size_t row, size_t col) const {
  Path smallest_path = paths_.at(row_of_smallest_change_);
  return smallest_path.GetRowFromCol(col) == row;
}
