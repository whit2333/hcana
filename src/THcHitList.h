#ifndef ROOT_THcHitList
#define ROOT_THcHitList

#include "THcRawHit.h"
#include "THaDetMap.h"
#include "THaEvData.h"
#include "TClonesArray.h"
#include "TObject.h"
#include "Decoder.h"
#include "THaCrateMap.h"
#include "Fadc250Module.h"

#include <iomanip>
#include <map>

#include "hcana/Logger.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////
//
// THcHitList
//
//////////////////////////////////////////////////////////////////////////

//class THaDetMap;
class THcConfigEvtHandler;

class THcHitList : public podd2::HitLogging<podd2::EmptyBase> {

public:

  virtual ~THcHitList();

  THcHitList();

  /** Populate the hitlist from the raw event data.
   * Clears the hit list then, finds all populated channels belonging to the detector and add
   * sort it into the hitlist.  A given counter in the detector can have
   * at most one entry in the hit list.  However, the raw "hit" can contain
   * multiple signal types (e.g. ADC+, ADC-, TDC+, TDC-), or multiplehits for multihit tdcs.
   * The hit list is sorted (by plane, counter) after filling.
   */
  virtual Int_t DecodeToHitList( const THaEvData& evdata, Bool_t suppress=kFALSE );
  void          InitHitList(THaDetMap* detmap,
			    const char *hitclass, Int_t maxhits,
			    Int_t tdcref_cut=0, Int_t adcref_cut=0);

  TClonesArray* GetHitList() const {return fRawHitList; }
  void          CreateMissReportParms(const char *prefix);
  void          MissReport(const char *name);
  void          DisableSlipCorrection() {fDisableSlipCorrection = kTRUE;}

  UInt_t        fNRawHits;
  Int_t         fNMaxRawHits;
  Int_t         fTDC_RefTimeCut;
  Int_t         fADC_RefTimeCut;
  Bool_t        fTDC_RefTimeBest;
  Bool_t        fADC_RefTimeBest;
  TClonesArray* fRawHitList;  // List of raw hits
  TClass*       fRawHitClass; // Class of raw hit object to use
  THaDetMap*    fdMap;

  Int_t fNTDCRef_miss;
  Int_t fNADCRef_miss;

protected:

  struct RefIndexMap { // Mapping for one reference channel
    Bool_t defined;
    Bool_t hashit;
    Int_t crate;
    Int_t slot;
    Int_t channel;
    Int_t reftime;
  };
  std::vector<RefIndexMap> fRefIndexMaps;
  // Should this be a sparse list instead in case user
  // picks ridiculously large refindexes?

  Int_t                   fNRefIndex;
  UInt_t                  fNSignals;
  THcRawHit::ESignalType* fSignalTypes;
  THcConfigEvtHandler*    fPSE125;
  Bool_t                  fHaveFADCInfo;
  Int_t                   fNSA;
  Int_t                   fNSB;
  Int_t                   fNPED;


  Decoder::THaCrateMap*                    fMap; /* The Crate map */
  Int_t                                    fTISlot;
  Int_t                                    fTICrate;
  Double_t                                 fDisableSlipCorrection;
  std::map<Int_t, Int_t>                   fTrigTimeShiftMap;
  std::map<Int_t, Decoder::Fadc250Module*> fFADCSlotMap;

  ClassDef(THcHitList,0);  // List of raw hits sorted by plane, counter
};
#endif
