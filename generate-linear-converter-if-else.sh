#!/bin/bash
strTypes=(SignedInt16 SignedInt32 UnsignedInt16 UnsignedInt32 Float4B Double); 
types=(qint16 qint32 quint16 quint32 float double);

if [ ${#strTypes[@]} -ne ${#types[@]} ]; then
	echo "Sizes don't match";
	exit 1;
fi; 

size=${#strTypes[@]};
let lastElemNo=$size-1;
prefix="";
for no1 in $(seq 0 $lastElemNo); do
	for no2 in $(seq 0 $lastElemNo); do
		echo "$prefix IF_CREATE_LIN_CONV(${strTypes[$no1]}, ${strTypes[$no2]}, ${types[$no1]}, ${types[$no2]})";
		prefix="else";
	done;
done;
