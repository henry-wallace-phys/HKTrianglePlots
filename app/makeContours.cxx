#include "contours2D.h"
#include "contours1D.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TPad.h"
#include <iostream>

TString gOutFileName, gInFileName;
int gOctOpt=0;
int gHierarchyOpt=0;


int parseargs(int argc, char **argv);

int main(int argc, char **argv){
    if(parseargs(argc, argv)!=0){
        std::cerr<<"Usage is: ./makeContours [-i inputFile] [-o outputFile] [-c Octant=(-1,0,1)] [-h Hiearachy=(-1,0,1)]"<<std::endl;
        throw;
    }
    int nBins=1000;
    std::vector<TString> oscPars = {"theta23", "theta13", "dm23", "dcp"};

    std::vector<double> lowerBounds = {0.4, 0.01, 0.02, 0.01, -TMath::Pi()};
    std::vector<double> upperBounds = {0.7, 0.02, 0.04, 0.02, TMath::Pi()};
    
    int burnin=100000;

    //Now to making the plots
    TCanvas* triCanv = new TCanvas("triCanv", "Canvas for Triangle Plots", 2000, 2000);
    triCanv->cd();
    triCanv->Draw();
    triCanv->Print(gOutFileName+".pdf[");
    //Loop over our hists
    for(int iPar1=0; iPar1<(int)oscPars.size(); iPar1++){
		std::cout<<"Making contours for "<<oscPars[iPar1]<<std::endl;
        std::vector<contours2D> tmpVec;
        THStack* tmpStack;
        for(int iPar2=0; iPar2<=iPar1; iPar2++){
            if(iPar1==iPar2){
                contours1D* tmpCont = new contours1D(gInFileName, oscPars[iPar1], oscPars[iPar1], lowerBounds[iPar1], upperBounds[iPar1], nBins, gHierarchyOpt, gOctOpt, burnin);
               
                tmpCont->plotContourHist(gOutFileName);
                tmpStack = (THStack*)tmpCont->getCredibleStack();
            }
            else{            
                contours2D* tmpCont = new contours2D(gInFileName, oscPars[iPar1], oscPars[iPar2], oscPars[iPar1], oscPars[iPar2],
                 lowerBounds[iPar1], upperBounds[iPar1], nBins,
                 lowerBounds[iPar2], upperBounds[iPar2], nBins,
                 gHierarchyOpt, gOctOpt, burnin);

                tmpCont->plotContourHist(gOutFileName);
                tmpStack = (THStack*)tmpCont->getCredibleStack();
            }

            triCanv->cd();
            TPad* tmpPad=new TPad("tmpPad", "tmpPad", 0.1+0.2*iPar2, 0.7-0.2*iPar1, 0.3+0.2*iPar2, 0.9-0.2*iPar1);
            tmpPad->cd();
            tmpPad->Draw();
            
            tmpStack->Draw();
            tmpPad->Draw();
        }
    }
    TFile* outFileROOT=new TFile(gOutFileName+".root", "UPDATE");
    outFileROOT->cd();
    triCanv->Write(0, TObject::kOverwrite);
    triCanv->Print(gOutFileName+".pdf]");
    
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
        default:
            std::cerr << "Unrecognised flag!" << std::endl;
            std::cerr<<"Usage is: ./makeContours [-i inputFile] [-o outputFile] [-c Octant=(-1,0,1)] [-h Hiearachy=(-1,0,1)]"<<std::endl;
            throw;
            break;
        }
    ++argv; --argc;
    }
    return 0;
}
