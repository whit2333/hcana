#ifndef hallc_HallC_Data_HH
#define hallc_HallC_Data_HH

#include <array>
#include <algorithm>

namespace hallc {
  namespace data {


    /** Stores the digitized pulse data in a fixed size array.
     */
    struct PulseWaveForm {
      static const UInt_t MaxNPulses  = 4;
      static const UInt_t MaxNSamples = 511;
      // From THcRawAdcHit.h
      PulseWaveForm() {}
      PulseWaveForm(Int_t* buf, Int_t size = MaxNSamples) { std::copy_n(buf, MaxNSamples, std::begin(_buffer)); }
      virtual ~PulseWaveForm() {}

      void ZeroBuffer() { std::fill(std::begin(_buffer), std::end(_buffer), 0); }

      std::array<Int_t, MaxNSamples> _buffer;

      ClassDef(PulseWaveForm, 1)
    };
  }
}

#endif
