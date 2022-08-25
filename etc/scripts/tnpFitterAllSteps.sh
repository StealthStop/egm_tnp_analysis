#!/bin/bash


scripts=( "2016_preVFP" "2016_postVFP" "2017" "2018" )
flags=(
    "eleTight94XV2noIso"
    "miniIsoTight"
)


function doAll(){
        script="etc/config/settings_ele_UL${1}.py"
        python tnpEGM_fitter.py  $script --flag $2 --createBins
        python tnpEGM_fitter.py  $script --flag $2 --createHists
        python tnpEGM_fitter.py  $script --flag $2 --doFit
        python tnpEGM_fitter.py  $script --flag $2 --doFit --mcSig --altSig
        python tnpEGM_fitter.py  $script --flag $2 --doFit --altSig
        python tnpEGM_fitter.py  $script --flag $2 --doFit --altBkg
        python tnpEGM_fitter.py  $script --flag $2 --sumUp
}
export -f doAll
doAll "$1" "$2"
#echo {2016_preVFP,2016_postVFP,2017,2018}" "{eleTight94XV2noIso,miniIsoTight} | xargs -I{} -n2 bash -c 'doAll "$@"' {}

