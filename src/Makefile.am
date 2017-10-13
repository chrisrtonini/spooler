## Processar este arquivo com automake para produzir Makefile.in
## Criado por Chris Robert Tonini <chrisrtonini@gmail.com>

bin_PROGRAMS = spooler

AM_CPPFLAGS = \
	-DPACKAGE_SERVICE=1 \
	-DPACKAGE_UTILS=0

AM_CFLAGS = \
	-Wall \
	-g \
	-m32

spooler_SOURCES = \
	main.cc \
	pktparam.h \
	pktparam.cc \
	packet.h \
	packet.cc \
	pkttags.h \
	pkttags.cc \
	pktrequest.h \
	pktrequest.cc \
	pktresponse.h \
	pktresponse.cc \
	cmdtags.h \
	cmdtags.cc \
	datapair.h \
	datapair.cc \
	txtwriter.h \
	txtwriter.cc \
	rspwriter.hpp \
	rqtreader.hpp \
	txtreader.h \
	txtreader.cc \
	txttags.h \
	collector.h \
	collector.cc \
	fs.h \
	fs.cc \
	loopctrl.h \
	loopctrl.cc \
	txtcollector.h \
	txtcollector.cc \
	bemafi.h \
	ecfbematech.h \
	ecfbematech.cc \
	ecf.h \
	ecf.cc \
	excp.h \
	factory.h \
	factory.cc \
	utils.h \
	aliquota.h \
	aliquota.cc \
	ecfbroker.h \
	ecfbroker.cc \
	session.h \
	session.cc \
	vrstags.h \
	log.h \
	log.cc \
	bemafi_cv0909.h \
	ecfbematechcv0909.h \
	ecfbematechcv0909.cc \
	filequeue.h \
	filequeue.cc

spooler_CFLAGS = \
	-m32

spooler_CXXFLAGS = \
	-m32

spooler_LDFLAGS = \
	-m32

spooler_LDADD = \
	$(BEMA_LIBS)
