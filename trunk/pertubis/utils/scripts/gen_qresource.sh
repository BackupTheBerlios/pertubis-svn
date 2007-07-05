echo -e "<!DOCTYPE RCC><RCC version=\"1.0\">
\t<qresource>" > pertubis.qrc
for i in *.xpm;do echo -e "\t\t<file>${i}</file>" >> pertubis.qrc;done
echo -e "\t</qresource>
</RCC>" >> pertubis.qrc
path=`pwd`
echo "${path}/pertubis.qrc"
