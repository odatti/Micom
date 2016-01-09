#!/bin/bash

mkdir $1
sed -e "s/SOURCE_NAME/$1/g" .cfg/MakeFile > $1/MakeFile
cp .cfg/template.c $1/$1.c
