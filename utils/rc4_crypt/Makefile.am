###
### Projeto: spooler
###          Gateway para emissao de documentos (fiscais e nao fiscais) em
###          ECFs (Emissores de Cupons Fiscais).
###
###         -= Makefile para construcao de utilitario 'rc4_crypt' =-
###
### Criado por Chris Robert Tonini <chrisrtonini@gmail.com>
###

bin_PROGRAMS = rc4_crypt

AM_CPPFLAGS = \
	-DPACKAGE_SERVICE=0 \
	-DPACKAGE_UTILS=1

rc4_crypt_LDADD = \
	$(CRYPTO_NEED_LIBS)

rc4_crypt_SOURCES = \
	rc4_crypt.c

