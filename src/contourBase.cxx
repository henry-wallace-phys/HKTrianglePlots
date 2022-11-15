#include "contourBase.h"

contourBase::contourBase(TString histName, int massHierarchyOpt, int octantOpt, int burn_in){
    _histTitle=histName;
    massHierarchy=massHierarchyOpt;
    octant=octantOpt;
    burnin=burn_in;
}

contourBase::~contourBase(){
	delete fullPosteriorStack;
}

TString contourBase::setHistOptions(){
    TString optStr;
    optStr.Form("(step>%d)", burnin);
    switch(massHierarchy){
        case kIH : optStr+="*(dm23<0)"; break;
        case kBothHierarchy : break;
        case kNH : optStr+="*(dm23>0)"; break;
    }

    switch(octant){
        case kLO : optStr+="*(th23<0.t)"; break;
        case kBothOct : break;
        case kUO : optStr+="*(th23>0)"; break;
    }

    return optStr;
}   

void contourBase::plotContourHist(TString outFile){
    gStyle->SetPalette(kCool);
    gStyle->SetOptStat(0);

    TFile* outFileROOT=new TFile(outFile+".root", "update");
    THStack* tempHist = (THStack*)outFileROOT->Get(_histTitle+"_intervals");
    if(tempHist){
        std::cout<<"Found "<<_histTitle<<"in "<<outFile<<".root, deleting"<<std::endl;
        outFileROOT->Delete(_histTitle+";1");
    }
    outFileROOT->cd();
    fullPosteriorStack->Write(_histTitle+"_intervals");

    TCanvas* canv = new TCanvas("PosteriorCanvas", "Posterior Canvas", 1200, 600);
    canv->cd();
    canv->Draw();
    fullPosteriorStack->Draw();
    gPad->BuildLegend();

    outFile+="_"+_histTitle;
    canv->Print(outFile+".pdf");
    outFileROOT->cd();
    canv->Write();
    outFileROOT->Close();
}

