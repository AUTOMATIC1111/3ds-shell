mkdir -p 3ds-shell-x64
cp Release-x64/3ds-shell.dll 3ds-shell-x64/3ds-shell-x64.dll
cp -r ../boxshot ../info images CHANGELOG.txt version.txt LICENSE.txt README.txt *.bat config.ini 3ds-shell-x64
cp ../custom/* 3ds-shell-x64/info

mkdir -p 3ds-shell-x32
cp Release-x32/3ds-shell.dll 3ds-shell-x32/3ds-shell-x32.dll
cp -r ../boxshot ../info images CHANGELOG.txt version.txt LICENSE.txt README.txt *.bat config.ini 3ds-shell-x32
cp ../custom/* 3ds-shell-x32/info

mkdir -p 3ds-shell-src
cp -r                                 CHANGELOG.txt version.txt LICENSE.txt README.txt *.bat config.ini 3ds-shell-src
cp -r images 0.sh *.sln *.vcxproj *.def *.idl 3ds-shell-src

for file in *.cc *.h; do
	echo "/*" > 3ds-shell-src/$file
	echo >> 3ds-shell-src/$file
	cat LICENSE.txt >> 3ds-shell-src/$file
	echo >> 3ds-shell-src/$file
	echo "*/" >> 3ds-shell-src/$file
	echo >> 3ds-shell-src/$file

	cat $file >> 3ds-shell-src/$file
done

rm 3ds-shell-src/ShellPreview_h.h

VERSION=`cat version.txt|tr -d \"`
echo $VERSION

rm -f 3ds-shell-$VERSION-x64.rar 3ds-shell-$VERSION-x32.rar 3ds-shell-$VERSION-src.rar
rar a 3ds-shell-$VERSION-x64.rar 3ds-shell-x64
rar a 3ds-shell-$VERSION-x32.rar 3ds-shell-x32
rar a 3ds-shell-$VERSION-src.rar 3ds-shell-src



