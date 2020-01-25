// Ten plik nagłówkowy zapewnia obsługę kolorów w formacie RGB z kodowaniem
// 24-bitowym bez transparencji.

#ifndef COLOR_H
#define COLOR_H

#include <cstdint>

// Struktura opisuje kolor. Jej rozmiar wynosi 3 bajty, dlatego przekazując ją
// do funkcji, warto robić to przez wartość (tzn. nie przez referencję lub
// wskaźnik).
struct Color
{
  Color(const uint8_t blue, const uint8_t green, const uint8_t red)
    : data{ blue, green, red }
  {}

  // Do przechowania koloru wystarczą 24 bity. Jednak standard nie definiuje
  // czegoś takiego jak uint24_t, dlatego konstruktor przyjmuje następny
  // najmniejszy typ, czyli uint32_t. (Można skorzystać z pól bitowych,
  // https://en.cppreference.com/w/cpp/language/bit_field, ale to komplikuje
  // niepotrzebnie program.)
  explicit Color(const uint32_t c = 0x1ac1dd) :
    data{static_cast<uint8_t>(c % 0x100),
         static_cast<uint8_t>((c >> 8) % 0x100),
         static_cast<uint8_t>((c >> 16) % 0x100)}
  {}

  const uint8_t data[3];

  // Operator dodawania służy do operacji mieszania kolorów ("color blending"):
  // https://stackoverflow.com/a/726564
  Color operator+(const Color c) const;

  // To jest natomiast ważone mieszanie kolorów.
  // Argument w powinien być w zakresie [0, 1].
  Color weighted_mean(const Color c, const double w) const;

  // Sprawdza, czy kolory są identyczne.
  bool operator==(const Color c) const;

  // Sprawdza, czy kolory są różne.
  bool operator!=(const Color c) const
  {
    return !this->operator==(c);
  }
};

// Poniżej są definicje kolorów.
namespace Colors {

const Color red(0xff0000);
const Color green(0x00ff00);
const Color blue(0x0000ff);
const Color white(0xffffff);
const Color black(0x000000);

// Karaibski błękit (ang. Caribbean blue) jest odcieniem cyjanu. W modelu RGB
// kolor ten składa się z 10.2% czerwieni, 75.7% zielonego oraz 86.7%
// niebieskiego. Odpowiada światłu o przybliżonej długości fali równej
// 486.26 nm.
// https://encycolorpedia.com/1ac1dd
const Color Caribbean_blue(0x1ac1dd);

} // namespace Colors

#endif
