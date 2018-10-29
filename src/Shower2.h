#ifndef ROOT_hcana_Shower2
#define ROOT_hcana_Shower2

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// hcana::Shower                                                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcShower.h"
#include "TObject.h"

namespace hcana {

class Shower2 : public THcShower {

public:
  Shower2(const char *name, const char *description = "",
             THaApparatus *a = nullptr)
      : THcShower(name, description, a) {}

  virtual ~Shower2(){}

  virtual THaAnalysisObject::EStatus Init(const TDatime &run_time);

  int _run_number = 0;
  std::vector<double> _neg_gain_cor;
  std::vector<double> _pos_gain_cor;
  std::vector<double> _arr_gain_cor;

  void SetRunNumber(int rn) { _run_number = rn;}
  void LoadJsonCalibration(int rn, std::string fname = "PARAM/SHMS/CAL/pcal_calib.json");

  // virtual void 	     Clear( Option_t* opt="" );
  // virtual Int_t      Decode( const THaEvData& );
  // virtual Int_t      CoarseProcess( TClonesArray& tracks );
  // virtual Int_t      FineProcess( TClonesArray& tracks );

  ClassDef(Shower2, 1) // Shower counter detector
};

} // namespace hcana

#endif
