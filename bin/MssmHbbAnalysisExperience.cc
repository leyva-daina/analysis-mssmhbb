#include "boost/program_options.hpp"
#include "boost/algorithm/string.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "TFile.h" 
#include "TFileCollection.h"
#include "TChain.h"
#include "TH1.h" 

#include "Analysis/MssmHbb/interface/MssmHbbAnalyser.h"

using namespace std;
using namespace analysis;
using namespace analysis::tools;
using namespace analysis::mssmhbb;

using TH1s = std::map<std::string, TH1F*>;
using TH2s = std::map<std::string, TH2F*>;
     

// float GetBTag(const Jet & jet, const std::string & algo);
// void CreateHistograms(TH1s &, const int & n = 3);
// void CreateHistograms(TH2s &, const int & n = 3);
// template <typename T>
// void WriteHistograms(T & );


// =============================================================================================   
int main(int argc, char ** argv)
{
   TH1::SetDefaultSumw2();  // proper treatment of errors when scaling histograms
   
   MssmHbbAnalyser mssmhbb(argc,argv);
   
   mssmhbb.jetHistograms(3,"initial");
   mssmhbb.jetHistograms(3,"after_bregression");
   if ( mssmhbb.config()->isMC() )
   {
      mssmhbb.jetHistograms(3,"after_jer");
      mssmhbb.jetHistograms(3,"before_btagsf");
      mssmhbb.jetHistograms(3,"after_btagsf");
   }
   mssmhbb.jetHistograms(3,"final");
   
   // Analysis of events
   std::cout << "The sample size is " << mssmhbb.analysis()->size() << " events" << std::endl;
   
   int seed = mssmhbb.seed();
   if ( seed > 0 ) std::cout << "Seed value for random number = " << seed << std::endl;
   else            std::cout << "NO seed value for random number :( " << std::endl;
   
// 
   for ( int i = 0 ; i < mssmhbb.nEvents() ; ++i )
   {
      if ( i > 0 && i%100000==0 ) std::cout << i << "  events processed! " << std::endl;
      bool goodEvent = mssmhbb.event(i);
      if ( ! goodEvent ) continue;
      
      if (!mssmhbb.config()->isMC() )
      {
      
      // trigger selection
         if ( ! mssmhbb.selectionHLT()           )   continue;
         if ( ! mssmhbb.selectionL1 ()           )   continue;  // to be used in case of "OR" of seeds
            
      // jet identification selection
         if ( ! mssmhbb.selectionJetId()         )   continue;
         if ( ! mssmhbb.selectionJetPileupId()   )   continue;
         if ( ! mssmhbb.selectionNJets()         )   continue;
         
         mssmhbb.fillJetHistograms("initial");
   
         
      //  1st and 2nd jet kinematic selection
         if ( ! mssmhbb.selectionJet(1)          )   continue;
         if ( ! mssmhbb.selectionJet(2)          )   continue;
         
      // jet delta eta 1,2 selection
         if ( ! mssmhbb.selectionJetDeta(1,2)    )   continue;
         
      // jets 1, 2 matching to online jets
         if ( ! mssmhbb.onlineJetMatching(1)     )   continue;
         if ( ! mssmhbb.onlineJetMatching(2)     )   continue;
         
      // btag of two leading jets
         if ( ! mssmhbb.selectionBJet(1)         )   continue;
         if ( ! mssmhbb.selectionBJet(2)         )   continue;
         
      // jets 1,2 matching to online btag objects
         if ( ! mssmhbb.onlineBJetMatching(1)    )   continue;
         if ( ! mssmhbb.onlineBJetMatching(2)    )   continue;
         
      // 3rd jet kinematic selection
         if ( ! mssmhbb.selectionJet(3)          )   continue;
         
         // delta R jet selection
         if ( ! mssmhbb.selectionJetDr(1,2)      )   continue;
         if ( ! mssmhbb.selectionJetDr(1,3)      )   continue;
         if ( ! mssmhbb.selectionJetDr(2,3)      )   continue;
         
      // 3rd jet btag selection
         if ( mssmhbb.config()->signalRegion()   )
         {
            if ( ! mssmhbb.selectionBJet(3)      )   continue;
         }
         else
         {
            if ( ! mssmhbb.selectionNonBJet(3)   )   continue;
         }
         
         mssmhbb.fillJetHistograms("final");
      }
      else // ==================== MONTE CARLO =========================
      {
      // jet identification selection
         if ( ! mssmhbb.selectionJetId()         )   continue;
         if ( ! mssmhbb.selectionJetPileupId()   )   continue;
         if ( ! mssmhbb.selectionNJets()         )   continue;
         mssmhbb.fillJetHistograms("initial");

      //  1st and 2nd jet kinematic selection
         if ( ! mssmhbb.selectionJet(1)          )   continue;
         if ( ! mssmhbb.selectionJet(2)          )   continue;
      // 3rd jet kinematic selection
         if ( ! mssmhbb.selectionJet(3)          )   continue;
         
      // btag of two leading jets
         if ( ! mssmhbb.selectionBJet(1)         )   continue;
         if ( ! mssmhbb.selectionBJet(2)         )   continue;
         
      // 3rd jet btag selection
         if ( mssmhbb.config()->signalRegion()   )
         {
            if ( ! mssmhbb.selectionBJet(3)      )   continue;
         }
         else
         {
            if ( ! mssmhbb.selectionNonBJet(3)   )   continue;
         }
         
      // delta R jet selection
         if ( ! mssmhbb.selectionJetDr(1,2)      )   continue;
         if ( ! mssmhbb.selectionJetDr(1,3)      )   continue;
         if ( ! mssmhbb.selectionJetDr(2,3)      )   continue;
         
      // jet delta eta 1,2 selection
         if ( ! mssmhbb.selectionJetDeta(1,2)    )   continue;
         
      // trigger selection
         if ( ! mssmhbb.selectionHLT()           )   continue;
         if ( ! mssmhbb.selectionL1 ()           )   continue;  // to be used in case of "OR" of seeds
      // jets 1, 2 matching to online jets
         if ( ! mssmhbb.onlineJetMatching(1)     )   continue;
         if ( ! mssmhbb.onlineJetMatching(2)     )   continue;
      // jets 1,2 matching to online btag objects
         if ( ! mssmhbb.onlineBJetMatching(1)    )   continue;
         if ( ! mssmhbb.onlineBJetMatching(2)    )   continue;
          
      // btag SF
         mssmhbb.fillJetHistograms("before_btagsf");
         mssmhbb.actionApplyBtagSF(1);
         mssmhbb.actionApplyBtagSF(2);
         mssmhbb.actionApplyBtagSF(3);
         mssmhbb.fillJetHistograms("after_btagsf");
         
      // jet energy resolution  
         mssmhbb.actionApplyJER();
         mssmhbb.fillJetHistograms("after_jer");
         
      // b energy regression
         mssmhbb.actionApplyBjetRegression();
         mssmhbb.fillJetHistograms("after_bregression");
         
      // final histograms
         mssmhbb.fillJetHistograms("final");
      }
      
   }
   
} //end main

