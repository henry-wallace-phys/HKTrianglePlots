#include "contourBase.h"

contourBase::contourBase(TString histName, int massHierarchyOpt, int octantOpt, int burn_in){
    _histTitle=histName;
    massHierarchy=massHierarchyOpt;
    octant=octantOpt;
    burnin=burn_in;
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

	if(!fullPosteriorStack){
		std::cerr<<"Can't find your stack"<<std::endl;
		throw;
	}
    TCanvas* canv = new TCanvas(_histTitle, _histTitle, 1200, 600);
    canv->Draw();
    canv->cd();
	fullPosteriorStack->Draw("pfc nostack");
    gPad->BuildLegend();

    outFile+="_"+_histTitle;
    canv->Print(outFile+".pdf");

}

