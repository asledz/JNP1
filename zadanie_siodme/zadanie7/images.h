#ifndef IMAGES_H
#define IMAGES_H

#include <functional>
#include <iostream>

#include "color.h"
#include "coordinate.h"
#include "functional.h"

using Fraction = double;

template <typename T> using Base_image = std::function<T(const Point)>;

using Region = Base_image<bool>;

using Image = Base_image<Color>;

using Blend = Base_image<Fraction>;

template <typename T> Base_image<T> constant(T t) {
  return [t](const Point p) {
    return t;
    (void)p;
  };
}

template <typename T> Base_image<T> rotate(Base_image<T> image, double phi) {
  return compose(
      to_polar, [=](Point p) { return Point(p.first, p.second - phi, true); },
      from_polar, image);
}

template <typename T> Base_image<T> translate(Base_image<T> image, Vector v) {
  return lift(image, [=](const Point p) {
    Point point = p.is_polar ? from_polar(p) : p;
    Point translated_point =
        Point(point.first - v.first, point.second - v.second);
    return translated_point;
  });
}

template <typename T> Base_image<T> scale(Base_image<T> image, double s) {
  return lift(image, [=](const Point p) {
    Point point = p.is_polar ? from_polar(p) : p;
    Point scaled_point = Point(point.first / s, point.second / s);
    return scaled_point;
  });
}

template <typename T>
Base_image<T> circle(Point q, double r, T inner, T outer) {
  return [=](const Point p) {
    Point point = p.is_polar ? from_polar(p) : p;
    return (distance(point, q) < r) ? inner : outer;
  };
}

template <typename T>
Base_image<T> vertical_stripe(double d, T this_way, T that_way) {
  return [=](const Point p) {
    Point point = p.is_polar ? from_polar(p) : p;
    return (abs(point.first) * 2 < d) ? this_way : that_way;
  };
}

template <typename T> Base_image<T> checker(double d, T this_way, T that_way) {
  return [=](const Point p) {
    Point point = p.is_polar ? from_polar(p) : p;
    return (static_cast<int>(floor(point.first / d) + floor(point.second / d)) %
            2)
               ? that_way
               : this_way;
  };
}

template <typename T>
Base_image<T> rings(Point q, double d, T this_way, T that_way) {
  return [=](const Point p) {
    Point point = p.is_polar ? from_polar(p) : p;
    return (static_cast<int>(distance(point, q) / d) % 2) ? this_way : that_way;
  };
}

template <typename T>
Base_image<T> polar_checker(double d, int n, T this_way, T that_way) {
  return compose(
      to_polar,
      [=](const Point p) {
        return Point(p.first,
                     p.second * static_cast<double>(n) * d / (2 * M_PI), false);
      },
      checker(d, this_way, that_way));
}

Image cond(Region region, Image this_way, Image that_way);

Image lerp(Blend blend, Image this_way, Image that_way);

Image darken(Image image, Blend blend);

Image lighten(Image image, Blend blend);

//  {
// 	return[=](const Point p) {
// 		if(region(p)) return this_way(p);
// 		return that_way(p);
// 	};
// }

#endif
