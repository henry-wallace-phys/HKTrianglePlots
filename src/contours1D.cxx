#include "contours1D.h"

contours1D::contours1D(TString reducedFileName, TString paramName, TString oscBranchName, double lowerBound, double upperBound, int nBins, int massHierarchyOpt=0, int octantOpt=0, int burnin=10000):
contourBase(paramName+"_1DHist", massHierarchyOpt, octantOpt, burnin)
{
    TFile* inFile = new TFile(reducedFileName, "open");
    
    if(inFile->IsZombie()){
        std::cerr<<"ERROR : Couldn't find : "<<reducedFileName<<std::endl;
        throw;
    }
    TTree* oscTree = (TTree*)inFile->Get("osc_posteriors");
	oscTree->SetBranchStatus("*", 1);
    TString histOptions = setHistOptions();

    posteriorHist = new TH1D( "posteriorHist", "posteriorHist", nBins, lowerBound, upperBound);

    oscTree->Draw(paramName+">>posteriorHist", histOptions);
	if(!posteriorHist->GetEntries()){
		std::cerr<<"ERROR : Posterior Histogram cannot be filled with entries from "<<paramName<<std::endl;
		std::cerr<<"Acceptable entries are : "<<std::endl;
		oscTree->Show();
		throw;
	}
		
		
    oscParamName=paramName;

    setup1DCredibleIntervals();
    makePrettyHist<TH1D*>(credibleIntervalHists, posteriorHist);
//    inFile->Close();
}

contours1D::~contours1D(){
	delete posteriorHist;
	credibleIntervalHists.clear();
	credibleIntervalHists.shrink_to_fit();
}


void contours1D::setup1DCredibleIntervals(){
    // {1sigma, 2sigma, 3sigma, 4sigma, 5sigma}

    double integral = posteriorHist->Integral(); 

    for(int iCredibleInt=0; iCredibleInt<(int)credibleIntervals_vec.size(); iCredibleInt++){
        TString histName;
        double credVal = credibleIntervals_vec[iCredibleInt];
        histName.Form("%0.7f Credible Interval", credVal);
        TH1D* copyHist = (TH1D*)posteriorHist->Clone(histName);
        copyHist->SetNameTitle(histName, histName);
        double tsum=0; //total sum
        double up = -9999999.; //upper bound
        double low = 9999999.; //lower bound
        int nbins = 0; //number of bins
        double xwidth; //width of bins
        double bf = copyHist->GetXaxis()->GetBinCenter(copyHist->GetMaximumBin()); //OG big bin

        double tmax=-99.0;

        while((tsum/integral)<credVal && copyHist->GetMaximum()!=tmax) {
            //Loop until we delete (set to -1) any bins inside of credible interval
            tmax = copyHist->GetMaximum();
            int bin = copyHist->GetMaximumBin();
            int xval = copyHist->GetXaxis()->GetBinCenter(bin);

            xwidth = copyHist->GetXaxis()->GetBinWidth(bin);
            copyHist->SetBinContent(bin,-1.0);
            if(xval<low && xval<bf) low = xval - xwidth/2.;
            if(xval>up && xval>bf) up = xval + xwidth/2.;
            nbins++;
            tsum+=tmax;
        }
        credibleIntervalHists.push_back(copyHist);
        std::vector<double> tmpbounds; //Keep track of this
        tmpbounds.push_back(low);
        tmpbounds.push_back(up);
        credibleBoundaries.push_back(tmpbounds);
    }
}
