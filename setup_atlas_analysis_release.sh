MY_ATH_ANALYSIS_VERSION=2.4.30

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh
source $AtlasSetup/scripts/asetup.sh AthAnalysisBase,${MY_ATH_ANALYSIS_VERSION},here

echo "---------------------------------------------------------------"
echo "Using AthAnalysisBase ${MY_ATH_ANALYSIS_VERSION}"
