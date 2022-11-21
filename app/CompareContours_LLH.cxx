#include "contours2D.h"
#include "contours1D.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TPad.h"
#include <iostream>

TString gOutFileName, gInFileName, gLLHFileName;
int gOctOpt=0;
int gHierarchyOpt=0;
int gNBins1D=1000;
int gNBins2D=500;

int parseargs(int argc, char **argv);
void usage();

int main(int argc, char **argv){
    if(parseargs(argc, argv)!=0) usage();
    gStyle->SetPalette(kCool);
    gStyle->SetOptStat(0);

    std::vector<TString> oscPars = {"theta23", "theta13", "dcp"};
    std::vector<TString > llhNames = {"theta_23", "theta_13", "delta_cp"};
    std::vector<double> lowerBounds = {0.3, 0.0, -TMath::Pi()};
    std::vector<double> upperBounds = {0.7, 0.1, TMath::Pi()};
    
    std::vector<std::vector<TH2D*>> histVec;

    int burnin=0;
    
    TFile* llhFile = new TFile(gLLHFileName);

    std::vector<std::vector<TH2D* > > Hist2DVec;

    //Now to making the plots
    //Loop over our hists
    for(int iPar1=0; iPar1<(int)oscPars.size(); iPar1++){
		std::cout<<"Making contours for "<<oscPars[iPar1]<<std::endl;
        std::vector<contours2D> tmpVec;

        contours2D* tmpCont;
        for(int iPar2=0; iPar2<=iPar1; iPar2++){
            if(iPar1==iPar2){
                continue;
            }
            TH2D* LLHPlot=NULL;
            llhFile->GetObject("llh_scan_"+llhNames[iPar1]+"_"+llhNames[iPar2], LLHPlot);
          
            if(LLHPlot){
                tmpCont = new contours2D(gInFileName, oscPars[iPar1], oscPars[iPar2], oscPars[iPar1], oscPars[iPar2],
                    lowerBounds[iPar1], upperBounds[iPar1], gNBins2D,
                    lowerBounds[iPar2], upperBounds[iPar2], gNBins2D,
                    gHierarchyOpt, gOctOpt, burnin);
                LLHPlot->GetXaxis()->SetTitle(oscPars[iPar1]);
                LLHPlot->GetYaxis()->SetTitle(oscPars[iPar2]);
            }
            else{
                llhFile->GetObject("llh_scan_"+llhNames[iPar2]+"_"+llhNames[iPar1], LLHPlot);
                if(!LLHPlot){
                    std::cout<<"Couldn't find LLH scan for combination of "<<llhNames[iPar2]+"_"+llhNames[iPar1]<<std::endl;
                    continue;
                }
                tmpCont = new contours2D(gInFileName, oscPars[iPar2], oscPars[iPar1], oscPars[iPar2], oscPars[iPar1],
                    lowerBounds[iPar2], upperBounds[iPar2], gNBins2D,
                    lowerBounds[iPar1], upperBounds[iPar1], gNBins2D,
                    gHierarchyOpt, gOctOpt, burnin);
                LLHPlot->GetXaxis()->SetTitle(oscPars[iPar2]);
                LLHPlot->GetYaxis()->SetTitle(oscPars[iPar1]);
            }

            std::vector<TH2D*> contourVec = tmpCont->getContours();
            
            std::vector<TH2D*> tmpVec;

            LLHPlot->SetContour(300);            
            tmpVec.push_back(LLHPlot);

            for(TH2D* iHist : contourVec){
                tmpVec.push_back(iHist);
            }
            histVec.push_back(tmpVec);
        }
    }

    TCanvas* canv = new TCanvas("canv", "canv", 1200, 600);
    canv->Print(gOutFileName+".pdf[");
    for(int iHistVec=0; iHistVec<(int)histVec.size(); iHistVec++){
        canv->cd();
        histVec[iHistVec][0]->Draw("COLZ");
        for(int iHist=1; iHist<histVec[iHistVec].size(); iHist++){
            canv->cd();
            histVec[iHistVec][iHist]->Smooth(1);
            histVec[iHistVec][iHist]->SetLineColor(kBlack);

            histVec[iHistVec][iHist]->Draw("SAME CONT3");
        }
        canv->Print(gOutFileName+".pdf");
    }
    canv->Print(gOutFileName+".pdf]");

    return 0;
}

int parseargs(int argc, char **argv){
    while( (argc > 1) && (argv[1][0] == '-') ){
        switch(argv[1][1]){
        case 'i':
            gInFileName = argv[2];
            std::cout << "Input File: " << gInFileName << std::endl;
            ++argv; --argc;
            break;
        case 'o':
            gOutFileName = argv[2];
            std::cout << "Output file: " << gOutFileName << std::endl;
            ++argv; --argc;
            break;
        case 'l':
            gLLHFileName = argv[2];
            std::cout << "LLH Scan File : "<<gLLHFileName<<std::endl;
            ++argv; --argc;
            break;
        case 'c':
            gOctOpt = atoi(argv[2]);
            std::cout << "Octant: " << gOctOpt << std::endl;
            ++argv; --argc;
            break;
        case 'h':
            gHierarchyOpt = atoi(argv[2]);
            std::cout << "Hierarchy: " << gHierarchyOpt << std::endl;
            ++argv; --argc;
            break;
        case 'b':
            gNBins1D = atoi(argv[2]);
            std::cout<<"Number of 1D bins "<<gNBins1D<<std::endl;
            ++argv; --argc;
            break;
        case 't':
            gNBins2D = atoi(argv[2]);
            std::cout<<"Number of 2D bins "<<gNBins2D<<std::endl;
            ++argv; --argc;
            break;
        default:
            std::cerr << "Unrecognised flag!" << std::endl;
            usage();
            break;
        }
    ++argv; --argc;
    }
    return 0;
}

void usage(){
        std::cerr<<"Usage is: ./makeContours [-i inputFile] [-o outputFile] [-c Octant=(-1,0,1)] [-h Hiearachy=(-1,0,1)] [-b number of 1d bins] [-t number of 2d bins]"<<std::endl;
        throw;
}