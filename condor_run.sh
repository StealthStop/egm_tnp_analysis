#!/bin/bash
echo "Starting job on " `date` #Date/time of start of job
echo "Running on: `uname -a`" #Condor job is running on this node
echo "System software: `cat /etc/redhat-release`" #Operating System on that node
echo "Parameters: ${@}"
export XRD_REQUESTTIMEOUT=1800
source /cvmfs/cms.cern.ch/cmsset_default.sh 
tar -xf ${1}.tar.gz
rm ${1}.tar.gz
cd ${1}/src/
scramv1 b ProjectRename # this handles linking the already compiled code - do NOT recompile
eval `scramv1 runtime -sh` # cmsenv is an alias not on the workers
echo $CMSSW_BASE "is the CMSSW we have on the local worker node"
cd ${_CONDOR_SCRATCH_DIR}
cd ${1}/src/egm_tnp_analysis
pwd
rm libCpp/*.so
ls libCpp
make clean-all
make cython-build
make 
ls libCpp
echo "Starting EGM Fitter with arguments ${@:2}"
./etc/scripts/tnpFitterAllSteps.sh "${@:2}"

