#!/bin/sh

cd $1

echo "<!DOCTYPE RCC><RCC version=\"1.0\">"  > perturbis.qrc
echo "  <qresource>" >> perturbis.qrc

for i in images/*; do
	echo "    <file>${i}</file>" >> perturbis.qrc;
done

for i in i18n/*.qm; do
	echo "    <file>${i}</file>" >> perturbis.qrc
done

echo "  </qresource>" >> perturbis.qrc
echo "</RCC>" >> perturbis.qrc

p=`pwd`
echo "${p}/perturbis.qrc"
