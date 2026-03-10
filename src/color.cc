#include "color.hpp"
#include <stdexcept>

Color::Color(int r, int g, int b):
    red_(r), green_(g), blue_(b) {
  // need to ensure valid color has been constructed...
  if (!IsValidColor(red_) || !IsValidColor(green_) || !IsValidColor(blue_)) {
    throw std::invalid_argument("Color RGB out of range.");
  }
}

bool Color::IsValidColor(int value) const {
  return value >= kColorValueMin && value <= kColorValueMax;
}

// do not modify
bool operator==(const Color& rhs, const Color& lhs) {
  return (rhs.Red() == lhs.Red() && rhs.Green() == lhs.Green() &&
          rhs.Blue() == lhs.Blue());
}
