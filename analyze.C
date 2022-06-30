#include <string> 
#include <iostream>
#include <TFile.h>
#include <TCanvas.h>
#include <THStack.h>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RSnapshotOptions.hxx>

static const std::array<float, 11> eta_bins = {-2.5, -2.0, -1.566, -1.4442, -0.8, 0.0, 0.8, 1.4442, 1.566, 2.0, 2.5 };
static const std::array<float, 7> pt_bins = {10,20,35,50,100,200,500};


struct InBins{
    int eta_bin, pt_bin;
    bool operator()(float eta, float pt){
        return 
            eta > eta_bins[eta_bin] && eta < eta_bins[eta_bin + 1]  && pt > pt_bins[pt_bin] && pt < pt_bins[pt_bin + 1] ;
    }
};

template<typename T>
auto makeHistForBins(T&& df, int eb, int pb, int weight=0){
    auto d_filtered = df.Filter(InBins{eb,pb},{"el_sc_eta", "el_pt"});
    std::cout << "Count: " << *d_filtered.Count() << "\n";
    if(weight == 1) {
        // double min = *d_filtered.Min("Weight");
        // double max = *d_filtered.Max("Weight");
        // double mean = *d_filtered.Mean("Weight");
        // std::cout << "Mean: " << mean << "\n";
        return  d_filtered.Histo1D("el_pt", "realweight");
    }else if(weight == 2) {
        // std::cout << "TOTWEIGHT: " << *d_filtered.Mean("totWeight") << "\n";
        return  d_filtered.Histo1D("el_pt", "totWeight");
    }else if(weight == 0) {
        return d_filtered.Histo1D("el_pt");
    }
}

void analyze(){

    std::cout << "Starting test\n";
    const int num = 1000000;
    ROOT::RDataFrame dfd("Data","snap.root");
    ROOT::RDataFrame dfm_1("MC","snap.root");
    //double min = *d_filtered.Min("Weight");
    //double max = *d_filtered.Max("Weight");
    //double mean = *d_filtered.Mean("Weight");
    //std::cout << "Min: " <<  min<< "\n";
    //std::cout << "Max: " <<  max << "\n";
    //std::cout << "Mean: " << mean << "\n";

    auto dfm = dfm_1.Define("realweight", [](float x ,double y){return x * y;}, {"totWeight", "Weight"});

    for(int i = 2 ; i < eta_bins.size() - 1; ++i){
        for(int j = 2 ; j < pt_bins.size() - 1; ++j){
            std::cout << "Examining bins eta =  " << eta_bins[i] << " - " << eta_bins[i+1] << " , pt = " << pt_bins[j] << " - " << pt_bins[j+1] << "\n";
            auto canvas = new TCanvas("c","xhist", 1000,1000);
            auto hd = makeHistForBins(dfd, i,j);
            auto hm = makeHistForBins(dfm, i,j, 1);
            auto hmu = makeHistForBins(dfm, i,j, 2);

            hd->SetLineColor(kBlue);
            hd->Draw("same");

            hm->SetLineColor(kGreen);
            hm->Draw("same");

            hmu->SetLineColor(kRed);
            hmu->Draw("same");


            canvas->SaveAs((std::string("test/SF_") + std::to_string(i) + "_" + std::to_string(j) + ".pdf").c_str());
        }}
}
