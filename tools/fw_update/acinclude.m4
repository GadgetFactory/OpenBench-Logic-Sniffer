AC_DEFUN([VL_LIB_LIBUSB], [

  ac_libusb_includedir=""
  ac_libusb_libdir=""
  ac_libusb_lib="-lusb"

  AC_ARG_WITH(usb,
        AC_HELP_STRING([--with-usb=DIR],[Where the libusb is inctalled. ]),
        [
          ac_libusb_includedir="-I$withval/include"
          ac_libusb_libdir="-L$withval/lib"
        ])

  AC_ARG_WITH(usb-lib,
        AC_HELP_STRING([--with-usb-lib=DIR],[Where the libusb libraries are. ]),
        [
          ac_libusb_libdir="-L$withval"
        ])

  AC_ARG_WITH(usb-includes,
        AC_HELP_STRING([--with-usb-includes=DIR],[Where the libusb includes are. ]),
        [
          ac_libusb_includedir="-I$withval"
        ]
        )

  AC_MSG_CHECKING([for libusb])

  safe_CFLAGS="$CFLAGS"
  safe_CXXFLAGS="$CXXFLAGS"
  CFLAGS="$CFLAGS $ac_libusb_includedir $ac_libusb_libdir $ac_libusb_lib"
  CXXFLAGS="$CXXFLAGS $ac_libusb_includedir $ac_libusb_libdir $ac_libusb_lib"
  libusb_link="no"
  AC_TRY_LINK(
              [
#include <usb.h>
              ],
              [
                usb_init();
              ],
                libusb_link="yes",
                libusb_link="no"
             )
  CFLAGS="$safe_CFLAGS"
  CXXFLAGS="$safe_CXXFLAGS"

  if test "$libusb_link" = "no"; then
    AC_MSG_ERROR([libusb not found])
  fi

  AC_MSG_RESULT([$libusb_link ($ac_libusb_libdir $ac_libusb_includedir $ac_libusb_lib)])

  LIBUSB_INCLUDES="$ac_libusb_includedir"
  LIBUSB_LIBRARIES="$ac_libusb_libdir $ac_libusb_lib"
  AC_SUBST(LIBUSB_INCLUDES)
  AC_SUBST(LIBUSB_LIBRARIES)
])dnl
