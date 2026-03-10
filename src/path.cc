#include "path.hpp"

Path::Path(size_t length, size_t starting_row):
    length_(length),
    starting_row_(starting_row),
    path_(length) {
}

void Path::SetLoc(size_t col, size_t row) {
  path_.at(col) = row;
}

void Path::IncEleChange(unsigned int value) {
  ele_change_ += value;
}

size_t Path::GetRowFromCol(size_t col) const {
  return path_.at(col);
}
