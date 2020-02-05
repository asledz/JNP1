#include "images.h"

Image cond(Region region, Image this_way, Image that_way) {
  return [=](Point p) {
    if (region(p))
      return this_way(p);
    return that_way(p);
  };
}

Image lerp(Blend blend, Image this_way, Image that_way) {
  return lift(
      [](double blend, Color this_way, Color that_way) {
        return this_way.weighted_mean(that_way, blend);
      },
      blend, this_way, that_way);
}

Image darken(Image image, Blend blend) {
  return lerp(blend, image, constant(Colors::black));
}

Image lighten(Image image, Blend blend) {
  return lerp(blend, image, constant(Colors::white));
}