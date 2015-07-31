///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcHodoEff                                                                //
//                                                                           //
// Class for accumulating statistics for and calculating hodoscope           //
// efficiencies.                                                             //
//                                                                           //
// Moddled after VDCeff                                                      //
// For now trying to emulate work done in h_scin_eff/h_scin_eff_shutdown     //
///////////////////////////////////////////////////////////////////////////////

#include "THcHodoEff.h"
#include "THaApparatus.h"
#include "THcHodoHit.h"
#include "THcGlobals.h"
#include "THcParmList.h"

using namespace std;

//_____________________________________________________________________________
THcHodoEff::THcHodoEff (const char *name, const char* description,
			const char* hodname) :
  THaPhysicsModule(name, description), fName(hodname), fHod(NULL), fNevt(0)
{

}

//_____________________________________________________________________________
THcHodoEff::~THcHodoEff()
{
  // Destructor

  RemoveVariables();
}

//_____________________________________________________________________________
void THcHodoEff::Reset( Option_t* opt )
// Clear event-by-event data
{
  Clear(opt);
}

//_____________________________________________________________________________
Int_t THcHodoEff::Begin( THaRunBase* )
{
  // Start of analysis

  if (!IsOK() ) return -1;

  // Book any special histograms here

  fNevt = 0;

  // Clear all the accumulators here
  for(Int_t ip=0;ip<fNPlanes;ip++) {
    fHitPlane[ip] = 0;
    for(Int_t ic=0;ic<fNCounters[ip];ic++) {
      fStatPosHit[ip][ic] = 0;
      fStatNegHit[ip][ic] = 0;
      fStatAndHit[ip][ic] = 0;
      fStatOrHit[ip][ic] = 0;
      fBothGood[ip][ic] = 0;
      fPosGood[ip][ic] = 0;
      fNegGood[ip][ic] = 0;
      for(Int_t idel=0;idel<20;idel++) {
	fStatTrkDel[ip][ic][idel] = 0;
	fStatAndHitDel[ip][ic][idel] = 0;
      }
    }
  }

  return 0;
}

//_____________________________________________________________________________
Int_t THcHodoEff::End( THaRunBase* )
{
  // End of analysis

  return 0;
}


//_____________________________________________________________________________
THaAnalysisObject::EStatus THcHodoEff::Init( const TDatime& run_time )
{
  // Initialize THcHodoEff physics module

  //  const char* const here = "Init";

  // Standard initialization. Calls ReadDatabase(), ReadRunDatabase(),
  // and DefineVariables() (see THaAnalysisObject::Init)
  
  fHod = dynamic_cast<THcHodoscope*>
    ( FindModule( fName.Data(), "THcHodoscope"));

  fSpectro = static_cast<THaSpectrometer*>(fHod->GetApparatus());
  
  if( THaPhysicsModule::Init( run_time ) != kOK )
    return fStatus;

  cout << "THcHodoEff::Init nplanes=" << fHod->GetNPlanes() << endl;
  cout << "THcHodoEff::Init Apparatus = " << fHod->GetName() << 
    " " <<
     (fHod->GetApparatus())->GetName() << endl;

  return fStatus = kOK;
}

