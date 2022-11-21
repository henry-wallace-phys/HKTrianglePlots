#include "contours2D.h"
#include "contours1D.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TPad.h"
#include <iostream>
#include <vector>


TString gOutFileName, gInFileName;
int gOctOpt=0;
int gHierarchyOpt=0;
int gNBins1D=1000;
int gNBins2D=500;


int parseargs(int argc, char **argv);
void usage();

int main(int argc, char **argv){
    gStyle->SetPalette(kCool);
    gStyle->SetOptStat(0);

    if(parseargs(argc, argv)!=0) usage();
    


    std::vector<TString> oscPars = {"theta23", "theta13", "dm23", "dcp"};

    std::vector<THStack* > Hist1DVec;
    std::vector<std::vector<TH2D* > > Hist2DVec;

    std::vector<double> lowerBounds = {0.3, 0.0, -0.005, -TMath::Pi()};
    std::vector<double> upperBounds = {0.7, 0.1, 0.005, TMath::Pi()};
    
    int burnin=0;

    for(int iPar1=0; iPar1<(int)oscPars.size(); iPar1++){
		std::cout<<"Making contours for "<<oscPars[iPar1]<<std::endl;
        for(int iPar2=0; iPar2<=iPar1; iPar2++){
            if(iPar1==iPar2){
                contours1D* tmpCont_1D = new contours1D(gInFileName, oscPars[iPar1], oscPars[iPar1], lowerBounds[iPar1], upperBounds[iPar1], gNBins1D, gHierarchyOpt, gOctOpt, burnin);
                THStack* tmpStack = tmpCont_1D->getCredibleStack();
                Hist1DVec.push_back(tmpStack);
                continue;
            } //1D 

            contours2D* tmpCont = new contours2D(gInFileName, oscPars[iPar1], oscPars[iPar2], oscPars[iPar1], oscPars[iPar2],
                 lowerBounds[iPar1], upperBounds[iPar1], gNBins2D,
                 lowerBounds[iPar2], upperBounds[iPar2], gNBins2D,
                 gHierarchyOpt, gOctOpt, burnin);

            TH2D* posteriorHist = tmpCont->getPosteriorHist();
            std::vector<TH2D*> contVec_reverse = tmpCont->getContours();
            std::vector<TH2D*> contVec(contVec_reverse.size()+1);

            contVec[0]=posteriorHist;
            for(int iHist=0; iHist<(int)contVec_reverse.size(); iHist++){
                contVec[iHist+1]=contVec_reverse[iHist];
            }

            Hist2DVec.push_back(contVec);
        } //iPar2 Loop
    }//iPar1 Loop

    //NOW WE PLOT (really I'd really have done this some nicer way!)
    std::cout<<"Making plots"<<std::endl;
    TCanvas* canv = new TCanvas("canv", "canv", 2000, 2000);
    canv->Divide(oscPars.size(), oscPars.size());
    for(int iPar1=0; iPar1<(int)oscPars.size(); iPar1++){
		std::cout<<"Making contours for "<<oscPars[iPar1]<<std::endl;
        for(int iPar2=0; iPar2<=iPar1; iPar2++){
            TPad* tmpPad = new TPad(oscPars[iPar1]+"_"+oscPars[iPar2], oscPars[iPar1]+"_"+oscPars[iPar2], 0.1+0.2*iPar1, 0.7-0.2*iPar2, 0.3+0.2*iPar1, 0.9-0.2*iPar2);
            canv->cd();
            tmpPad->Draw();
            
            //Draw 1D
            if(iPar1==iPar2){
                tmpPad->cd();
                Hist1DVec[iPar1]->Draw("pfc nostack");
                continue;
            }
            
            tmpPad->cd();
            Hist2DVec[iPar1][0]->Draw("COL");

            for(int iHist=1; iHist<Hist2DVec[iPar1].size(); iHist++){
                tmpPad->cd();
                TH2D* tmpHist = Hist2DVec[iPar1][iHist];
                tmpHist->SetLineColor(kBlack);
                tmpHist->Smooth(1);
                tmpHist->Draw("SAME CONT3");
            }

        }//iPar2
    }//iPar1
    canv->Print(gOutFileName+"_triangle.pdf");
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