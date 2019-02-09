#ifndef hallc_HallC_Data_HH
#define hallc_HallC_Data_HH

#include <array>

namespace hallc {
  namespace data {

    struct PulseWaveForm {
      std::array<float,200> _buffer;
      ClassDef(PulseWaveForm,1)
    };

  }
}

#endif
