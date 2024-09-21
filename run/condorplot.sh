#! /bin/sh
#       --do-clu-ntuple \
#       --do-noise \
#       --small-noise-hists \
TestArea="/home/hengli/testarea/RPCBase20"
IFS=$'\n' read -d '' -r -a lines < $1
python $TestArea/PhysicsAnpRPC/macros/runPanelEff.py \
       "${lines[@]}" \
       --file-key='.*$' \
       --rpc-geo='/lustre/AtlUser/rustem/public/OfflineGeometry/rpc_geometry_athena_21.0.71_2018-09-12.root' \
       --do-noise \
       --small-noise-hists \
       -o ANP_HIST.root 
