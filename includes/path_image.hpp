#ifndef PATH_IMAGE_H
#define PATH_IMAGE_H

#include <cstdlib>  // for size_t
#include <vector>
#include <string>

#include "color.hpp"
#include "path.hpp"
#include "grayscale_image.hpp"

class PathImage {
public:
  PathImage(const GrayscaleImage& image, const ElevationDataset& dataset);
  size_t Width() const { return width_; };
  size_t Height() const { return height_; };
  unsigned int MaxColorValue() const { return kMaxColorValue; };
  const std::vector<Path>& Paths() const { return paths_; };
  const std::vector<std::vector<Color>>& GetPathImage() const { return path_image_; };
  void ToPpm(const std::string& name) const;

  Path CalculatePath(size_t starting_row);

  bool IsInAPath(size_t row, size_t col) const;
  bool IsInSmallestPath(size_t row, size_t col) const;

  size_t FindSmallestEleChange();

private:
  size_t height_ = 0;
  size_t width_ = 0;
  std::vector<Path> paths_;
  ElevationDataset dataset_;
  GrayscaleImage image_;
  std::vector<std::vector<Color>> path_image_;
  static const int kMaxColorValue = 255;
  size_t row_of_smallest_change_ = 0;

  const Color kGeneralPath = Color(252, 25, 63);
  const Color kSmallestResistancePath = Color(31, 253, 13);
};

unsigned int CompareOptions(int current, int o1, int o2, int o3);

#endif
