# not a GNU package. You can remove this line, if
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 1.4

# set the include path found by configure
INCLUDES = $(LIBUSB_INCLUDES)

bin_PROGRAMS = fw_update
fw_update_SOURCES = fu_usage.cpp main.cpp pic_bootloader.cpp
SUBDIRS = exception buffer dtrace image osdep parser
fw_update_LDADD = $(top_builddir)/buffer/libbuffer.a \
	$(top_builddir)/dtrace/libdtrace.a $(top_builddir)/exception/libexception.a \
	$(top_builddir)/image/libimage.a $(top_builddir)/parser/libparser.a $(LIBUSB_LIBRARIES) \
	-L$(top_builddir)/buffer -L$(top_builddir)/dtrace -L$(top_builddir)/exception \
	-L$(top_builddir)/image -L$(top_builddir)/parser -lbuffer -ldtrace -lexception -limage -lparser
noinst_HEADERS = boot_if.h fu_usage.h pic_bootloader.h u2io_fw_update.h
