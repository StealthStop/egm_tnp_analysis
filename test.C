#include <string>
#include <iostream>
#include <TFile.h>
#include <TCanvas.h>
#include <THStack.h>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RSnapshotOptions.hxx>


void test(){
//    ROOT::EnableImplicitMT();

    // std::string datapath="results/UL2016_postVFP/tnpEleID/eleTight94XV2noIso/data_Run2016F_postVFP_eleTight94XV2noIso.root";
    // std::string mcpath="results/UL2016_postVFP/tnpEleID/eleTight94XV2noIso/DY_madgraph_eleTight94XV2noIso.root";
    std::cout << "Starting test\n";

    std::string mcpath="/store/group/phys_egamma/tnpTuples/rasharma/2021-02-10/UL2016postVFP/merged/DY_LO_L1matched.root";
    std::vector<std::string> datapaths={
        "root://eoscms.cern.ch//store/group/phys_egamma/tnpTuples/rasharma/2021-02-10/UL2016postVFP/merged/Run2016F_L1merged.root",
        "root://eoscms.cern.ch//store/group/phys_egamma/tnpTuples/rasharma/2021-02-10/UL2016postVFP/merged/Run2016G_L1matched.root",
        "root://eoscms.cern.ch//store/group/phys_egamma/tnpTuples/rasharma/2021-02-10/UL2016postVFP/merged/Run2016H_L1matched.root"
    };

    // auto dataf = TFile::Open((std::string("root://eoscms.cern.ch/") + datapath).c_str());
    auto mcf = TFile::Open((std::string("root://eoscms.cern.ch/") + mcpath).c_str());
    auto sf = TFile::Open("file:oldresults/UL2016_postVFP/tnpEleID/eleTight94XV2noIso/egammaEffi.txt_EGM2D.root");
    auto weights = TFile::Open("root://eoscms.cern.ch//store/group/phys_egamma/asroy/Tag-and-Probe_Tree/UL2016/PU_Trees/postVFP/DY_amcatnloext_ele.pu.puTree.root");

    auto sf2d = (TH2D*)sf->Get("EGamma_SF2D");

    auto isCap = [](float eta){return std::abs(eta) > 1.479f;};
    auto getWeight = [&](float eta, float pt){return sf2d->GetBinContent(sf2d->FindBin(eta,pt));};

    auto isTight = [](float sie, float dphi, float sc_eta, float over, int veto, float hoe, float sc_e, float rho, float etaseed){
        bool iscap = std::abs(sc_eta) > 1.479f;
        return ( sie < ( (iscap)? 0.0104f : 0.353 ) )
             &&  ( std::abs(dphi) < ( (iscap)? 0.022 : 0.236 ))
             && ( over <  ( (iscap)? 0.159 : 0.0197 ))
             && ( std::abs(hoe) <= 1)
             && ( veto )
             && ( std::abs(hoe) < ( ( iscap ? 0.026 : 0.18) + (iscap ? 1.15 : 2.06) * ( 1/ sc_e) + (iscap ? 0.0324 : 0.183) * rho/sc_e) )
             && ( etaseed < ( ( iscap )? 0.00255 : 0.00501 ) );

    };


    const int num = 1000000;
    auto chain = new TChain("tnpEleIDs/fitter_tree");
    for(const auto& file : datapaths){
        chain->Add(file.c_str());
    }
    //auto datatree = (TTree*)dataf->Get("tnpEleIDs/fitter_tree");
    ROOT::RDataFrame dfd_1(*chain);
    auto dfd_2 = dfd_1.Range(num);
    auto dfd = dfd_2.Filter(isTight, {"el_5x5_sieie", "el_dPhiIn", "el_sc_eta", "el_1overEminus1overP", "passingCutBasedTight94XV2GsfEleConversionVetoCut", "el_hoe", "el_sc_e" ,"event_rho.rho", "el_dEtaSeed"});
    // auto dfd = dfd_3.Define("Weight", getWeight, {"el_sc_eta", "el_pt"});


    auto mctree = (TTree*)mcf->Get("tnpEleIDs/fitter_tree");
    auto weighttree  = (TTree*)weights->Get("weights_2016_run2016");
    mctree->AddFriend(weighttree);
    ROOT::RDataFrame dfm_1(*mctree);
    auto dfm_2 = dfm_1.Range(num);
    auto dfm_3 = dfm_2.Filter(isTight, {"el_5x5_sieie", "el_dPhiIn", "el_sc_eta", "el_1overEminus1overP", "passingCutBasedTight94XV2GsfEleConversionVetoCut", "el_hoe", "el_sc_e" ,"event_rho.rho", "el_dEtaSeed"});
    auto dfm = dfm_3.Define("Weight", getWeight, {"el_sc_eta", "el_pt"});

    ROOT::RDF::RSnapshotOptions opts;
    opts.fMode = "UPDATE";
    dfm.Snapshot("MC", "snap.root", {"el_pt", "el_sc_eta", "Weight", "totWeight", "PUweight"}, opts);
    dfd.Snapshot("Data", "snap.root", {"el_pt", "el_sc_eta"}, opts);
}
