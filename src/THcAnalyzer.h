#ifndef ROOT_THcAnalyzer
#define ROOT_THcAnalyzer

//////////////////////////////////////////////////////////////////////////
//
// THcAnalyzer
//
//////////////////////////////////////////////////////////////////////////

#include "THaAnalyzer.h"
#include "THaRunBase.h"
#include "THaEvent.h"
#include "THaOutput.h"
#include "THaEvData.h"
#include "THaGlobals.h"
#include "THaSpectrometer.h"
#include "THaNamedList.h"
#include "THaCutList.h"
#include "THaCut.h"
#include "THaPhysicsModule.h"
#include "THaPostProcess.h"
#include "THaBenchmark.h"
#include "THaEvtTypeHandler.h"
#include "THaEpicsEvtHandler.h"
#include "TList.h"
#include "TTree.h"
#include "TFile.h"
#include "TClass.h"
#include "TDatime.h"
#include "TClass.h"
#include "TError.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TMath.h"
#include "TDirectory.h"
#include "THaCrateMap.h"

#include <algorithm>
#include <csignal>
#include <cstring>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

class THcAnalyzer : public THaAnalyzer {

public:

  THcAnalyzer();
  virtual ~THcAnalyzer();

  void SetPedestalEvtype( Int_t evtype ) { fPedestalEvtype = evtype; }

  void PrintReport( const char* templatefile, const char* ofile);

protected:

  Int_t fPedestalEvtype;

private:
  //  THcAnalyzer( const THcAnalyzer& );
  //  THcAnalyzer& operator=( const THcAnalyzer& );
  void LoadInfo();

  ClassDef(THcAnalyzer,0)  //Hall C Analyzer Standard Event Loop

};

#endif
