//
// Author:  Jan Heyninck
// Created: Tue Apr  3 17:33:23 PDT 2007
//
// $Id: TtSemiLRSignalSelCalc.cc,v 1.1 2007/06/01 09:10:19 heyninck Exp $
//
#include "TopQuarkAnalysis/TopEventSelection/interface/TtSemiLRSignalSelCalc.h"

// constructor with path; default should not be used
TtSemiLRSignalSelCalc::TtSemiLRSignalSelCalc(TString fitInputPath, std::vector<int> observables) {

  std::cout << "=== Constructing a TtSemiLRSignalSelCalc... " << std::endl; 
  myLR = new LRHelpFunctions();
  addPurity = false;
  if(observables[0] == -1) addPurity = true;
  myLR -> readObsHistsAndFits(fitInputPath, observables, addPurity);
  std::cout << "=== done." << std::endl;

}


// destructor
TtSemiLRSignalSelCalc::~TtSemiLRSignalSelCalc() {
  delete myLR;
}

void  TtSemiLRSignalSelCalc::operator()(TtSemiEvtSolution & sol){
  // find the used observables
  std::vector<double> obsVals;
  for(unsigned int o = 0; o<100; o++){
    if( myLR->obsFitIncluded(o) ) { std::cout<<"uses obs value of obs"<<o<<std::endl; obsVals.push_back(sol.getLRSignalEvtObsVal(o)); };
  }
  
  // calculate the logLR and the purity
  double logLR = myLR->calcLRval(obsVals);
  double prob  = -999.;
  if(addPurity) myLR->calcProb(logLR);
  
  // fill these values to the members in the TtSemiEvtSolution
  sol.setLRSignalEvtLRval(logLR);
  sol.setLRSignalEvtProb(prob);
  std::cout<<"  Found logLR = "<<logLR<<" and Prob = "<<prob<<std::endl;
}
