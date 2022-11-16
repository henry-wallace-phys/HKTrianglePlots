#include "contours2D.h"

contours2D::contours2D(TString reducedFileName, TString param1Name, TString param2Name, TString hist1Name, TString hist2Name, double lowerBound1, double upperBound1, int nBins1,
                        double lowerBound2, double upperBound2, int nBins2, int massHierarchyOpt=0, int octantOpt=0, int burnin=100000):
    contourBase(param1Name+param2Name+"_2DHist", massHierarchyOpt, octantOpt, burnin){
    TFile* inFile = new TFile(reducedFileName, "open");
    if(inFile->IsZombie()){
        std::cerr<<"ERROR : Couldn't find : "<<reducedFileName<<std::endl;
        throw;
    }
    TTree* oscTree = (TTree*)inFile->Get("osc_posteriors");


	oscTree->SetBranchStatus("*", 1);
    TString histOptions = setHistOptions();

    TTree* cloneTree = (TTree*)oscTree->Clone();
    posteriorHist = new TH2D(param1Name+param2Name+"_2DHist", param1Name+param2Name+"_2DHist", nBins1, lowerBound1, upperBound1, nBins2, lowerBound2, upperBound2);
  //  oscTree->Draw(param1Name+":"+param2Name+">>"+param1Name+param2Name+"_2DHist"); //<-The nice way... which doesn't work

    double par1, par2, dm23, th23;
    int step;
    
    oscTree->SetBranchAddress(param1Name, &par1);
    oscTree->SetBranchAddress(param2Name, &par2);

    //Now for the other options
    oscTree->SetBranchAddress("step", &step);
    cloneTree->SetBranchAddress("theta23", &th23);
    cloneTree->SetBranchAddress("dm23", &dm23);

    for(int iEntry=0; iEntry<oscTree->GetEntries(); iEntry++){
        oscTree->GetEntry(iEntry);
        cloneTree->GetEntry(iEntry);
        if(step<burnin) continue; //No point gathering steps here

        // Mass hierarchy
        if(massHierarchyOpt==-1 && dm23>0.0) continue;
        else if(massHierarchyOpt==1 && dm23<0.0) continue;

        // Octant
        if(octantOpt==-1 && th23>0.5) continue;
        else if(octantOpt==1 && th23<0.5) continue;
        //Now we've removed any junk we can do
        posteriorHist->Fill(par1, par2);
    }
    get2DCredibleIntervals();
}
	

contours2D::~contours2D(){
	delete posteriorHist;
	contourHists.clear();
	contourHists.shrink_to_fit();
}

void contours2D::get2DCredibleIntervals(){
    //Givs us a vector of contour levels for plotting later!

    double integral=posteriorHist->Integral();

	// Make contours for each credible interval
    for(int iCredibleInt=0; iCredibleInt <(int)credibleIntervals_vec.size(); iCredibleInt++){
        TString histName;
        histName.Form("%0.7f Credible Interval", credibleIntervals_vec[iCredibleInt]);
        TH2D* copyHist = (TH2D*)posteriorHist->Clone(histName);
        copyHist->SetNameTitle(histName, histName);

        double cred = credibleIntervals_vec[iCredibleInt];
        double tsum=0;
        double tmax=-99.0;

        while((tsum/integral) < cred && copyHist->GetMaximum()!=tmax){
            tmax = copyHist->GetMaximum();
            int bin = copyHist->GetMaximumBin();
            copyHist->SetBinContent(bin, -1.0);
            tsum += tmax;
        }
        double contourLevels[1];
        contourLevels[0]=tmax; //Get maximum bin for the interval
        copyHist->SetContour(1, contourLevels);
        copyHist->Smooth();
        contourHists.push_back(copyHist);
    }
}

void contours2D::plot2DContourHistWPosterior(TString outFile){
    gStyle->SetPalette(kCool);
    TFile* outFileROOT=new TFile(outFile+".root", "UPDATE");
    outFileROOT->cd();
    TCanvas* canv = new TCanvas(_histTitle, _histTitle, 1200, 600);
    canv->Draw();
    canv->cd();
	posteriorHist->DrawCopy("COLZ");
    for(TH2D* iHist : contourHists){
        iHist->SetLineColor(kRed);
        iHist->Draw("SAME CONT3");
    }
    gPad->BuildLegend();

    outFile+="_"+_histTitle;
  
    canv->Print(outFile+".pdf");
    outFileROOT->cd();
    canv->Write();
    outFileROOT->Close();
}


void contours2D::plot2DContourHist(TString outFile){
   //gStyle->SetPalette(kCool);
    TFile* outFileROOT=new TFile(outFile+".root", "UPDATE");
    outFileROOT->cd();
    TCanvas* canv = new TCanvas(_histTitle, _histTitle, 1200, 600);
    canv->Draw();
    canv->cd();
    posteriorHist->SetLineColor(kRed);
    posteriorHist->Smooth();
    // posteriorHist->SetContour(100, );
    posteriorHist->Draw("cont3");
    outFile+="_"+_histTitle;
  
    canv->Print(outFile+".pdf");
    outFileROOT->cd();
    canv->Write();
    outFileROOT->Close();
}

void contours2D::plot2DPosterior(TString outFile){
  
    gStyle->SetPalette(kCool);
    TFile* outFileROOT=new TFile(outFile+".root", "UPDATE");
    outFileROOT->cd();
    TCanvas* canv = new TCanvas(_histTitle, _histTitle, 1200, 600);
    canv->Draw();
    canv->cd();
	posteriorHist->Draw("COLZ");

    outFile+="_"+_histTitle;
  
    canv->Print(outFile+".pdf");
    outFileROOT->cd();
    canv->Write();
    outFileROOT->Close();
}
