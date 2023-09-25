#!/bin/bash

scripts=( "2016_preVFP" "2016_postVFP" "2017" "2018" )
flags=(
    "eleTight94XV2noIso"
    "miniIsoTight"
)

function doAll(){
        script="etc/config/settings_ele_UL${1}.py"
        echo "================================================================"
        echo "Creating Bins"
        python tnpEGM_fitter.py  $script --flag $2 --createBins
        echo "================================================================"
        echo "Creating Hists"
        python tnpEGM_fitter.py  $script --flag $2 --createHists
        echo "================================================================"
        echo "Creating Doing Fits"
        python tnpEGM_fitter.py  $script --flag $2 --doFit
        echo "================================================================"
        echo "Doing Fits"
        python tnpEGM_fitter.py  $script --flag $2 --doFit --mcSig --altSig
        echo "================================================================"
        echo "Doing Fits"
        python tnpEGM_fitter.py  $script --flag $2 --doFit --altSig
        echo "================================================================"
        echo "Doing Fits"
        python tnpEGM_fitter.py  $script --flag $2 --doFit --altBkg
        echo "================================================================"
        echo "Summing"
        python tnpEGM_fitter.py  $script --flag $2 --sumUp
        echo "================================================================"
        echo "Done"
}
export -f doAll
doAll "$1" "$2"
#echo {2016_preVFP,2016_postVFP,2017,2018}" "{eleTight94XV2noIso,miniIsoTight} | xargs -I{} -n2 bash -c 'doAll "$@"' {}

