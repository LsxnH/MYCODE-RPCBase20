#! /bin/sh
TestArea="/home/hengli/testarea/RPCBase20"
IFS=$'\n' read -d '' -r -a lines < $1
python $TestArea/PhysicsAnpRPC/macros/runPanelEff.py \
       "${lines[@]}" \
			 --file-key='.*$' \
			 --min-lb=$2 \
			 --max-lb=$3 \
			 --rpc-geo='/moose/AtlUser/liheng/public/OfflineGeometry/rpc_geometry_athena_21.0.62_2017-03-05.root' \
			 -o ANP_HIST.root 
