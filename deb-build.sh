
rm -rf deb

mkdir deb
mkdir deb/DEBIAN
mkdir deb/usr
mkdir deb/usr/bin
mkdir deb/usr/share

cp control deb/DEBIAN/

cd deb

echo -e "2.0\n" > DEBIAN/debian-binary

cp /usr/bin/tuxanci-ng usr/bin/
cp -rf /usr/share/tuxanci-ng usr/share/

md5sum `find ./ -type f | awk '/.\// { print substr($0, 3) }'` > DEBIAN/md5sums

cd ..
rm -rf *.deb
dpkg -b ./deb tuxanci-ng_i386.deb