//_____________________________________________________________________________
Int_t THcHodoEff::Process( const THaEvData& evdata )
{
  // Accumulate statistics for efficiency

  // const char* const here = "Process";

  if( !IsOK() ) return -1;

  // Project the golden track to each
  // plane.  Need to get track at Focal Plane, not tgt.
  //
  // Assumes that planes are X, Y, X, Y 
  THaTrack* theTrack = fSpectro->GetGoldenTrack();
  // Since fSpectro knows the index of the golden track, we can
  // get other information about the track from fSpectro.
  // Need to remove the specialized stuff from fGoldenTrack

  if(!theTrack) return 0;
  Int_t trackIndex = theTrack->GetTrkNum()-1;

  // May make these member variables
  Double_t hitPos[fNPlanes];
  Double_t hitDistance[fNPlanes];
  Int_t hitCounter[fNPlanes];
  Int_t checkHit[fNPlanes];
  Bool_t goodTdcBothSides[fNPlanes];
  Bool_t goodTdcOneSide[fNPlanes];

  for(Int_t ip=0;ip<fNPlanes;ip++) {
    // Should really have plane object self identify as X or Y
    if(ip%2 == 0) {		// X Plane
      hitPos[ip] = theTrack->GetX() + theTrack->GetTheta()*fPosZ[ip];
    } else {			// Y Plane
      hitPos[ip] = theTrack->GetY() + theTrack->GetPhi()*fPosZ[ip];
    }
    // Which counter does track pass through?
    hitCounter[ip] = TMath::Min(TMath::Max(TMath::Nint((hitPos[ip] - fCenterFirst[ip])/fSpacing[ip]) + 1,1),fNCounters[ip]); 
    // How far from paddle center is track?
    if(ip%2 == 0) {		// X Plane
      hitDistance[ip] =  hitPos[ip] - (fSpacing[ip]*(hitCounter[ip]-1) + 
				       fCenterFirst[ip]);
    } else {			// Y Plane
      hitDistance[ip] =  hitPos[ip] - (fSpacing[ip]*(hitCounter[ip]-1) -
				       fCenterFirst[ip]);
    }
  }

  // Fill dpos histograms and set checkHit for each plane.
  // dpos stuff not implemented
  // Why do dpos stuff here, does any other part need the dpos historgrams
  // Look to VDCEff code to see how to create and fill histograms
       
  for(Int_t ip=0;ip<fNPlanes;ip++) {
    Int_t hitcounter=hitCounter[ip];
    goodTdcBothSides[ip] = kFALSE;
    goodTdcOneSide[ip] = kFALSE;
    checkHit[ip] = 2;
    Int_t nphits=fPlanes[ip]->GetNScinHits();
    TClonesArray* hodoHits = fPlanes[ip]->GetHits();
    for(Int_t ihit=0;ihit<nphits;ihit++) {
      THcHodoHit* hit=(THcHodoHit*) hodoHits->At(ihit);
      Int_t counter = hit->GetPaddleNumber();
      if(counter == hitcounter) {
	checkHit[ip] = 0;
      } else {
	if(TMath::Abs(counter-hitcounter) == 1 && checkHit[ip] != 0) {
	  checkHit[ip] = 1;
	}
      }
    }
  }
       
  // Record position differences between track and center of scin
  // and increment 'should have hit' counters
  for(Int_t ip=0;ip<fNPlanes;ip++) {
    Int_t hitcounter = hitCounter[ip];
    Double_t dist = hitDistance[ip];
    if(TMath::Abs(dist) <= fStatSlop &&
       theTrack->GetChi2()/theTrack->GetNDoF() <= fMaxChisq) {
      // && hsshtrk >= 0.05
      Double_t delta = theTrack->GetDp();
      Int_t idel = TMath::Floor(delta+10.0);
      // Should
      if(idel >=0 && idel < 20) {
	fStatTrkDel[ip][hitcounter][idel]++;
      }
      // lookat[ip] = TRUE;
    }
    fHitPlane[ip] = 0;
  }
  // Is there a hit on or adjacent to paddle that track
  // passes through?

  // May collapse this loop into last

  // record the hits as a "didhit" if track is near center of
  // scintillator, the chisqared of the track is good and it is the
  // first "didhit" in that plane.

  for(Int_t ip=0;ip<fNPlanes;ip++) {
    Int_t hitcounter = hitCounter[ip];
    Double_t dist = hitDistance[ip];
    Int_t nphits=fPlanes[ip]->GetNScinHits();
    TClonesArray* hodoHits = fPlanes[ip]->GetHits();
    for(Int_t ihit=0;ihit<nphits;ihit++) {
      THcHodoHit* hit=(THcHodoHit*) hodoHits->At(ihit);
      Int_t counter = hit->GetPaddleNumber();
      // Finds first best hit
      Bool_t onTrack, goodScinTime, goodTdcNeg, goodTdcPos;
      fHod->GetFlags(trackIndex,ip,ihit,
		     onTrack, goodScinTime, goodTdcNeg, goodTdcPos);
      if(TMath::Abs(dist) <= fStatSlop &&
	 TMath::Abs(hitcounter-counter) <= checkHit[ip] &&
	 fHitPlane[ip] == 0 &&
	 theTrack->GetChi2()/theTrack->GetNDoF() <= fMaxChisq) {
	// && hsshtrk >= 0.05
	fHitPlane[ip]++;
		
	// Need to find out hgood_tdc_pos(igoldentrack,ihit) and neg
	if(goodTdcPos) {
	  if(goodTdcNeg) {	// Both fired
	    fStatPosHit[ip][hitcounter]++;
	    fStatNegHit[ip][hitcounter]++;
	    fStatAndHit[ip][hitcounter]++;
	    fStatOrHit[ip][hitcounter]++;
	    Double_t delta = theTrack->GetDp();
	    Int_t idel = TMath::Floor(delta+10.0);
	    if(idel >=0 && idel < 20) {
	      fStatAndHitDel[ip][hitcounter][idel]++;
	    }
	  } else {
	    fStatPosHit[ip][hitcounter]++;
	    fStatOrHit[ip][hitcounter]++;
	  }
	} else if(goodTdcNeg) {
	  fStatNegHit[ip][hitcounter]++;
	  fStatOrHit[ip][hitcounter]++;
	}
      }
      // Increment pos/neg/both fired.  Track independent, so
      // no chisquared cut, but note that only scintillators on the
      // track are examined.
      if(goodTdcPos) {
	if(goodTdcNeg) {
	  fBothGood[ip][hitcounter]++;
	} else {
	  fPosGood[ip][hitcounter]++;
	}
      } else if (goodTdcNeg) {
	fNegGood[ip][hitcounter]++;
      }
      // Determine if one or both PMTs had a good tdc
      
      if(goodTdcPos && goodTdcNeg) {
	goodTdcBothSides[ip] = kTRUE;
      }
      if(goodTdcPos || goodTdcNeg) {
	goodTdcOneSide[ip] = kTRUE;
      }
    }
  }
  /*
  For each plane, see of other 3 fired.  This means that they were enough
  to form a 3/4 trigger, and so the fraction of times this plane fired is
  the plane trigger efficiency.  NOTE: we only require a TDC hit, not a
  TDC hit within the SCIN 3/4 trigger window, so high rates will make
  this seem better than it is.  Also, make sure we're not near the edge
  of the hodoscope (at the last plane), using the same hhodo_slop param. as for h_tof.f
  NOTE ALSO: to make this check simpler, we are assuming that all planes
  have identical active areas.  y_scin = y_cent + y_offset, so shift track
  position by offset for comparing to edges.
  */

  // Need to add calculation and cuts on
  // xatback and yatback in order to set the
  // htrig_hododidflag, htrig_hodoshouldflag and otherthreehit flags
  //

  ++fNevt;

  return 0;
}

