#!/usr/bin/env bash

rm  ${CMSSW_VERSION}.tar.gz
tar --exclude-caches-all --exclude-vcs -zcf ${CMSSW_VERSION}.tar.gz -C "$CMSSW_BASE/.." $CMSSW_VERSION --exclude=tmp --exclude=condor --exclude prep_for_launch.sh --exclude '*.tar.gz' --exclude logs --exclude results --exclude results_no_op_charge  --exclude oldresults  --exclude snap.root

