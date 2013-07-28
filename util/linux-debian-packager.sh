#!/bin/sh
#
# Pencil 2D - Package build script
# Copyright (c) 2009-2013 Konstantin Dmitriev
#
#
# 1. Set up chroots (you need to do that only once)
# bash ./util/linux-debian-chroot-setup.sh
#
# 2. Build i386 binary:
# mount -o bind ./ ./pencil-buildroot.i386/mnt/
# linux32 chroot ./pencil-buildroot.i386
# bash /mnt/util/linux-debian-packager.sh
# exit
# umount ./pencil-buildroot.i386/mnt/
#
# 3. Build x86_64 binary:
# mount -o bind ./ ./pencil-buildroot.amd64/mnt/
# chroot ./pencil-buildroot.amd64
# bash /mnt/util/linux-debian-packager.sh
# exit
# umount ./pencil-buildroot.amd64/mnt/

export VERSION=0.5.4
export RELEASE=1
export EMAIL='ksee.zelgadis@gmail.com'

set -e

export SCRIPTPATH=`dirname "$0"`
cd $SCRIPTPATH/..

REVISION=`git show --pretty=format:%ci HEAD |  head -c 10 | tr -d '-'`

PREFIX=/opt/pencil
MACHINE_TYPE=`uname -m`
		case ${MACHINE_TYPE} in
			i586)
			ARCH=i386;;
			i686)
			ARCH=i386;;
			*)
			ARCH=${MACHINE_TYPE};;
		esac

apt-get install -y --force-yes libqt4-dev libming-dev rpm alien libphonon-dev libpng-dev

#build

#cd pencil
# temp compile fix in pencil SVN 54
#if [ ! -e .patched ]; then
#	sed -e "18d" -e "26d" -i src/main.cpp && 
#	touch .patched
#fi
make clean || true
qmake-qt4
make

#(cd src/plugins/imageplugin && qmake-qt4 && make) || return 1
#(cd src/plugins/xsheetplugin && qmake-qt4 && make) || return 1

#install
install -d ${PREFIX}/
install -m755 Pencil ${PREFIX}/pencil-bin
install -d ${PREFIX}/lib
#install ming library
#install -m755  /usr/lib/libming.so* ${PREFIX}/lib/
install -m755  /usr/lib/libming* ${PREFIX}/lib/
#install -m755  /usr/lib/libphonon.so.4 ${PREFIX}/lib/
#install -m755 release/Pencil ${PREFIX}/Pencil
#install -d ${PREFIX}/plugins
#install -m644 release/plugins/libpencil_imageplugin.so ${PREFIX}/plugins/libpencil_imageplugin.so
#install -m644 release/plugins/libpencil_xsheetplugin.so ${PREFIX}/plugins/libpencil_xsheetplugin.so
#install -D -m644 release/libpencil_structure.so.1.0.0 \
#${PREFIX}/libpencil_structure.so.1
install -D -m644 icons/icon.png ${PREFIX}/pencil.png

