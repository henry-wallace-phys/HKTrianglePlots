// Contour making base class
// Contains 1D hist for parameter and 5 sigma credible intervals
#pragma once
#include "contourBase.h"

class contours1D : public contourBase{
    public:
        contours1D(TString reducedFileName, TString paramName, TString oscBranchName, double lowerBound, double upperBound, int nBins, int massHierarchyOpt, int octantOpt, int burnin);

        TH1D* getPosteriorHist(){return posteriorHist;}


    private:
        void setup1DCredibleIntervals();

        std::vector<std::vector<double>> credibleBoundaries;
        std::vector<TH1D*> credibleIntervalHists; //Contains (lower, upper for each interval)
        TString oscParamName;
        TH1D* posteriorHist;
};