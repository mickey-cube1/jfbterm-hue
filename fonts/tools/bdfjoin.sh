#! /bin/bash

proc_1st_file_header()
{
	while read s ; do
		case "$s" in
		FONT\ *)
			echo "${s%-*-*}-$1-$2"
			;;
		CHARSET_REGISTRY\ *)
			echo "CHARSET_REGISTRY \"$1\""
			;;
		CHARSET_ENCODING\ *)
			echo "CHARSET_ENCODING \"$2\""
			;;
		CHARS\ *)
			echo "$s"
			return
			;;
		*)
			echo "$s"
			;;
		esac
	done
}

parse_font()
{
	fs=""

	while read s ; do
		case "$s" in
		ENCODING\ *)
			code=${s#ENCODING}
			codeh=$(printf "0x%04X" $code)
			fs="$codeh,"
			;;
		ENDCHAR)
			echo "$fs"
			;;
		*)
			fs="$fs:$s"
			;;
		esac
	done
}

load_code_map()
{
	while read lc uc comment ; do
		echo "$lc,$uc"
	done
}

remap()
{
	while [ "x$1" != x -a "x$2" != x ] ; do
		join -1 1 -2 1 -e '*' -t, -o 1.2,2.2 \
			<(cat "$1" | grep -v "^#" | load_code_map) \
			<(cat "$2" | parse_font)
		shift 2
	done
}

expand()
{
	c=0

	while read l ; do
		code="${l%%,*}"
		printf "STARTCHAR ${code#0x}\nENCODING %d\n" $code
		echo "${l#*,:}" | tr ':' '\n'
		printf "ENDCHAR\n\n"
		c=$((c + 1))
	done

	echo "#CHARS $c"
}

proc_1st_file_header ISO10646 1 < shnm8x16r.bdf
remap jis0201-unicode.map shnm8x16r.bdf jis0208-unicode.map shnmk16.bdf | sort | expand
#remap jis0201-unicode.map shnm8x16r.bdf | sort | expand
echo "ENDFONT"

exit 0
