#!/bin/bash

LOOP=/dev/loop0
DIR=/mnt/my-system

echo "Creating loop device..."
sudo /sbin/losetup $LOOP floppy.img
echo "Mounting loop device..."
sudo mount $LOOP $DIR
echo "Copying files..."
sudo cp -v src/kernel $DIR/kernel
sudo cp -v initrd.img $DIR/initrd
sleep 2
echo "Unmounting loop device..."
sudo umount $LOOP
echo "Deleting loop device..."
sudo /sbin/losetup -d $LOOP
