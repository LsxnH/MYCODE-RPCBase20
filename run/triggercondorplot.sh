#! /bin/sh
TestArea="/home/hengli/testarea/RPCBase20"
IFS=$'\n' read -d '' -r -a lines < $1
python $TestArea/PhysicsAnpRPC/macros/runTrigger.py \
       "${lines[@]}" \
			 --file-key='.*$' \
       --trigger-file='/lustre/AtlUser/liheng/RPC/ntuples/triggersMTV.txt' \
			 --rpc-geo='/moose/AtlUser/liheng/public/OfflineGeometry/rpc_geometry_athena_21.0.62_2017-03-05.root' \
			 -o ANP_HIST.root 
