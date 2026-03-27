#!bin/bash

echo "" ; \
echo "---------------------------------" ; \
echo "./generate_datasets" ; \
./generate_datasets 1000000 1000000 42 1048576 dataset ; \
echo "---------------------------------" ; \
echo "./plain_novec" ; \
./plain_novec 128 ; \
echo "---------------------------------" ; \
echo "./plain_vec" ; \
./plain_vec 128 ; \
echo "---------------------------------" ; \
echo ""