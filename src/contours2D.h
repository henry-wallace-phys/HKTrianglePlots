//For dealing with 2D contours
#pragma once
#include "contourBase.h"

class contours2D : contourBase{
    public:
        contours2D(TString reducedFileName, TString param1Name, TString param2Name, TString hist1Name, TString hist2Name, double lowerBound1, double upperBound1, int nBins1,
                        double lowerBound2, double upperBound2, int nBins2, int massHierarchyOpt, int octantOpt, int burnin);

    private:
        void get2DCredibleIntervals();
        THStack* contourStack;
        void makePrettyHist2D(){makePrettyHist(contourHists);}
        TH2D* posteriorHist;

        std::vector<double> contourLevels;
        std::vector<TH2D*> contourHists;
};