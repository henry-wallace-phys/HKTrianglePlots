#include "contours1D.h"

contours1D::contours1D(TString reducedFileName, TString paramName, TString oscBranchName, double lowerBound, double upperBound, double nBins, int massHierarchyOpt=0, int octantOpt=0, int burnin=10000):
contourBase(paramName+"_1DHist", massHierarchyOpt, octantOpt, burnin)
{
    TFile* inFile = new TFile(reducedFileName, "open");
    
    TTree* oscTree = (TTree*)inFile->Get("osc_posteriors");

    TString histOptions = setHistOptions();

    posteriorHist = new TH1D( "posteriorHist", "posteriorHist", nBins, lowerBound, upperBound);

    oscTree->Draw(paramName+">>posteriorHist+", histOptions);

    oscParamName=paramName;
    inFile->Close();

    posteriorHist->SetNameTitle("TotalDistribution_"+paramName, "Full distribution for "+paramName);

    setup1DCredibleIntervals();
    makePrettyHist<std::vector<TH1D* > >(credibleIntervalHists);
}


void contours1D::setup1DCredibleIntervals(){
    std::vector<double> credibleIntervals_vec = {0.67, 0.95, 99.9, 99.99, 99.99999};
    // {1sigma, 2sigma, 3sigma, 4sigma, 5sigma}
    double integral = posteriorHist->Integral();

    for(int iCredibleInt=0; iCredibleInt<(int)credibleIntervals_vec.size(); iCredibleInt++){
        TString histName;
        double credVal = credibleIntervals_vec[iCredibleInt];
        histName.Form("%0.7f Credible Interval", credVal);
        TH1D* copyHist = (TH1D*)posteriorHist->Clone(histName);

        double tsum=0;
        double up = -9999999.;
        double low = 9999999.;
        int nbins = 0;
        double xwidth;
        double bf = copyHist->GetXaxis()->GetBinCenter(copyHist->GetMaximumBin());

        while((tsum/integral)<credVal) {
            double tmax = copyHist->GetMaximum();
            int bin = copyHist->GetMaximumBin();
            int xval = copyHist->GetXaxis()->GetBinCenter(bin);

            xwidth = copyHist->GetXaxis()->GetBinWidth(bin);
            if((tsum/integral)<credVal) {
                copyHist->SetBinContent(bin,-1.0);
                copyHist->SetBinContent(bin,0.);
                if(xval<low && xval<bf) low = xval - xwidth/2.;
                if(xval>up && xval>bf) up = xval + xwidth/2.;
                nbins++;
            }
            tsum+=tmax;
        }
        credibleIntervalHists.push_back(copyHist);
        std::vector<double> tmpbounds = {low,up}; //Keep track of this
        credibleBoundaries.push_back(tmpbounds);
    }
}
