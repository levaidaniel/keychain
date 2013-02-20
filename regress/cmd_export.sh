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

rm -f regress/test_export.kcd

PASSWORD=aabbccdd112233

printf "xport regress/test_export\n${PASSWORD}\n${PASSWORD}\n" |./kc -b -k regress/test -p regress/testpass

if [ ! -r "regress/test_export.kcd" ];then
	echo "$0 test failed (unreadable export file)!"
	exit 1
fi

if printf "${PASSWORD}" |./kc -b -k regress/test_export.kcd;then
	echo "$0 test ok (xport)!"
else
	echo "$0 test failed (xport)!"
	exit 1
fi


rm -f regress/test_dump.xml

printf "dump regress/test_dump\n" |./kc -b -k regress/test -p regress/testpass

if [ ! -r "regress/test_dump.xml" ];then
	echo "$0 test failed (unreadable dump file)!"
	exit 1
fi

SHA1=$(cat regress/test_dump.xml |sed -e 's/ created="[0-9]\{1,\}"//' -e 's/ modified="[0-9]\{1,\}"//' -e 's/ description=".*"//' |$SHA1_BIN |cut -d' ' -f1)
if [ "$SHA1" = 'fcd724024dbbab3a99afbd103f3ead5e97fe24b4' ];then
	echo "$0 test ok (dump)!"
else
	echo "$0 test failed (dump)!"
	exit 1
fi

exit 0
