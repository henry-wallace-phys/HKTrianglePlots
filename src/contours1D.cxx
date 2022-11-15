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
    makePrettyHist<std::vector<TH1D* > >(credibleIntervalHists);
    inFile->Close();
}

contours1D::~contours1D(){
	delete posteriorHist;
	credibleIntervalHists.clear();
	credibleIntervalHists.shrink_to_fit();
}


void contours1D::setup1DCredibleIntervals(){
    std::vector<double> credibleIntervals_vec = {0.67, 0.95, 99.9, 99.99, 99.99999};
    // {1sigma, 2sigma, 3sigma, 4sigma, 5sigma}

	double integral = posteriorHist->Integral(); 
	credibleBoundaries.reserve(credibleIntervals_vec.size());
	credibleIntervalHists.reserve(credibleIntervals_vec.size());
	
	for(int iCredibleInt=0; iCredibleInt<(int)credibleIntervals_vec.size(); iCredibleInt++){
        TString histName;
        double credVal = credibleIntervals_vec[iCredibleInt];
        histName.Form("%0.7f Credible Interval", credVal);
        TH1D* copyHist = (TH1D*)posteriorHist->Clone(histName);

        double tsum=0; //total sum
        double up = -9999999.; //upper bound
        double low = 9999999.; //lower bound
        int nbins = 0; //number of bins
        double xwidth; //width of bins
        double bf = copyHist->GetXaxis()->GetBinCenter(copyHist->GetMaximumBin()); //OG big bin

        while((tsum/integral)<credVal && copyHist->GetMaximum()!=-1.0) {
			//Loop until we delete (set to -1) any bins inside of credible interval
            double tmax = copyHist->GetMaximum(); 
            int bin = copyHist->GetMaximumBin();
            int xval = copyHist->GetXaxis()->GetBinCenter(bin);

			
            xwidth = copyHist->GetXaxis()->GetBinWidth(bin);
			copyHist->SetBinContent(bin,-1.0);
			copyHist->SetBinContent(bin,0.);
			if(xval<low && xval<bf) low = xval - xwidth/2.;
			if(xval>up && xval>bf) up = xval + xwidth/2.;
			nbins++;
            tsum+=tmax;
        }
        credibleIntervalHists[iCredibleInt]=copyHist;
        std::vector<double> tmpbounds = {low,up}; //Keep track of this
        credibleBoundaries[iCredibleInt]=tmpbounds;
    }
}
