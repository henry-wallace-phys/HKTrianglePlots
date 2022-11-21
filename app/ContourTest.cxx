#include "contours2D.h"
#include "contours1D.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TPad.h"
#include <iostream>

int main(int argc, char **argv){
    TString inFileName = "../Files/HKOA2020_StatOnly_SmallLLH_reduced.root";
    TString oscPar="theta23";

    contours1D* tmpCont = new contours1D(inFileName, oscPar, oscPar, 0.3, 0.7, 1000, 0, 0, 0);

    THStack* stack = tmpCont->getCredibleStack();
    gStyle->SetPalette(kCool);
    gStyle->SetOptStat(0);
    TCanvas* canv = new TCanvas("c", "c", 1200, 600);
    canv->Draw();
    canv->cd();
    stack->Draw("pfc nostack");
    canv->Print("Yeet.pdf");
    std::cout<<"DONE \n";
}


