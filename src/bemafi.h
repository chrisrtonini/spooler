/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * spooler
 * Copyright (C) Chris Robert Tonini 2015 <chrisrtonini@gmail.com>
 * 
 * spooler is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * spooler is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _BEMAFI_H_
#define _BEMAFI_H_

#ifdef WIN32

#define	DLL_IMP			__declspec(dllimport)
#define CALL_CONV		__stdcall

#else

#define	CALL_CONV

#ifdef __cplusplus

#define DLL_IMP
extern "C"	{

#else

#define DLL_IMP			extern

#endif

#endif

#define BEMAEX			DLL_IMP int CALL_CONV

#define	ACRESCIMO		"A"
#define	DESCONTO		"D"
#define ACRESC_DESC		"X"

#define PERCENTUAL		"%"
#define	VALOR			"$"

#ifndef ASCII_ACK
#define ASCII_ACK		0x06
#endif

#ifndef	ASCII_NAK
#define	ASCII_NAK		0x15
#endif

#define BMTCH_IMPRESSORA_OK			1
#define BMTCH_CHEQUE_IMPRESSAO		2
#define BMTCH_CHEQUE_POSICIONADO	3
#define BMTCH_AGUARDANDO_CHEQUE		4

#define BMTCH_CORTE_PARCIAL			0
#define BMTCH_CORTE_TOTAL			1
#define BMTCH_SEM_CORTE				2

#define BMTCH_DADOS_TXT				0
#define BMTCH_DADOS_RTF				1
#define BMTCH_DADOS_MDB				2

#define BMTCH_DOWNLOAD_TOTAL		0
#define BMTCH_DOWNLOAD_DATA			1
#define BMTCH_DOWNLOAD_COO			2

#define BMTCH_LEITURA_COMPLETA		"c"
#define BMTCH_LEITURA_SIMPLIFICADA  "s"

#define BMTCH_TIPO_DATA				"D"
#define BMTCH_TIPO_COO				"C"

#define BMTCH_GERACAO_MF			0
#define BMTCH_GERACAO_MFD			1
#define BMTCH_GERACAO_TDM			2
#define BMTCH_GERACAO_RZ			3
#define BMTCH_GERACAO_RFD			4


// Status estendido...
union bematech_status_estendido	{
	unsigned char	byte;
	struct			{
		unsigned short	compr_nao_fiscal_aberto		: 1;
		unsigned short	cdc_aberto					: 1;
		unsigned short	rel_gerencial_aberto		: 1;
		unsigned short	totalizando_cupom			: 1;
		unsigned short	unused_1					: 1; // não utilizado
		unsigned short	permite_cancelamento_cnf	: 1;
		unsigned short	estorno_cdc_permitido		: 1;
		unsigned short	unused_2					: 1; // não utilizado
	} bits;
};

// Estrutura de dados para aplicar...
union bematech_flags_fiscais	{
	// byte contendo flags fiscais...
	unsigned char	byte;
	// à estrutura de dados que permita analisá-los.
	struct 			{
		unsigned short	cupom_fiscal_aberto			: 1;
		unsigned short	fecha_pgto_iniciado			: 1;
		unsigned short	horario_verao				: 1;
		unsigned short	reducao_z					: 1;
		unsigned short	unused_1					: 1;	// não utilizado
		unsigned short	pode_canc_cupom_fiscal		: 1;
		unsigned short	unused_2					: 1;	// não utilizado
		unsigned short	memoria_fiscal_lotada		: 1;
	} bits;
};

// Flags Fiscais III MFD
union bematech_flags_fiscais_3	{
	unsigned char	byte;
	struct			{
		unsigned short	pouco_papel					: 1;
		unsigned short	sensor_papel_habilitado		: 1;
		unsigned short	cancelamento_automatico		: 1;
		unsigned short	desconto_issqn_habilitado	: 1;
		unsigned short	reducaoz_auto_habilitada	: 1;
		unsigned short	impressora_online			: 1;
		unsigned short	unused_1					: 1;
		unsigned short	unused_2					: 1;
	} bits;
};

// Registro de informações de uso de departamentos.
struct bematech_reg_dpto	{
	char				vlr_acumulado[20];
	char				qtde_uso[20];
	char				descricao[10];
};

// Registro de informações de uso de formas de pagamento.
struct bematech_reg_fpgto	{
	char				descricao[16];
	char				vlr_acumulado[20];
	char				vlr_rec_ult_cupom[20];
	char				flag_uso_vinc;
};

struct bematech_reg_fpgto_mfd	{
	char				descricao[16];
	char				vlr_acumulado[14];
	char				vlr_rec_ult_cupom[14];
	char				flag_uso_vinc;
	char				terminador; // DLL retorna uma ',' (vírgula) como
									// separador de registro.
};

// Registro de dados da última redução Z
struct bematech_dados_reducaoz	{
	char				modo[2];
	char				grande_total[18];
	char				cancelamentos[14];
	char				descontos[14];
	char				tributos[64];
	char				tot_parc_tributados[266];
	char				sangria[14];
	char				suprimentos[14];
	char				tot_nao_icms[126];
	char				cont_tot_nao_icms[36];
	char				coo[6];
	char				coo_nao_icms[6];
	char				nr_aliquotas[2];
	char				data_movimento[6];
	char				acrescimos[14];
	char				acrescimo_financeiro[14];
};

// Dados da última redução Z (MFD)
struct bematech_dados_reducaoz_mfd	{
	char				modo[3];
	char				contador_reinicio[5];
	char				contador_reducaoz[5];
	char				coo[7];
	char				contador_op_nao_fiscais[7];
	char				contador_cupom_fiscal[7];
	char				contador_rel_gerencial[7];
	char				contador_fita_detalhe[7];
	char				contador_canc_nao_fiscal[5];
	char				contador_cupom_fiscal_canc[5];
	char				contador_nao_fiscais[121];
	char				contador_rel_gerenciais[121];
	char				contador_cdc[5];
	char				contador_cdc_nao_emitidos[5];
	char				contador_cdc_cancelados[5];
	char				total_geral[19];
	char				tot_parciais_tributados[225];
	char				tot_isencao_icms[15];
	char				tot_nao_incidencia_icms[15];
	char				tot_substituicao_trib_icms[15];
	char				tot_isencao_issqn[15];
	char				tot_nao_incidencia_issqn[15];
	char				tot_substituicao_trib_issqn[15];
	char				tot_descontos_icms[15];
	char				tot_descontos_issqn[15];
	char				tot_acrescimos_icms[15];
	char				tot_acrescimos_issqn[15];
	char				tot_cancelamentos_icms[15];
	char				tot_cancelamentos_issqn[15];
	char				tot_parciais_nao_icms[393];
	char				tot_sangria[15];
	char				tot_suprimento[15];
	char				tot_canc_nao_fiscais[15];
	char				tot_desc_nao_fiscais[15];
	char				tot_acresc_nao_fiscais[15];
	char				aliquotas_tributarias[65];
	char				data_movimento[7];
};

// Registro de informações de recebimentos não fiscais
struct bematech_rcbto_nao_fiscal	{
	char				qtde_uso[4];
	char				vlr_acumulado[20];
	char				descricao[19];
};

struct bematech_rcbto_nao_fiscal_mfd	{
	char				descricao[19];
	char				vlr_acumulado[14];
};

// Registro de informações de uso de relatórios gerenciais
struct bematech_rel_ger_mfd	{
	char				qtde_uso[4];
	char				descricao[17];
	char				terminador; // é uma vírgula "," que consta no retorno
									// da DLL
};

// Tipo de impressora retornados pela DLL
enum bematech_tp_impressora	{
	ECF_GAVETA_AUTENTICACAO	= 1,
	ECF_GAVETA_CUTTER,
	ECF_PRESENTER_AUTENTICACAO,
	ECF_PRESENTER_CUTTER,
	PASSAGEM_GAVETA_AUTENTICACAO,
	PASSAGEM_GAVETA_CUTTER,
	PASSAGEM_PRESENTER_AUTENTICACAO,
	PASSAGEM_PRESENTER_CUTTER
};

// Registro de uso de totalizadores parciais...
struct bematech_reg_parciais	{
	char				tributados[224];
	char				isencao[14];
	char				nao_incidencia[14];
	char				substituicao[14];
	char				nao_icms[126];
	char				sangria[14];
	char				suprimento[14];
	char				grande_total[18];
};

struct bematech_reg_parciais_mfd	{
	char				tributados[224];
	char				isencao_icms[14];
	char				nao_incidencia_icms[14];
	char				substituicao_icms[14];
	char				isencao_issqn[14];
	char				nao_incidencia_issqn[14];
	char				substituicao_issqn[14];
	char				descontos_icms[14];
	char				acrescimos_icms[14];
	char				cancelamentos_icms[14];
	char				descontos_issqn[14];
	char				acrescimos_issqn[14];
	char				cancelamentos_issqn[14];
	char				nao_fiscais[420];			// + sangria e suprimento
	char				desc_nao_fiscais[14];
	char				acresc_nao_fiscais[14];
	char				canc_nao_fiscais[14];
	char				grande_total[18];
};


// Bits de status
union ST1	{
	unsigned char	byte;
	struct	{
		unsigned short num_params_invalido			: 1;
		unsigned short cupom_fiscal_aberto			: 1;
		unsigned short comando_inexistente			: 1;
		unsigned short inicio_dif_esc				: 1;
		unsigned short erro_impressora				: 1;
		unsigned short erro_rtc						: 1;
		unsigned short pouco_papel					: 1;
		unsigned short fim_de_papel					: 1;
	} bits;
};

union ST2	{
	unsigned char	byte;
	struct	{
		unsigned short comando_nao_executado		: 1;
		unsigned short proprietario_nao_programado	: 1;
		unsigned short canc_nao_permitido			: 1;
		unsigned short capacidade_aliq_esgotada		: 1;
		unsigned short aliq_nao_programada			: 1;
		unsigned short erro_cmos					: 1;
		unsigned short mem_fiscal_lotada			: 1;
		unsigned short tipo_param_invalido			: 1;
	} bits;
};

struct bematech_status_mfd	{
	unsigned char	st3;
	union ST2		st2;
	union ST1		st1;
	unsigned char	ack;
};


// Mensagens de status
static const char* const bematech_st3_msgs[] = {
	"Comando Ok",														// 0
	"Comando invalido",													// 1
	"Erro desconhecido",												// 2
	"Numero de parametro invalido",										// 3
	"Tipo de parametro invalido",										// 4
	"Todas aliquotas ja programadas",									// 5
	"Totalizador nao fiscal ja programado",								// 6
	"Cupom Fiscal aberto",												// 7
	"Cupom Fiscal fechado",												// 8
	"ECF ocupado",														// 9
	"Impressora em erro",												// 10
	"Impressora sem papel",												// 11
	"Impressora com cabeca levantada",									// 12
	"Impressora off line",												// 13
	"Aliquota nao programada",											// 14
	"Terminador de string faltando",									// 15
	"Acrescimo ou desconto maior que o total do cupom fiscal",			// 16
	"Cupom Fiscal sem item vendido",									// 17
	"Comando nao efetivado",											// 18
	"Sem espaco para novas formas de pagamento",						// 19
	"Forma de pagamento nao programada",								// 20
	"Indice maior que numero de forma de pagamento",					// 21
	"Formas de pagamento encerradas",									// 22
	"Cupom nao totalizado",												// 23
	"Comando maior que 7Fh (127d)",										// 24
	"Cupom Fiscal aberto e sem item",									// 25
	"Cancelamento nao imediataento apos",								// 26
	"Cancelamento ja efetuado",											// 27
	"Comprovante de credito ou debito nao permitido ou ja emitido",		// 28
	"Meio de pagamento nao permite TEF",								// 29
	"Sem comprovante nao fiscal aberto",								// 30
	"Comprovante de credito ou debito ja aberto",						// 31
	"Reimpressao nao permitida",										// 32
	"Comprovante nao fiscal ja aberto",									// 33
	"Totalizador nao fiscal nao programado",							// 34
	"Cupom nao fiscal sem item vendido",								// 35
	"Acrescimo e desconto maior que total CNF",							// 36
	"Meio de pagamento nao indicado",									// 37
	"Meio de pagamento diferente do total do recebimento",				// 38
	"Nao permitido mais de uma sangria ou suprimento",					// 39
	"Relatorio gerencial ja programado",								// 40
	"Relatorio gerencial nao programado",								// 41
	"Relatorio gerencial nao permitido",								// 42
	"MFD nao inicializada",												// 43
	"MFD ausente",														// 44
	"MFD sem numero de serie",											// 45
	"MFD ja inicializada",												// 46
	"MFD lotada",														// 47
	"Cupom nao fiscal aberto",											// 48
	"Memoria fiscal desconectada",										// 49
	"Memoria fiscal sem numero de serie da MFD",						// 50
	"Memoria fiscal lotada",											// 51
	"Data inicial invalida",											// 52
	"Data final invalida",												// 53
	"Contador de Reducao Z inicial invalido",							// 54
	"Contador de Reducao Z final invalido",								// 55
	"Erro de alocacao",													// 56
	"Dados do RTC incorretos",											// 57
	"Data anterior ao ultimo documento emitido",						// 58
	"Fora de intervencao tecnica",										// 59
	"Em intervencao tecnica",											// 60
	"Erro na memoria de trabalho",										// 61
	"Ja houve movimento no dia",										// 62
	"Bloqueio por RZ",													// 63
	"Forma de pagamento aberta",										// 64
	"Aguardando primeiro proprietario",									// 65
	"Aguardando RZ",													// 66
	"ECF ou loja igual a zero",											// 67
	"Cupom adicional nao permitido",									// 68
	"Desconto maior que o total vendido em ICMS",						// 69
	"Recebimento nao fiscal nulo nao permitido",						// 70
	"Acrescimo ou desconto maior que total nao fiscal",					// 71
	"Memoria fiscal lotada para novo cartucho",							// 72
	"Erro de gravacao na MF",											// 73
	"Erro de gravacao na MFD",											// 74
	"Dados do RTC anteriores ao ultimo DOC armazenado",					// 75
	"Memoria fiscal sem espaco para gravar leituras da MFD",			// 76
	"Memoria fiscal sem espaco para gravar versao do SB",				// 77
	"Decricao igual a DEFAULT nao permitido",							// 78
	"Extrapolado numero de repeticoes permitidas",						// 79
	"Segunda via do comprovante de credito ou debito nao permitido",	// 80
	"Parcelamento fora da sequencia",									// 81
	"Comprovante de credito ou debito aberto",							// 82
	"Texto com sequencia de ESC invalida",								// 83
	"Texto com sequencia de ESC incompleta",							// 84
	"Venda com valor nulo",												// 85
	"Estorno de valor nulo",											// 86
	"Forma de pagamento diferente do total da sangria",					// 87
	"Reducao nao permitida em intervencao tecnica",						// 88
	"Aguardando RZ para entrada em intervencao tecnica",				// 89
	"Forma de pagamento com valor nulo nao permitido",					// 90
	"Acrescimo e desconto maior que valor do item",						// 91
	"Autenticacao nao permitida",										// 92
	"Timeout na validacao",												// 93
	"Comando nao executado em impressora bilhete de passagem",			// 94
	"Comando nao executado em impressora de cupom fiscal",				// 95
	"Cupom nao fiscal fechado",											// 96
	"Parametro nao ASCII em campo ASCII",								// 97
	"Parametro nao ASCII numerico em campo ASCII numerico",				// 98
	"Tipo de transporte invalido",										// 99
	"Data e hora invalida",												// 100
	"Sem relatorio gerencial ou comprovante de credito ou debito aberto",	// 101
	"Numero do totalizador nao fiscal invalido",						// 102
	"Parametro de acrescimo ou desconto invalido",						// 103
	"Acrescimo ou desconto em sangria ou suprimento nao permitido",		// 104
	"Numero do relatorio gerencial invalido",							// 105
	"Forma de pagamento origem nao programada",							// 106
	"Forma de pagamento destino nao programada",						// 107
	"Estorno maior que forma pagamento",								// 108
	"Caracter numerico na codificacao GT nao permitido",				// 109
	"Erro na inicializacao da MF",										// 110
	"Nome do totalizador em branco nao permitido",						// 111
	"Data e hora anteriores ao ultimo DOC armazenado",					// 112
	"Parametro de acrescimo ou desconto invalido",						// 113
	"Item anterior aos trezentos ultimos",								// 114
	"Item nao existe ou ja cancelado",									// 115
	"Codigo com espacos nao permitido",									// 116
	"Descricao em caracter alfabetico nao permitido",					// 117
	"Acrescimo maior que valor do item",								// 118
	"Desconto maior que valor do item",									// 119
	"Desconto em ISS nao permitido",									// 120
	"Acrescimo em item ja efetuado",									// 121
	"Desconto em item ja efetuado",										// 122
	"Erro na memoria fiscal. Chamar credenciado"						// 123
	"Aguardando gravacao na memoria fiscal",							// 124
	"Caracter repetido na codificacao do GT",							// 125
	"Versao ja gravada na memoria fiscal",								// 126
	"Estouro de capacidade no cheque",									// 127
	"Timeout na leitura do cheque",										// 128
	"Mes invalido",														// 129
	"Coordenada invalida",												// 130
	"Sobreposicao de texto",											// 131
	"Sobreposicao de texto no valor",									// 132
	"Sobreposicao de texto no extenso",									// 133
	"Sobreposicao de texto no favorecido",								// 134
	"Sobreposicao de texto na localidade",								// 135
	"Sobreposicao de texto no opcional",								// 136
	"Sobreposicao de texto no dia",										// 137
	"Sobreposicao de texto no mes",										// 138
	"Sobreposicao de texto no ano",										// 139
	"Usando MFD de outro ECF",											// 140
	"Primeira dado diferente de ESC ou 1C",								// 141
	"Nao permitido alterar sem intervencao tecnica",					// 142
	"Dados da ultima RZ corrompidos",									// 143
	"Comando nao permitido no modo inicializacao",						// 144
	"Aguardando acerto de relogio",										// 145
	"MFD ja inicializada para outra MF",								// 146
	"Aguardando acerto do relogio ou desbloqueio pelo teclado",			// 147
	"Valor forma de pagamento maior que maximo permitido",				// 148
	"Razao social em branco",											// 149
	"Nome de fantasia em branco",										// 150
	"Endereco em branco",												// 151
	"Estorno de CDC nao permitido",										// 152
	"Dados do proprietario iguais ao atual",							// 153
	"Estorno de forma de pagamento nao permitido",						// 154
	"Descricao forma de pagamento igual ja programada",					// 155
	"Aerto de horario de verao so imediatamente apos RZ",				// 156
	"IT nao permitida MF reservada para RZ",							// 157
	"Senha CNPJ invalida",												// 158
	"Timeout na inicializacao da nova MF",								// 159
	"Nao encontrado dados na MFD",										// 160
	"Sangria ou suprimento devem ser unicos no CNF",					// 161
	"Indice de forma de pagamento nulo nao permitido",					// 162
	"UF destino invalida",												// 163
	"Tipo de transporte incompativel com UF destino",					// 164
	"Descricao do primeiro item do bilhete de passagem diferente de TARIFA",	// 165
	"Aguardando impressao de cheque ou autenticacao",					// 166
	"Nao permitido programacao CNPJ IE com espacos em branco",			// 167
	"Nao permitido programacao UF com espacos em branco",				// 168
	"Numero de impressoes da fita detalhe nesta intervencao tecnica esgotado",	// 169
	"CF ja subtotalizado",												// 170
	"Cupom nao subtotalizado",											// 171
	"Acrescimo em subtotal ja efetuado",								// 172
	"Desconto em subtotal ja efetuado",									// 173
	"Acrescimo nulo nao permitido",										// 174
	"Desconto nulo nao permitido",										// 175
	"Cancelamento de acrescimo ou desconto em subtotal nao permitido",	// 176
	"Data invalida",													// 177
	"Valor do cheque nulo nao permitido",								// 178
	"Valor do cheque invalido",											// 179
	"Cheque sem localidade nao permitido",								// 180
	"Cancelamento acrescimo em item nao permitido",						// 181
	"Cancelamento desconto em item nao permitido",						// 182
	"Numero maximo de itens atingido",									// 183
	"Numero de item nulo nao permitido",								// 184
	"Mais que duas aliquotas diferentes no bilhete de passagem nao permitido",	// 185
	"Acrescimo ou desconto em item nao permitido",						// 186
	"Cancelamento de acrescimo ou desconto em item nao permitido",		// 187
	"Cliche ja impresso",												// 188
	"Texto opcional do cheque excedeu o maximo permitido",				// 189
	"Impressao automatica no verso nao permitido neste equipamento",	// 190
	"Timeout na insercao do cheque",									// 191
	"Overflow na capacidade de texto do comprovante de credito ou debito",	// 192
	"Programacao de espacos entre cupons menor que o minimo permitido",	// 193
	"Equipamento nao possui leitor de cheque",							// 194
	"Programacao de aliquota com valor nulo nao permitido",				// 195
	"Parametro baud rate invalido",										// 196
	"Configuracao permitida somente pela porta do fisco",				// 197
	"Valor total do item excede 11 digitos",							// 198
	"Programacao da moeda com espacos em branco nao permitido",			// 199
	"Casas decimais devem ser programadas com 2 ou 3",					// 200
	"Nao permite cadastrar usuarios diferentes na mesma MFD",			// 201
	"Identificacao do consumidor nao permitida para sangria ou suprimento",	// 202
	"Casas decimais em quantidade maior do que a permitida",			// 203
	"Casas decimais do unitario maior do que a permitida",				// 204
	"Posicao reservada para ICMS",										// 205
	"Posicao reservada para ISS",										// 206
	"Todas as aliquotas com a mesma vinculacao nao permitido",			// 207
	"Data de embarque anterior a data de emissao",						// 208
	"Aliquota de ISS nao permitida sem inscricao municipal",			// 209
	"Retorno pacote cliche fora da sequencia",							// 210
	"Espaco para armazenamento do cliche esgotado",						// 211
	"Cliche grafico nao disponivel para confirmacao",					// 212
	"CRC do cliche grafico diferente do informado",						// 213
	"Intervalo invalido",												// 214
	"Usuario ja programado",											// 215
	"",																	// 216
	"Detectada abertura do equipamento",								// 217
	"Cancelamento de acrescimo/desconto nao permitido"					// 218
};

#define	BEMATECH_LOWER_BOUND		0
#define	BEMATECH_LOWER_BOUND_MSG	"Valor muito pequeno para ST3"
#define	BEMATECH_ARRAY_HOLE			216
#define BEMATECH_ARRAY_HOLE_MSG		"Codigo ST3 indefinido";
#define	BEMATECH_HIGHER_BOUND		218
#define BEMATECH_HIGHER_BOUND_MSG	"Valor muito grande para ST3"


// Funções de inicialização...

BEMAEX Bematech_FI_AlteraSimboloMoeda(const char* moeda);
BEMAEX Bematech_FI_EspacoEntreLinhas(int dots);
BEMAEX Bematech_FI_LinhasEntreCupons(int linhas);
BEMAEX Bematech_FI_NomeiaTotalizadorNaoSujeitoIcms(int idx, const char* nome);
BEMAEX Bematech_FI_ProgramaAliquota(const char* aliq, int icms_iss);
BEMAEX Bematech_FI_ProgramaArredondamento(void);
BEMAEX Bematech_FI_ProgramaHorarioVerao(void);
BEMAEX Bematech_FI_ProgramaTruncamento(void);

BEMAEX Bematech_FI_AtivaDesativaAlinhamentoEsquerdaMFD(int flag);
BEMAEX Bematech_FI_AtivaDesativaCancelamentoCupom2HorasMFD(int flag);
BEMAEX Bematech_FI_AtivaDesativaTratamentoONOFFLineMFD(int flag);
BEMAEX Beamtech_FI_AtivaDesativaVendaUmaLinhaMFD(int flag);
BEMAEX Bematech_FI_AtivaDesativaSensorPoucoPapelMFD(int flag);
BEMAEX Beamtech_FI_VerificaCancelamentoCupom2HorasMFD(char* flag);


// Funções de cupom fiscal...

BEMAEX Bematech_FI_AbreCupon(const char* cpf_cnpj);
BEMAEX Bematech_FI_AumentaDescricaoItem(const char* descr);
BEMAEX Bematech_FI_UsaUnidadeMedida(char* un);
BEMAEX Bematech_FI_VendeItem(const char* codigo, const char* descr, 
    const char* aliq, const char* tpquant, const char* quant, int decimais, 
    const char* vlrunit, const char* tpdesc, const char* desc);
BEMAEX Bematech_FI_VendeItemDepartamento(const char* codigo, const char* descr,
    const char* aliq, const char* vlrunit, const char* quant, 
    const char* acresc, const char* desc, const char* dpto, const char* un);
BEMAEX Bematech_FI_CancelaItemAnterior(void);
BEMAEX Bematech_FI_CancelaItemGenerico(const char* num_item);
BEMAEX Bematech_FI_IniciaFechamentoCupom(const char* acr_ou_dsc,
    const char* vlr_ou_pc, const char* valor);
BEMAEX Bematech_FI_EfetuaFormaPagamento(const char* fpgto, const char* valor);
BEMAEX Bematech_FI_EfetuaFormaPagamentoDescricaoForma(const char* fpgto,
    const char* valor, const char* descr);
BEMAEX Bematech_FI_EfetuaFormaPagamentoIndice(const char* idx, 
    const char* valor);
BEMAEX Bematech_FI_EfetuaFormaPagamentoIndiceDescricaoForma(const char* idx,
    const char* valor, const char* descr_opt);
BEMAEX Bematech_FI_TerminaFechamentoCupom(const char* msg);
BEMAEX Bematech_FI_FechaCupom(const char* fpgto, const char* acr_ou_dsc,
    const char* tp_acr_dsc, const char* vlr_acr_dsc, const char* vlr,
    const char* msg);
BEMAEX Bematech_FI_FechaCupomResumido(const char* fpgto, const char* msg);
BEMAEX Bematech_FI_CancelaCupom(void);
BEMAEX Bematech_FI_EstornoFormasPagamento(const char* forig, const char* fdest,
    const char* vlr);

BEMAEX Bematech_FI_AbreCupomMFD(const char* cpf_cnpj, const char* nome, 
    const char* endr);
BEMAEX Bematech_FI_AcrescimoDescontoItemMFD(const char* item, 
    const char* acr_dsc, const char* tp_acr_dsc, const char* valor);
BEMAEX Bematech_FI_AcrescimoDescontoSubtotalMFD(const char* acr_dsc, 
    const char* tp_acr_dsc, const char* valor);
BEMAEX Bematech_FI_CancelaAcrescimoDescontoItemMFD(const char* acr_dsc, 
    const char* item);
BEMAEX Bematech_FI_CancelaAcrescimoDescontoSubtotalMFD(const char* acr_dsc);
BEMAEX Bematech_FI_CancelaCupomMFD(const char* cpf_cnpj, const char* nome,
    const char* endr);
BEMAEX Bematech_FI_CupomAdicionalMFD(void);
BEMAEX Bematech_FI_EfetuaFormaPagamentoIndiceMFD(const char* idx,
    const char* valor, const char* num_parc, const char* descr);
BEMAEX Bematech_FI_EfetuaFormaPagamentoMFD(const char* fpgto, const char* valor,
    const char* num_parc, const char* descr);
BEMAEX Bematech_FI_IniciaFechamentoCupomMFD(const char* acr_dsc, 
    const char* tp_acr_dsc, const char* acr, const char* dsc);
BEMAEX Bematech_FI_ProgramaFormaPagamentoMFD(const char* desc, 
    const char* flag_tef);
BEMAEX Bematech_FI_SubTotalizaCupomMFD(void);
BEMAEX Bematech_FI_TotalizaCupomMFD(void);
BEMAEX Bematech_FI_VendeItemArredondamentoMFD(const char* codigo, 
    const char* descr, const char* aliq, const char* un, const char* frac,
    const char* vlr_un, const char* acr, const char* desc, int arred);


// Relatórios Fiscais...

BEMAEX Bematech_FI_LeituraX(void);
BEMAEX Bematech_FI_LeituraXSerial(void);
BEMAEX Bematech_FI_LeituraMemoriaFiscalData(const char* ini, const char* fim);
BEMAEX Bematech_FI_LeituraMemoriafiscalReducao(const char* ini, 
    const char* fim);
BEMAEX Bematech_FI_LeituraMemoriaFiscalSerialData(const char* ini, 
    const char* fim);
BEMAEX Bematech_FI_LeituraMemoriaFiscalSerialReducao(const char* ini, 
    const char* fim);
BEMAEX Bematech_FI_ReducaoZ(const char* data, const char* hora);

BEMAEX Bematech_FI_AbreRelatorioGerencialMFD(const char* idx);
BEMAEX Bematech_FI_LeituraMemoriaFiscalDataMFD(const char* ini, 
    const char* fim, const char* simpl_compl);
BEMAEX Bematech_FI_LeituraMemoriaFiscalReducaoMFD(const char* ini, 
    const char* fim, const char* simpl_compl);
BEMAEX Bematech_FI_LeituraMemoriaFiscalSerialDataMFD(const char* ini, 
    const char* fim, const char* simpl_compl);
BEMAEX Bematech_FI_LeituraMemoriaFiscalSerialReducaoMFD(const char* ini,
    const char* fim, const char* simpl_compl);
BEMAEX Bematech_FI_NomeiaRelatorioGerencialMFD(const char* idx, 
    const char* descr);
BEMAEX Bematech_FI_UsaRelatorioGerencialMFD(const char* texto);
BEMAEX Bematech_FI_UsaRelatorioGerencialMFDTEF(const char* text);


// Operações Não Fiscais...

BEMAEX Bematech_FI_RelatorioGerencial(const char* texto);
BEMAEX Bematech_FI_FechaRelatorioGerencial(void);
BEMAEX Bematech_FI_AbreComprovanteNaoFiscalVinculado(const char* fpgto,
    const char* valor, const char* coo);
BEMAEX Bematech_FI_UsaComprovanteNaoFiscalVinculado(const char* texto);
BEMAEX Bematech_FI_FechaComprovanteNaoFiscalVinculado(void);
BEMAEX Bematech_FI_Suprimento(const char* valor, const char* fpgto);
BEMAEX Bematech_FI_Sangria(const char* valor);
BEMAEX Bematech_FI_RecebimentoNaoFiscal(const char* idxtot, const char* valor,
    const char* fpgto);

BEMAEX Bematech_FI_AbreComprovanteNaoFiscalVinculadoMFD(const char* fpgto,
    const char* valor, const char* coo, const char* cpf_cgc, const char* nome,
    const char* endr);
BEMAEX Bematech_FI_AbreRecebimentoNaoFiscalMFD(const char* cpf_cgc,
    const char* nome, const char* endr);
BEMAEX Bematech_FI_AbreSegundaViaNaoFiscalVinculadoMFD(void);
BEMAEX Bematech_FI_AcrescimoDescontoSubtotalRecebimentoMFD(
    const char* acr_ou_dsc, const char* vlr_ou_pct, const char* valor);
BEMAEX Bematech_FI_AcrescimoItemNaoFiscalMFD(const char* item,
    const char* acr_ou_dsc, const char* vlr_ou_pct, const char* valor);
BEMAEX Bematech_FI_CancelaAcrescimoDescontoSubtotalRecebimentoMFD(
    const char* acr_ou_dsc);
BEMAEX Bematech_FI_CancelaAcrescimoNaoFiscalMFD(const char* item,
    const char* acr_ou_dsc);
BEMAEX Bematech_FI_CancelaItemNaoFiscalMFD(const char* item);
BEMAEX Bematech_FI_CancelaRecebimentoNaoFiscalMFD(const char* cpf_cnpj,
    const char* nome, const char* endr);
BEMAEX Bematech_FI_EfetuaRecebimentoNaoFiscalMFD(const char* idx_tot,
    const char* valor);
BEMAEX Bematech_FI_EstornoNaoFiscalVinculadoMFD(const char* cpf_cnpj,
    const char* nome, const char* endr);
BEMAEX Bematech_FI_EstornoNaoFiscalVinculadoPosteriorMFD(const char* fpgto,
    const char* valor, const char* coo_cupom, const char* coo_cdc,
    const char* cpf_cnpj, const char* nome, const char* endr);
BEMAEX Bematech_FI_FechaRecebimentoNaoFiscalMFD(const char* texto);
BEMAEX Bematech_FI_IniciaFechamentoRecebimentoNaoFiscalMFD(
    const char* acr_ou_dsc, const char* vlr_ou_pct, const char* vlr_acr,
    const char* vlr_dsc);
BEMAEX Bematech_FI_ReimpressaoNaoFiscalVinculadoMFD(void);
BEMAEX Bematech_FI_SegundaViaNaoFiscalVinculadoMFD(void);
BEMAEX Bematech_FI_SubTotalizaRecebimentoMFD(void);
BEMAEX Bematech_FI_TotalizaRecebimentoMFD(void);


// Gaveta de dinheiro...

BEMAEX Bematech_FI_AcionaGaveta(void);
BEMAEX Bematech_FI_VerificaEstadoGaveta(char* buffer_estado);


// Informações da impressora.

BEMAEX Bematech_FI_Acrescimos(char* buffer_valor);
BEMAEX Bematech_FI_Cancelamentos(char* buffer_valor);
BEMAEX Bematech_FI_CGC_IE(char* buffer_cgc, char* buffer_ie);
BEMAEX Bematech_FI_ClicheProprietario(char* buffer_cliche);
BEMAEX Bematech_FI_ContadoresTotalizadoresNaoFiscais(char* buffer_cnf);
BEMAEX Bematech_FI_DadosUltimaReducao(char* buffer_dados);
BEMAEX Bematech_FI_DataHoraImpressora(char* buffer_data, char* buffer_hora);
BEMAEX Bematech_FI_DataHoraReducao(char* buffer_data, char* buffer_hora);
BEMAEX Bematech_FI_DataMovimento(char* buffer_data);
BEMAEX Bematech_FI_Descontos(char* buffer_valor);
BEMAEX Bematech_FI_FlagsFiscais(int* buffer_flag);
BEMAEX Bematech_FI_FlagsVinculacaoIss(int* buffer_flag1, int* buffer_flag2);
BEMAEX Bematech_FI_GrandeTotal(char* buffer_gt);
BEMAEX Bematech_FI_MinutosImprimindo(char* buffer_min);
BEMAEX Bematech_FI_MinutosLigada(char* buffer_min);
BEMAEX Bematech_FI_ModeloImpressora(char* buffer_modelo);
BEMAEX Bematech_FI_MonitoramentoPapel(char* buffer_linhas);
BEMAEX Bematech_FI_NumeroCaixa(char* buffer_num);
BEMAEX Bematech_FI_NumeroCupom(char* buffer_num);
BEMAEX Bematech_FI_NumeroCuponsCancelados(char* buffer_num);
BEMAEX Bematech_FI_NumeroIntervencoes(char* buffer_num);
BEMAEX Bematech_FI_NumeroLoja(char* buffer_num);
BEMAEX Bematech_FI_NumeroOperacoesNaoFiscais(char* buffer_num);
BEMAEX Bematech_FI_NumeroReducoes(char* buffer_num);
BEMAEX Bematech_FI_NumeroSerie(char* buffer);
BEMAEX Bematech_FI_NumeroSerieCriptografado(char* buffer);
BEMAEX Bematech_FI_NumeroSerieDescriptografado(char* nr_cript, char* buffer);
BEMAEX Bematech_FI_NumeroSubstituicoesProprietario(char* buffer_num);
BEMAEX Bematech_FI_RetornoAliquotas(char* buffer_aliq);
BEMAEX Bematech_FI_SimboloMoeda(char* buffer_moeda);
BEMAEX Bematech_FI_SubTotal(char* buffer_valor);
BEMAEX Bematech_FI_TotalIcmsCupom(char* buffer_valor);
BEMAEX Bematech_FI_UltimoItemVendido(char* buffer_num);
BEMAEX Bematech_FI_ValorFormaPagamento(char* fpgto, char* buffer_valor);
BEMAEX Bematech_FI_ValorPagoUltimoCupom(char* buffer_valor);
BEMAEX Bematech_FI_ValorTotalizadorNaoFiscal(char* tot, char* buffer_valor);
BEMAEX Bematech_FI_VendaBruta(char* buffer_valor);
BEMAEX Bematech_FI_VerificaAliquotasIss(char* buffer_aliq);
BEMAEX Bematech_FI_VerificaEstadoImpressora(int* buffer_ack, int* buffer_st1, 
    int* buffer_st2);
BEMAEX Bematech_FI_VerificaFormasPagamento(char* buffer_fpgto);
BEMAEX Bematech_FI_VerificaIndiceAliquotasIss(char* buffer_idx);
BEMAEX Bematech_FI_VerificaModoOperacao(char* buffer_modo);
BEMAEX Bematech_FI_VerificaRecebimentoNaoFiscal(char* buffer_rcbto);
BEMAEX Bematech_FI_VerificaReducaoZAutomatica(short int* buffer_flag);
BEMAEX Bematech_FI_VerificaTipoImpressora(int* buffer_tipo);
BEMAEX Bematech_FI_VerificaTotalizadoresNaoFiscais(char* buffer_tot);
BEMAEX Bematech_FI_VerificaTotalizadoresParciais(char* buffer_tot);
BEMAEX Bematech_FI_VerificaTruncamento(char* buffer_flag);
BEMAEX Bematech_FI_VersaoFirmware(char* buffer_versao);

BEMAEX Bematech_FI_FlagsFiscais3MFD(int* buffer_flags);
BEMAEX Bematech_FI_TotalIssCupomMFD(char* buffer_valor);
BEMAEX Bematech_FI_CNPJMFD(char* buffer_cnpj);
BEMAEX Bematech_FI_ComprovantesNaoFiscaisNaoEmitidosMFD(char* buffer_num);
BEMAEX Bematech_FI_ContadorComprovantesCreditoMFD(char* buffer_num);
BEMAEX Bematech_FI_ContadorCupomFiscalMFD(char* buffer_num);
BEMAEX Bematech_FI_ContadoresTotalizadoresNaoFiscaisMFD(char* buffer_num);
BEMAEX Bematech_FI_ContadorFitaDetalheMFD(char* buffer_num);
BEMAEX Bematech_FI_ContadorOperacoesNaoFiscaisCanceladasMFD(char* buffer_num);
BEMAEX Bematech_FI_ContadorRelatoriosGerenciaisMFD(char* buffer_num);
BEMAEX Bematech_FI_DadosUltimaReducaoMFD(char* buffer_dados);
BEMAEX Bematech_FI_DataHoraUltimoDocumentoMFD(char* buffer_datahora);
BEMAEX Bematech_FI_DataMovimentoUltimaReducaoMFD(char* buffer_data);
BEMAEX Bematech_FI_GrandeTotalUltimaReducaoMFD(char* buffer_gt);
BEMAEX Bematech_FI_InicioFimCOOsMFD(char* buffer_ini, char* buffer_fim);
BEMAEX Bematech_FI_InicioFimGTsMFD(char* buffer_ini, char* buffer_fim);
BEMAEX Bematech_FI_InscricaoEstadualMFD(char* buffer_ie);
BEMAEX Bematech_FI_InscricaoMunicipalMFD(char* buffer_im);
BEMAEX Bematech_FI_MarcaModeloTipoImpressoraMFD(char* buffer_marca, 
    char* buffer_modelo, char* buffer_tipo);
BEMAEX Bematech_FI_MinutosEmitindoDocumentosFiscaisMFD(char* buffer_min);
BEMAEX Bematech_FI_NumeroSerieMFD(char* buffer_numserie);
BEMAEX Bematech_FI_PercentualLivreMFD(char* buffer_size);
BEMAEX Bematech_FI_ReducoesRestantesMFD(char* buffer_num);
BEMAEX Bematech_FI_StatusEstendidoMFD(int* buffer_status);
BEMAEX Bematech_FI_SubTotalComprovanteNaoFiscalMFD(char* buffer_valor);
BEMAEX Bematech_FI_TempoOperacionalMFD(char* buffer_tempo);
BEMAEX Bematech_FI_TempoRestanteComprovanteMFD(char* buffer_tempo);
BEMAEX Bematech_FI_UFProprietarioMFD(char* buffer_uf);
BEMAEX Bematech_FI_ValorFormaPagamentoMFD(char* fpgto, char* buffer_valor);
BEMAEX Bematech_FI_ValorTotalizadorNaoFiscalMFD(char* tot, char* buffer_valor);
BEMAEX Bematech_FI_VerificaEstadoImpressoraMFD(int* buffer_ack, int* buffer_st1,
    int* buffer_st2, int* buffer_st3);
BEMAEX Bematech_FI_VerificaFlagCorteMFD(int* buffer_flag);
BEMAEX Bematech_FI_VerificaFormasPagamentoMFD(char* buffer_fpgto);
BEMAEX Bematech_FI_VerificaRecebimentoNaoFiscalMFD(char* buffer_rcbto);
BEMAEX Bematech_FI_VerificaRelatorioGerencialMFD(char* buffer_rel_ger);
BEMAEX Bematech_FI_VerificaSensorPoucoPapelMFD(char* buffer_flag);
BEMAEX Bematech_FI_VerificaTotalizadoresNaoFiscaisMFD(char* buffer_tot);
BEMAEX Bematech_FI_VerificaTotalizadoresParciaisMFD(char* buffer_tot);
BEMAEX Bematech_FI_VersaoFirmwareMFD(char* buffer_versao);


// TEF...

BEMAEX Bematech_FI_IniciaModoTEF(void);
BEMAEX Bematech_FI_UsaComprovanteNaoFiscalVinculadoTEF(const char* msg);
BEMAEX Bematech_FI_RelatorioGerencialTEF(const char* texto);
BEMAEX Bematech_FI_FinalizaModoTEF(void);


// Impressão de código de barras

BEMAEX Bematech_FI_CodigoBarrasCODABARMFD(const char* codigo);
BEMAEX Bematech_FI_CodigoBarrasCODE123MFD(const char* codigo);
BEMAEX Bematech_FI_CodigoBarrasCODE39MFD(const char* codigo);
BEMAEX Bematech_FI_CodigoBarrasCODE93MFD(const char* codigo);
BEMAEX Bematech_FI_CodigoBarrasEAN13MFD(const char* codigo);
BEMAEX Bematech_FI_CodigoBarrasEAN8MFD(const char* codigo);
BEMAEX Bematech_FI_CodigoBarrasISBNMFD(const char* codigo);
BEMAEX Bematech_FI_CodigoBarrasITFMFD(const char* codigo);
BEMAEX Bematech_FI_CodigoBarrasMSIMFD(const char* codigo);
BEMAEX Bematech_FI_CodigoBarrasPDF417MFD(int correcao, int altura, int largura,
    int linhas, const char* codigo);
BEMAEX Bematech_FI_CodigoBarrasPLESSEYMFD(const char* codigo);
BEMAEX Bematech_FI_CodigoBarrasUPCAMFD(const char* codigo);
BEMAEX Bematech_FI_CodigoBarrasUPCEMFD(const char* codigo);
BEMAEX Bematech_FI_ConfiguraCodigoBarrasMFD(int altura, int largura, 
    int posicao, int fonte, int margem);


// Outras funções...

BEMAEX Bematech_FI_AberturaDoDia(const char* valor, const char* fpgto);
BEMAEX Bematech_FI_AbrePortaSerial(void);
BEMAEX Bematech_FI_DadosSintegra(const char* data_ini, const char* data_fim);
BEMAEX Bematech_FI_FechamentoDoDia(void);
BEMAEX Bematech_FI_FechaPortaSerial(void);
BEMAEX Bematech_FI_FechaRelatorioXouZ(void);
BEMAEX Bematech_FI_ImpressaoCarne(const char* titulo, const char* vlr_parcs, 
    const char* data_parcs, const char* qtde_parcs, const char* texto,
    const char* cliente, const char* rg_cpf, const char* cupom, int vias,
    int assina);
BEMAEX Bematech_FI_ImprimeConfiguracoesImpressora(void);
BEMAEX Bematech_FI_BematechInfoBalanca(const char* porta, int model, char* peso,
    char* preo_kilo, char* total);
BEMAEX Bematech_FI_BaudRateBalanca(int bauds);
BEMAEX Bematech_FI_MapaResumo(void);
BEMAEX Bematech_FI_RegistrosTipo60(void);
BEMAEX Bematech_FI_RelatorioTipo60Analitico(void);
BEMAEX Bematech_FI_RelatorioTipo60Mestre(void);
BEMAEX Bematech_FI_RetornoImpressora(int* buffer_ack, int* buffer_st1,
    int* buffer_st2);
BEMAEX Bematech_FI_VerificaImpressoraLigada(void);

BEMAEX Bematech_FI_AcionaGuilhotinaMFD(int tipo_corte);
BEMAEX Bematech_FI_AtivaDesativaCorteProximoMFD(int flag);
BEMAEX Bematech_FI_AvancaPapelAcionaGuilhotinaMFD(int linhas, int tipo_corte);
BEMAEX Bematech_FI_ConfiguraCorteGuilhotinaMFD(int tempo_ms);
BEMAEX Bematech_FI_DadosSintegraMFD(const char* data_ini, const char* data_fim);
BEMAEX Bematech_FI_DownloadMF(const char* path_mf);
BEMAEX Bematech_FI_DownloadMFD(const char* path_mfd, const char* tipo, 
    const char* ini, const char* fim, const char* user);
BEMAEX Bematech_FI_DownloadSB(const char* path_sb);
BEMAEX Bematech_FI_FormatoDadosMFD(const char* path_orig, const char* path_dest,
    const char* fmt, const char* download, const char* ini, const char* fim,
    const char* user);
BEMAEX Bematech_FI_GeraRelatorioSintegraMFD(int tipo);
BEMAEX Bematech_FI_HabilitaDesabilitaRetornoEstendidoMFD(const char* flag);
BEMAEX Bematech_FI_ImprimeClicheMFD(void);
BEMAEX Bematech_FI_MapaResumoMFD(void);
BEMAEX Bematech_FI_ProgramaIdAplicativoMFD(const char* id);
BEMAEX Bematech_FI_RelatorioSintegraMFD(int tipo, const char* path, 
    const char* mes, const char* ano, const char* razao, const char* endr,
    const char* num, const char* complem, const char* bairro, 
    const char* cidade, const char* cep, const char* fone, const char* fax, 
    const char* contato);
BEMAEX Bematech_FI_RelatorioTipo60AnaliticoMFD(void);
BEMAEX Bematech_FI_RetornoImpressoraMFD(int* buffer_ac, int* buffer_st1,
    int* buffer_st2, int* buffer_st3);


// Funções para PAF-ECF

BEMAEX Bematech_FI_AbreDocumentoAuxiliarVenda(const char* idx, 
    const char* titulo, const char* num, const char* emitente, 
    const char* doc_emit, const char* destinat, const char* doc_dest);
BEMAEX Bematech_FI_UsaDocumentoAuxiliarVenda(const char* dsc_item, 
    const char* vlr_un, const char* vlr_total);
BEMAEX Bematech_FI_FechaDocumentoAuxiliarVenda(const char* total);
BEMAEX Bematech_FI_TerminaFechamentoCupomPreVenda(const char* md5,
    const char* num, const char* msg);
BEMAEX Bematech_FI_LeituraMemoriaFiscalSerialDataPAFECF(
    const char* data_ini, const char* data_fim, const char* tipo_leitura,
    const char* key_pub, const char* key_priv);
BEMAEX Bematech_FI_LeituraMemoriaFiscalSerialReducaoPAFECF(const char* red_ini,
    const char* red_fim, const char* tipo_leitura, const char* key_pub,
    const char* key_priv);
BEMAEX Bematech_FI_EspelhoMFD(const char* path, const char* data_ini,
    const char* data_fim, const char* tipo_download, const char* user,
    const char* key_pub, const char* key_priv);
BEMAEX Bematech_FI_ArquivoMFD(const char* path, const char* ini,
    const char* fim, const char* tipo_download, const char* user, int geracao,
    const char* key_pub, const char* key_priv, int unico);
BEMAEX Bematech_FI_IdentificacaoPAFECF(const char* idx, const char* laudo,
    const char* cnpj, const char* razao, const char* endr, const char* fone,
    const char* contato, const char* num_coml, const char* path,
    const char* md5, const char* arqs, const char* md5_arqs, const char* serie);
BEMAEX Bematech_FI_GrandeTotalCriptografado(char* buffer_gt);
BEMAEX Bematech_FI_GrandeTotalDescriptografado(char* gt, char* buffer_gt);
BEMAEX Bematech_FI_AbreRelatorioMeiosPagamento(const char* idx);
BEMAEX Bematech_FI_UsaRelatorioMeiosPagamento(const char* descr,
    const char* tp_doc, const char* vlr, const char* data);
BEMAEX Bematech_FI_FechaRelatorioMeiosPagamento(void);
BEMAEX Bematech_FI_NomeiaRelatorioMeiosDePagamento(void);
BEMAEX Bematech_FI_NomeiaRelatorioDocumentoAuxiliarDeVenda(void);
BEMAEX Bematech_FI_NomeiaRelatorioDAVEmitidos(void);
BEMAEX Bematech_FI_NomeiaRelatorioIdentificacaoPAFECF(void);
BEMAEX Bematech_FI_NomeiaRelatoriosPAFECF(void);
    

#ifndef WIN32

#ifdef __cplusplus
}

#endif

#endif

#endif
