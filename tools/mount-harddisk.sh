#!/bin/bash

set -e

IMG=harddisk
DIR=/mnt/my-system

sudo mount $IMG $DIR -o uid=$(id -u),gid=$(id -g),loop,offset=$((2048 * 512))
pushd $DIR >/dev/null
printf "When finished, type 'exit' to unmount.\n"
bash
popd >/dev/null
sudo umount $DIR
