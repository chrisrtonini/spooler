###
### Projeto: spooler
###          Gateway para emissao de documentos (fiscais e nao fiscais) em
###          ECFs (Emissores de Cupons Fiscais).
###
###         -= Constroi todo o projeto, incluindo subdiretorios =-
###
### Criado por Chris Robert Tonini <chrisrtonini@gmail.com>
###

AUTOMAKE_OPTIONS = gnu
ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src utils/rc4_crypt man tests
EXTRA_DIST = man/rc4_crypt.pod \
				man/spooler.cfg.pod \
				man/spooler.pod

###
### ---
### Na criacao do link simbolico (necessario aa execucao dos scripts contidos
### no pacote 'spooler-tools') o script abaixo trata duas situacoes distintas:
### 1) na primeira instalacao, a execucao do 'test' para verificar a exis-
###		tencia previa do link e entao remove-lo, estava falhando e interrom-
###		pendo o 'make'
### 2) nas demais instalacoes, quando ja existe um link, sem o teste e o
###		subsequente 'unlink' o 'ln' simplesmente nao estava substituindo o
###		link ja existente.
### ---
### Diretorio /sgi/ecf/ecf0 utilizado (em modo de compatibilidade com o
### ecf_server) para o PDV efetuar 'bloqueio' de sessao com ECF.
### ---
### Diretorio /sgi/ecf/registro criado para armazenar registros de leituras
### para analise futura
### ---
###
install-exec-hook:
	$(LN_S) -f $(abs_top_srcdir) ../spooler
	test -h ../spooler && unlink ../spooler
	$(LN_S) -f $(abs_top_srcdir) ../spooler
	test -d /sgi/ecf/ecf0 || $(MKDIR_P) /sgi/ecf/ecf0
	test -d /sgi/ecf/registro || $(MKDIR_P) /sgi/ecf/registro
