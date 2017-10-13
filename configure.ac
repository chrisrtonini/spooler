#
# Projeto: spooler
#          Gateway para emissao de documentos (fiscais e nao fiscais) em
#          ECFs (Emissores de Cupons Fiscais).
# Criado por Chris Robert Tonini <chrisrtonini@gmail.com>
#

AC_INIT(spooler, 4.1-0b, [chrisrtonini@gmail.com])
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

AC_LANG([C++])
AC_PROG_CXX
AC_ISC_POSIX
AC_HEADER_STDC

AC_CONFIG_HEADERS([config.h])

AC_PREFIX_DEFAULT([/usr])

AC_SUBST(APP_NAME)
AC_SUBST(APP_DESCR)
AC_SUBST(APP_VERSION)
AC_SUBST(APP_RELEASE)
AC_SUBST(APP_STATUS)

AC_CONFIG_MACRO_DIR([m4])
AM_MAINTAINER_MODE

AM_PROG_CC_STDC
AM_PROG_CC_C_O
AM_PROG_LIBTOOL


dnl Geracao condicional de manuais, buscando evitar 'quebra' durante o make
dnl especialmente por falta do modulo (Perl) 'ExtUtils::Command:MM', que
dnl contem em sua interface o comando 'pod2man'.
#
# Habilitar/desabilitar geracao de manuais, os quais sao codificados em
# formato POD (Plain Old Document), que necessita de um conversor em Perl.
#
AC_ARG_WITH([manual],
            AS_HELP_STRING([--without-manual], [Nao gera manuais])
)

#
# Verificar a disponibilidade do modulo conversor, para gerar arquivos de
# manual a partir de seus fontes POD. Caso nao esteja disponivel, desabilitara
# a geracao da documentacao, a despeito da opcao do usuario.
#
AC_MSG_NOTICE([Testar possibilidade de geracao de manuais])
AS_IF([test "x$with_manual" != "xno"],
	[AS_IF([perl -e 'eval "use ExtUtils::Command::MM; 1" or die "Nao encontrado ExtUtils::Command::MM"'],
		[WITH_MAN="yes"],
		[AC_MSG_WARN([Impossivel gerar manuais])])],
	[WITH_MAN="no"]
)

AM_CONDITIONAL([GEN_MANUAL], [test "x$WITH_MAN" == "xyes"])
AM_COND_IF([GEN_MANUAL],
	[AC_MSG_NOTICE([Geracao de manuais habilitada])],
	[AC_MSG_NOTICE([Geracao de manuais desabilitada])]
)


dnl Criptografia de informacoes (cifras RC4) e geracao de hashes (MD5 e SHA1)
dnl dependem da 'libcrypto'.
#
# Testar disponibilidade de bibliotecas para criptografia e hashes
#
PKG_CHECK_MODULES(CRYPTO_NEED, libcrypto)


dnl Diante da impossibilidade de utilizar 'pkg-config' para testar a presenca
dnl das bibliotecas da Bematech, os testes a seguir verificam individualmente
dnl cada componente, definindo simbolos de compilacao visando habilitar ou
dnl desabilitar o suporte aa manipulacao da MFD.
#
# Verificacao da presenca da biblioteca libbemafiscal.so
#
AC_MSG_NOTICE([Verificacao do kit de desenvolvimento Bematech])
AC_CHECK_FILE(
	[/usr/lib/libbemafiscal.so],
	[AC_SUBST(BEMA_LIBS, "-ldl -lpthread -lbemafiscal")],
	[AC_MSG_FAILURE([Kit Bematech faltando ou incompleto!], 2)]
)

#
# Verificacao das bibliotecas adicionais: 
# libbemamfd, libbemamfd2 e libbemamfd3
#
AC_MSG_NOTICE([Verificando libbemamfd.so])
AC_CHECK_FILE(
	[/usr/lib/libbemamfd.so],
	[AC_DEFINE([BEMAMFD_SUPPORT], [1], [libbemamfd.so presente no sistema])],
	[AC_MSG_WARN([Kit Bematech sem libbemamfd.so])]
)

AC_MSG_NOTICE([Verificando libbemamfd2.so])
AC_CHECK_FILE(
	[/usr/lib/libbemamfd2.so],
	[AC_DEFINE([BEMAMFD2_SUPPORT], [1], [libbemamfd2.so presente no sistema])],
	[AC_MSG_WARN([Kit Bematech sem libbemamfd2.so])]
)

AC_MSG_NOTICE([Verificando libbemamfd3.so])
AC_CHECK_FILE(
	[/usr/lib/libbemamfd3.so],
	[AC_DEFINE([BEMAMFD3_SUPPORT], [1], [libbemamfd3.so presente no sistema])],
	[AC_MSG_WARN([Kit Bematech sem libbemamfd3.so])]
)


dnl Como as bibliotecas da Bematech somente sao distribuidas em sua versao de
dnl 32 bits, a seguir, eh testada a possibilidade de linka-la. Acusando assim,
dnl uma possivel falta do kit de 32 bits, caso o ambiente seja de 64 bits.
#
# Definicao de flag de compilacao para 32 bits. Necessario ao uso da 
# libbemafiscal, distribuida somente para esta plataforma.
#
CXXFLAGS="$CXXFLAGS -m32"
AC_SUBST(CXXFLAGS)

AC_CHECK_LIB([bemafiscal], [Bematech_FI_ReducaoZ],,
[AC_MSG_FAILURE([Impossivel linkar com libbemafiscal!
Verificar se o kit de desenvolvimento de 32bits esta presente.],2)],
[$BEMA_LIBS])


dnl A variavel LIBS e resetada como forma de evitar a linkedicao de todos
dnl os componentes do projeto com a libbemafiscal ('-lbemafiscal').

AC_SUBST(LIBS, "")

AC_OUTPUT([
Makefile
utils/rc4_crypt/Makefile
src/Makefile
man/Makefile
])