//_____________________________________________________________________________
Int_t THcHodoEff::ReadDatabase( const TDatime& date )
{
  // Read database. Gets variable needed for efficiency calculation
  // Get # of planes and their z positions here.

  fNPlanes = fHod->GetNPlanes();
  fPlanes = new THcScintillatorPlane* [fNPlanes];
  fPosZ = new Double_t[fNPlanes];
  fSpacing = new Double_t[fNPlanes];
  fCenterFirst = new Double_t[fNPlanes];
  fNCounters = new Int_t[fNPlanes];
  fHodoSlop = new Double_t[fNPlanes];

  for(Int_t ip=0;ip<fNPlanes;ip++) {
    fPlanes[ip] = fHod->GetPlane(ip);
    fPosZ[ip] = fPlanes[ip]->GetZpos() + 0.5*fPlanes[ip]->GetDzpos();
    fSpacing[ip] = fPlanes[ip]->GetSpacing();\
    fCenterFirst[ip] = fPlanes[ip]->GetPosCenter(0);
    fNCounters[ip] = fPlanes[ip]->GetNelem();
  }
  
  char prefix[2];
  prefix[0] = tolower((fHod->GetApparatus())->GetName()[0]);
  prefix[1] = '\0';

  DBRequest list[]={
    {"stat_slop", &fStatSlop, kDouble},
    {"stat_maxchisq",&fMaxChisq, kDouble},
    {"hodo_slop", fHodoSlop, kDouble, fNPlanes},
    {0}
  };
  //  fMaxShTrk = 0.05;		// For cut on fraction of momentum seen in shower
  gHcParms->LoadParmValues((DBRequest*)&list,prefix);

  cout << "THcHodoEff::ReadDatabase nplanes=" << fHod->GetNPlanes() << endl;
  // Setup statistics arrays
  // Better method to put this in?
  // These all need to be cleared in Begin
  fHitPlane = new Int_t[fNPlanes];
  fStatTrkDel.resize(fNPlanes);
  fStatAndHitDel.resize(fNPlanes);
  fStatPosHit.resize(fNPlanes);
  fStatNegHit.resize(fNPlanes);
  fStatAndHit.resize(fNPlanes);
  fStatOrHit.resize(fNPlanes);
  fBothGood.resize(fNPlanes);
  fPosGood.resize(fNPlanes);
  fNegGood.resize(fNPlanes);
  for(Int_t ip=0;ip<fNPlanes;ip++) {
    fStatTrkDel[ip].resize(fNCounters[ip]);
    fStatAndHitDel[ip].resize(fNCounters[ip]);
    fStatPosHit[ip].resize(fNCounters[ip]);
    fStatNegHit[ip].resize(fNCounters[ip]);
    fStatAndHit[ip].resize(fNCounters[ip]);
    fStatOrHit[ip].resize(fNCounters[ip]);
    fBothGood[ip].resize(fNCounters[ip]);
    fPosGood[ip].resize(fNCounters[ip]);
    fNegGood[ip].resize(fNCounters[ip]);
    for(Int_t ic=0;ic<fNCounters[ip];ic++) {
      fStatTrkDel[ip][ic].resize(20); // Max this settable
      fStatAndHitDel[ip][ic].resize(20); // Max this settable
    }
  }
  return kOK;
}
//_____________________________________________________________________________
ClassImp(THcHodoEff)
////////////////////////////////////////////////////////////////////////////////

