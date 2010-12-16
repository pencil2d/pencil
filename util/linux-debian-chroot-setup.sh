#!/bin/sh

set -e


PACKAGES_BUILDROOT=./pencil-buildroot
MACHINE_TYPE=`uname -m`
case ${MACHINE_TYPE} in
	i586)
		ARCH=i386;;
	i686)
		ARCH=i386;;
	x86_64)
		ARCH=amd64;;
	*)
		ARCH=${MACHINE_TYPE};;
esac
if [[ $ARCH == 'i386' ]];then
	SETARCH='linux32'
else
	SETARCH='linux64'
fi
if ! [ -e $PACKAGES_BUILDROOT.$ARCH/etc/chroot.id ]; then
debootstrap --arch=$ARCH --variant=buildd  --include=sudo lenny $PACKAGES_BUILDROOT.$ARCH http://mirrors2.kernel.org/debian
echo "Lenny Buildroot" > $PACKAGES_BUILDROOT.$ARCH/etc/chroot.id
#keep proxy settings
		if ! [ -z $http_proxy ]; then 
			#echo "export http_proxy=\"$http_proxy\";" >> $PACKAGES_BUILDROOT.$ARCH/root/.bashrc
			#echo "echo 'proxy export done';" >> $PACKAGES_BUILDROOT.$ARCH/root/.bashrc
			echo "Acquire::http::Proxy \"$http_proxy\";" > $PACKAGES_BUILDROOT.$ARCH/etc/apt/apt.conf
		fi
fi		

#mount -o bind /dev 
#mount -o 
#umask 0022
