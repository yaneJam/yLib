#!/bin/bash
#echo hello world 
current_dir=$(pwd)
echo "current_dir:$current_dir"

prj_dir=$current_dir/../prj_gcc

if [ ! -d "$prj_dir" ];then
	mkdir $prj_dir	
fi

src_dir=$current_dir/../src

cmake -S $src_dir -B $prj_dir
cmake --build $prj_dir