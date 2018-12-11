#include "Scandalizer.h"

using namespace std;

namespace hcana {

Int_t Scandalizer::ReadOneEvent()
{
  // Read one event from current run (fRun) and raw-decode it using the
  // current decoder (fEvData)

  if( fDoBench ) fBench->Begin("RawDecode");

  bool to_read_file = false;
  if( !fEvData->IsMultiBlockMode() ||
      (fEvData->IsMultiBlockMode() && fEvData->BlockIsDone()) )
    to_read_file = true;

  // Find next event buffer in CODA file. Quit if error.
  Int_t status = THaRunBase::READ_OK;
  if (to_read_file)
    status = fRun->ReadEvent();

  // there may be a better place to do this, but this works
  if (fWantCodaVers > 0) {
    fEvData->SetDataVersion(fWantCodaVers);
  } else {
    fEvData->SetDataVersion(fRun->GetDataVersion());
  }

  switch( status ) {
    case THaRunBase::READ_OK:
      // Decode the event
      if (to_read_file) {
        status = fEvData->LoadEvent( fRun->GetEvBuffer() );
      } else {
        status = fEvData->LoadFromMultiBlock( );  // load next event in block
      }
      switch( status ) {
        case THaEvData::HED_WARN:
          //std::cout << "HED_WARN\n";
        case THaEvData::HED_OK:     // fall through
          status = THaRunBase::READ_OK;
          Incr(kNevRead);
          break;
        case THaEvData::HED_ERR:
          // Decoding error
          status = THaRunBase::READ_ERROR;
          Incr(kDecodeErr);
          break;
        case THaEvData::HED_FATAL:
          status = THaRunBase::READ_FATAL;
          break;
      }
      break;

    case THaRunBase::READ_EOF:    // fall through
      std::cout << " READ_EOF\n";
    case THaRunBase::READ_FATAL:
      // Just exit on EOF - don't count it
      break;
    default:
      Incr(kCodaErr);
      break;
  }

  if( fDoBench ) fBench->Stop("RawDecode");
  return status;
}

Int_t Scandalizer::Process( THaRunBase* run )
{
  // Process the given run. Loop over all events in the event range and
  // analyze all apparatuses defined in the global apparatus list.
  // Fill Event structure if it is defined.
  // If Event and Filename are defined, then fill the output tree with Event
  // and write the file.

  static const char* const here = "Process";

  if( !run ) {
    if( fRun )
      run = fRun;
    else
      return -1;
  }

  //--- Initialization. Creates fFile, fOutput, and fEvent if necessary.
  //    Also copies run to fRun if run is different from fRun
  Int_t status = Init( run );
  if( status != 0 ) {
    return status;
  }

  // Restart "Total" since it is stopped in Init()
  fBench->Begin("Total");

  //--- Re-open the data source. Should succeed since this was tested in Init().
  if( (status = fRun->Open()) != THaRunBase::READ_OK ) {
    Error( here, "Failed to re-open the input file. "
	   "Make sure the file still exists.");
    fBench->Stop("Total");
    return -4;
  }

  // Make the current run available globally - the run parameters are
  // needed by some modules
  gHaRun = fRun;

  // Enable/disable helicity decoding as requested
  fEvData->EnableHelicity( HelicityEnabled() );
  // Set decoder reporting level. FIXME: update when THaEvData is updated
  fEvData->SetVerbose( (fVerbose>2) );
  fEvData->SetDebug( (fVerbose>3) );

  // Informational messages
  if( fVerbose>1 ) {
    cout << "Decoder: helicity "
	 << (fEvData->HelicityEnabled() ? "enabled" : "disabled")
	 << endl;
    cout << endl << "Starting analysis" << endl;
  }
  if( fVerbose>2 && fRun->GetFirstEvent()>1 )
    cout << "Skipping " << fRun->GetFirstEvent() << " events" << endl;

  //--- The main event loop.

  fNev = 0;
  bool terminate = false, fatal = false;
  UInt_t nlast = fRun->GetLastEvent();
  fAnalysisStarted = kTRUE;
  BeginAnalysis();
  if( fFile ) {
    fFile->cd();
    fRun->Write("Run_Data");  // Save run data to first ROOT file
  }
  void (*prev_handler)(int);
  prev_handler = signal (SIGINT, handle_sig);

  while ( !terminate && (fNev < nlast)) { 
    //&& (status = ReadOneEvent()) != THaRunBase::READ_EOF ) 
    //std::cout << " evtype(last) " << fEvData->GetEvType() << "\n";
    status = ReadOneEvent();
    //std::cout << " status " << status << "\n";
    //std::cout << " evtype " << fEvData->GetEvType() << "\n";

    // If an interupt signal is sent (ctrl-c)
    // // 
    if(sig_caught) {
      terminate = true;
      break;
    }
      

    if( status == THaRunBase::READ_EOF  ) {
      //if(  fEvData->GetEvType() != 20 ) {
      //  std::cout << " sleeping....";
      //  //gSystem->Sleep(1000);
      //}else {
      //}
      break;
    }

    //--- Skip events with errors, unless fatal
    if( status == THaRunBase::READ_FATAL ){
      std::cout << " READ_FATAL\n";
      break;
    }
    if( status != THaRunBase::READ_OK ){
      continue;
    }

    UInt_t evnum = fEvData->GetEvNum();

    // Count events according to the requested mode
    // Whether or not to ignore events prior to fRun->GetFirstEvent()
    // is up to the analysis routines.
    switch(fCountMode) {
    case kCountPhysics:
      if( fEvData->IsPhysicsTrigger() )
	fNev++;
      break;
    case kCountAll:
      fNev++;
      break;
    case kCountRaw:
      fNev = evnum;
      break;
    default:
      break;
    }

    //--- Print marks periodically
    if( (fVerbose>1) && (evnum > 0) && (evnum % fMarkInterval == 0)){
      cout << "test run: "<< fRun->GetNumber() << ", " << dec << evnum << endl;
    }
    if( (evnum > 0) &&(evnum % fAutoSaveInterval == 0)){
      fOutput->GetTree()->AutoSave("SaveSelf");
    }

    //--- Update run parameters with current event
    if( fUpdateRun ){
      fRun->Update( fEvData );
    }

    //--- Clear all tests/cuts
    if( fDoBench ) fBench->Begin("Cuts");
    gHaCuts->ClearAll();
    if( fDoBench ) fBench->Stop("Cuts");

    //--- Perform the analysis
    Int_t err = MainAnalysis();
    switch( err ) {
    case kOK:
      break;
    case kSkip:
      continue;
    case kFatal:
      fatal = terminate = true;
      continue;
    case kTerminate:
      terminate = true;
      break;
    default:
      Error( here, "Unknown return code from MainAnalysis(): %d", err );
      terminate = fatal = true;
      continue;
    }

    Incr(kNevAccepted);

  }  // End of event loop

  signal (SIGINT, prev_handler);
  EndAnalysis();

  //--- Close the input file
  fRun->Close();

  // Save final run parameters in run object of caller, if any
  *run = *fRun;

  // Write the output file and clean up.
  // This writes the Tree as well as any objects (histograms etc.)
  // that are defined in the current directory.

  if( fDoBench ) fBench->Begin("Output");
  // Ensure that we are in the output file's current directory
  // ... someone might have pulled the rug from under our feet

  // get the CURRENT file, since splitting might have occurred
  if( fOutput && fOutput->GetTree() )
    fFile = fOutput->GetTree()->GetCurrentFile();
  if( fFile )   fFile->cd();
  if( fOutput ) fOutput->End();
  if( fFile ) {
    fRun->Write("Run_Data");  // Save run data to ROOT file
    //    fFile->Write();//already done by fOutput->End()
    fFile->Purge();         // get rid of excess object "cycles"
  }
  if( fDoBench ) fBench->Stop("Output");

  fBench->Stop("Total");

  //--- Report statistics
  if( fVerbose>0 ) {
    cout << dec;
    if( status == THaRunBase::READ_EOF )
      cout << "End of file";
    else if ( fNev == nlast )
      cout << "Event limit reached.";
    else if ( fatal )
      cout << "Fatal processing error.";
    else if ( terminate )
      cout << "Terminated during processing.";
    cout << endl;

    if( !fatal ) {
      PrintCounters();

      if( fVerbose>1 )
	PrintScalers();
    }
  }

  // Print cut summary (also to file if one given)
  //if( !fatal )
  //  PrintCutSummary();

  // Print timing statistics, if benchmarking enabled
  //if( fDoBench && !fatal ) {
  //  cout << "Timing summary:" << endl;
  //  fBench->Print("Init");
  //  fBench->Print("RawDecode");
  //  fBench->Print("Decode");
  //  fBench->Print("CoarseTracking");
  //  fBench->Print("CoarseReconstruct");
  //  fBench->Print("Tracking");
  //  fBench->Print("Reconstruct");
  //  fBench->Print("Physics");
  //  fBench->Print("Output");
  //  fBench->Print("Cuts");
  //}
  //if( (fVerbose>1 || fDoBench) && !fatal )
  //  fBench->Print("Total");

  //keep the last run available
  //  gHaRun = NULL;
  return fNev;
}

}
