#!/bin/sh -e

set -e


echo "test => $0"

case "$(uname -s)" in
	Linux)
		SHA1_BIN=$(which sha1sum)
	;;
	*BSD)
		SHA1_BIN="$(which sha1) -r"
	;;
	*)
		echo "unknown system."
		exit 1
	;;
esac

if [ ! -r regress/test_export.kcd ];then
	echo "$0 test failed (unreadable export file)!"
	exit 1
fi


PASSWORD=aabbccdd112233


printf "del 0\nyes\nwrite\n" |./kc -b -k regress/test -p regress/testpass
printf "del 0\nyes\nwrite\n" |./kc -b -k regress/test -p regress/testpass
printf "append regress/test_export.kcd\n${PASSWORD}\nwrite\n" |./kc -b -k regress/test -p regress/testpass

SHA1=$(printf "list\n" |KC_DEBUG=yes ./kc -b -k regress/test -p regress/testpass |grep -E -e '^[[:space:]]*<.*>$' |sed -e 's/created="[0-9]\{1,\}"//' -e 's/modified="[0-9]\{1,\}"//' |$SHA1_BIN |cut -d' ' -f1)
if [ "$SHA1" = '978106cf0f2307586b663a7782bba61f69cf8d13' ];then
	echo "$0 test ok (append)!"
else
	echo "$0 test failed (append)!"
	exit 1
fi


printf "del 0\nyes\nwrite\n" |./kc -b -k regress/test -p regress/testpass
printf "del 0\nyes\nwrite\n" |./kc -b -k regress/test -p regress/testpass
printf "import regress/test_export.kcd\n${PASSWORD}\nwrite\n" |./kc -b -k regress/test -p regress/testpass

SHA1=$(printf "list\n" |KC_DEBUG=yes ./kc -b -k regress/test -p regress/testpass |grep -E -e '^[[:space:]]*<.*>$' |sed -e 's/created="[0-9]\{1,\}"//' -e 's/modified="[0-9]\{1,\}"//' |$SHA1_BIN |cut -d' ' -f1)
if [ "$SHA1" = '23fa5cccf87201c76a9fd7b29be13cfda3b2295b' ];then
	echo "$0 test ok (import)!"
else
	echo "$0 test failed (import)!"
	exit 1
fi


printf "del 0\nyes\nwrite\n" |./kc -b -k regress/test -p regress/testpass
printf "del 0\nyes\nwrite\n" |./kc -b -k regress/test -p regress/testpass
printf "appendxml regress/test_dump.xml\nwrite\n" |./kc -b -k regress/test -p regress/testpass

SHA1=$(printf "list\n" |KC_DEBUG=yes ./kc -b -k regress/test -p regress/testpass |grep -E -e '^[[:space:]]*<.*>$' |sed -e 's/created="[0-9]\{1,\}"//' -e 's/modified="[0-9]\{1,\}"//' |$SHA1_BIN |cut -d' ' -f1)
if [ "$SHA1" = '978106cf0f2307586b663a7782bba61f69cf8d13' ];then
	echo "$0 test ok (appendxml)!"
else
	echo "$0 test failed (appendxml)!"
	exit 1
fi


printf "del 0\nyes\nwrite\n" |./kc -b -k regress/test -p regress/testpass
printf "del 0\nyes\nwrite\n" |./kc -b -k regress/test -p regress/testpass
printf "importxml regress/test_dump.xml\nwrite\n" |./kc -b -k regress/test -p regress/testpass

SHA1=$(printf "list\n" |KC_DEBUG=yes ./kc -b -k regress/test -p regress/testpass |grep -E -e '^[[:space:]]*<.*>$' |sed -e 's/created="[0-9]\{1,\}"//' -e 's/modified="[0-9]\{1,\}"//' |$SHA1_BIN |cut -d' ' -f1)
if [ "$SHA1" = '23fa5cccf87201c76a9fd7b29be13cfda3b2295b' ];then
	echo "$0 test ok (importxml)!"
else
	echo "$0 test failed (importxml)!"
	exit 1
fi

exit 0
