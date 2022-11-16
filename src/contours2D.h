//For dealing with 2D contours
#pragma once
#include "contourBase.h"

class contours2D : public contourBase{
    public:
        contours2D(TString reducedFileName, TString param1Name, TString param2Name, TString hist1Name, TString hist2Name, double lowerBound1, double upperBound1, int nBins1,
                        double lowerBound2, double upperBound2, int nBins2, int massHierarchyOpt, int octantOpt, int burnin);

        void plot2DPosterior(TString outFile);
        TH2D* getPosteriorHist(){return posteriorHist;}

        void makeContourHist();
        void plot2DContourHist(TString outFile); //Just plots contours
        void plot2DContourHistWPosterior(TString outFile); //Plots contours with posterior in the background

	~contours2D();
    private:
        void get2DCredibleIntervals();
        TH2D* posteriorHist;

        std::vector<TH2D*> contourHists;
        TH2D* posteriorContours;
};
