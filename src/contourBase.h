#pragma once

//Base class, contains getters, setters and generic stuff

#include <iostream>
#include <vector>

#include <TString.h>
#include <TStyle.h>
#include <TPaletteAxis.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TObject.h>
#include <THStack.h>
#include <TFile.h>
#include <TVirtualPad.h>
#include <TCanvas.h>
#include <TTree.h>

class contourBase{
    public:
        contourBase(TString histName, int massHierarchyOpt, int octantOpt, int burn_in);
        THStack* getCredibleStack(){return fullPosteriorStack;}

        TString setHistOptions();
        void plotContourHist(TString outFile); //NO EXTENSION HERE!

        template <typename T>
        void makePrettyHist(T inHist){    
            THStack* fullPosteriorStack = new THStack(_histTitle+"_intervals", _histTitle+"_intervals");
            for(auto &iHist : inHist){
                fullPosteriorStack->Add(iHist);
            }
        }
        
        THStack* fullPosteriorStack;
        TString _histTitle;

        int massHierarchy, octant, burnin;

        enum contourTypes{
            k1D=1, k2D=2
        };

        enum massHierarchy{
            kIH=-1, kBothHierarchy=0, kNH=1
        };

        enum octants{
            kLO=-1, kBothOct=0, kUO=1
        };
};