cat > ${PREFIX}/pencil << EOS
#!/bin/sh
BF_DIST_BIN=\`dirname "\$0"\`
BINARY=\`basename "\$0"\`
BF_PROGRAM=\${BINARY}-bin
exitcode=0

LD_LIBRARY_PATH=\${BF_DIST_BIN}/lib:\${LD_LIBRARY_PATH}

if [ -n "\$LD_LIBRARYN32_PATH" ]; then
    LD_LIBRARYN32_PATH=\${BF_DIST_BIN}/lib:\${LD_LIBRARYN32_PATH}
fi
if [ -n "\$LD_LIBRARYN64_PATH" ]; then
    LD_LIBRARYN64_PATH=\${BF_DIST_BIN}/lib:\${LD_LIBRARYN64_PATH}
fi
if [ -n "\$LD_LIBRARY_PATH_64" ]; then
    LD_LIBRARY_PATH_64=\${BF_DIST_BIN}/lib:\${LD_LIBRARY_PATH_64}
fi

export LD_LIBRARY_PATH LD_LIBRARYN32_PATH LD_LIBRARYN64_PATH LD_LIBRARY_PATH_64 LD_PRELOAD

"\$BF_DIST_BIN/\$BF_PROGRAM" \${1+"\$@"} 2>&1 | sed -e "s|\${BF_DIST_BIN}/\${BF_PROGRAM}|\${BF_DIST_BIN}/\${BINARY}|"
exitcode=\$?
exit \$exitcode
EOS
chmod uga+x ${PREFIX}/pencil

#package
#%define __spec_install_post /bin/true
cat > pencil.spec << EOS

Name:           pencil2d
Version:        ${VERSION}
Release:        ${REVISION}.$RELEASE
Summary:        2D Traditional Animation package
Group:          Applications/Graphics
License:        GPL
URL:            http://www.pencil-animation.org/
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
Requires:       qt >= 4.4.0 qt-x11 giflib phonon
AutoReqProv: no

%description
Pencil is an animation/drawing software for Mac OS X, Windows, and Linux. It lets you create traditional hand-drawn animation (cartoon) using both bitmap and vector graphics.

%prep


%build

%install
rm -rf \$RPM_BUILD_ROOT
mkdir -p \$RPM_BUILD_ROOT/${PREFIX}
cp -r  ${PREFIX}/* \$RPM_BUILD_ROOT/${PREFIX}

mkdir -p \$RPM_BUILD_ROOT/usr/share/applications
cat > \$RPM_BUILD_ROOT/usr/share/applications/pencil.desktop << EOF
[Desktop Entry]
Encoding=UTF-8
Name=Pencil 2D
Comment=2D Traditional Animation
Exec=pencil2d
Icon=pencil.png
Terminal=false
Type=Application
Categories=Graphics;Application;
MimeType=application/x-pencil;
X-Desktop-File-Install-Version=0.15
EOF

mkdir -p \$RPM_BUILD_ROOT/usr/share/mime/packages
cat > \$RPM_BUILD_ROOT/usr/share/mime/packages/pencil.xml <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<mime-info xmlns="http://www.freedesktop.org/standards/shared-mime-info">
    <mime-type type="application/x-pencil">
        <comment xml:lang="en">Pencil Animation</comment>
        <glob pattern="*.pcl" />
        <magic priority="80">
			<match value="&lt;!DOCTYPE PencilDocument" type="string" offset="0:64"/>
    	</magic>
    	<icon name="pencil"/>
  </mime-type>
</mime-info>
EOF

mkdir -p \$RPM_BUILD_ROOT/usr/share/pixmaps
mv \$RPM_BUILD_ROOT/${PREFIX}/pencil.png \$RPM_BUILD_ROOT/usr/share/pixmaps/pencil.png

mkdir -p \$RPM_BUILD_ROOT/usr/bin
cat > \$RPM_BUILD_ROOT/usr/bin/pencil2d << EOF
#!/bin/sh

${PREFIX}/pencil \\\${1+"\\\$@"} 2>&1 | sed -e 's|${PREFIX}/pencil|pencil|'
EOF
chmod a+x \$RPM_BUILD_ROOT/usr/bin/pencil2d

%clean
rm -rf \$RPM_BUILD_ROOT

%post
if [ -x /usr/bin/update-mime-database ]; then
  update-mime-database /usr/share/mime
fi
if [ -x /usr/bin/update-desktop-database ]; then
  update-desktop-database
fi

%postun
if [ -x /usr/bin/update-mime-database ]; then
  update-mime-database /usr/share/mime
fi
if [ -x /usr/bin/update-desktop-database ]; then
  update-desktop-database
fi

%files
%defattr(-,root,root,-)
$PREFIX
/usr/share/*
/usr/bin/*

%changelog
* Wed Jul 15 2009 Konstantin Dmitriev <ksee.zelgadis@gmail.com> - 0.4.4b-54.morevna.1
- First release

EOS

	rpmbuild -bb pencil.spec
    
    mv /root/rpmbuild/RPMS/$ARCH/pencil2d-${VERSION}-${REVISION}.$RELEASE.${ARCH}.rpm /root
    cd /root
	export LD_LIBRARY_PATH=${PREFIX}/lib:${LD_LIBRARY_PATH}
    alien -k pencil2d-${VERSION}-${REVISION}.$RELEASE.${ARCH}.rpm --scripts
    [ ! -d pencil2d-${VERSION} ] || rm -rf pencil2d-${VERSION}
    mv /root/pencil2d*${VERSION}*${REVISION}*$RELEASE* /mnt
    echo
    echo
    echo "Generated packages:"
    ls -1 /mnt/pencil2d*${VERSION}*${REVISION}*$RELEASE*
