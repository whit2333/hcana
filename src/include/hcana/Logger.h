#ifndef hallc_hcana_Logger_hh
#define hallc_hcana_Logger_hh

#include "podd2/Logger.h"

//#include "podd2/spdlog/spdlog.h"
//#include "podd2/spdlog/sinks/stdout_color_sinks.h" //support for stdout logging
//#include "podd2/spdlog/sinks/basic_file_sink.h" // support for basic file logging

namespace hcana {

  template <typename Base>
  using RunLogging = podd2::RunLogging<Base>;

  template <typename Base>
  using ConfigLogging = podd2::ConfigLogging<Base>;

  template <typename Base>
  using HitLogging = podd2::HitLogging<Base>;

  template <typename Base>
  using DetectorLogging = podd2::DetectorLogging<Base>;

  template <typename Base>
  using SpectrometerLogging = podd2::SpectrometerLogging<Base>;

}


#endif
