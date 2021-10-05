/////////////////////////////////////////////////////////////////////////////////////////
//  Steve Quinn     October 2013                                                        //
//  Alex Dombos             2014         Added Traces, TACs, PINs, DSSD, and Veto	//
//  Antonius Torode   11-29-2017         Modified for use with e17503                   //
//  Caley Harris    October 2018         Modified for use with e16033                   //
//  Mallory Smith   October 2018         Modified to use on fireside                    //
//                  March 2019           Modified for use with e17028                   //
//////////////////////////////////////////////////////////////////////////////////////////
// PURPOSE: Take the output ROOT file from ddasdumper and turn it into another ROOT     //
//   file with a pretty SuN tree.                                                       //
//                                                                                      //
// OPTIONS: Change the variable "timewindow" to the amount of time that you want to     //
//   consider for coincidences.  For example a time window of 300 ns means              //
//   that you group everything up to 300 ns after the trigger as a single event.        //
//   (This is assuming you are using the 100 MSPS modules)                              //
//                                                                                      //
// HOW TO RUN:                                                                          //
//   1.  To uncompile  <terminal> make clean                                            //
//   2.  To compile    <terminal> make                                                  //
//   3.  Before compiling, make sure that the timewindow, input file, and output file   //
//        are what you want them to be.                                                 //
//                                                                                      //
// IMPORTANT: Have Fun =)                                                               //
//////////////////////////////////////////////////////////////////////////////////////////

#include <iomanip>
#include <fstream>

#include "ddaschannel.h"
#include "DDASEvent.h"

#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

