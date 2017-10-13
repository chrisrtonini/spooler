###
### Projeto: spooler
###          Gateway para emissao de documentos (fiscais e nao fiscais) em
###          ECFs (Emissores de Cupons Fiscais).
###
###          -= Produz Makefile para documentacao do projeto =-
###
### Criado por Chris Robert Tonini <chrisrtonini@gmail.com>
###

###
### Linha de comando para execucao do interpretador Perl, acrescido do modulo
### necessario para a conversao dos arquivos de documentacao POD em manuais. 
###
PERL = /usr/bin/perl
PERLRUN = $(PERL) -MExtUtils::Command::MM
PODC = $(PERLRUN) -e pod2man "--"

###
### Flags para definir informacoes adicionais aos manuais.
###
PODCFLAGS = -c "Framework de impressao fiscal" -r "@PACKAGE_STRING@" -u

### Manual do utilitario 'rc4_crypt'
rc4_crypt.man:
	$(PODC) $(PODCFLAGS)	--name="rc4_crypt"		--section=1 \
		rc4_crypt.pod rc4_crypt.man

### Manual do servidor de impressao ('spooler')
spooler.man:
	$(PODC) $(PODCFLAGS)	--name="spooler"		--section=1 \
		spooler.pod spooler.man

### Manual detalhando opcoes de configuracao do servidor.
spooler.cfg.man:
	$(PODC) $(PODCFLAGS)	--name="spooler.cfg"	--section=5 \
		spooler.cfg.pod spooler.cfg.man

## 
## Definicao opcional de targets para Makefile. Caso nao seja possivel
## converter .pod em .man, o bloco a seguir sera' suprimido do Makefile.in
## 

if GEN_MANUAL

### Geracao de manuais de aplicativos.
man1_MANS = rc4_crypt.man \
			spooler.man

### Geracao de manuais de arquivos de configuracao.
man5_MANS = spooler.cfg.man

### Geracao de todos os manuais (target personalizado)
manual: \
	rc4_crypt.man \
	spooler.man  \
	spooler.cfg.man

endif

CLEANFILES = *.man
