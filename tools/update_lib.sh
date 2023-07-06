#!/bin/bash

dest_dir=$3../lib/$2/
src_dir=$1
dest_h=$3../$lib/$2/*.h

echo "dest_dir: $dest_dir"
echo "src_dir: $src_dir"
echo "dest_h: $dest_h"
echo projectfile name is $4



rm -rf $dest_dir
#find $src -iname "*.h" -type f -exec cp {} $dest_dir



function recursion_copy_file()
{
    if [ ! -d $2 ];then
        echo "mkdir $2"
        mkdir $2
    fi
    
    cp $1$3 $2 #copy file first 

    #find the directory to recursion
    dirlist=$(ls $1)
    for name in ${dirlist[*]}
    do
        if [ -d $1/$name ];then
            recursion_copy_file $1/$name $2/$name $3 
        fi
    done
}

recursion_copy_file $src_dir $dest_dir "/*.h"
