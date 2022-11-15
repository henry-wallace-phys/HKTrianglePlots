#include <contours2D.h>

contours2D::contours2D(TString reducedFileName, TString param1Name, TString param2Name, TString hist1Name, TString hist2Name, double lowerBound1, double upperBound1, int nBins1,
                        double lowerBound2, double upperBound2, int nBins2, int massHierarchyOpt=0, int octantOpt=0, int burnin=100000):
    contourBase(param1Name+param2Name+"_2DHist", massHierarchyOpt, octantOpt, burnin){
    TFile* inFile = new TFile(reducedFileName, "open");
    if(inFile->IsZombie()){
        std::cerr<<"ERROR : Couldn't find : "<<reducedFileName<<std::endl;
        throw;
    }
 
    TTree* oscTree = (TTree*)inFile->Get("osc_posteriors");

    TString histOptions = setHistOptions();

    posteriorHist = new TH2D( "posteriorHist", "posteriorHist", nBins1, lowerBound1, upperBound1, nBins2, lowerBound2, upperBound2);
    oscTree->Draw(param1Name+":"+param2Name+">>posteriorHist+", histOptions);
    get2DCredibleIntervals();
    makePrettyHist<std::vector<TH2D* > >(contourHists);
}


void contours2D::get2DCredibleIntervals(){
    //Givs us a vector of contour levels for plotting later!
    std::vector<double> credibleIntervals = {0.67, 0.95, 99.9, 99.999, 99.999999};
    double integral=posteriorHist->Integral();

    for(int iCredibleInt=0; iCredibleInt <(int)credibleIntervals.size(); iCredibleInt++){
        TString histName;
        histName.Form("%0.7f Credible Interval", credibleIntervals[iCredibleInt]);
        TH2D* copyHist = (TH2D*)posteriorHist->Clone(histName);

        double tsum=0;
        double tmax;
        while((tsum/integral) < iCredibleInt){
            tmax = copyHist->GetMaximum();
            tsum += tmax;
            int bin = copyHist->GetMaximumBin();
            copyHist->SetBinContent(bin, -999.0);
        }

        double con[1];
        con[0]=tmax;
        copyHist->SetContour(1, con);
        copyHist->SetLineColor(kWhite);
        copyHist->Smooth(1);
        copyHist->SetLineWidth(2);
        contourHists.push_back(copyHist);
        contourLevels.push_back(tmax); //Get maximum bin for the interval
    }
}
