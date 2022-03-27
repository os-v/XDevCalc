
AC_INIT

AC_CANONICAL_HOST

AC_PROG_CC
AC_PROG_CXX

PLATFORM=
DEBUG=
CFLAGS=
LFLAGS=
PCFLAGS=
INCLUDEPATH=
LIBFILES=

AC_ARG_ENABLE(debug, AS_HELP_STRING([--enable-debug], [Enable debug linkage]))

if test x$enable_debug = xyes; then
	DEBUG=-g
fi

case $host_os in
	*bsd*)
		PLATFORM=FreeBSD
		CFLAGS="-pipe -O2 -std=gnu++11 -Wno-unused-result -Wno-int-to-pointer-cast -fPIC"
		LFLAGS="-Wl,-O1 -static"
		INCLUDEPATH="-I/usr/local/include -I./Libs"
		LIBFILES="-L./Libs/mpir -L/usr/local/lib -lmpfr -lgmp -lgmpxx"
		;;
	linux*)
		PLATFORM=Linux
		CFLAGS="-pipe -O2 -std=gnu++11 -Wno-unused-result -Wno-int-to-pointer-cast -fPIC"
		LFLAGS="-Wl,-O1 -static"
		INCLUDEPATH="-I/usr/local/include -I./Libs"
		LIBFILES="-L./Libs/mpir -L/usr/local/lib -lmpfr -lgmp -lgmpxx"
		;;
	darwin*)
		PLATFORM=MacOS
		CFLAGS="-pipe -std=gnu++11 -Wno-unused-result -Wno-int-to-pointer-cast -fPIC"
		LFLAGS="-Wl"
		PCFLAGS="-x objective-c++"
		INCLUDEPATH="-I/usr/local/include -I./Libs"
		LIBFILES="-L./Libs/mpir -L/usr/local/lib -lmpfr -lmpz -lgmp -framework Cocoa"
		;;
esac

AC_SUBST(DEBUG)
AC_SUBST(CFLAGS)
AC_SUBST(LFLAGS)
AC_SUBST(PCFLAGS)
AC_SUBST(INCLUDEPATH)
AC_SUBST(LIBFILES)

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

