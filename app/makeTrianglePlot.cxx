#include <iostream>
#include <vector>

#include "contours2D.h"
#include "contours1D.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TGaxis.h"
#include "TPaveText.h"

TString gOutFileName, gInFileName;
int gOctOpt=0;
int gHierarchyOpt=0;
int gNBins1D=300;
int gNBins2D=100;


int parseargs(int argc, char **argv);
void usage();

int main(int argc, char **argv){
    gStyle->SetPalette(kCool);
    gStyle->SetOptStat(0);
  	gStyle->SetTextFont(132) ;
  	TGaxis::SetMaxDigits(3);

    if(parseargs(argc, argv)!=0) usage();
    


    std::vector<TString> oscPars = {"theta23", "theta13", "dm23", "dcp"};

    std::vector<THStack* > Hist1DVec;
	std::vector<std::vector<std::vector<TH2D* > > > Hist2DVec;

	std::vector<double> lowerBounds;
	std::vector<double> upperBounds;
	if(gHierarchyOpt==1){
	    lowerBounds = {0.42, 0.016, 0.00244, -TMath::Pi()};
	    upperBounds = {0.58, 0.031, 0.00261, TMath::Pi()};
    }
	else if(gHierarchyOpt==0){
	    lowerBounds = {0.42, 0.016, -0.004, -TMath::Pi()};
	    upperBounds = {0.58, 0.03, 0.003, TMath::Pi()};
    }
	else if(gHierarchyOpt==-1){
	    lowerBounds = {0.42, 0.016, -0.00265, -TMath::Pi()};
	    upperBounds = {0.58, 0.03, -0.0025, TMath::Pi()};
    }

    int burnin=150000;

    for(int iPar1=0; iPar1<(int)oscPars.size(); iPar1++){
		std::cout<<"Making contours for "<<oscPars[iPar1]<<std::endl;
		std::vector<std::vector<TH2D* > > tmp_hist_vec;
        for(int iPar2=0; iPar2<=iPar1; iPar2++){
			if(iPar1==iPar2){
                contours1D* tmpCont_1D = new contours1D(gInFileName, oscPars[iPar1], oscPars[iPar1], lowerBounds[iPar1], upperBounds[iPar1], gNBins1D, gHierarchyOpt, gOctOpt, burnin);
                THStack* tmpStack = tmpCont_1D->getCredibleStack();
                Hist1DVec.push_back(tmpStack);
                continue;
            } //1D

            contours2D* tmpCont = new contours2D(gInFileName, oscPars[iPar2], oscPars[iPar1], oscPars[iPar2], oscPars[iPar1],
                 lowerBounds[iPar2], upperBounds[iPar2], gNBins2D,
                 lowerBounds[iPar1], upperBounds[iPar1], gNBins2D,
                 gHierarchyOpt, gOctOpt, burnin);

            TH2D* posteriorHist = tmpCont->getPosteriorHist();
            std::vector<TH2D*> contVec_reverse = tmpCont->getContours();
            std::vector<TH2D*> contVec(contVec_reverse.size()+1);

            contVec[0]=posteriorHist;
            for(int iHist=0; iHist<(int)contVec_reverse.size(); iHist++){
                contVec[iHist+1]=contVec_reverse[iHist];
            }

            tmp_hist_vec.push_back(contVec);
        } //iPar2 Loop
		Hist2DVec.push_back(tmp_hist_vec);
    }//iPar1 Loop

    //NOW WE PLOT (really I'd really have done this some nicer way!)
    std::cout<<"Making plots"<<std::endl;
    TCanvas* canv = new TCanvas("canv", "canv", 2000, 2000);
	canv->SetBottomMargin(0.25);
	canv->SetLeftMargin(0.25);
    canv->Divide((int)oscPars.size(), (int)oscPars.size(), 0.0, 0.0);


	canv->Draw();
	// Now let's add text
	for(int iPar=0; iPar<(int)oscPars.size(); iPar++){
		// Let's grab our x+y coordinates
		TPad* x_pad = (TPad*)canv->GetPad(13+iPar);
		TPad* y_pad = (TPad*)canv->GetPad(4*iPar+1);

		// Get corner coordinates
		double x_coord_left = x_pad->GetXlowNDC();
		double x_width = x_pad->GetWNDC();
		double x_centre = 0.5*(x_coord_left*2+x_width);
		
		double y_coord_bottom = y_pad->GetYlowNDC();
		double y_width = y_pad->GetHNDC();
		double y_centre = 0.5*(y_coord_bottom*2+y_width);

		TPaveText* x_textbox = new TPaveText(x_centre-0.1, 0.03, x_centre+0.1, 0.07);
		TPaveText* y_textbox = new TPaveText(0.02, y_centre-0.1, 0.05, y_centre+0.1);

		
		canv->cd();

		//Get a nice string version
		TString text = "";

		if(oscPars[iPar]=="theta13") text = "sin^{2} #theta_{13} [#times 10^{-3}]";
		else if(oscPars[iPar]=="theta23") text = "sin^{2} #theta_{23}";
		else if(oscPars[iPar]=="dm23") text= "#Delta m_{32}^{2} [#times 10^{-3} eV^{2}]";
		else if(oscPars[iPar]=="dcp") text = "#delta_{CP}";
		else text = oscPars[iPar];

		x_textbox->AddText(text);
		x_textbox->Draw();

	  	x_textbox -> SetTextFont(132) ;
	  	x_textbox -> SetTextSize(0.025) ;
  		x_textbox -> SetBorderSize(0) ;
	  	x_textbox -> SetFillColor(0) ;

		//I hate root
	  	y_textbox -> SetTextFont(132) ;
	  	y_textbox -> SetTextSize(0.025) ;
  		y_textbox -> SetBorderSize(0) ;
	  	y_textbox -> SetFillColor(0) ;

		TText* y_textbox_text = (TText*)y_textbox->AddText(text);
		y_textbox_text->SetTextAngle(90);
		y_textbox->Draw();
				
			
	}
	canv->Update();

	for(int iPar1=0; iPar1<(int)oscPars.size(); iPar1++){
		std::cout<<"Making contours for "<<oscPars[iPar1]<<std::endl;
        for(int iPar2=-1; iPar2<iPar1; iPar2++){
            
	    //Draw 1D
            if(iPar2==-1){
                canv->cd(5*iPar1+1);
                Hist1DVec[iPar1]->Draw("pfc nostack");
				Hist1DVec[iPar1]->GetXaxis()->SetNdivisions(6);
				Hist1DVec[iPar1]->GetYaxis()->SetNdivisions(400);
				Hist1DVec[iPar1]->GetXaxis()->SetLabelSize(0.05);
				Hist1DVec[iPar1]->GetYaxis()->SetLabelSize(0.05);

				Hist1DVec[iPar1]->SetTitle("");

				canv->Update();
				continue;
			}
			// else if(iPar1!=(int)oscPars.size()-1){
			// 	 Hist2DVec[iPar1][iPar2][0]->GetXaxis()->SetLabelSize(0);
			// 	if(iPar1-iPar2==0){
			// 		Hist2DVec[iPar1][iPar2][0]->GetYaxis()->SetLabelSize(0);
			// 	}
			// }
            
			canv->cd(5*iPar1-iPar2);
			Hist2DVec[iPar1][iPar1-1-iPar2][0]->SetTitle("");
			Hist2DVec[iPar1][iPar1-1-iPar2][0]->GetXaxis()->SetNdivisions(4);
			Hist2DVec[iPar1][iPar1-1-iPar2][0]->GetXaxis()->SetLabelSize(0.05);
			Hist2DVec[iPar1][iPar1-1-iPar2][0]->GetYaxis()->SetNdivisions(4);
			Hist2DVec[iPar1][iPar1-1-iPar2][0]->GetYaxis()->SetLabelSize(0.05);
            Hist2DVec[iPar1][iPar1-1-iPar2][0]->Draw("COL");
            for(int iHist=1; iHist<(int)Hist2DVec[iPar1][iPar1-1-iPar2].size(); iHist++){
                canv->cd(5*iPar1-iPar2);
                TH2D* tmpHist = Hist2DVec[iPar1][iPar1-1-iPar2][iHist];
                tmpHist->SetLineColor(kBlack);
                tmpHist->Smooth(1);
                tmpHist->Draw("SAME CONT3");
            }

        }//iPar2
    }//iPar1
	canv->Update();	
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
