#!/bin/sh

cd $1

echo "<!DOCTYPE RCC><RCC version=\"1.0\">"  > pertubis.qrc
echo "  <qresource>" >> pertubis.qrc

for i in images/*; do
	echo "    <file>${i}</file>" >> pertubis.qrc;
done

for i in i18n/*.qm; do
	echo "    <file>${i}</file>" >> pertubis.qrc
done

echo "  </qresource>" >> pertubis.qrc
echo "</RCC>" >> pertubis.qrc

p=`pwd`
echo "${p}/pertubis.qrc"