int main(int argc, char* argv[]){

  //***** TO CHANGE THE COINCIDENCE WINDOW *****//
  int timewindow = 10000;  // (nanoseconds for 100 MSPS module)
  //..................test..........................//

  std::cout << "runConverter is working on: " << argv[1] << std::endl;

  // Filenames
  TFile *fIn  = new TFile(Form("/mnt/analysis/e20019/dumpedfiles/run%s.root", argv[1]), "READ");
  TFile *fOut = new TFile(Form("/mnt/analysis/e20019/rootfiles/run%s_10us.root", argv[1]), "RECREATE");
  std::cout << "Now processing run " << argv[1] << std::endl;

  // Variables for input file
  int crate;                // event crate number
  int slot;                 // event slot number
  int chan;                 // event channel number
  int energy;               // event energy
  double time;              // event time
  double refT = 0.0;        // placeholder for time of previous trigger
  double deltaT;            // difference in time between events
  int nEntries;             // number of entries
  int nEvents;              // number of events per entry

  // Variables for output file
  vector<int> e_Pulser_JENSA;
  vector<double> t_Pulser_JENSA;

  vector<int> e_SiMon1;
  vector<double> t_SiMon1;
  int m_SiMon1;

  vector<int> e_SiMon2;
  vector<double> t_SiMon2;
  int m_SiMon2;

  vector<int> c_LENDA;
  vector<int> e_LENDA;
  vector<double> t_LENDA;
  int m_LENDA;

  vector<int> c_LScin;
  vector<int> e_LScin;
  vector<double> t_LScin;
  int m_LScin;

  vector<int> e_UMCP_t;
  vector<double> t_UMCP_t;
  int m_UMCP_t;

  vector<int> e_IC_dE;
  vector<double> t_IC_dE;
  int m_IC_dE;

  vector<int> c_Si;
  vector<int> e_Si;
  vector<double> t_Si;
  int m_Si;

  vector<int> e_Pulser_FP;
  vector<double> t_Pulser_FP;

  double tTrigger = 0.0; // trigger time (the time of the first event in an entry)
  double max = 0.0;
  double tmax = 0.0;
  int counter = 10; // counter for status bar

  // Create tree and branches for output file
  TTree *tOut = new TTree("t","FP Tree");
  tOut->Branch("energy_Pulser_JENSA", "vector<int>", &e_Pulser_JENSA);
  tOut->Branch("time_Pulser_JENSA", "vector<double>", &t_Pulser_JENSA);

  tOut->Branch("energy_SiMon1", "vector<int>", &e_SiMon1);
  tOut->Branch("time_SiMon1", "vector<double>", &t_SiMon1);
  tOut->Branch("mult_SiMon1",&m_SiMon1,"m_SiMon1/I");

  tOut->Branch("energy_SiMon2", "vector<int>", &e_SiMon2);
  tOut->Branch("time_SiMon2", "vector<double>", &t_SiMon2);
  tOut->Branch("mult_SiMon2",&m_SiMon2,"m_SiMon2/I");

  tOut->Branch("chan_LENDA", "vector<int>", &c_LENDA);
  tOut->Branch("energy_LENDA", "vector<int>", &e_LENDA);
  tOut->Branch("time_LENDA", "vector<double>", &t_LENDA);
  tOut->Branch("mult_LENDA",&m_LENDA,"m_LENDA/I");

  tOut->Branch("chan_LScin", "vector<int>", &c_LScin);
  tOut->Branch("energy_LScin", "vector<int>", &e_LScin);
  tOut->Branch("time_LScin", "vector<double>", &t_LScin);
  tOut->Branch("mult_LScin",&m_LScin,"m_LScin/I");

  tOut->Branch("energy_UMCP_t", "vector<int>", &e_UMCP_t);
  tOut->Branch("time_UMCP_t", "vector<double>", &t_UMCP_t);
  tOut->Branch("mult_UMCP_t",&m_UMCP_t,"m_UMCP_t/I");

  tOut->Branch("energy_IC_dE", "vector<int>", &e_IC_dE);
  tOut->Branch("time_IC_dE", "vector<double>", &t_IC_dE);
  tOut->Branch("mult_IC_dE",&m_IC_dE,"m_IC_dE/I");

  tOut->Branch("chan_Si", "vector<int>", &c_Si);
  tOut->Branch("energy_Si", "vector<int>", &e_Si);
  tOut->Branch("time_Si", "vector<double>", &t_Si);
  tOut->Branch("mult_Si",&m_Si,"m_Si/I");

  tOut->Branch("energy_Pulser_FP", "vector<int>", &e_Pulser_FP);
  tOut->Branch("time_Pulser_FP", "vector<double>", &t_Pulser_FP);

  tOut->Branch("tmax",&tmax,"tmax/D");
  tOut->Branch("trigger",&tTrigger,"time/D");

  // Get DDAS tree from input file
  fIn->cd();
  TTree *tIn = (TTree*)fIn->Get("dchan");
  DDASEvent *dEvent = new DDASEvent();
  tIn->SetBranchAddress("ddasevent",&dEvent);

  nEntries = tIn->GetEntries();

  std::vector<bool> first_slot = {true,true,true,true,true,true,true,true};

  // Read in data, entry-by-entry
  for (int i=0; i<nEntries; i++){
    tIn->GetEntry(i);
    // status bar
    if (i % (int)(0.1*nEntries) == 0){
      std::cerr << counter << " ";
      if (counter==0){
        std::cerr << std::endl;
      }
      counter --;
    }
    // Readout now has correlations, so entries may have multiple events
    nEvents = dEvent->GetNEvents();
    // Read in data event-by-event
    for (int j=0; j<nEvents; j++){
      // Get all the input variables we need
      ddaschannel *dchan = dEvent->GetData()[j];
      crate  = dchan->GetCrateID();
      slot   = dchan->GetSlotID();
      chan   = dchan->GetChannelID();
      energy = dchan->GetEnergy();
      time   = dchan->GetTime();

      deltaT = time - refT;

      // If the new time is outside of the timewindow, calculate the multiplicities, fill the tree,
      // save the new timestamp, and set everything back to zero
      if (i==0 || deltaT > timewindow){// zero all variable for the first entry in each new time window
        tOut->Fill();

        refT = time;
        max = 0.0;
        tmax = 0.0;

        c_LENDA.clear();
        e_LENDA.clear();
        t_LENDA.clear();
        m_LENDA = 0;

        c_LScin.clear();
        e_LScin.clear();
        t_LScin.clear();
        m_LScin = 0;

        e_Pulser_JENSA.clear();
        t_Pulser_JENSA.clear();

        e_SiMon1.clear();
        t_SiMon1.clear();
        m_SiMon1 = 0;

        e_SiMon2.clear();
        t_SiMon2.clear();
        m_SiMon2 = 0;

        e_UMCP_t.clear();
        t_UMCP_t.clear();
        m_UMCP_t = 0;

        e_IC_dE.clear();
        t_IC_dE.clear();
        m_IC_dE = 0;

        c_Si.clear();
        e_Si.clear();
        t_Si.clear();
        m_Si = 0;

        e_Pulser_FP.clear();
        t_Pulser_FP.clear();
      }

      //..................Set tree variables to correct values..................//

      if(crate==0){
        if(slot>=2 && slot<=4){
          c_LENDA.push_back((slot-2)*16+chan);
          e_LENDA.push_back(energy);
          t_LENDA.push_back(time);
          m_LENDA++;
        }
        else if(slot==5){
          c_LScin.push_back(chan);
          e_LScin.push_back(energy);
          t_LScin.push_back(time);
          m_LScin++;
        }
        else if(slot==6){
          if(chan==0){//SiMon1
            e_SiMon1.push_back(energy);
            t_SiMon1.push_back(time);
            m_SiMon1++;
          }
          else if(chan==1){//SiMon2
            e_SiMon2.push_back(energy);
            t_SiMon2.push_back(time);
            m_SiMon2++;
          }
          else if(chan==3){//JENSA pulser
            e_Pulser_JENSA.push_back(energy);
            t_Pulser_JENSA.push_back(time);
          }
        }
      }
      else if(crate==1){
        if(slot==2){
          if(chan==10){//IC_dE
            e_IC_dE.push_back(energy);
            t_IC_dE.push_back(time);
            m_IC_dE++;
          }
          else if(chan==12){//UMCP timing
            e_UMCP_t.push_back(energy);
            t_UMCP_t.push_back(time);
            m_UMCP_t++;
          }
          else if(chan==15){//FP pulser
            e_Pulser_FP.push_back(energy);
            t_Pulser_FP.push_back(time);
          }
        }
        else if(slot>=3 && slot<=7){
          c_Si.push_back((slot-3)*16+chan);
          e_Si.push_back(energy);
          t_Si.push_back(time);
          m_Si++;
        }
      }
      tTrigger = refT; // Trigger time
    } // finish loop over j=events
  } // finish loop over i=entries
  // Write to output file
  fOut->cd();
  tOut->Write();
  fOut->Close();
  return 0;
}
