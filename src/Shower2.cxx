#include "Shower2.h"

#include "THcShower.h"
#include "THcHallCSpectrometer.h"
#include "THaEvData.h"
#include "THaDetMap.h"
#include "THcDetectorMap.h"
#include "THcGlobals.h"
#include "THaCutList.h"
#include "THcParmList.h"
#include "VarDef.h"
#include "VarType.h"
#include "THaTrack.h"
#include "TClonesArray.h"
#include "THaTrackProj.h"
#include "TMath.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <numeric>

#include "nlohmann/json.hpp" 

using namespace std;

namespace hcana {

THaAnalysisObject::EStatus Shower2::Init(const TDatime &date) {

  auto res = THcShower::Init(date);

  LoadJsonCalibration(_run_number);
 
  // hard coding cal_*_cal_const  to 0.001. 
  // Not sure what their purpose is... 

  for (UInt_t i=0; i<fNTotBlocks; i++) {
    fPosGain[i] = 0.001 *  _pos_gain_cor[i];
    fNegGain[i] = 0.001 *  _neg_gain_cor[i];
  }

  if(fHasArray) {
    auto gains = fArray->GetGains();
    for (Int_t i=0; i<fArray->GetNelem(); i++) {
      gains[i] = 0.001 * _arr_gain_cor[i];
    }
  }

  return fStatus = res;
}

void Shower2::LoadJsonCalibration(int rn, std::string fname) {
  // file checks assumed already complete.
  using nlohmann::json;
  json j_database;
  {
    std::ifstream json_input_file(fname);
    json_input_file >> j_database;
  }

  std::cout << " runs : ";
  std::vector<int> runs;
  for (json::iterator it = j_database.begin(); it != j_database.end(); ++it) {
    std::cout << it.key() << ", ";
    runs.push_back(std::stoi(it.key()));
  }
  std::cout << "\n";

  auto closest = [](std::vector<int> const &vec, int value) {
    auto it = std::lower_bound(vec.begin(), vec.end(), value);
    if (*it > value) {
      it = it - 1;
    }
    if (it == vec.end()) {
      return -1;
    }
    return *it;
  };
  // auto best_run = std::upper_bound(runs.begin(), runs.end(), rn);
  // std::cout << "using run upper " << rn << " :::: " << *best_run << "\n";
  // auto best_run = std::lower_bound(runs.begin(), runs.end(), rn);
  // std::cout << "using run lower " << rn << " :::: " << *(best_run) << "\n";

  int best_run = closest(runs, rn);
  std::cout << " closest " << best_run << std::endl;
  json j = j_database[std::to_string(best_run)];
  // std::cout << j_database[std::to_string(best_run)] << "\n";;
  // std::cout << j.dump(-1) << "\n";


  if (j.find("histogram") != j.end()) {
  }
  if (j.find("cal") != j.end()) {
    json j_cal = j["cal"];
    if (j_cal.find("neg_gain_cor") != j_cal.end()) {
      // std::cout << " neg_gain_cor!!!\n";
      _neg_gain_cor = j["cal"]["neg_gain_cor"].get<std::vector<double>>();
    }
    if (j_cal.find("pos_gain_cor") != j_cal.end()) {
      _pos_gain_cor = j["cal"]["pos_gain_cor"].get<std::vector<double>>();
    }
    if (j_cal.find("arr_gain_cor") != j_cal.end()) {
      _arr_gain_cor = j["cal"]["arr_gain_cor"].get<std::vector<double>>();
    }
  }
}

} // namespace hcana
