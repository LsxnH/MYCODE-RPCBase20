pushd $TestArea/PhysicsAnpRPC/cmt
cmt bro rm -r ../x86_64-slc6*
cmt bro make -j4
popd
