#include <string> 
#include <iostream>
#include <TFile.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <THStack.h>
#include <TRatioPlot.h>
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
        return d_filtered.Histo1D({"","",100,0,200},"el_pt");
    }
}

//const std::string den = "PassesTightID";
//const std::string numerator = "el_miniIsoAll_fall17 < 0.1";
//const std::string den = "tag_Ele_pt > 20 && abs(tag_sc_eta) < 2.4";
#define PREMODE 1
 const std::string den = "abs(el_sc_eta) < 2.4 && abs(pair_mass - 91.18 ) < 0.1 && ( el_q * tag_Ele_q ) < 0";
//const std::string den = "tag_Ele_pt > 20  && abs(el_sc_eta) < 2.4 && ( el_q * tag_Ele_q ) < 0";
//const std::string den = "true";
const std::string numerator = "PassesTightID";

void analyze(const std::string& inname){

    std::cout << "Starting test\n";
    const int num = 1000000;
    ROOT::RDataFrame dfd_1("Data",inname.c_str());
    ROOT::RDataFrame dfm_1("MC",inname.c_str());

    // auto dfd_2 = dfd_1.Filter(den).Filter("passingHLTsafe == 1");
    auto dfd_den = dfd_1.Filter(den);
    auto dfd_num = dfd_den.Filter(numerator);

    auto dfm_den = dfm_1.Filter(den);

    std::cout << "Using method pre\n";
    float data_integral = (float)*dfd_den.Count();

    auto dfm_den_weighted = dfm_den.Define("mc_weight",
            [&](float x)
            {return x;}, {"totWeight"});

    float mc_integral = (float)*dfm_den_weighted.Sum("mc_weight");
    float overall_scale = data_integral / mc_integral;

    auto dfm_den_normalized= dfm_den_weighted.Define("normalized_weight",
            [&](float x)
            {return x * overall_scale ;}, {"mc_weight"});


    auto dfm_den_scaled = dfm_den_normalized.Define("total_scale",
            [&](float x, double y)
            {return x * y;}, {"normalized_weight", "Weight"});

    auto dfm_num = dfm_den_scaled.Filter(numerator);

#if PREMODE
#else
    std::cout << "Using method post\n";
    auto dfm_4 = dfm_3.Define("midweight", 
            [&](double x, float y)
            {return x * y;}, {"Weight", "totWeight"});

    auto dfm_4_noscale = dfm_3.Define("midweight_ns", 
            [&](float y)
            {return  y ;}, {"totWeight"});

    float data_integral = (float)*dfd.Count();

    float mc_integral = (float)*dfm_4.Sum("midweight");
    float overall_scale = data_integral / mc_integral;

    float mc_integral_noscale = (float)*dfm_4_noscale.Sum("midweight_ns");
    float overall_scale_noscale = data_integral / mc_integral_noscale;

    auto dfm = dfm_4.Define("realweight", 
            [&](double x)
            {return x * overall_scale;}, {"midweight"});


    auto dfm_noscale = dfm_4_noscale.Define("datacorrection", 
            [&](float x)
            {return x * overall_scale_noscale;}, {"midweight_ns"});
#endif

    std::cout << "Total Data is " << data_integral  << "\n";
    std::cout << "Total MC is " << mc_integral << "\n";
    std::cout << "MC Correction is " << overall_scale << "\n";
    std::cout << "After correction mc is " << (float)*dfm_den_scaled.Sum("mc_weight") << "\n";
    std::cout << "After scale mc is " << (float)*dfm_den_scaled.Sum("total_scale") << "\n";


    auto f = TFile::Open("output.root", "RECREATE");

    gStyle->SetOptStat(1111111);
    auto canvas = new TCanvas("c","xhist", 1000,1000);

     auto hd = dfd_num.Histo1D({"","",100,0,200}, "el_pt");
     auto hm = dfm_num.Histo1D({"","",100,0,200}, "el_pt", "total_scale");
     auto hm_noscale = dfm_num.Histo1D({"","",100,0,200}, "el_pt", "normalized_weight");

    // auto hd = dfd_num.Histo1D({"","",100,0,200}, "pair_mass");
    // auto hm = dfm_num.Histo1D({"","",100,0,200}, "pair_mass", "total_scale");
    // auto hm_noscale = dfm_num.Histo1D({"","",100,0,200}, "pair_mass", "normalized_weight");
    hd->GetXaxis()->SetRangeUser(0, 200);
    hm->GetXaxis()->SetRangeUser(0, 200);
    hm_noscale->GetXaxis()->SetRangeUser(0, 200);
    hd->Write("data");
    hm->Write("mc");
    hm_noscale->Write("noscale");
    auto tempd= *hd;
    auto tempm= *hm;
    auto tempm_noscale= *hm_noscale;
    auto trp = new TRatioPlot(&tempd, &tempm);
    auto trp_noscale = new TRatioPlot(&tempd, &tempm_noscale);
    trp->Draw();
    trp->GetLowerRefGraph()->SetMaximum(1.5);
    trp->GetLowerRefGraph()->SetMinimum(0);
    trp->Draw();
    canvas->SaveAs("ratio.pdf");
    trp_noscale->Draw();
    trp_noscale->GetLowerRefGraph()->SetMaximum(1.5);
    trp_noscale->GetLowerRefGraph()->SetMinimum(0);
    trp_noscale->Draw();
    canvas->SaveAs("ratio_unscaled.pdf");
    trp->Write("ratio");
    trp_noscale->Write("unscaled");

    // for(int i = 2 ; i < eta_bins.size() - 1; ++i){
    //     for(int j = 2 ; j < pt_bins.size() - 1; ++j){
    //         std::cout << "Examining bins eta =  " << eta_bins[i] << " - " << eta_bins[i+1] << " , pt = " << pt_bins[j] << " - " << pt_bins[j+1] << "\n";
    //     }}
}

int main(int argc , char* argv[]){
    analyze(argv[1]);
    return 0 ;
}
