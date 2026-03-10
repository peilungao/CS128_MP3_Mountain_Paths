#ifndef GRAYSCALE_IMAGE_H
#define GRAYSCALE_IMAGE_H

#include <cstdlib>  // for size_t
#include <vector>
#include <string>

#include "color.hpp"
#include "elevation_dataset.hpp"

class GrayscaleImage {
public:
  GrayscaleImage(const ElevationDataset& dataset);
  GrayscaleImage(const std::string& filename, size_t width, size_t height);  // Calls the first constructor
  size_t Width() const { return width_; };
  size_t Height() const { return height_; };
  unsigned int MaxColorValue() const { return kMaxColorValue; };
  const std::vector<std::vector<Color>>& GetImage() const { return image_; };

  const Color& ColorAt(size_t row, size_t col) const;
  void ToPpm(const std::string& name) const;

private:
  size_t height_;
  size_t width_;
  std::vector<std::vector<Color>> image_;
  static const int kMaxColorValue = 255;
};

#endif
