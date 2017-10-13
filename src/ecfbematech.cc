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

#include "ecfbematech.h"


/**
 \brief Método construtor.
 \details Inicializa propriedades do objeto e define alguns parâmetros da
 DLL/ECF:
 - habilita o retorno estendido do equipamento (ST3)
 - habilita o sensor de pouco papel
 
 */
ecf_bematech::ecf_bematech(const std::string& cfg_file) :
	m_retmsg(""), m_stsmsg(""), m_marca(""), m_modelo(""), m_tipo(""),
	ecf(cfg_file)
{
	debug_log("ecf_bematech::ecf_bematech()");

	info_log("Inicializacao do driver ecf_bematech");

	m_size_fpgto = BEMATECH_SIZE_FPGTO;
	m_size_totcnf = BEMATECH_SIZE_TOTCNF;
	m_size_relger = BEMATECH_SIZE_RELGER;

	m_fpgto.clear();
	m_vinc_ccd.clear();
	m_tot_cnf.clear();
	m_relger.clear();
	m_aliqs.clear();

	m_dont_pay = false;
	
	m_fake = false;
	m_ignorar_pouco_papel = true;
	
	m_pgto_seq = IDX_NOT_FOUND;

	char* flag = getenv(SPOOLER_FAKE);
	if (flag)   {
		m_fake = (atoi(flag) > 0);
	}

	if (m_fake)
		info_log("Modo FAKE habilitado.");

	if (ignorar_pouco_papel())
		info_log("Status de pouco papel sera ignorado durante a inicializacao.");
	
	try {
		debug_log("Habilitacao de retorno estendido");
		check(Bematech_FI_HabilitaDesabilitaRetornoEstendidoMFD(
		    std::string("1").c_str()));
		
		try {
			load_specific();
		}
		catch (...) {
			throw;
		}
		
		if (atoi(m_specific.get_value_by_key(
				BEMATECH_SENSOR_POUCO_PAPEL).c_str()) > 0) {
			debug_log("Ativacao do sensor de pouco papel");
			check(Bematech_FI_AtivaDesativaSensorPoucoPapelMFD(1));
		}
		else	{
			debug_log("Desativacao do sensor de pouco papel");
			check(Bematech_FI_AtivaDesativaSensorPoucoPapelMFD(0));
		}

		if (ignorar_pouco_papel())
			info_log("Sensor de pouco papel sera ignorado.");
	}
	catch (...) {
		// Já está sendo gerado log. Vamos evitar
		// problemas na instanciação, em virtude de parâmetros
		// que só podem ser definidos com o equipamento
		// "aguardando início de movimento"
		// Assim, nenhuma exceção será lançada neste ponto.
	}
}


/**
 \brief Método construtor.
 \param size_fpgto Tamanho da descrição dos meios de pagamento.
 \param size_totcnf Tamanho da descrição dos totalizadores não fiscais.
 \param size_relger Tamanho da descrição dos relatórios gerenciais.
 \details Inicializa propriedades do objeto e define alguns parâmetros da
 DLL/ECF:
 - habilita o retorno estendido do equipamento (ST3)
 - habilita o sensor de pouco papel
 
 */
ecf_bematech::ecf_bematech(short size_fpgto, short size_totcnf, 
    short size_relger, const std::string& cfg_file) : ecf(cfg_file)
{
	debug_log("ecf_bematech::ecf_bematech(...)");

	info_log("Inicializacao do driver ecf_bematech");
	
	m_size_fpgto = size_fpgto;
	m_size_totcnf = size_totcnf;
	m_size_relger = size_relger;

	m_fpgto.clear();
	m_vinc_ccd.clear();
	m_tot_cnf.clear();
	m_relger.clear();
	m_aliqs.clear();
	
	m_dont_pay = false;
	
	m_fake = false;
	m_ignorar_pouco_papel = true;
	
	m_pgto_seq = IDX_NOT_FOUND;

	char* flag = getenv(SPOOLER_FAKE);
	if (flag)   {
		m_fake = (atoi(flag) > 0);
	}

	if (m_fake)
		info_log("Modo FAKE habilitado.");
	
	if (ignorar_pouco_papel())
		info_log("Status de pouco papel sera ignorado durante a inicializacao.");
	
	try {
		debug_log("Habilitacao de retorno estendido");
		check(Bematech_FI_HabilitaDesabilitaRetornoEstendidoMFD(
		    std::string("1").c_str()));

		try {
			load_specific();
		}
		catch (...) {
			throw;
		}
		
		if (atoi(m_specific.get_value_by_key(
				BEMATECH_SENSOR_POUCO_PAPEL).c_str()) > 0) {
			debug_log("Ativacao do sensor de pouco papel");
			check(Bematech_FI_AtivaDesativaSensorPoucoPapelMFD(1));
		}
		else	{
			debug_log("Desativacao do sensor de pouco papel");
			check(Bematech_FI_AtivaDesativaSensorPoucoPapelMFD(0));
		}
		
		if (ignorar_pouco_papel())
			info_log("Sensor de pouco papel sera ignorado.");
	}
	catch (...) {
		// Já está sendo gerado log. Vamos evitar
		// problemas na instanciação, em virtude de parâmetros
		// que só podem ser definidos com o equipamento
		// "aguardando início de movimento"
		// Assim, nenhuma exceção será lançada neste ponto.
	}
}


/**
 \brief Verifica o retorno do último comando submetido ao ECF.
 \param retcode Código de retorno da última invocação feita à DLL/SO.
 \throws Exceção em qualquer caso de erro, contendo a mensagem correspondente
 ao problema identificado (tanto na DLL quando no ECF).
 */
void ecf_bematech::check(int retcode)
{
	/*
	 * Comentei o log de depuração abaixo, pois estava poluindo DEMAIS os
	 * registros de execução.
	 *
	debug_log(std::string("ecf_bematech::check(") + into_string(retcode) +
	    std::string(")"));
	 *
	 */
	
	int	 buffer_ack,
			buffer_st1,
			buffer_st2,
			buffer_st3;
	int		ret;
	char	buf_log[64];

	memset(buf_log, 0, sizeof(buf_log));
	
	// Caso a DLL tenha processado o comando, é necessário colher
	// o retorno do equipamento (ECF).
	if (retcode == BEMATECH_OK)  {
		//
		// Log DEMAIS. Muitas mensagens repetidas no registro de execução.
		//
		//debug_log("ecf_bematech::check() Obter retorno do ECF");
		//
		ret = Bematech_FI_RetornoImpressoraMFD(&buffer_ack, &buffer_st1,
		    &buffer_st2, &buffer_st3);

		// A avaliação dos registros colhidos do ECF, somente devem ser
		// avaliados, caso não tenha ocorrido erro na sua coleta.
		if (ret != BEMATECH_NOT_OK_STATUS)  {

			sprintf(buf_log,
			    "Retorno ACK: %02x, ST1: %02x, ST2: %02x, ST3: %02x",
			    (buffer_ack & 0x00FF), (buffer_st1 & 0x00FF),
			    (buffer_st2 & 0x00FF), (buffer_st3 & 0x00FF));
			debug_log(buf_log);
			
			// Instância de objeto mantém cópia do registro de estado da
			// última operação executada.
			m_status.ack      = static_cast<char>(buffer_ack & 0x00FF);
			m_status.st1.byte = static_cast<char>(buffer_st1 & 0x00FF);
			m_status.st2.byte = static_cast<char>(buffer_st2 & 0x00FF);
			m_status.st3      = static_cast<char>(buffer_st3 & 0x00FF);

			// Seta indicador de estado
			set_status(m_status);

			// Solicitada avaliação de ST1, ST2 e ST3...
			if (!eval_status(m_status)) {
				throw ecf_exp(m_stsmsg);
			}
		}
		else
			retcode = ret;
	}
	
	// Erros ocorridos na chamada. Não envolvem comunicação com o
	// equipamento (ECF).
	// eval_retcode retornará 'true', caso retcode seja igual a BEMATECH_OK
	if (!eval_retcode(retcode)) {
		throw ecf_exp(m_retmsg);
	}
}


/**
 \brief Avalia o registro de retorno do ECF.
 \details Alimenta propriedade do objeto, contendo mensagem descritiva de erro.
 Por se tratar de avaliação de registro "estendido" de erros, apenas o conteúdo 
 de ST3 é avaliado.
 \param sts Estrutura de dados (exclusiva para ECFs equipados com MFD),
 contendo ACK + ST1 + ST2 + ST3.
 \returns Indicador lógico, significando sucesso ou falha na operação.
 */
bool ecf_bematech::eval_status(bematech_status_mfd sts)
{
	/*
	 * Comentei o log de depuração abaixo, pois estava poluindo DEMAIS
	 * os registros de execução.
	 *
	debug_log("ecf_bematech::eval_status(...)");
	 *
	 */

	bool	flg1 = (static_cast<int>(sts.st1.byte) == 0);
	bool	flg2 = (static_cast<int>(sts.st2.byte) == 0);
	bool	flag = (static_cast<int>(sts.st3) == 0);

	// Avaliação preferencial do erro estendido (ST3)
	if (!flag)   {
		if (sts.st3 < BEMATECH_LOWER_BOUND) {
			m_stsmsg = BEMATECH_LOWER_BOUND_MSG;
		}
		else if (sts.st3 == BEMATECH_ARRAY_HOLE)	{
			m_stsmsg = BEMATECH_ARRAY_HOLE_MSG;
		}
		else if (sts.st3 > BEMATECH_HIGHER_BOUND)   {
			m_stsmsg = BEMATECH_HIGHER_BOUND_MSG;
		}
		else	{
			m_stsmsg = bematech_st3_msgs[static_cast<int>(sts.st3)];
		}
	}
	else	{
		// Avaliação de ST1
		if (!flg1)  {
			if (sts.st1.bits.num_params_invalido)   {				// bit 0
				m_stsmsg = BEMATECH_ST1_NUM_PAR_INV;
			}
			else if (sts.st1.bits.cupom_fiscal_aberto)  {			// bit 1
				m_stsmsg = BEMATECH_ST1_CUPOM_ABERTO;
			}
			else if (sts.st1.bits.comando_inexistente)  {			// bit 2
				m_stsmsg = BEMATECH_ST1_CMD_NAO_EXIST;
			}
			else if (sts.st1.bits.inicio_dif_esc)   {				// bit 3
				m_stsmsg = BEMATECH_ST1_CMD_NAO_ESC;
			}
			else if (sts.st1.bits.erro_impressora)  {				// bit 4
				m_stsmsg = BEMATECH_ST1_ERRO_IMPR;
			}
			else if (sts.st1.bits.erro_rtc) {						// bit 5
				m_stsmsg = BEMATECH_ST1_ERRO_RTC;
			}
			else if (sts.st1.bits.pouco_papel)  {					// bit 6
				if (!ignorar_pouco_papel()) {
					m_stsmsg = BEMATECH_ST1_POUCO_PAPEL;
				}
				else	{
					sts.st1.bits.pouco_papel = 0;
					flg1 = (static_cast<int>(sts.st1.byte) == 0);
					warn_log(std::string("ecf_bematech::eval_status():") +
					         std::string("Ignorado indicador de pouco papel"));
				}
			}
			else if (sts.st1.bits.fim_de_papel) {					// bit 7
				m_stsmsg = BEMATECH_ST1_FIM_PAPEL;
			}
			if (!flg1)
				error_log(std::string("ecf_bematech::eval_status() ST1: ") +
				          dquote(m_stsmsg.c_str()));
		}

		// Avaliação de ST2
		if (!flg2)  {
			if (sts.st2.bits.comando_nao_executado) {				// bit 0
				m_stsmsg = BEMATECH_ST2_CMD_NAO_EXEC;
			}
			else if (sts.st2.bits.proprietario_nao_programado)  {   // bit 1
				m_stsmsg = BEMATECH_ST2_PROP_NAO_PRG;
			}
			else if (sts.st2.bits.canc_nao_permitido)   {			// bit 2
				m_stsmsg = BEMATECH_ST2_PROP_NAO_PRG;
			}
			else if (sts.st2.bits.capacidade_aliq_esgotada) {		// bit 3
				m_stsmsg = BEMATECH_ST2_CP_ALQ_LOTADA;
			}
			else if (sts.st2.bits.aliq_nao_programada)  {			// bit 4
				m_stsmsg = BEMATECH_ST2_ALIQ_NAO_PRG;
			}
			else if (sts.st2.bits.erro_cmos)	{					// bit 5
				m_stsmsg = BEMATECH_ST2_ERRO_CMOS;
			}
			else if (sts.st2.bits.mem_fiscal_lotada)	{			// bit 6
				m_stsmsg = BEMATECH_ST2_MEM_LOTADA;
			}
			else if (sts.st2.bits.tipo_param_invalido)  {			// bit 7
				m_stsmsg = BEMATECH_ST2_TP_PARAM_INV;
			}
			error_log(std::string("ecf_bematech::eval_status() ST2: ") +
			          dquote(m_stsmsg.c_str()));
		}
	}

	return (flg1 && flg2 && flag);
}


/**
 \brief Avalia código de invocação da DLL
 \details Alimenta propriedade do objeto, contendo mensagem descritiva de erro.
 \param retcode Código retornado pela rotina da DLL.
 \returns Indicador lógico de sucesso ou falha na operação.
 */
bool ecf_bematech::eval_retcode(int retcode)
{
	/*
	 * Comentei o log de depuração abaixo, pois estava poluindo DEMAIS os
	 * registros de execução.
	 *
	debug_log(std::string("ecf_bematech::eval_retcode(") + 
	    into_string(retcode) + std::string(")"));
	 *
	 */
	
	bool		status = false;
	char		code[6];

	switch (retcode)
	{
		case BEMATECH_COMM_ERR			:
			m_retmsg = "Erro de comunicacao.";
			break;
		case BEMATECH_OK				:
			status = true;
			m_retmsg = "Ok.";
			break;
		case BEMATECH_EXEC_FAILURE		:
			m_retmsg = "Erro de execucao da funcao.";
			break;
		case BEMATECH_INVALID_PARAM		:
			m_retmsg = "Parametro invalido na funcao.";
			break;
		case BEMATECH_MISSING_ALIQ		:
			m_retmsg = "Aliquota nao programada.";
			break;
		case BEMATECH_MISSING_INI_FILE  :
			m_retmsg = "Arquivo BemaFI32.ini nao encontrado.";
			break;
		case BEMATECH_SERIAL_OPEN_ERR   :
			m_retmsg = "Erro ao abrir porta de comunicacao.";
			break;
		case BEMATECH_UNPLUGGED_ECF		:
			m_retmsg = "Impressora desligada ou cabo de comunicacao descontado.";
			break;
		case BEMATECH_TXT_WRITE_ERR		:
			m_retmsg = "Erro ao criar/gravar STATUS.TXT ou RETORNO.TXT.";
			break;
		case BEMATECH_MISSING_FPGTO		:
			m_retmsg = "Forma de pagamento nao programada.";
			break;
		case BEMATECH_NOT_OK_STATUS		:
			m_retmsg = "Status da impressora diferente de 6,0,0 (ACK, ST1 e ST2)";
			break;
		default							:
			memset(code, 0, sizeof(code));
			sprintf(code, "%5d", retcode);
			m_retmsg = "Codigo de retorno nao identificado! (#";
			m_retmsg += code;
			m_retmsg += ")";
	}

	return status;
}


/**
 \brief Obtém o número de loja, gravado na memória fiscal do ECF.
 \returns Valor numérico.
 */
short ecf_bematech::get_nr_loja(void)
{
	debug_log("ecf_bematech::get_nr_loja()");
	
	char	buff[5];

	memset(buff, 0 , sizeof(buff));
	
	try {
		xdebug_log("Bematech_FI_NumeroLoja(...)");
		check(Bematech_FI_NumeroLoja(buff));
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::get_nr_loja() = ") + dquote(buff));

	return atoi(buff);
}


/**
 \brief Obtém o número de série do equipamento
 \returns String de 20 posições, contendo o numéro de série.
 */
std::string ecf_bematech::get_nr_serie(void)
{
	debug_log("ecf_bematech::get_nr_serie()");
	
	char	buff[21];

	memset(buff, 0, sizeof(buff));

	try {
		xdebug_log("Bematech_FI_NumeroSerieMFD(...)");
		check(Bematech_FI_NumeroSerieMFD(buff));
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::get_nr_serie() = ") + dquote(buff));

	return buff;
}


/**
 \brief Obtém a marca do equipamento.
 \returns String de 15 posições.
 */
std::string ecf_bematech::get_marca(void)
{
	debug_log("ecf_bematech::get_marca()");
	
	char	marca[32];
	char	modelo[32];
	char	tipo[32];

	memset(marca, 0, sizeof(marca));
	memset(modelo, 0, sizeof(modelo));
	memset(tipo, 0, sizeof(tipo));

	if (m_marca.empty())	{
		xdebug_log("Bematech_FI_MarcaModeloTipoImpressoraMFD(...)");
		
		try {
			check(Bematech_FI_MarcaModeloTipoImpressoraMFD(marca, modelo, tipo));
			xdebug_log(std::string("Marca: ") + dquote(marca) +
			    std::string(" - Modelo: ") + dquote(modelo) +
			    std::string(" - Tipo: ") + dquote(tipo));
			m_marca = marca;
			m_modelo = modelo;
			m_tipo = tipo;
		}
		catch (...) {
			throw;
		}
	}

	xdebug_log(std::string("ecf_bematech::get_marca() = ") + 
	    dquote(m_marca.c_str()));

	return m_marca;
}


/**
 \brief Obtém o modelo do equipamento
 \returns String de 10 posições.
 */
std::string ecf_bematech::get_modelo(void)
{
	debug_log("ecf_bematech::get_modelo()");
	
	char	modelo[32];

	memset(modelo, 0, sizeof(modelo));
	
	if (m_modelo.empty())   {
		try {
			xdebug_log("Bematech_FI_ModeloImpressora(...)");
			check(Bematech_FI_ModeloImpressora(modelo));
			m_modelo = modelo;
		}
		catch (...) {
			throw;
		}
	}

	xdebug_log(std::string("ecf_bematech::get_modelo() = ") +
	    dquote(m_modelo.c_str()));

	return m_modelo;
}


/**
 \brief Obtém o número sequencial do proprietário cadastrado no equipamento.
 \returns Valor inteiro.
 */
short ecf_bematech::get_nr_usuario(void)
{
	debug_log("ecf_bematech::get_nr_usuario()");
	
	char	num[5];

	memset(num, 0, sizeof(num));
	
	try {
		xdebug_log("Bematech_FI_NumeroSubstituicoesProprietario(...)");
		check(Bematech_FI_NumeroSubstituicoesProprietario(num));
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::get_nr_usuario() = ") + dquote(num));

	return atoi(num);
}


/**
 \brief Obtém o número de série do equipamento, criptografado.
 \returns String de 20 posições.
 */
std::string ecf_bematech::get_nr_serie_cript(void)
{
	debug_log("ecf_bematech:get_nr_serie_cript()");
	
	char	serie[21];

	memset(serie, 0, sizeof(serie));
	
	try {
		xdebug_log("Bematech_FI_NumeroSerieCriptografado(...)");
		check(Bematech_FI_NumeroSerieCriptografado(serie));
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::get_nr_serie_cript() = ") + 
	    dquote(serie));

	return serie;
}


/**
 \brief Obtém o número do caixa correspondente ao equipamento.
 \returns Valor numérico.
 */
short ecf_bematech::get_nr_caixa(void)
{
	debug_log("ecf_bematech::get_nr_caixa()");
	
	char	caixa[5];

	memset(caixa, 0, sizeof(caixa));
	
	try {
		xdebug_log("Bematech_FI_NumeroCaixa(...)");
		check(Bematech_FI_NumeroCaixa(caixa));
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::get_nr_caixa() = ") + dquote(caixa));

	return atoi(caixa);
}


/**
 \brief Obtém a versão do firware do equipamento.
 \returns String de 6 posições.
 */
std::string ecf_bematech::get_firmware(void)
{
	debug_log("ecf_bematech::get_firmware()");
	
	char	versao[7];

	memset(versao, 0, sizeof(versao));

	try {
		xdebug_log("Bematech_FI_VersaoFirmwareMFD(...)");
		check(Bematech_FI_VersaoFirmwareMFD(versao));
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::get_firmware() = ") + dquote(versao));

	return versao;
}


/**
 \brief Obtém data e hora atuais do equipamento.
 \param data Referência a vetor de 'char' para receber a data.
 \param hora Referência a vetor de 'char' para recebera hora.
 */
void ecf_bematech::get_data_hora_atual(char* data, char* hora)
{
	debug_log("ecf_bematech::get_data_hora_atual()");
	
	try {
		xdebug_log("Bematech_FI_DataHoraImpressora(...)");
		check(Bematech_FI_DataHoraImpressora(data, hora));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém a data (atual) do equipamento.
 \returns String de 6 posições, contendo a data em formado "ddmmaa".
 */
std::string ecf_bematech::get_data_atual(void)
{
	debug_log("ecf_bematech::get_data_atual()");
	
	char		data[7],
				hora[7];
	char		ano[3],
				mes[3],
				dia[3];
	std::string ret;

	try {
		get_data_hora_atual(data, hora);

		memset(ano, 0, sizeof(ano));
		strncpy(ano, data+4, 2);
		ret = "20";
		ret += ano;

		memset(mes, 0, sizeof(mes));
		strncpy(mes, data+2, 2);
		ret += mes;

		memset(dia, 0, sizeof(dia));
		strncpy(dia, data+0, 2);
		ret += dia;
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::get_data_atual() = ") +
	    dquote(ret.c_str()));

	return ret.c_str();
}


/**
 \brief Obtém a hora (atual) do equipamento.
 \returns String de 6 posições, contendo a hora em formato "hhmmss".
 */
std::string ecf_bematech::get_hora_atual(void)
{
	debug_log("ecf_bematech::get_hora_atual()");
	
	char	data[7],
			hora[7];

	try {
		get_data_hora_atual(data, hora);
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::get_hora_atual() = ") + dquote(hora));

	return hora;
}


/**
 \brief Obtém a data de movimento do equipamento.
 \returns String de 6 posições, contendo a data no formato "ddmmaa".
 */
std::string ecf_bematech::get_data_mvto(void)
{
	debug_log("ecf_bematech::get_data_mvto()");
	
	char		data[7],
				ano[3],
				mes[3],
				dia[3];
	std::string ret;

	try {
		xdebug_log("Bematech_FI_DataMovimento(...)");
		check(Bematech_FI_DataMovimento(data));

		memset(ano, 0, sizeof(ano));
		strncpy(ano, data+4, 2);
		ret = "20";
		ret += ano;

		memset(mes, 0, sizeof(mes));
		strncpy(mes, data+2, 2);
		ret += mes;

		memset(dia, 0, sizeof(dia));
		strncpy(dia, data+0, 2);
		ret += dia;
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::get_data_mvto() = ") + 
	    dquote(ret.c_str()));

	return ret.c_str();
}


/**
 \brief Obtém data e hora da última Redução Z.
 \note Alimenta propriedades da instância para uso futuro.
 */
void ecf_bematech::get_data_hora_ult_red(void)
{
	char	data[7],
		hora[7];

	debug_log("ecf_bematech::get_data_hora_ult_red()");

	memset(data, 0, sizeof(data));
	memset(hora, 0, sizeof(hora));

	try	{
		get_data_hora_ult_red(data, hora);

		// Obtenção da hora
		std::string tmp_hora = hora;
		trim(tmp_hora);

		if (m_hr_ult_red.empty() && (!tmp_hora.empty()))
			m_hr_ult_red = tmp_hora;

		// Obtenção e formatação da data
		std::string tmp_data = data;
		trim(tmp_data);

		if (m_dt_ult_red.empty() && (!tmp_data.empty()))	{
			char ano[3], mes[3], dia[3];

			memset(ano, 0, sizeof(ano));
			memset(mes, 0, sizeof(mes));
			memset(dia, 0, sizeof(dia));

			strncpy(ano, data + 4, 2);
			m_dt_ult_red = "20";
			m_dt_ult_red += ano;

			strncpy(mes, data + 2, 2);
			m_dt_ult_red += mes;

			strncpy(dia, data + 0, 2);
			m_dt_ult_red += dia;
		}

		xdebug_log(std::string(
			"ecf_bematech::get_data_hora_ult_red() = ") +
			dquote(m_dt_ult_red.c_str()) + std::string(" | ") +
			dquote(m_hr_ult_red.c_str()));
	}
	catch (...)	{
		throw;
	}
}


/**
 \brief Obtém data e hora da última Redução Z.
 \param data Referência a vetor de 'char' onde será armazenada a data.
 \param hora Referẽncia a vetor de 'char' onde será armazenada a hora.
 */
void ecf_bematech::get_data_hora_ult_red(char* data, char* hora)
{
	debug_log("ecf_bematech::get_data_hora_ult_red(...)");

	if (m_hr_ult_red.empty() || m_dt_ult_red.empty())	{
		try {
			xdebug_log("Bematech_FI_DataHoraReducao(...)");
			check(Bematech_FI_DataHoraReducao(data, hora));
		}
		catch (...) {
			throw;
		}
	}

	xdebug_log(std::string("ecf_bematech::get_data_hora_ult_red(...) = ") +
		dquote(data) + std::string(" | ") + dquote(hora));
}


/**
 \brief Retorna a data da última Redução Z.
 \returns String de 6 posições, contendo a data no formato "ddmmaa".
 */
std::string ecf_bematech::get_data_ult_red(void)
{
	debug_log("ecf_bematech::get_data_ult_red()");
	
	try {
		get_data_hora_ult_red();
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::get_data_ult_red() = ") +
	    dquote(m_dt_ult_red.c_str()));

	return m_dt_ult_red.c_str();
}


/**
 \brief Retorna a hora da última Redução Z.
 \returns String de 6 posições, contendo a hora no formato "hhmmss".
 */
std::string ecf_bematech::get_hora_ult_red(void)
{
	debug_log("ecf_bematech::get_hora_ult_red()");
	
	try {
		get_data_hora_ult_red();
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::get_hora_ult_red() = ") + 
	    dquote(m_hr_ult_red.c_str()));

	return m_hr_ult_red.c_str();
}


/**
 \brief Retorna o estado do Emissor de Cupons Fiscais.
 \returns Estrutura de flags, indicando o estado do ECF:
	pouco_papel			: sensor sinalizando pouco papel
	sem_papel			: sensor sinalizando fim do papel
	cupom_fiscal_aberto : cupom fiscal em processamento
	z_pendente			: data de movto "expirada", e red. Z ainda não emitida
	memoria_cheia		: memória fiscal lotada
	nao_fiscal_aberto   : comprovante não fiscal em processamento
 */
ecf_status ecf_bematech::get_status_ecf(void)
{
	debug_log(std::string("ecf_bematech::get_status_ecf()"));
	
	bematech_status_mfd sts;
	int					ack,
						st1,
						st2,
						st3,
						retcode;

	// Obter o estado do ECF...
	try {
		xdebug_log("Bematech_FI_VerificaEstadoImpressoraMFD(...)");
		retcode = Bematech_FI_VerificaEstadoImpressoraMFD(&ack, &st1, &st2, &st3);

		if (retcode == BEMATECH_OK) {
			sts.ack = static_cast<char>(ack & 0x00FF);
			sts.st1.byte = static_cast<char>(st1 & 0x00FF);
			sts.st2.byte = static_cast<char>(st2 & 0x00FF);
			sts.st3 = static_cast<char>(st3 & 0x00FF);

			set_status(sts);
		}
		else	{
			// Caso algum erro tenha ocorrido, sem que tenha sido possível
			// se estabelecer comunicação com o ECF, é definida mensagem de
			// erro de acordo com o código de retorno.
			if (!eval_retcode(retcode)) {
				throw ecf_exp(m_retmsg);
			}
		}
	}
	catch (...) {
		throw;
	}

	return m_sts;
}


/**
 \brief Atribui propriedades de status do objeto, em relação ao último comando
 \param sts Estrutura contendo bytes de estado da última operação.
 */
void ecf_bematech::set_status(const bematech_status_mfd sts)
{
	/*
	 * Comentei o log de depuração abaixo, pois estava poluindo DEMAIS
	 * os registros de execução.
	 *
	debug_log("ecf_bematech::set_status()");
	 *
	 */
	
	// Reset de indicadores...
	m_sts.sem_papel = false;
	m_sts.pouco_papel = false;
	m_sts.cupom_fiscal_aberto = false;
	m_sts.z_pendente = false;
	m_sts.memoria_cheia = false;
	m_sts.nao_fiscal_aberto = false;

	if (sts.st1.bits.fim_de_papel)  {
		xdebug_log("ecf_bematech::set_status() = \"Sem papel\"");
		m_sts.sem_papel = true;
	}

	if (sts.st1.bits.pouco_papel)   {
		if (ignorar_pouco_papel())  {
			xdebug_log("ecf_bematech::set_status() = \"Pouco papel\" (ignorado)");
			m_sts.pouco_papel = false;
		}
		else	{
			xdebug_log("ecf_bematech::set_status() = \"Pouco papel\"");
			m_sts.pouco_papel = true;
		}
	}

	if (sts.st1.bits.cupom_fiscal_aberto)   {
		xdebug_log("ecf_bematech::set_status() = \"Cupom Fiscal aberto\"");
		m_sts.cupom_fiscal_aberto = true;
	}

	// TODO: Z pendente...
	// Não há comando indicando se a redução está pendente. Caso isso ocorra,
	// será reportado erro indicando que o movimento já foi fechado, ou seja,
	// que a redução já foi efetuada.

	if (sts.st2.bits.mem_fiscal_lotada) {
		xdebug_log("ecf_bematech::set_status() = \"Memoria cheia\"");
		m_sts.memoria_cheia = true;
	}

	// Status de comprovante não fiscal aberto, apenas passível de teste
	// através do byte de retorno estendido (ST3).
	if (sts.st3 == BEMATECH_NAO_FISCAL_ABERTO)  {
		xdebug_log("ecf_bematech::set_status() = \"Comprovante Nao Fiscal aberto\"");
		m_sts.nao_fiscal_aberto = true;
	}
}


/**
 \brief Obtém o número de cupom correspondente ao último COO.
 \returns Valor numérico.
 */
unsigned int ecf_bematech::get_nr_cupom(void)
{
	debug_log("ecf_bematech::get_nr_cupom()");
	
	char	num_cupom[7];

	try {
		xdebug_log("Bematech_FI_NumeroCupom(...)");
		check(Bematech_FI_NumeroCupom(num_cupom));
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::get_nr_cupom() = ") + 
	    dquote(num_cupom));

	//return atoi(num_cupom);
	return static_cast<unsigned int>(strtoul(num_cupom, NULL, 10));
}


/**
 \brief Obtém o índice correspondente a uma string, em um vetor.
 \param item Elemento desejado.
 \param vct  Vetor de strings.
 \returns Valor inteiro do índice do elemento no vetor, ou "-1" caso o
 elemento não se encontre no vetor.
 */
short ecf_bematech::idx_element(const std::string& item, 
    std::vector<std::string>* vct) NO_THROW
{
	debug_log(std::string("ecf_bematech::idx_element(") + 
	    dquote(item.c_str()) + std::string(")"));
	    
	std::vector<std::string>::iterator  it;
	short								idx = IDX_NOT_FOUND,
										i = 0;

	if (!vct->empty())  {
		for (it = vct->begin(); it != vct->end(); it++) {
			
			xdebug_log(std::string("Comparando ") +
			    dquote((*it).c_str()) + std::string(" e ") +
			    dquote(item.c_str()));
			
			if (str_iequals(*it, item)) {
				idx = i;
				xdebug_log(std::string("Identificado Indice # ") +
				    into_string(idx));
				break;
			}
			i++;
		}
	}

	xdebug_log(std::string("ecf_bematech::idx_element(...) = ") + 
	    (idx < MIN_IDX_VALID ? std::string("Nao encontrado!") : 
			    into_string(idx)));

	return idx;
}


/**
 \brief Adiciona item a uma lista de elementos
 \param vct Vetor de elementos.
 \param item Elemento a ser adicionado.
 */
void ecf_bematech::add_element(std::vector<std::string>* vct,
    const std::string& item)
{
	debug_log(std::string("ecf_bematech::add_element(") +
	    dquote(item.c_str()) + std::string(")"));
	
	vct->push_back(item);
}


/**
 \brief Obtém o índice de uma forma de pagamento.
 \param fpgto Descrição da forma de pagamento.
 \returns Valor inteiro, iniciando em ZERO, indicando a posição, dentro da 
 tabela de formas de pagamento, onde se encontra a forma passada como argumento.
 */
short ecf_bematech::get_id_forma_pgto(const std::string& fpgto)
{
	debug_log(std::string("ecf_bematech::get_id_forma_pgto(") +
	    dquote(fpgto.c_str()) + std::string(")"));
	
	char					buffer_fpgto[920];
	short					idx = idx_element(fpgto.substr(0, m_size_fpgto), 
										&m_fpgto);
	short					rec_size = sizeof(bematech_reg_fpgto_mfd);
	short					total = 920 / rec_size;
	short					i = 0;
	char					descr[BEMATECH_SIZE_FPGTO+1];
	std::string				fmt_dsc;
	bool					vinc_ccd = false;
	bematech_reg_fpgto_mfd* reg;

	// Caso a forma de pagamento não tenha sido encontrada
	// no cache do objeto, solicita a carga da tabela de formas de
	// pagamento do ECF.
	if (idx == IDX_NOT_FOUND)  {
		try {
			xdebug_log("Bematech_FI_VerificaFormasPagamentoMFD(...)");
			check(Bematech_FI_VerificaFormasPagamentoMFD(buffer_fpgto));

			m_fpgto.clear();
			m_vinc_ccd.clear();

			for (i = 0; i < total; i++)	{

				reg = reinterpret_cast<bematech_reg_fpgto_mfd*>
					(buffer_fpgto+(i*rec_size)); //+(i >= 1 ? 1 : 0));
				
				// Retorno está deslocado, pois há uma "," sozinha, ao final
				// do primeiro registro.
				if (reg->descricao[0] == ',')   {
					debug_log("PRIMEIRO BYTE DA FORMA DE PAGAMENTO = ','");
					reg = reinterpret_cast<bematech_reg_fpgto_mfd*>
						(buffer_fpgto+(i*rec_size)+1);
				}

				memset(descr, 0, sizeof(descr));
				strncpy(descr, reg->descricao, m_size_fpgto);

				vinc_ccd = (reg->flag_uso_vinc == '1' ? true : false);

				fmt_dsc = descr;
				trim(fmt_dsc);

				if (!fmt_dsc.empty())   {
					xdebug_log(std::string("Adicionando Forma de Pgto # ") +
					    into_string(i) + std::string(": ") +
					    dquote(fmt_dsc.c_str()) + 
					    std::string(vinc_ccd ? "(V)" : ""));
					add_element(&m_fpgto, fmt_dsc);
					m_vinc_ccd.push_back(vinc_ccd);

					if (str_iequals(fpgto.substr(0, m_size_fpgto), fmt_dsc))
						idx = i;
				}
			}
		}
		catch (...) {
			throw;
		}
	}

	xdebug_log(std::string("ecf_bematech::get_id_forma_pgto(...) = ") + 
	    (idx < MIN_IDX_VALID ? std::string("Nao encontrada!") : 
			    into_string(idx + 1)));

	return (idx < MIN_IDX_VALID ? idx : idx + 1);
}


/**
 \brief Obtém o flag de vinculação de CCD de uma forma de pagamento.
 \param idx Índice da forma de pagamento.
 \returns Valor lógico, indicando se a forma de pagamento, indicada pelo índice
 passado como argumento, permite ou não a emissão de CCD vinculado.
 */
bool ecf_bematech::get_vinc_forma_pgto(const short idx)
{
	debug_log(std::string("ecf_bematech::get_vinc_forma_pgto(") +
	    dquote(into_string(idx).c_str()) + std::string(")"));

	std::vector<bool>::iterator			it;
	short								index = IDX_NOT_FOUND;
	bool								ret = false;
	try {
		// Força a carga da tabela de formas de pagamento, caso ainda tenha
		// sido devidamente carregada para a memória
		//get_id_forma_pgto("");

		if (!m_vinc_ccd.empty())	{
			index = 0;
			for (it = m_vinc_ccd.begin(); it != m_vinc_ccd.end(); it++) {
				index++;
				if (idx == index)   {
					ret = (*it);
				}
			}
		}
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::get_vinc_forma_pgto(...) = ") +
	           std::string((ret ? "Verdadeiro" : "Falso")));

	return (ret);
}


/**
 \brief Obtém o índice de um totalizador não fiscal.
 \param tot String contendo o nome/descrição do totalizador não fiscal.
 \returns Valor inteiro, representando o índice, dentro da tabela de 
 totalizadores não fiscais, correspondente à descrição passada como argumento.
 */
short ecf_bematech::get_totalizador_cnf(const std::string& tot)
{
	debug_log(std::string("ecf_bematech::get_totalizador_cnf(") +
	    dquote(tot.c_str()) + std::string(")"));

	char				buffer_cnf[600];
	short				idx = idx_element(tot.substr(0, m_size_totcnf), 
										&m_tot_cnf);
	short				i = 0;
	short				rec_size = BEMATECH_SIZE_TOTCNF + 1; // O "1" a mais, 
									// corresponde ao separador "," (virgula)
	short				total = (sizeof(buffer_cnf) / rec_size);
	std::string			one_cnf;
	char				buff_um[BEMATECH_SIZE_TOTCNF + 1];

	if (idx == IDX_NOT_FOUND)   {
		try {
			xdebug_log("Bematech_FI_VerificaTotalizadoresNaoFiscaisMFD(...)");
		
			memset(buffer_cnf, 0, sizeof(buffer_cnf));
			check(Bematech_FI_VerificaTotalizadoresNaoFiscaisMFD(buffer_cnf));

			xdebug_log(std::string("{") + dquote(buffer_cnf) + std::string("}"));

			m_tot_cnf.clear();

			for (i = 0; i < total; i++) {
				memset(buff_um, 0, sizeof(buff_um));
				strncpy(buff_um, static_cast<char*>(buffer_cnf+(i*rec_size)), 
					m_size_totcnf);
				
				if (i < BEMATECH_TOTAL_TOTCNF) {
					one_cnf = buff_um;
					trim(one_cnf);

					if (str_iequals(one_cnf, BEMATECH_TXT_SANGRIA) || 
						    str_iequals(one_cnf, BEMATECH_TXT_SUPRIMENTO) || 
						    one_cnf.empty())   {
						xdebug_log(std::string("Adicionando CNF (vazio) # ") +
						    into_string(i) + std::string(" = ") +
						    dquote(BEMATECH_TXT_AVAILABLE));

						add_element(&m_tot_cnf, BEMATECH_TXT_AVAILABLE);
					}
					else	{
						xdebug_log(std::string("Adicionando CNF # ") +
							into_string(i) + std::string(" = ") +
							dquote(one_cnf.c_str()));

						add_element(&m_tot_cnf, one_cnf);
						if (str_iequals(tot.substr(0, m_size_totcnf), one_cnf))
							idx = i;
					}
				}
				else	{
					if (i == 28)	{
						xdebug_log("Adicionando CNF # 28 = \"Sangria\"");

						add_element(&m_tot_cnf, BEMATECH_TXT_SANGRIA);
						if (str_iequals(tot, BEMATECH_TXT_SANGRIA))
							idx = i;
					}
					else if (i == 29)   {
						xdebug_log("Adicionando CNF # 29 = \"Suprimento\"");

						add_element(&m_tot_cnf, BEMATECH_TXT_SUPRIMENTO);
						if (str_iequals(tot, BEMATECH_TXT_SUPRIMENTO))
							idx = i;
					}
				}
			}
		}
		catch (...) {
			throw;
		}
	}

	xdebug_log(std::string("ecf_bematech::get_totalizador_cnf(...) = ") +
	    (idx < MIN_IDX_VALID ? std::string("Nao encontrado!") : 
			    into_string(idx + 1)));

	return (idx < MIN_IDX_VALID ? idx : idx + 1);
}


/**
 \brief Obtém o índice para o próximo totalizador não fiscal (a ser cadastrado).
 \returns Valor inteiro representando a próxima posição livre na tabela.
 */
short ecf_bematech::get_prox_totalizador(void)
{
	debug_log("ecf_bematech::get_prox_totalizador()");

	short   idx = IDX_NOT_FOUND,
			i = 0;
	std::vector<std::string>::iterator  it;

	// Força a carga da tabela de totalizadores,
	// caso esta ainda não tenha sido carregada.
	get_totalizador_cnf("");

	if (!m_tot_cnf.empty())   {
		for (it = m_tot_cnf.begin(); it != m_tot_cnf.end(); it++)   {
			if (idx < MIN_IDX_VALID)	{
				xdebug_log(std::string("Examinando # ") + into_string(i) +
				    std::string(" = ") + dquote((*it).c_str()));
				if (*it == BEMATECH_TXT_AVAILABLE)
					idx = i;
			}
			i++;
		}
	}
	else
		xdebug_log("ecf_bematech::get_prox_totalizador() Tabela vazia!");

	xdebug_log(std::string("ecf_bematech::get_prox_totalizador() = ") +
	    into_string(idx));
	
	return idx;
}


/**
 \brief Grava uma nova forma de pagamento no ECF.
 \param forma Nome/descrição da nova forma de pagamento (moeda).
 \param vinc Flag que indica vinculação com CDC, para uso de TEF.
 */
void ecf_bematech::set_forma_pgto(const std::string& forma, 
    const std::string& vinc)
{
	debug_log(std::string("ecf_bematech::set_forma_pgto(") +
	    dquote(forma.c_str()) + std::string(")"));

	int idx = IDX_NOT_FOUND;

	try {
		idx = get_id_forma_pgto(forma);

		// Caso o índice tenha valor significativo, indica que a forma
		// de pagamento já está cadastrada.
		if (idx < MIN_IDX_VALID)   {
			std::string tmp = forma.substr(0, m_size_fpgto);
			trim(tmp);

			std::string flg = "0";
			if (str_iequals(vinc, std::string("true")) || (vinc[0] == '1'))
				flg = "1";
		
			xdebug_log(std::string("Bematech_FI_ProgramaFormaPagamentoMFD(") +
			    dquote(tmp.c_str()) + std::string(", ") + dquote(flg.c_str()) +
			    std::string(")"));

			if (!fake())
				check(Bematech_FI_ProgramaFormaPagamentoMFD(tmp.c_str(), 
				    flg.c_str()));
			else
			    debug_log("FAKE >> Bematech_FI_ProgramaFormaPagamentoMFD");
			
			m_fpgto.clear();
			m_vinc_ccd.clear();
		}
		else
			throw ecf_exp(ECF_FPGTO_PRESENT);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Cadastra uma nova alíquota tributária no ECF.
 \param aliq Objeto do tipo 'aliquota', definido no namespace 'aliq_trib'.
 */
void ecf_bematech::set_aliquota(const aliq_trib::aliquota& aliq)
{
	debug_log(std::string("ecf_bematech::set_aliquota(") + 
	    const_cast<aliq_trib::aliquota&>(aliq).get_serialized() + 
	    std::string(")"));
	
	int flag = ( aliq.get_tipo() == aliq_trib::TRIBUTADO_ISS
					? aliq_trib::FLAG_ISS
					: aliq_trib::FLAG_ICMS );

	try {
		int idx = get_idx_aliquota(aliq);

		if (idx < MIN_IDX_VALID)	{
			xdebug_log(std::string("Bematech_FI_ProgramaAliquota(") +
			    dquote(aliq.get_raw_numbers().c_str()) + std::string(", ") +
			    into_string(flag) + std::string(")"));

			if (!fake())
				check(Bematech_FI_ProgramaAliquota(
				    aliq.get_raw_numbers().c_str(),flag));
			else
				debug_log("FAKE >> Bematech_FI_ProgramaAliquota");
			
			m_aliqs.clear();
		}
		else
			throw ecf_exp(ECF_ALIQ_DEFINED);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Cadastra um novo totalizador não sujeito ao ICMS
 \param descr Nome/descrição para o novo totalizador
 \param tipo  Tipo do totalizador, sendo "E" para "entrada" ou "S" para "saida"
 \note O parametro "tipo" é ignorado.
 */
void ecf_bematech::set_totalizador(const std::string& descr, 
    const std::string& tipo)
{
	debug_log(std::string("ecf_bematech::set_totalizador(") +
	    dquote(descr.c_str()) + std::string(", ") + dquote(tipo.c_str()) +
	    std::string(")"));
	
	try {
		int idx = get_totalizador_cnf(descr);

		if (idx < MIN_IDX_VALID)	{
			idx = get_prox_totalizador() + 1;

			std::string tmp = descr.substr(0, m_size_totcnf);
			trim(tmp);
		
			xdebug_log(
			    std::string("Bematech_FI_NomeiaTotalizadorNaoSujeitoIcms(")
				+ into_string(idx) + std::string(", ") + dquote(tmp.c_str())
				+ std::string(")"));

			if (!fake())
				check(Bematech_FI_NomeiaTotalizadorNaoSujeitoIcms(
				    idx, tmp.c_str()));
			else
				debug_log("FAKE >> Bematech_FI_NomeiaTotalizadorNaoSujeitoIcms");
			
			m_tot_cnf.clear();
		}
		else
			throw ecf_exp(ECF_TOT_CNF_PRESENT);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Abre um Cupom Fiscal
 \param dcto CPF ou CNPJ do consumidor.
 \param nome Nome ou razão social do consumidor.
 \param endr Endereço do consumidor.
 */
void ecf_bematech::abre_cupom(const std::string& dcto, const std::string& nome,
    const std::string& endr)
{
	debug_log(std::string("ecf_bematech::abre_cupom(") + dquote(dcto.c_str()) +
	    std::string(", ") + dquote(nome.c_str()) + std::string(", ") +
	    dquote(endr.c_str()) + std::string(")"));
	
	try {
		std::string tmp_dcto = dcto;
		trim(tmp_dcto);
		
		std::string tmp_nome = nome.substr(0, BEMATECH_SIZE_NOME);
		trim(tmp_nome);
		
		std::string tmp_endr = endr.substr(0, BEMATECH_SIZE_ENDR);
		trim(tmp_endr);

		xdebug_log(std::string("Bematech_FI_AbreCupomMFD(") +
		    dquote(tmp_dcto.c_str()) + std::string(", ") + 
		    dquote(tmp_nome.c_str()) + std::string(", ") +
		    dquote(tmp_endr.c_str()) + std::string(")"));

		if (!fake())
			check(Bematech_FI_AbreCupomMFD(tmp_dcto.c_str(), tmp_nome.c_str(), 
				tmp_endr.c_str()));
		else
			debug_log("FAKE >> Bematech_FI_AbreCupomMFD");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Cancela o último cupom fiscal emitido.
 */
void ecf_bematech::cancela_cupom(void)
{
	debug_log("ecf_bematech::cancela_cupom()");
	
	try {
		xdebug_log("Bematech_FI_CancelaCupom()");

		if (!fake())
			check(Bematech_FI_CancelaCupom());
		else
			debug_log("FAKE >> Bematech_FI_CancelaCupom");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Verifica se uma alíquota é de ICMS.
 \param idx Índice da alíquota na tabela.
 \returns Indicador lógico.
 */
bool ecf_bematech::aliq_is_icms(short idx)
{
	debug_log(std::string("ecf_bematech::aliq_is_icms(") +
	    into_string(idx) + std::string(")"));
	
	bool	found = false;
	
	if (!m_aliqs.empty())   {
		if (m_aliqs.size() >= idx)  {
			xdebug_log(std::string("Testando ") + 
			    dquote(m_aliqs[idx].get_serialized().c_str()));
			
			xdebug_log(std::string("Verificando ") + 
			    into_string(m_aliqs[idx].get_tipo()) + std::string(" == ") +
			    into_string(aliq_trib::TRIBUTADO_ICMS));
			
			found = (m_aliqs[idx].get_tipo() == aliq_trib::TRIBUTADO_ICMS);
		}
	}

	xdebug_log(std::string("ecf_bematech::aliq_is_icms(...) = ") +
	    std::string((found ? "TRUE" : "FALSE")));
	return found;
}


/**
 \brief Verifica se uma alíquota é de ISS.
 \param idx Índice da alíquota na tabela.
 \returns Indicador lógico.
 */
bool ecf_bematech::aliq_is_iss(short idx)
{
	debug_log(std::string("ecf_bematech::aliq_is_iss(") +
	    into_string(idx) + std::string(")"));
	
	bool	found = false;
	
	if (!m_aliqs.empty())   {
		if (m_aliqs.size() >= idx)  {
			xdebug_log(std::string("Testando ") + 
			    dquote(m_aliqs[idx].get_serialized().c_str()));
			
			xdebug_log(std::string("Verificando ") + 
			    into_string(m_aliqs[idx].get_tipo()) + std::string(" == ") +
			    into_string(aliq_trib::TRIBUTADO_ISS));
			
			found = (m_aliqs[idx].get_tipo() == aliq_trib::TRIBUTADO_ISS);
		}
	}

	xdebug_log(std::string("ecf_bematech::aliq_is_iss(...) = ") +
	    std::string((found ? "TRUE" : "FALSE")));
	return found;
}


/**
 \brief Obtém o índice de uma alíquota tributária.
 \param aliq Alíquota cujo índice se deseja.
 \returns Valor inteiro, indicando a posição da alíquota no ECF, ou -1, caso
 esta não seja encontrada.
 */
short ecf_bematech::get_idx_aliquota(const aliq_trib::aliquota& aliq)
{
	debug_log(std::string("ecf_bematech::get_idx_aliquota(") +
	    const_cast<aliq_trib::aliquota&>(aliq).get_serialized() + std::string(")"));
	
	short										idx = IDX_NOT_FOUND,
												x = 0;
	std::map<short, aliq_trib::aliquota>::iterator  i;
	char										ret_aliq[81];
	char										idx_iss[50];
	unsigned char								flag[16];
	std::string									one_idx;
	std::string									one_aliq;
	aliq_trib::aliquota							obj_aliq;

	// Cache de alíquotas vazio... carregar...
	if (m_aliqs.empty())	{

		// Vetor de flags de vinculação ao ISS, convertidos
		// a partir de retorno do equipamento.
		for (x = 0; x < BEMATECH_TOTAL_ALIQS; x++)	{
			flag[x] = aliq_trib::TRIBUTADO_ICMS;
		}
		
		try {
			memset(idx_iss, 0, sizeof(idx_iss));
			
			xdebug_log("Bematech_FI_VerificaIndiceAliquotasIss(...)");
			
			// Obtenção dos flags de vinculação de ISS
			check(Bematech_FI_VerificaIndiceAliquotasIss(idx_iss));

			short cm = 0;
			short pv = 0;
			short t = strlen(idx_iss);
			short j = 0;
			char buft[BEMATECH_TOTAL_ALIQS];
			do  {
				memset(buft, 0, sizeof(buft));
				if ((idx_iss[j] == ',') || (j == t))  {
					cm = j;
					strncpy(buft, idx_iss+pv, (cm - pv));
					pv = (cm + 1);
					one_idx = buft;
					trim(one_idx);
					// Incluído teste para evitar falsa entrada, verificada
					// quando não há nenhuma alíquota de ISS cadastrada.
					if (!one_idx.empty())   {
						flag[atoi(one_idx.c_str())] = aliq_trib::TRIBUTADO_ISS;
						xdebug_log(std::string("Aliquota # ") + one_idx +
							std::string(" = ISS"));
					}
				}
				j++;
			} while (j <= t);

			memset(ret_aliq, 0, sizeof(ret_aliq));

			xdebug_log("Bematech_FI_RetornoAliquotas(...)");
			
			// Obtenção da tabela de alíquotas tributárias...
			check(Bematech_FI_RetornoAliquotas(ret_aliq));

			// Atribuição da cache, associando um valor de índice,
			// a um objeto 'aliquota', contendo o tipo e o percentual.
			// TODO: encontrar uma forma de alimentar também o atributo 'cst'.
			x = 0;
			pv = 0;
			cm = 0;
			j = 0;
			t = strlen(ret_aliq);
			std::string fmt;
			do  {
				if ((ret_aliq[j] == ',') || (j == t)) {
					cm = j;
					strncpy(buft, ret_aliq+pv, (cm - pv));
					pv = (cm + 1);
					fmt = buft;
					// Tamanhos abaixo fixos em "2" (duas) posições,
					// pois as alíquotas encontram-se no formato: "99.99"
					one_aliq = fmt.substr(0, 2);
					one_aliq += ".";
					one_aliq += fmt.substr(2);
					obj_aliq.set(flag[x], 0, atof(one_aliq.c_str()));
					m_aliqs.insert(std::pair<short, aliq_trib::aliquota>
					    (x++, obj_aliq));
					xdebug_log(std::string("Aliquota # ") + into_string(x) +
						std::string(" = ") + one_aliq);
				}
				j++;
			} while (j <= t);
		}
		catch (...) {
			throw;
		}
	}

	// Varrer a lista, buscando compatibilidade de 'tipo' e 'percentual'...
	// caso não encontre, o retorno ser '-1'
	for (i = m_aliqs.begin(); i != m_aliqs.end(); i++)  {
		xdebug_log(std::string("Comparando ") + 
		    into_string(i->second.get_tipo()) + std::string(":") +
		    into_string(i->second.get_percentual()) + std::string(" e ") +
		    into_string(aliq.get_tipo()) + std::string(":") +
		    into_string(aliq.get_percentual()));
		if ((i->second.get_tipo() == aliq.get_tipo()) &&
				(i->second.get_percentual() == aliq.get_percentual()))   {
			idx = i->first;
			xdebug_log(std::string("Aliquota selecionada: ") + into_string(idx));
		}
	}

	xdebug_log(std::string("ecf_bematech::get_idx_aliquota(...) = ") +
	    (idx <MIN_IDX_VALID ? std::string("Nao encontrada!") : 
			    into_string(idx + 1)));
	
	return (idx < MIN_IDX_VALID ? idx : idx + 1);
}


/**
 \brief Acrescenta um item ao cupom fiscal aberto.
 \param codprod Código do produto.
 \param nomeprod Nome/descrição do produto.
 \param straliq Alíquota 'serializada' no formato "T:00:1700"
 \param qtde Valor real, contendo a quantidade a ser vendida.
 \param valor Valor unitário do produto.
 \param desc Valor do desconto a ser aplicado ao item.
 */
void ecf_bematech::vende_item(const std::string& codprod, 
							    const std::string& nomeprod, 
							    const std::string& straliq,
								const std::string& unidade,
								float qtde,
								float valor,
								float desc)
{
	debug_log(std::string("ecf_bematech::vende_item(") +
	    dquote(codprod.c_str()) + std::string(", ") +
	    dquote(nomeprod.c_str()) + std::string(", ") +
	    dquote(straliq.c_str()) + std::string(", ") +
	    dquote(unidade.c_str()) + std::string(", ") +
	    into_string(qtde) + std::string(", ") +
	    into_string(valor) + std::string(", ") +
	    into_string(desc) + std::string(")"));
	
	aliq_trib::aliquota*	obj_aliq;
	short					idx_aliq;
	std::string				tmp_un = unidade;
	std::string				tmp_qtde;
	std::string				tmp_valor;
	std::string				tmp_desc;
	size_t					pos;

	// Caso não tenha sido informada a UN, tentaremos obtê-la
	// do final da string da descrição.
	if (tmp_un.empty()) {
		std::string tmp_descr = nomeprod;
		trim(tmp_descr);
		pos = tmp_descr.rfind(' ');
		if (pos != std::string::npos)   {
			tmp_un = tmp_descr.substr(pos+1);
			if (tmp_un.length() > 2)	{
				if (tmp_un.at(0) == '(')
				    tmp_un = tmp_un.substr(1, 2);
			}
		}
	}

	// Formatação da quantidade deve ser "9999,999". Ou seja, no máximo 8
	// dígitos, sendo 7 numéricos (4 inteiras e 3 decimais) mais o separador 
	// decimal, que DEVE SER VÍRGULA
	tmp_qtde = format_valor(qtde, "%3.3f");

	// Formatação do valor unitário deve ser "99999,999". No máximo 9 dígitos.
	// 8 numéricos (5 inteiros e 3 decimais) e o separador decimal, que DEVE
	// SER VÍRGULA.
	tmp_valor = format_valor(valor, "%4.3f");

	// Formatação do desconto. Sempre em valor (e não percentual). No máximo
	// 9 dígitos, sendo 8 numéricos (6 inteiros e APENAS 2 decimais) e o
	// separador decimal: VÍRGULA.
	tmp_desc = format_valor(desc, "%6.2f");
	
	try {
		// Tratamento da alíquota:
		// Verificação do definir de tipo, para casos em que o equipamento
		// oferece totalizadores próprios. A saber:
		// SUBSTITUIÇÃO TRIBUTÁRIA => "FF" ("F1" ou "F2" nos equip. com MFD)
		// NAO INCIDENTE => "NN" ("N1" ou "N2" nos equipamentos com MFD)
		// ISENTO = > "II" ("I1" ou "I2" nos equipamentos com MFD)
		// Em não se tratando de nenhum desses, busca na tabela de alíquotas
		// do equipamento, verificando além do percentual, o tipo de tributo
		// (ICMS/ISS).
		// Caso não encontre (índice = "-1"), lança exceção.
		obj_aliq = new aliq_trib::aliquota(straliq);
		std::string aux_aliq;
		
		if (obj_aliq->get_tipo() == aliq_trib::SUBST_TRIBUTARIA)	{
			aux_aliq = "FF";
		}
		else if (obj_aliq->get_tipo() == aliq_trib::NAO_INCIDENTE)  {
			aux_aliq = "NN";
		}
		else if (obj_aliq->get_tipo() == aliq_trib::ISENTO) {
			aux_aliq = "II";
		}
		else	{
			idx_aliq = get_idx_aliquota(
							const_cast<aliq_trib::aliquota&>(*obj_aliq));
			
			if (idx_aliq < MIN_IDX_VALID)
				throw ecf_exp(ECF_ALIQ_NOT_DEFINED);

			aux_aliq = format_valor(static_cast<float>(idx_aliq), "%02.0f");
		}

		if (nomeprod.length() > 29) {
			xdebug_log(std::string("Bematech_FI_AumentaDescricaoItem(") +
			    dquote(nomeprod.c_str()) +std::string(")"));

			if (!fake())
				check(Bematech_FI_AumentaDescricaoItem(nomeprod.c_str()));
			else
				debug_log("FAKE >> Bematech_FI_AumentaDescricaoItem");
		}

		// Não acrescemos valores na venda do item, apenas no sub-total.
		const char* aux_acr = "0,00";
		xdebug_log(std::string("Bematech_FI_VendeItem(") +
		    dquote(codprod.substr(0, 13).c_str()) + std::string(", ") +
		    dquote(nomeprod.substr(0, 29).c_str()) + std::string(", ") +
		    dquote(aux_aliq.c_str()) + std::string(", \"F\", ") +
		    dquote(tmp_qtde.c_str()) + std::string(", 3, ") +
		    dquote(tmp_valor.c_str()) + std::string(", \"$\", ") +
		    dquote(tmp_desc.c_str()) + std::string(")"));

		if (!fake())
			check(Bematech_FI_VendeItem(
			    codprod.substr(0, 13).c_str(),
			    nomeprod.substr(0, 29).c_str(),
			    aux_aliq.c_str(), "F", tmp_qtde.c_str(), 3,
			    tmp_valor.c_str(), "$", tmp_desc.c_str()));
		else
			debug_log("FAKE >> Bematech_FI_VendeItem");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Subtotaliza o cupom fiscal aberto, e atribui valor de acréscimo.
 \param acresc Valor do acréscimo.
 */
void ecf_bematech::inicia_fechamento(float acresc)
{
	debug_log(std::string("ecf_bematech::inicia_fechamento(") +
	    into_string(acresc) + std::string(")"));
	
	std::string tmp_acr;
	std::string tmp_dsc = "0,00";
	const char*	flag = ACRESCIMO;
	const char*	tipo = VALOR;

	tmp_acr = format_valor(acresc, "%11.2f");

	try {
		xdebug_log(std::string("Bematech_FI_IniciaFechamentoCupomMFD(") +
		    dquote(flag) + std::string(", ") + dquote(tipo) + 
		    std::string(", ") + dquote(tmp_acr.c_str()) + std::string(", ") +
		    dquote(tmp_dsc.c_str()) + std::string(")"));

		if (!fake())
			check(Bematech_FI_IniciaFechamentoCupomMFD(flag, tipo, 
			    tmp_acr.c_str(), tmp_dsc.c_str()));
		else
			debug_log("FAKE >> Bematech_FI_IniciaFechamentoCupomMFD");

		set_pgto_seq(IDX_NOT_FOUND);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Efetua uma forma de pagamento, no cupom aberto.
 \param fpgto Nome/descrição da forma de pagamento (15 posições).
 \param valor Valor do pagamento.
 */
void ecf_bematech::pagamento(const std::string& fpgto, float valor)
{
	debug_log(std::string("ecf_bematech::pagamento(") + dquote(fpgto.c_str()) +
	    std::string(", ") + into_string(valor) + std::string(")"));
	
	try {
		short idx = get_id_forma_pgto(fpgto);

		if (idx < MIN_IDX_VALID)
			throw ecf_exp(ECF_FPGTO_MISSING);
		
		std::string cidx = format_valor(static_cast<float>(idx), "%02.0f");
		std::string cvlr = format_valor(valor, "%11.2f");

		xdebug_log(std::string("Bematech_FI_EfetuaFormaPagamentoIndice(") +
		    dquote(cidx.c_str()) + std::string(", ") +
		    dquote(cvlr.c_str()) + std::string(")"));

		if (!fake())
			check(Bematech_FI_EfetuaFormaPagamentoIndice(cidx.c_str(), 
				cvlr.c_str()));
		else
			debug_log("FAKE >> Bematech_FI_EfetuaFormaPagamentoIndice");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Formata valor de ponto flutuante.
 \details Converte em string, usando a máscara informada, e substituindo
 a ocorrência de separador decimal "." por ",".
 \param valor Valor de ponto flutuante.
 \param fmt Máscara de formatação para sprintf
 \returns Ponteiro para o vetor de caracteres do valor convertido.
 */
// TODO: implementar formatação com base ná máscara, pois atualmente, apenas
//			está convertendo de float para string, e trocando o separador de
//			decimal.
// TODO: validar se TRIM está sendo aplicado corretamente, ou seja, se não está
//			causando problema em algum lugar.
std::string ecf_bematech::format_valor(float valor, const char* fmt)
{
	std::string tmp;
	size_t		pos;
	char		aux[32];

	memset(aux, 0, sizeof(aux));
	
	if (sprintf(aux, fmt, valor) < 0)
		throw ecf_exp(ECF_NUM_FORMAT_ERR);

	tmp = aux;
	if ((pos = tmp.find(".")) != std::string::npos) {
		tmp.replace(pos, 1, ",");
	}

	trim(tmp);

	return tmp.c_str();
}


/**
 \brief Encerra o cupom fiscal aberto.
 \param msg Mensagem promocional.
 */
void ecf_bematech::fecha_cupom(const std::string& msg)
{
	debug_log(std::string("ecf_bematech::fecha_cupom(") +
	    dquote(msg.c_str()) + std::string(")"));
	
	try {
		if (!fake())
			check(Bematech_FI_TerminaFechamentoCupom(msg.c_str()));
		else
			debug_log("FAKE >> Bematech_FI_TerminaFechamentoCupom");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Abre comprovante não fiscal vinculado.
 \param fpgto Forma de pagamento vinculada.
 \param valor Valor efetuado na forma de pagamento.
 \param coo COO do cupom fiscal.
 */
void ecf_bematech::abre_nfiscal_vinc(const std::string& fpgto, float valor,
    unsigned int coo)
{
	debug_log(std::string("ecf_bematech::abre_nfiscal_vinc(") +
	    dquote(fpgto.c_str()) + std::string(", ") + 
	    into_string(valor) + std::string(")"));
	
	std::string tmp_valor;
	std::string tmp_coo;

	try {
		tmp_valor = format_valor(valor, "%11.2f");
		tmp_coo = format_valor(static_cast<float>(coo), "%06.0f");

		std::string tmp = fpgto.substr(0, m_size_fpgto);
		trim(tmp);
		    
		xdebug_log(
		    std::string("Bematech_FI_AbreComprovanteNaoFiscalVinculado(") +
			dquote(tmp.c_str()) + std::string(", ") +
			dquote(tmp_valor.c_str()) + std::string(", ") +
			dquote(tmp_coo.c_str()) + std::string(")"));

		if (!fake())
			check(Bematech_FI_AbreComprovanteNaoFiscalVinculado(tmp.c_str(),
				tmp_valor.c_str(), tmp_coo.c_str()));
		else
			debug_log("FAKE >> Bematech_FI_AbreComprovanteNaoFiscalVinculado");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Imprime mensagem em comprovante não fiscal vinculado.
 \param msg Mensagem a ser impressa.
 */
void ecf_bematech::msg_nfiscal_vinc(const std::string& msg)
{
	debug_log(std::string("ecf_bematech::msg_nfiscal_vinc(") +
	    dquote(msg.c_str()) + std::string(")"));
	
	try {
		xdebug_log(
		    std::string("Bematech_FI_UsaComprovanteNaoFiscalVinculado(") +
		    dquote(msg.c_str()) + std::string(")"));
		
		if (!fake())
			check(Bematech_FI_UsaComprovanteNaoFiscalVinculado(msg.c_str()));
		else
			debug_log("FAKE >> Bematech_FI_UsaComprovanteNaoFiscalVinculado");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Fecha comprovante não fiscal vinculado.
 */
void ecf_bematech::fecha_nfiscal_vinc(void)
{
	debug_log("ecf_bematech::fecha_nfiscal_vinc()");
	
	try {
		xdebug_log("Bematech_FI_FechaComprovanteNaoFiscalVinculado()");

		if (!fake())
			check(Bematech_FI_FechaComprovanteNaoFiscalVinculado());
		else
			debug_log("FAKE >> Bematech_FI_FechaComprovanteNaoFiscalVinculado");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Abre comprovante não fiscal não vinculado
 \param nome_tot Nome do totalizador não fiscal.
 \param valor Valor da totalização.
 \param moeda Moeda utilizada.
 */
void ecf_bematech::abre_nfiscal_nvinc(const std::string& nome_tot, float valor,
    const std::string& moeda)
{
	debug_log(std::string("ecf_bematech::abre_nfiscal_nvinc(") +
	    dquote(nome_tot.c_str()) + std::string(", ") +
	    into_string(valor) + std::string(", ") + 
	    dquote(moeda.c_str()) + std::string(")"));
	
	short		idx;
	std::string tmp_idx;
	std::string tmp_valor;
	std::string tmp_moeda;

	try {
		idx = get_totalizador_cnf(nome_tot);
		
		if (idx < MIN_IDX_VALID)
			throw ecf_exp(ECF_TOT_CNF_MISSING);
		
		tmp_idx = format_valor(static_cast<float>(idx), "%02.0f");
		tmp_valor = format_valor(valor, "%11.2f");
		tmp_moeda = moeda.substr(0, m_size_fpgto);
		trim(tmp_moeda);

		xdebug_log(std::string("Bematech_FI_RecebeimentoNaoFiscal(") +
		    dquote(tmp_idx.c_str()) + std::string(", ") +
		    dquote(tmp_valor.c_str()) + std::string(", ") +
		    dquote(tmp_moeda.c_str()) + std::string(")"));

		if (!fake())
			check(Bematech_FI_RecebimentoNaoFiscal(tmp_idx.c_str(),
				tmp_valor.c_str(), tmp_moeda.c_str()));
		else
			debug_log("FAKE >> Bematech_FI_RecebimentoNaoFiscal");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o número de colunas imprimíveis do equipamento.
 \returns Número inteiro curto com o número de caracteres por linha.
 */
short ecf_bematech::get_max_col(void)
{
	debug_log("ecf_bematech::get_max_col()");
	
	return BEMATECH_MAX_COL;
};


/**
 \brief Comando a abertura da gaveta de dinheiro conectada ao ECF.
 */
void ecf_bematech::abre_gaveta(void)
{
	debug_log("ecf_bematech::abre_gaveta()");
	
	try {
		xdebug_log("Bematech_FI_AcionaGaveta()");
		
		check(Bematech_FI_AcionaGaveta());
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Emite a Redução Z.
 */
void ecf_bematech::reducao_z(void)
{
	debug_log("ecf_bematech::reducao_z()");
	
	const char* data = "";
	const char* hora = "";

	// Por alguma razão, a execução da chamada Bematech_FI_ReducaoZ(...)
	// está retornando "Erro de comunicação", mesmo tendo, de fato,
	// efetuado a redução.
	// Portanto, estou deliberadamente, ignorando o retorno da chamada.
	// Futuramente, talvez, seja necessário ler o status, de modo temporizado.

	try {
		xdebug_log("Bematech_FI_ReducaoZ(\"\", \"\")");

		if (!fake())	{
			check(Bematech_FI_ReducaoZ(data, hora));
			m_dt_ult_red = "";
			m_hr_ult_red = "";
		}
		else
			debug_log("FAKE >> Bematech_FI_ReducaoZ");
		
		// Forçar espera, de modo a impedir erro na próxima leitura.
		sleep(50);
	}
	catch (...) {
		// Sem lançamento de erros neste momento. Contudo, a leitura de 
		// status do ECF está sendo realizada.
//		throw;
	}
}


/**
 \brief Verifica se há um cupom fiscal aberto.
 \returns Valor lógico indicando se há ou não cupom fiscal aberto.
 */
bool ecf_bematech::get_cupom_aberto(void)
{
	debug_log("ecf_bematech::get_cupom_aberto()");
	
	try {
		ecf_status sts = get_status_ecf();

		return (sts.cupom_fiscal_aberto != 0);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Programa arredondamento.
 */
void ecf_bematech::set_arredondamento(void)
{
	debug_log("ecf_bematech::set_arredondamento()");
	
	try {
		xdebug_log("Bematech_FI_ProgramaArredondamento()");
		
		check(Bematech_FI_ProgramaArredondamento());
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Verifica se equipamento possibilita impressão de cheques.
 \details Como o modelo MP 4200 TH FI não oferece o recurso de impressão de
 cheques, retorna sempre FALSE.
 */
bool ecf_bematech::get_imprime_cheques(void)
{
	debug_log("ecf_bematech::get_imprime_cheques()");
	
	return false;
}


/**
 \brief Obém o numéro de reduções Z já realizadas no ECF.
 */
short ecf_bematech::get_nr_reducoes(void)
{
	debug_log("ecf_bematech::get_nr_reducoes()");
	
	char	buff[6];
	short   nr_red;

	try {
		xdebug_log("Bematech_FI_NumeroReducoes(...)");
		
		check(Bematech_FI_NumeroReducoes(buff));
		nr_red = atoi(buff);
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::get_nr_reducoes() = ") +
	    into_string(nr_red) + std::string(")"));

	return nr_red;
}


/**
 \brief Obtém os dados da última Redução Z.
 */
std::string ecf_bematech::dados_ult_reducao(void)
{
	debug_log("ecf_bematech::dados_ult_reducao()");
	
	short						i;
	bematech_dados_reducaoz_mfd buffer;
	std::string					dados;
	char						buff_aliq[5];
	char						buff_valiq[15];
	std::string					out_dat = today_rec_path(m_rec_folder,
									std::string("dados_reducao"), 0, 
									std::string("dat"));
	std::string					out_txt = today_rec_path(m_rec_folder,
									std::string("dados_reducao"), 0, 
									std::string("txt"));

	try {
		xdebug_log("Bematech_FI_DadosUltimaReducaoMFD(...)");
		
		check(Bematech_FI_DadosUltimaReducaoMFD(
		    reinterpret_cast<char*>(&buffer)));
		
		// === I N I C I O   D O   L O G   R E T O R N O   D O   E C F ===
		// Registrar neste ponto, para avaliacao futura, os dados da forma
		// como foram obtidos do equipamento. O arquivo gerado podera ser
		// utilizado futuramente para depuracao, caso se verifique o erro de
		// atribuicao de valores aos totalizadores.
		try {
			std::ofstream dat;
			dat.open(out_dat.c_str());
			dat.write(const_cast<char*>(reinterpret_cast<char*>(&buffer)),
						sizeof(bematech_dados_reducaoz_mfd));
			dat.close();
		}
		catch (std::ofstream::failure& err) {
			error_log(std::string("ecf_bematech::dados_ult_reducao(): ") +
				std::string("Falha ao criar arquivo de registro [") +
				out_dat + std::string("]"));
		}
		catch (std::bad_alloc& err) {
			error_log(std::string("ecf_bematech::dados_ult_reducao(): ") +
				std::string("Falha ao alocar memoria para stream de registro."));
		}
		catch (std::exception& err) {
			error_log(std::string("ecf_bematech::dados_ult_reducao(): ") +
				std::string("Impossivel gerar registro de dados (raw)."));
		}
		// === F I M   D O   L O G   R E T O R N O   D O   E C F ===

		// Processar todo o buffer, substituindo todas vírgulas (',') por 
		// sinalizadores de fim de string ('\0'). Assim, todas as substrings
		// terão seu fim assinalado.
		char *ptr = reinterpret_cast<char*>(&buffer);
		for (i = 0; i < sizeof(bematech_dados_reducaoz_mfd); i++)   {
			if (ptr[i] == ',')
				ptr[i] = '\0';
		}

		dados = "|";
		
		xdebug_log(std::string("Grande Total = ") + buffer.total_geral);
		dados += buffer.total_geral;
		dados += "|";

		xdebug_log(std::string("Cancelamentos ICMS = ") + 
		    buffer.tot_cancelamentos_icms);
		dados += buffer.tot_cancelamentos_icms;
		dados += "|";

		xdebug_log(std::string("Descontos ICMS = " ) +
		    buffer.tot_descontos_icms);
		dados += buffer.tot_descontos_icms;
		dados += "|";

		for (i = 0; i < BEMATECH_TOTAL_ALIQS; i++)	{
			if (aliq_is_icms(i))	{
				memset(buff_aliq, 0, sizeof(buff_aliq));
				memset(buff_valiq, 0, sizeof(buff_valiq));
				strncpy(buff_aliq, buffer.aliquotas_tributarias+
					i*(sizeof(buff_aliq)-1), sizeof(buff_aliq)-1);
				strncpy(buff_valiq, buffer.tot_parciais_tributados+
					i*(sizeof(buff_valiq)-1), sizeof(buff_valiq)-1);

				dados += buff_aliq;
				dados += "|";
				dados += buff_valiq;
				dados += "|";
				
				xdebug_log(std::string("ICMS # ") + into_string(i) +
				    std::string(" ") + buff_aliq + std::string("% = R$ ") +
				    buff_valiq);
			}
			else	{
				dados += "0000|00000000000000|";

				xdebug_log(std::string("ICMS # ") + into_string(i) +
				    std::string("0000% = R$ 00000000000000"));
			}
		}

		xdebug_log(std::string("Isencao ICMS = ") + buffer.tot_isencao_icms);
		dados += buffer.tot_isencao_icms;
		dados += "|";

		xdebug_log(std::string("Nao Incidencia ICMS = ") +
		    buffer.tot_nao_incidencia_icms);
		dados += buffer.tot_nao_incidencia_icms;
		dados += "|";

		xdebug_log(std::string("Substituicao Tributaria ICMS = ") +
		    buffer.tot_substituicao_trib_icms);
		dados += buffer.tot_substituicao_trib_icms;
		dados += "|";

		xdebug_log(std::string("Sangria = ") + buffer.tot_sangria);
		dados += buffer.tot_sangria;
		dados += "|";

		xdebug_log(std::string("Suprimento = ") + buffer.tot_suprimento);
		dados += buffer.tot_suprimento;
		dados += "|";

		xdebug_log(std::string("COO = ") + buffer.coo);
		dados += buffer.coo;
		dados += "|";

		xdebug_log(std::string("Data Mvto. = ") + buffer.data_movimento);
		dados += buffer.data_movimento;
		dados += "|";

		xdebug_log(std::string("Acrescimos ICMS = ") + 
		    buffer.tot_acrescimos_icms);
		dados += buffer.tot_acrescimos_icms;
		dados += "|";
		
		// IOF não existe mais. Mantido para compatibilidade.
		xdebug_log("IOF = 00000000000000");
		dados += "00000000000000|";

		xdebug_log(std::string("Cancelamentos ISS = ") +
		    buffer.tot_cancelamentos_issqn);
		dados += buffer.tot_cancelamentos_issqn;
		dados += "|";

		xdebug_log(std::string("Descontos ISS = ") +
		    buffer.tot_descontos_issqn);
		dados += buffer.tot_descontos_issqn;
		dados += "|";
		
		for (i = 0; i < BEMATECH_TOTAL_ALIQS; i++)	{
			if (aliq_is_iss(i))	{
				memset(buff_aliq, 0, sizeof(buff_aliq));
				memset(buff_valiq, 0, sizeof(buff_valiq));
				strncpy(buff_aliq, buffer.aliquotas_tributarias+
					i*(sizeof(buff_aliq)-1), sizeof(buff_aliq)-1);
				strncpy(buff_valiq, buffer.tot_parciais_tributados+
					i*(sizeof(buff_valiq)-1), sizeof(buff_valiq)-1);

				dados += buff_aliq;
				dados += "|";
				dados += buff_valiq;
				dados += "|";
				
				xdebug_log(std::string("ISS # ") + into_string(i) +
				    std::string(" ") + buff_aliq + std::string("% = R$ ") +
				    buff_valiq);
			}
			else	{
				dados += "0000|00000000000000|";

				xdebug_log(std::string("ISS # ") + into_string(i) +
				    std::string("0000% = R$ 00000000000000"));
			}
		}

		xdebug_log(std::string("Isencao ISS = ") + buffer.tot_isencao_issqn);
		dados += buffer.tot_isencao_issqn;
		dados += "|";

		xdebug_log(std::string("Nao Incidencia ISS = ") + 
		    buffer.tot_nao_incidencia_issqn);
		dados += buffer.tot_nao_incidencia_issqn;
		dados += "|";

		xdebug_log(std::string("Substituicao Tributaria ISS = ") +
		    buffer.tot_substituicao_trib_issqn);
		dados += buffer.tot_substituicao_trib_issqn;
		dados += "|";

		xdebug_log(std::string("Acrescimos ISS = ") + 
		    buffer.tot_acrescimos_issqn);
		dados += buffer.tot_acrescimos_issqn;
		dados += "|";
	}
	catch (...) {
		throw;
	}

	// === I N I C I O   D O   L O G   D E   D A D O S   F M T ===
	// Registrar neste ponto, para avaliacao futura, os dados da forma
	// como foram obtidos do equipamento. O arquivo gerado podera ser
	// utilizado futuramente para depuracao, caso se verifique o erro de
	// atribuicao de valores aos totalizadores.
	try {
		std::ofstream txt;
		txt.open(out_txt.c_str());
		txt << dados.c_str();
		txt.flush();
		txt.close();
	}
	catch (std::ofstream::failure& err) {
		error_log(std::string("ecf_bematech::dados_ult_reducao(): ") +
			std::string("Falha ao criar arquivo de dados [") +
			out_txt + std::string("]"));
	}
	catch (std::bad_alloc& err) {
		error_log(std::string("ecf_bematech::dados_ult_reducao(): ") +
			std::string("Falha ao alocar memoria para stream de dados."));
	}
	catch (std::exception& err) {
		error_log(std::string("ecf_bematech::dados_ult_reducao(): ") +
			std::string("Impossivel gerar registro de dados (fmt)."));
	}
	// === F I M   D O   L O G   D E   D A D O S   F M T ===
		
	return dados.c_str();
}


/**
 \brief Formata valores.
 \details Formata valores com zeros à esquerda, e suprimindo separadores.
 \returns String contendo o valor formatado.
 */
std::string ecf_bematech::format_monetary(float valor, const char* fmt)
{
	size_t		i,
				pos = 0;
	char		buffer[64];
	char		output[64];

	if (strfmon(buffer, 64, fmt, valor) == -1)	{
		throw ecf_exp(ECF_NUM_FORMAT_ERR);
	}

	for (i = 0; i < strlen(buffer); i++)	{
		if (isdigit(buffer[i])) {
			output[pos++] = buffer[i];
		}
	}

	output[pos] = '\0';

	return output;
}


/**
 \brief Obtém o contador de reinício.
 \returns Numero inteiro.
 \note Historicamente, se retorna o contador de intervenções técnicas. De fato,
 não há certeza de que seja este mesmo o valor a ser obtido/retornado.
 Nesta implementação, está sendo utilizado o número de substituições do
 proprietário.
 */
short ecf_bematech::get_nr_reinicio(void)
{
	debug_log("ecf_bematech::get_nr_reinicio()");
	
	char	buff[6];

	try {
		xdebug_log("Bematech_FI_NumeroSubstituicoesProprietario(...)");
		
		check(Bematech_FI_NumeroSubstituicoesProprietario(buff));
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::get_nr_reinicio() = ") +
	    dquote(buff));

	return atoi(buff);
}


/**
 \brief Realiza leitura da memória fiscal.
 \param tipo Indica se leitura será por DATA ou COO.
 \param inicio Inicio da faixa, de acordo com o "tipo".
 \param fim Fim da faixa, de acordo com o "tipo".
 \param completa Indicador de leitura completa ou resumida.
 \param saida Define se a saída será pela serial ou a própria ECF.
 \returns Caminho e nome de arquivo, caso o retorno da leitura seja pela serial.
 */
std::string ecf_bematech::memoria_fiscal(int tipo, const std::string& inicio,
    const std::string& fim, bool completa, int saida)
{
	debug_log(std::string("ecf_bematech::memoria_fiscal(") +
	    into_string(tipo) + std::string(", ") + 
	    dquote(inicio.c_str()) + std::string(", ") + 
	    dquote(fim.c_str()) + std::string(", ") +
	    into_string(completa) + std::string(", ") + 
	    into_string(saida) + std::string(")"));
	
	const char* tag_simples = "s";
	const char* tag_completa = "c";

	std::string data = "";

	std::string ret_path = BEMATECH_OUTPUT_PATH;

	ret_path += BEMATECH_RETURN_FILE;

	try {
		if (tipo == ECF_LEITURA_DATA)   {
			std::string dt_ini = format_date(inicio);
			std::string dt_fim = format_date(fim);
			
			if (saida == ECF_SAIDA_SERIAL) {
				xdebug_log(
				    std::string(
					    "Bematech_FI_LeituraMemoriaFiscalSerialDataMFD(") +
				    dquote(dt_ini.c_str()) + std::string(",") +
				    dquote(dt_fim.c_str()) + std::string(", ") +
				    (completa ? tag_completa : tag_simples) +
				    std::string(")"));
				
				check(Bematech_FI_LeituraMemoriaFiscalSerialDataMFD(
				    dt_ini.c_str(), dt_fim.c_str(),
				    (completa ? tag_completa : tag_simples)));
			}
			else	{
				xdebug_log(
				    std::string("Bematech_FI_LeituraMemoriaFiscalDataMFD(") +
				    dquote(dt_ini.c_str()) + std::string(",") +
				    dquote(dt_fim.c_str()) + std::string(", ") +
				    (completa ? tag_completa : tag_simples) +
				    std::string(")"));
				
				check(Bematech_FI_LeituraMemoriaFiscalDataMFD( dt_ini.c_str(),
				    dt_fim.c_str(), (completa ? tag_completa : tag_simples)));
			}
		}
		else if (tipo == ECF_LEITURA_REDUCAO)   {
			if (saida == ECF_SAIDA_SERIAL)	{
				xdebug_log(
				    std::string(
					    "Bematech_FI_LeituraMemoriaFiscalSerialReducaoMFD") +
				    dquote(inicio.c_str()) + std::string(", ") +
				    dquote(fim.c_str()) + std::string(", ") +
				    (completa ? tag_completa : tag_simples) +
				    std::string(")"));
				
				check(Bematech_FI_LeituraMemoriaFiscalSerialReducaoMFD(
				    inicio.c_str(), fim.c_str(),
				    (completa ? tag_completa : tag_simples)));
			}
			else	{
				xdebug_log(
				    std::string("Bematech_FI_LeituraMemoriaFiscalReducaoMFD") +
				    dquote(inicio.c_str()) + std::string(", ") +
				    dquote(fim.c_str()) + std::string(", ") +
				    (completa ? tag_completa : tag_simples) +
				    std::string(")"));
				
				check(Bematech_FI_LeituraMemoriaFiscalReducaoMFD(
				    inicio.c_str(), fim.c_str(),
				    (completa ? tag_completa : tag_simples)));
			}
		}
		else	{
			throw ecf_exp(ECF_TIPO_LEITURA_ERR);
		}

		if (saida == ECF_SAIDA_SERIAL)  {
			if (fs::found(ret_path))	{
				data = BEMATECH_OUTPUT_PATH;
				data += fs::random_name("mem_fiscal_");
				data += ".txt";
				fs::copy_file(ret_path, data);

				xdebug_log(std::string("ecf_bematech::memoria_fiscal(...) = ") +
				    dquote(data.c_str()));
			}
			else	{
				throw ecf_exp(BEMATECH_OUTFILE_NOT_FOUND);
			}
		}
	}
	catch (...) {
		throw;
	}

	return data.c_str();
}


/**
 \brief Formata data, acrescentando "/" ao valor.
 \param valor Data sem separador (ddmmaa ou ddmmaaaa)
 */
std::string ecf_bematech::format_date(const std::string& valor)
{
	std::string fmt;

	fmt = valor.substr(0, 2);
	fmt += "/";
	fmt += valor.substr(2, 2);
	fmt += "/";
	fmt += valor.substr(4);

	return fmt.c_str();
}


/**
 \brief Emite leitura X.
 \param saida Informa se o relatório deve ser emitido no ECF ou obtido
 pela porta serial.
 \returns String contendo o nome do arquivo, caso os dados sejam retornados
 pela serial.
 */
std::string ecf_bematech::leitura_x(int saida)
{
	debug_log(std::string("ecf_bematech::leitura_x(") +
	    into_string(saida) + std::string(")"));
	
	std::string ret_path = BEMATECH_OUTPUT_PATH;
				ret_path += BEMATECH_RETURN_FILE;
	std::string data = "";

	try {
		if (saida == ECF_SAIDA_SERIAL)  {
			xdebug_log("Bematech_FI_LeituraXSerial()");
			
			check(Bematech_FI_LeituraXSerial());
		}
		else	{
			xdebug_log("Bematech_FI_LeituraX()");
			
			check(Bematech_FI_LeituraX());
		}
	}
	catch (...) {
		throw;
	}

	if (saida == ECF_SAIDA_SERIAL)  {
		if (fs::found(ret_path))	{
			data = BEMATECH_OUTPUT_PATH;
			data += fs::random_name("leitura_x_");
			data += ".txt";
			fs::copy_file(ret_path, data);

			xdebug_log(std::string("ecf_bematech::leitura_x(...) = ") +
			    dquote(data.c_str()));
		}
		else	{
			throw ecf_exp(BEMATECH_OUTFILE_NOT_FOUND);
		}
	}

	return data.c_str();
}


/**
 \brief Programa moeda no singular.
 \note Método mantido apenas por compatibilidade.
 */
void ecf_bematech::set_moeda_singular(const std::string& moeda)
{
	warn_log(std::string("DEPRECATED: ecf_bematech::set_moeda_singular(") +
	    dquote(moeda.c_str()) + std::string(")"));
}


/**
 \brief Programa moeda no plural.
 \note Método mantido apenas por compatibilidade.
 */
void ecf_bematech::set_moeda_plural(const std::string& moeda)
{
	warn_log(std::string("DEPRECATED: ecf_bematech::set_moeda_plural(") +
	    dquote(moeda.c_str()) + std::string(")"));
}


/**
 \brief Comando a impressão de cheque.
 \note Método mantido apenas por compatibilidade.
 */
void ecf_bematech::imprime_cheque(int valor_x, int valor_y, float valor,
    int ext1_x, int ext1_y, int ext2_x, int ext2_y, int fav_x, int fav_y,
    const std::string& favorecido, int local_x, int local_y, 
    const std::string& localidade, int dia_x, int dia, int mes_x, int mes, 
    int ano_x, int ano, const std::string& msg)
{
	warn_log("DEPRECATED: ecf_bematech::imprime_cheque(...)");
}


/**
 \brief Abre um relatório gerencial, com o nome especificado.
 \param titulo Nome do relatório gerencial.
 */
void ecf_bematech::abre_rel_gerencial(const std::string& titulo)
{
	debug_log(std::string("ecf_bematech::abre_rel_gerencial(") +
	    dquote(titulo.c_str()) + std::string(")"));
	
	try {
		short nidx = get_id_rel_gerencial(titulo);

		if (nidx < MIN_IDX_VALID)
			throw ecf_exp(ECF_REL_GER_MISSING);

		std::string cidx;
		cidx = format_valor(static_cast<float>(nidx), "%02.0f");

		xdebug_log(std::string("Bematech_FI_AbreRelatorioGerencialMFD(") +
		    dquote(cidx.c_str()) + std::string(")"));

		if (!fake())
			check(Bematech_FI_AbreRelatorioGerencialMFD(cidx.c_str()));
		else
			debug_log("FAKE >> Bematech_FI_AbreRelatorioGerencialMFD");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o índice de um relatório gerencial.
 \param titulo Nome/descrição do relatório gerencial.
 \returns Valor inteiro representando a posição na tabela do ECF.
 */
short ecf_bematech::get_id_rel_gerencial(const std::string& titulo)
{
	debug_log(std::string("ecf_bematech::get_id_rel_gerencial(") +
	    dquote(titulo.c_str()) + std::string(")"));
	
	char					buffer_relger[660+1]; // acrescentado NULL
	short					idx = idx_element(titulo.substr(0, m_size_relger), 
										&m_relger);
	short					total = 660 / sizeof(bematech_rel_ger_mfd);
	short					i = 0;
	short					start_offset = 21;	  // primeira ","
	char					descr[BEMATECH_SIZE_RELGER + 1];
	std::string				fmt_dsc;

	bematech_rel_ger_mfd*   reg;

	// Caso a forma de pagamento não tenha sido encontrada
	// no cache do objeto, solicita a carga da tabela de formas de
	// pagamento do ECF.
	if (idx == IDX_NOT_FOUND)  {
		try {
			xdebug_log("Bematech_FI_VerificaRelatorioGerencialMFD(...)");

			memset(buffer_relger, 0, sizeof(buffer_relger));
			check(Bematech_FI_VerificaRelatorioGerencialMFD(buffer_relger));

			xdebug_log(std::string("{")+dquote(buffer_relger)+std::string("}"));

			m_relger.clear();

			// Foi identificado um problema com a codificação do equipamento.
			// O primeiro relatório gerencial, por vezes retorna "Relatório Geral"
			// (com o "ó" corretamente codificado) e em outros casos,
			// "Relat363rio Geral". Esta "363" aparentemente se torna "", o que
			// desloca toda a leitura. Portanto, inserimos o primeiro elemento
			// fixo na tabela (NÃO É UTILIZADO), e buscamos a ",", que indica
			// o fim do primeiro registro. Assim, ajustamos o início da próxima
			// leitura.
			xdebug_log(std::string("Adicionando Rel. Gerencial (fixo) # 0: ") +
			    dquote("Relatorio Geral"));
			add_element(&m_relger, std::string("Relatório Geral"));

			if (buffer_relger[start_offset] != ',') {
				start_offset++;
			}
			
			for (i = 0; i < (total-1); i++) {
				reg = reinterpret_cast<bematech_rel_ger_mfd*>
					(buffer_relger+start_offset+
					 (i*sizeof(bematech_rel_ger_mfd))+1);

				memset(descr, 0, sizeof(descr));
				strncpy(descr, reg->descricao, m_size_relger);
					    
				fmt_dsc = descr;
				trim(fmt_dsc);

				if (!fmt_dsc.empty())   {
					xdebug_log(std::string("Adicionando Rel. Gerencial # ") +
					    into_string(i+1) + std::string(": ") +
					    dquote(fmt_dsc.c_str()));
					add_element(&m_relger, fmt_dsc);

					if (str_iequals(titulo.substr(0, m_size_relger), fmt_dsc))
						idx = i+1;
				}
			}
		}
		catch (...) {
			throw;
		}
	}

	xdebug_log(std::string("ecf_bematech::get_id_rel_gerencial(...) = ") +
	    (idx < MIN_IDX_VALID ? std::string("Nao encontrada!") :
			    into_string(idx + 1)));

	return (idx < 0 ? idx : idx + 1);
}


/**
 \brief Imprime texto em relatório gerencial.
 \param msg Mensagem de texto.
 */
void ecf_bematech::txt_rel_gerencial(const std::string& msg)
{
	debug_log(std::string("ecf_bematech::txt_rel_gerencial(") +
	    dquote(msg.c_str()) + std::string(")"));
	
	try {
		size_t  buf_siz = 480;
		size_t  tot_siz = strlen(msg.c_str());
		size_t  i = 0;
		char	buff[buf_siz+1];
		char	*ptr;

		while (i < tot_siz) {
			memset(buff, 0, sizeof(buff));
			ptr = const_cast<char*>(msg.c_str());
			strncpy(buff, static_cast<char*>(ptr+i), buf_siz);
			i += buf_siz;

			xdebug_log(std::string("Bematech_FI_UsaRelatorioGerencialMFD(") +
			    dquote(buff) + std::string(")"));

			if (!fake())
				check(Bematech_FI_UsaRelatorioGerencialMFD(buff));
			else
				debug_log("FAKE >> Bematech_FI_UsaRelatorioGerencialMFD");
		}
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Imprime texto em relatório gerencial.
 \param msg Mensagem de texto.
 \param tag Sequencia de caracteres que sinaliza ponto de acionamento 
 da guilhotina.
 */
void ecf_bematech::txt_rel_gerencial(const std::string& msg, const char* tag)
{
	debug_log(std::string("ecf_bematech::txt_rel_gerencial(") +
	    dquote(msg.c_str()) + std::string(", ") +
	    dquote(tag) + std::string(")"));
	
	size_t		pivot = 0;
	size_t		idx = 0;
	std::string slice = "";
	std::string tmp = "";

	try {
		do  {
			do  {
				tmp = msg.substr(pivot);
				idx = tmp.find(tag);

				if (idx != std::string::npos)   {
					slice = tmp.substr(0, idx);
					pivot = pivot + idx + strlen(tag);
					xdebug_log("Tag encontrada!");
				}
				else
					slice = tmp;

				xdebug_log(std::string("Bloco [") + slice + std::string("]"));
				
				txt_rel_gerencial(slice);
				slice = "";

				if (idx != std::string::npos)   {
					xdebug_log("Cortar papel!");
					aciona_guilhotina();
				}
			} while ((pivot < msg.length()) && (idx != std::string::npos));
		} while (!slice.empty());
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Fecha o relatório gerencial aberto.
 */
void ecf_bematech::fecha_rel_gerencial(void)
{
	debug_log("ecf_bematech::fecha_rel_gerencial()");
	
	try {
		xdebug_log("Bematech_FI_FechaRelatorioGerencial()");

		if (!fake())
			check(Bematech_FI_FechaRelatorioGerencial());
		else
			debug_log("FAKE >> Bematech_FI_FechaRelatorioGerencial");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Adiciona um relatório gerencial.
 \param titulo Nome/descrição do novo relatório.
 */
void ecf_bematech::set_rel_gerencial(const std::string& titulo)
{
	debug_log(std::string("ecf_bematech::set_rel_gerencial(") +
	    dquote(titulo.c_str()) + std::string(")"));
	
	try {
		short nidx = get_id_rel_gerencial(titulo);
		if (nidx < MIN_IDX_VALID)   {
			nidx = m_relger.size() + 1;
			std::string cidx;
			cidx = format_valor(static_cast<float>(nidx), "%02.0f");
			std::string tmp = titulo.substr(0, m_size_relger);
			trim(tmp);

			xdebug_log(std::string("Bematech_FI_NomeiaRelatorioGerencialMFD(") +
			    dquote(cidx.c_str()) + std::string(", ") +
			    dquote(tmp.c_str()) + std::string(")"));

			if (!fake())
				check(Bematech_FI_NomeiaRelatorioGerencialMFD(cidx.c_str(),
				    tmp.c_str()));
			else
				debug_log("FAKE >> Bematech_FI_NomeiaRelatorioGerencialMFD");

			m_relger.clear();
		}
		else
			throw ecf_exp(ECF_REL_GER_PRESENT);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Aciona a guilhotina.
 \param modo Indica se corte parcial (padrão) ou total.
 */
void ecf_bematech::aciona_guilhotina(int modo)
{
	debug_log(std::string("ecf_bematech::aciona_guilhotina(") +
	    into_string(modo) + std::string(")"));
	
	try {
		if ((modo != ECF_CORTE_PARCIAL) && (modo != ECF_CORTE_TOTAL))   {
			throw ecf_exp(ECF_TIPO_CORTE_ERR);
		}
		xdebug_log(std::string("Bematech_FI_AcionaGuilhotinaMFD(") +
		    into_string(modo) + std::string(")"));

		if (!fake())
			check(Bematech_FI_AcionaGuilhotinaMFD(modo));
		else
			debug_log("FAKE >> Bematech_FI_AcionaGuilhotinaMFD");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Desativa (apenas) o próximo corte automático.
 */
void ecf_bematech::corte_auto(void)
{
	debug_log("ecf_bematech::corte_auto()");
	
	try {
		xdebug_log("Bematech_FI_AtivaDesativaCorteProximoMFD(0)");

		if (!fake())
			check(Bematech_FI_AtivaDesativaCorteProximoMFD(0));
		else
			debug_log("FAKE >> Bematech_FI_AtivaDesativaCorteProximoMFD");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Retorna se a impressora é térmica.
 \note TODOS os equipamentos do convênio 09/09 o são. Função mantida por razões
 de compatibilidade. SEMPRE retorna 'true'.
 \returns Valor lógico VERDADEIRO.
 */
bool ecf_bematech::is_termica(void)
{
	debug_log("ecf_bematech::is_termica()");
	
	return true;
}


/**
 \brief Retorna se o equipamento suporta requisitos de PAF-ECF.
 \note TODOS os equipamentos do convênio 09/09 suportam. Função mantida por razões
 de compatibilidade. SEMPRE retorna 'true'.
 \returns Valor lógico VERDADEIRO.
 */
bool ecf_bematech::is_paf_compliant(void)
{
	debug_log("ecf_bematech::is_paf_compliant()");
	
	return true;
}


/**
 \brief Define o tipo de corte (automático): parcial ou total.
 \note Equipamentos para atender ao convênio 09/09 não suportam desativação de
 corte total, portanto este método é sem efeito. Mantido apenas por razões de
 compatibilidade.
 \param total Valor lógico indicando se o corte total está ou não habilitado.
 */
void ecf_bematech::set_tipo_corte(bool total)
{
	warn_log(std::string("DEPRECATED: ecf_bematech::set_tipo_corte(") +
	    into_string(total) + std::string(")"));
}


/**
 \brief Obtém o Grande Total.
 \returns Valor numérico de ponto flutuante.
 */
float ecf_bematech::get_grande_total(void)
{
	debug_log("ecf_bematech::get_grande_total()");
	
	char	buff[20];

	try {
		xdebug_log("Bematech_FI_GrandeTotal(...)");
		
		memset(buff, 0, sizeof(buff));
		check(Bematech_FI_GrandeTotal(buff));
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::get_grande_total() = ") +
	    dquote(buff) + std::string(")"));

	return atof(buff);
}


/**
 \brief Verifica se este equipamento está configurado corretamente.
 \returns Valor lógico.
 */
bool ecf_bematech::match_conf(void)
{
	debug_log("ecf_bematech::match_conf()");
	
	// TODO: implementar a validação de número de série e GT.
	//			Por ora, retorna VERDADEIRO, de modo a evitar bloqueio de
	//			execução da automação. DEVE ser complementado ANTES da próxima
	//			homologação de PAF-ECF.

	return true;
}


/**
 \brief Efetua "sangria" do equipamento.
 \param valor Quantia a ser "sangrada".
 */
void ecf_bematech::sangria(float valor)
{
	debug_log(std::string("ecf_bematech::sangria(") + into_string(valor) +
	    std::string(")"));
	
	try {
		std::string fmt_valor = format_valor(valor, "%11.2f");

		xdebug_log(std::string("Bematech_FI_Sangria(") +
		    dquote(fmt_valor.c_str()) + std::string(")"));
		
		if (!fake())
			check(Bematech_FI_Sangria(fmt_valor.c_str()));
		else
			debug_log("FAKE >> Bematech_FI_Sangria");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Efetua "suprimento" do equipamento.
 \param valor Quantia a ser suprida.
 \param moeda Caso não seja informada é assumido "Dinheiro" (compatibilidade).
 */
void ecf_bematech::suprimento(float valor, const std::string& moeda)
{
	debug_log(std::string("ecf_bematech::suprimento(") + into_string(valor) +
	    std::string(", ") + dquote(moeda.c_str()) + std::string(")"));
	
	try {
		std::string fmt_valor = format_valor(valor, "%11.2f");

		xdebug_log(std::string("Bematech_FI_Suprimento(") +
		    dquote(fmt_valor.c_str()) + std::string(", ") +
		    dquote(moeda.c_str()) + std::string(")"));
		
		if (!fake())
			check(Bematech_FI_Suprimento(fmt_valor.c_str(), moeda.c_str()));
		else
			debug_log("FAKE >> Bematech_FI_Suprimento");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Ativa/desativa horário de verão no ECF.
 */
void ecf_bematech::set_horario_verao(void)
{
	debug_log("ecf_bematech::set_horario_verao()");
	
	try {
		xdebug_log("Bematech_FI_ProgramaHorarioVerao()");

		if (!fake())
			check(Bematech_FI_ProgramaHorarioVerao());
		else
			debug_log("FAKE >> Bematech_FI_ProgramaHorarioVerao");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Verifica se o ECF encontra-se com horário de verão programado.
 \returns Valor lógico.
 */
bool ecf_bematech::is_horario_verao(void)
{
	debug_log("ecf_bematech::is_horario_verao()");
	
	bematech_flags_fiscais  flags;
	bool					is_hv = false;

	try {
		xdebug_log("Bematech_FI_FlagsFiscais(...)");
		
		int tmp;
		check(Bematech_FI_FlagsFiscais(&tmp));
		flags.byte = static_cast<unsigned char>(tmp & 0x00FF);
		is_hv = flags.bits.horario_verao;
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::is_horario_verao() = ") +
	    into_string(is_hv) + std::string(")"));

	return is_hv;
}


/**
 \brief Efetua o download da MFD
 \param tipo 0 - total, 1 - data, 2 - COO
 \param ini Parâmetro inicial, conforme o tipo.
 \param fim Parâmetro final, conforme o tipo.
 \param usr Obrigatório quando tipo for 1 (COO).
 \returns String contendo o caminho + nome de arquivo onde for gravado o
 conteúdo obtivo via download.
 \note IMPORTANTE: A rotina da DLL utilizada, não retorna ACK, ST1, ST2 (e ST3).
 */
std::string ecf_bematech::download_mfd(int tipo, const std::string& ini, 
    const std::string& fim, int usr)
{
	debug_log(std::string("ecf_bematech::download_mfd(") + 
	    into_string(tipo) + std::string(", ") + dquote(ini.c_str()) + 
	    std::string(", ") + dquote(fim.c_str()) + std::string(", ") + 
	    into_string(usr) + std::string(")"));
	    
	std::string mfd_name;
	
	try {
		if ((tipo != ECF_DOWNLOAD_MFD_TOTAL) && 
				(tipo != ECF_DOWNLOAD_MFD_DATA) &&
				(tipo != ECF_DOWNLOAD_MFD_COO)) {
			throw ecf_exp(ECF_TIPO_DOWNLOAD_ERR);
		}

		mfd_name = BEMATECH_OUTPUT_PATH;
		mfd_name += fs::random_name("mfd_");
		mfd_name += ".dat";

		std::string ctipo = format_valor(static_cast<float>(tipo), "%1.0f");
		std::string cusr  = format_valor(static_cast<float>(usr),  "%1.0f");

		xdebug_log(std::string("Bematech_FI_DownloadMFD(") + 
		    dquote(mfd_name.c_str()) + std::string(", ") +
		    dquote(ctipo.c_str()) + std::string(", ") +
		    dquote(ini.c_str()) + std::string(", ") +
		    dquote(fim.c_str()) + std::string(", ") +
		    dquote(cusr.c_str()) + std::string(")"));
		
		if  (!eval_retcode(Bematech_FI_DownloadMFD(mfd_name.c_str(),
				ctipo.c_str(), ini.c_str(), fim.c_str(), cusr.c_str())))   {
			throw ecf_exp(m_retmsg);
		}

		if (!fs::found(mfd_name))   {
			throw ecf_exp(BEMATECH_OUTFILE_NOT_FOUND);
		}
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::download_mfd(...) = ") +
	    dquote(mfd_name.c_str()));

	return mfd_name.c_str();
}


/**
 \brief Efetua conversao para texto, do download da MFD
 \param tipo 0 - total, 1 - data, 2 - COO
 \param ini Parâmetro inicial, conforme o tipo.
 \param fim Parâmetro final, conforme o tipo.
 \param origem Arquivo binário, contendo o download da MFD.
 \param destino Arquivo de texto a ser gerado após a conversão.
 \param fmt Formato do texto gerado, pode ser espelho da MFD ou ATO COTEPE.
 \param usr Obrigatório quando tipo for 1 (COO).
 */
void ecf_bematech::converte_mfd(int tipo, const std::string& ini, 
    const std::string& fim, const std::string& origem, 
    const std::string& destino, int fmt, int usr)
{
	bool	mfd2_support = false;
	bool	mfd3_support = false;

#ifdef BEMAMFD2_SUPPORT
	mfd2_support = true;
#endif

#ifdef BEMAMFD3_SUPPORT
	mfd3_support = true;
#endif

	debug_log(std::string("ecf_bematech::converte_mfd(") + 
	    into_string(tipo) + std::string(", ") + dquote(ini.c_str()) + 
	    std::string(", ") + dquote(fim.c_str()) + std::string(", ") + 
	    dquote(origem.c_str()) + std::string(", ") + dquote(destino.c_str()) +
	    std::string(", ") + into_string(fmt) + std::string(", ") +
	    into_string(usr) + std::string(")"));
	
	if (!(mfd2_support || mfd3_support)) {
		throw ecf_exp(BEMATECH_MFD_NOT_SUPPORTED);
	}
	    
	try {
		if ((tipo != ECF_DOWNLOAD_MFD_TOTAL) && 
				(tipo != ECF_DOWNLOAD_MFD_DATA) &&
				(tipo != ECF_DOWNLOAD_MFD_COO)) {
			throw ecf_exp(ECF_TIPO_DOWNLOAD_ERR);
		}
		
		if ((fmt != ECF_CONV_MFD_TEXTO) &&
				(fmt != ECF_CONV_MFD_COTEPE)) {
			throw ecf_exp(ECF_FMT_CONV_ERR);
		}

		std::string ctipo = format_valor(static_cast<float>(tipo), "%1.0f");
		std::string cfmt  = format_valor(static_cast<float>(fmt),  "%1.0f");
		std::string cusr  = format_valor(static_cast<float>(usr),  "%1.0f");

		xdebug_log(std::string("Bematech_FI_FormatoDadosMFD(") + 
		    dquote(origem.c_str()) + std::string(", ") +
		    dquote(destino.c_str()) + std::string(", ") +
		    dquote(cfmt.c_str()) + std::string(", ") +
		    dquote(ctipo.c_str()) + std::string(", ") +
		    dquote(ini.c_str()) + std::string(", ") +
		    dquote(fim.c_str()) + std::string(", ") +
		    dquote(cusr.c_str()) + std::string(")"));
		
		if  (!eval_retcode(Bematech_FI_FormatoDadosMFD(origem.c_str(),
				destino.c_str(), cfmt.c_str(), ctipo.c_str(), ini.c_str(),
				fim.c_str(), cusr.c_str())))   {
			throw ecf_exp(m_retmsg);
		}

		if (!fs::found(destino))   {
			throw ecf_exp(BEMATECH_OUTFILE_NOT_FOUND);
		}
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Retorna o estado do sensor de pouco papel.
 \details Reporta o estado durante o último comando executado.
 \returns Valor lógico.
 */
bool ecf_bematech::pouco_papel(void)
{
	debug_log("ecf_bematech::pouco_papel()");
	xdebug_log(std::string("ecf_bematech::pouco_papel() = ") + 
	    into_string(m_status.st1.bits.pouco_papel));
	
	return m_status.st1.bits.pouco_papel;
}


/**
 \brief Obtém o contador de cupons fiscais emitidos.
 \returns Valor numérico (inteiro longo) de até 7 posições contendo o contador.
 */
long ecf_bematech::contador_cupom_fiscal(void)
{
	debug_log("ecf_bematech::contador_cupom_fiscal()");
	
	char	buff[8];

	try {
		xdebug_log("Bematech_FI_ContadorCupomFiscalMFD(...)");
		
		memset(buff, 0, sizeof(buff));
		check(Bematech_FI_ContadorCupomFiscalMFD(buff));
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::contador_cupom_fiscal() = ") +
	    dquote(buff));

	return atol(buff);
}


/**
 \brief Obtén o contador de operações não fiscais (GNF).
 \returns Valor numérico (inteiro longo) de até 7 posições contendo o contador.
 */
long ecf_bematech::contador_tot_naofiscal(void)
{
	debug_log("ecf_bematech::contador_tot_naofiscal()");
	
	char	buff[8];

	try {
		xdebug_log("Bematech_FI_NumeroOperacoesNaoFiscais(...)");
		
		memset(buff, 0, sizeof(buff));
		check(Bematech_FI_NumeroOperacoesNaoFiscais(buff));
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::contador_tot_naofiscal() = ") +
	    dquote(buff));

	return atol(buff);
}


/**
 \brief Obtém o contador de comprovantes de Crédito ou Débito emitidos.
 \returns Valor numérico (inteiro longo) de até 5 posições contendo o contador.
 */
long ecf_bematech::contador_credito_debito(void)
{
	debug_log("ecf_bematech::contador_credito_debito()");
	
	char	buff[8];

	try {
		xdebug_log("Bematech_FI_ContadorComprovantesCreditoMFD(...)");
		
		memset(buff, 0, sizeof(buff));
		check(Bematech_FI_ContadorComprovantesCreditoMFD(buff));
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::contador_credito_debito() = ") +
	    dquote(buff));

	return atol(buff);
}


/**
 \brief Obtém o contador de relatórios gerenciais emitidos.
 \returns Valor numérico (inteiro longo) de até 7 posições.
 */
long ecf_bematech::contador_rel_gerenciais(void)
{
	debug_log("ecf_bematech::contador_rel_gerenciais()");
	
	char	buff[8];

	try {
		xdebug_log("Bematech_FI_ContadorRelatoriosGerenciasMFD(...)");
		
		memset(buff, 0, sizeof(buff));
		check(Bematech_FI_ContadorRelatoriosGerenciaisMFD(buff));
	}
	catch (...) {
		throw;
	}

	xdebug_log(std::string("ecf_bematech::contador_rel_gerenciais() = ") +
	    dquote(buff));

	return atol(buff);
}


/**
 \brief Cancela item de cupom fiscal.
 \param idx Número ordinal do item, dentro do cupom.
 */
void ecf_bematech::cancela_item(int idx)
{
	debug_log(std::string("ecf_bematech::cancela_item(") + into_string(idx) +
	    std::string(")"));
	
	try {
		if (idx > 0)	{
			std::string cidx = format_valor(static_cast<float>(idx), "%04.0f");

			xdebug_log(std::string("Bematech_FI_CancelaItemGenerico(") +
			    dquote(cidx.c_str()) + std::string(")"));

			if (!fake())
				check(Bematech_FI_CancelaItemGenerico(cidx.c_str()));
			else
				debug_log("FAKE >> Bematech_FI_CancelaItemGenerico");
		}
		else	{
			xdebug_log("Bematech_FI_CancelaItemAnterior()");

			if (!fake())
				check(Bematech_FI_CancelaItemAnterior());
			else
				debug_log("FAKE >> Bematech_FI_CancelaItemAnterior");
		}
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Abre recebimento não fiscal.
 \param doc CPF ou CNPJ do cliente.
 \param nome Nome ou Razão Social do cliente.
 \param endr Endereço do cliente.
 */
void ecf_bematech::abre_recibo(const std::string& doc, const std::string& nome,
    const std::string& endr)
{
	debug_log(std::string("ecf_bematech::abre_recibo(") +
	    dquote(doc.c_str()) + std::string(", ") +
	    dquote(nome.c_str()) + std::string(", ") +
	    dquote(endr.c_str()) + std::string(")"));

	try {
		xdebug_log(
		    std::string("Bematech_FI_AbreRecebimentoNaoFiscalMFD(") +
		    dquote(doc.c_str()) + std::string(", ") +
		    dquote(nome.substr(0, BEMATECH_SIZE_NOME).c_str()) +
		    std::string(", ") +
			dquote(endr.substr(0, BEMATECH_SIZE_ENDR).c_str()));

		if (!fake())
			check(Bematech_FI_AbreRecebimentoNaoFiscalMFD(doc.c_str(),
				nome.substr(0, BEMATECH_SIZE_NOME).c_str(),
				endr.substr(0, BEMATECH_SIZE_ENDR).c_str()));
		else
			debug_log("FAKE >> Bematech_FI_AbreRecebimentoNaoFiscalMFD");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Abre comprovante de Crédito ou Débito.
 \param fpgto Descrição da forma de pagamento (vinculada pelo Cupom Fiscal).
 \param valor Valor da moeda no Cupom Fiscal.
 \param coo Número do cupom ao qual está vinculado.
 \param doc CPF ou CNPJ do cliente.
 \param nome Nome ou Razão Social do cliente.
 \param endr Endereço do cliente.
 */
void ecf_bematech::abre_credito_debito(const std::string& fpgto, float valor,
    unsigned int coo, const std::string& doc, const std::string& nome,
    const std::string& endr)
{
	debug_log(std::string("ecf_bematech::abre_credito_debito(") +
	    dquote(fpgto.c_str()) + std::string(", ") +
	    into_string(valor) + std::string(", ") + into_string(coo) +
	    std::string(", ") + dquote(doc.c_str()) + std::string(", ") +
	    dquote(nome.c_str()) + std::string(", ") + dquote(endr.c_str()) +
	    std::string(")"));
	
	try {
		std::string cvalor = format_valor(valor, "%11.2f");
		std::string ccoo   = format_valor(static_cast<float>(coo), "%06.0f");
		std::string tmp    = fpgto.substr(0, m_size_fpgto);
		trim(tmp);

		xdebug_log(
		    std::string("Bematech_FI_AbreComprovanteNaoFiscalVinculadoMFD(") +
		    dquote(tmp.c_str()) + std::string(", ") +
		    dquote(cvalor.c_str()) + std::string(", ") +
		    dquote(ccoo.c_str()) + std::string(", ") +
		    dquote(doc.c_str()) + std::string(", ") +
		    dquote(nome.substr(0, BEMATECH_SIZE_NOME).c_str()) +
		    std::string(", ") +
		    dquote(endr.substr(0, BEMATECH_SIZE_ENDR).c_str()) +
		    std::string(")"));

		if (!fake())
			check(Bematech_FI_AbreComprovanteNaoFiscalVinculadoMFD(
				tmp.c_str(), cvalor.c_str(),
				ccoo.c_str(), doc.c_str(),
				nome.substr(0, BEMATECH_SIZE_NOME).c_str(),
				endr.substr(0, BEMATECH_SIZE_ENDR).c_str()));
		else
			debug_log("FAKE >> Bematech_FI_AbreComprovanteNaoFiscalVinculadoMFD");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Emite segunda via de Comprovante de Crédito ou Débito
 */
void ecf_bematech::segunda_via_ccd(void)
{
	debug_log("ecf_bematech::segunda_vida_ccd()");
	
	try {
		xdebug_log("Bematech_FI_SegundaViaNaoFiscalVinculadoMFD()");

		if (!fake())
			check(Bematech_FI_SegundaViaNaoFiscalVinculadoMFD());
		else
			debug_log("FAKE >> Bematech_FI_SegundaViaNaoFiscalVinculadoMFD");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Reimprime Comprovante de Crédito ou Débito.
 */
void ecf_bematech::reimprime_ccd(void)
{
	debug_log("ecf_bematech::reimprime_ccd()");
	
	try {
		xdebug_log("Bematech_FI_ReimpressaoNaoFiscalVinculadoMFD()");

		if (!fake())
			check(Bematech_FI_ReimpressaoNaoFiscalVinculadoMFD());
		else
			debug_log("FAKE >> Bematech_FI_ReimpressaoNaoFiscalVinculadoMFD");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Cancela recebimento não fiscal não vinculado (recibo)
 \param doc CPF ou CNPJ do cliente/consumidor (opcional).
 \param nome Nome ou Razão Social do cliente/comsumidor (opcional).
 \param endr Endereço do cliente/consumidor (opcional).
 */
void ecf_bematech::cancela_recibo(const std::string& doc, 
    const std::string& nome, const std::string& endr)
{
	debug_log(std::string("ecf_bematech::cancela_recibo(") +
	    dquote(doc.c_str()) + std::string(", ") +
	    dquote(nome.c_str()) + std::string(", ") +
	    dquote(endr.c_str()) + std::string(")"));
	
	try {
		xdebug_log(std::string("Bematech_FI_CancelaRecebimentoNaoFiscalMFD(") +
		    dquote(doc.c_str()) + std::string(", ") +
		    dquote(nome.substr(0, BEMATECH_SIZE_NOME).c_str()) +
		    std::string(", ") +
		    dquote(endr.substr(0, BEMATECH_SIZE_ENDR).c_str()) +
		    std::string(")"));

		if (!fake())
			check(Bematech_FI_CancelaRecebimentoNaoFiscalMFD(doc.c_str(),
				nome.substr(0, BEMATECH_SIZE_NOME).c_str(),
				endr.substr(0, BEMATECH_SIZE_ENDR).c_str()));
		else
			debug_log("FAKE >> Bematech_FI_CancelaRecebimentoNaoFiscalMFD");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Estorna um Comprovante de Crédito ou Débito.
 \param doc CPF ou CNPJ do contribuinte (cliente).
 \param nome Nome ou Razão Social do contribuinte (cliente).
 \param endr Endereço do cliente (contribuinte).
 */
void ecf_bematech::estorna_ccd(const std::string& doc, const std::string& nome,
    const std::string& endr)
{
	debug_log(std::string("ecf_bematech::estorna_ccd(") +
	    dquote(doc.c_str()) + std::string(", ") +
	    dquote(nome.c_str()) + std::string(", ") +
	    dquote(endr.c_str()) + std::string(")"));
	
	try {
		xdebug_log(std::string("Bematech_FI_EstornoNaoFiscalVinculadoMFD(") +
		    dquote(doc.c_str()) + std::string(", ") +
		    dquote(nome.substr(0, BEMATECH_SIZE_NOME).c_str()) +
		    std::string(", ") +
		    dquote(endr.substr(0, BEMATECH_SIZE_ENDR).c_str()) +
		    std::string(")"));

		if (!fake())
			check(Bematech_FI_EstornoNaoFiscalVinculadoMFD(doc.c_str(),
				nome.substr(0, BEMATECH_SIZE_NOME).c_str(),
				endr.substr(0, BEMATECH_SIZE_ENDR).c_str()));
		else
			debug_log("FAKE >> Bematech_FI_EstornoNaoFiscalVinculadoMFD");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Registra um totalizador, em um documento não fiscal não vinculado.
 \param tot_cnf Nome/descrição do totalizador não fiscal.
 \param valor Valor a ser lançado no totalizador.
 */
void ecf_bematech::reg_nao_fiscal(const std::string& tot_cnf, float valor)
{
	debug_log(std::string("ecf_bematech::reg_nao_fiscal(") +
	    dquote(tot_cnf.c_str()) + std::string(", ") +
	    into_string(valor) + std::string(")"));
	
	try {
		short idx = get_totalizador_cnf(tot_cnf);

		if (idx < 0)	{
			throw ecf_exp(ECF_TOT_CNF_MISSING);
		}

		std::string cidx = format_valor(static_cast<float>(idx), "%02.0f");
		std::string cvlr = format_valor(valor, "%11.2f");

		xdebug_log(std::string("Bematech_FI_EfetuaRecebimentoNaoFiscalMFD(") +
		    dquote(cidx.c_str()) + std::string(", ") +
		    dquote(cvlr.c_str()) + std::string(")"));

		if (!fake())
			check(Bematech_FI_EfetuaRecebimentoNaoFiscalMFD(cidx.c_str(),
				cvlr.c_str()));
		else
			debug_log("FAKE >> Bematech_FI_EfetuaRecebimentoNaoFiscalMFD");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Realiza um lançamento de moeda, em comprovante não fiscal (recibo).
 \param moeda Nome/descrição da forma de pagamento.
 \param valor Valor do pagamento.
 */
void ecf_bematech::lcto_nao_fiscal(const std::string& moeda, float valor)
{
	debug_log(std::string("ecf_bematech::lcto_nao_fiscal(") +
	    dquote(moeda.c_str()) + std::string(", ") +
	    into_string(valor) + std::string(")"));
		
	try {
		pagamento(moeda, valor);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Subtotaliza recibo (comprovante não fiscal).
 \param tipo Permite que no subtotal haja:
	- ACRESCIMO_PERCENTUAL
	- DESCONTO_PERCENTUAL
	- ACRESCIMO_VALOR
	- DESCONTO_VALOR
	- ACR_DSC_PERCENTUAL
	- ACR_DSC_VALOR
 \param valor Valor correspondente ao tipo.
 */
void ecf_bematech::inicia_fechamento_nao_fiscal(unsigned char tipo,
    float valor)
{
	debug_log(std::string("ecf_bematech::inicia_fechamento_nao_fiscal(") +
	    dquote(into_string(tipo).c_str()) + std::string(", ") +
	    into_string(valor) + std::string(")"));
	
	try {
		if (tipo != ecf::NO_OP) {
			std::string cvlr;
			char		ad;
			char		tp;
			std::string acr;
			std::string dsc;
//"%!=0#16.2i"
			switch (tipo)
			{
				case 'A'	:   // ACRESCIMO_PERCENTUAL
					ad = 'A';
					tp = '%';
					acr = format_monetary(valor, "%!=0#2.2i");
					dsc = format_monetary(0.0, "%!=0#2.2i");
//					acr = format_valor(valor, "%02.2f");
//					dsc = "0,00";
					break;
				case 'D'	:   // DESCONTO_PERCENTUAL
					ad = 'D';
					tp = '%';
					acr = format_monetary(0.0, "%!=0#2.2i");
					dsc = format_monetary(valor, "%!=0#2.2i");
//					acr = "0,00";
//					dsc = format_valor(valor, "%02.2f");
					break;
				case 'X'	:   // ACR_DSC_PERCENTUAL
					ad = 'X';
					tp = '%';
					acr = format_monetary(0.0, "%!=0#2.2i");
					dsc = format_monetary(0.0, "%!=0#2.2i");
//					acr = "0,00";
//					dsc = "0,00";
					(
					    valor > 0 ?
						acr = format_monetary(valor, "%!=0#2.2i") :
						dsc = format_monetary(valor, "%!=0#2.2i")
//					    acr = format_valor(valor, "%02.2f") :
//						dsc = format_valor(valor, "%02.2f")
					);
					break;
				case 'a'	:   // ACRESCIMO_VALOR
					ad = 'A';
					tp = '$';
					acr = format_monetary(valor, "%!=0#12.2i");
					dsc = format_monetary(0.0, "%!=0#12.2i");
//					acr = format_valor(valor, "%11.2f");
//					dsc = "0,00";
					break;
				case 'd'	:   // DESCONTO_VALOR
					ad = 'D';
					tp = '$';
					acr = format_monetary(0.0, "%!=0#12.2i");
					dsc = format_monetary(valor, "%!=0#12.2i");
//					acr = "0,00";
//					dsc = format_valor(valor, "%11.2f");
					break;
				case 'x'	:   // ACR_DSC_VALOR
					ad = 'X';
					tp = '$';
					acr = format_monetary(0.0, "%!=0#12.2i");
					dsc = format_monetary(0.0, "%!=0#12.2i");
//					acr = "0,00";
//					dsc = "0,00";
					(
					    valor > 0 ?
						acr = format_monetary(valor, "%!=0#12.2i") :
						dsc = format_monetary(valor, "%!=0#12.2i")
//					    acr = format_valor(valor, "%11.2f") :
//						dsc = format_valor(valor, "%11.2f")
					);
					break;
				default							:
					throw ecf_exp(ECF_SUBTOTAL_MODE_ERR);
			}

			xdebug_log("Bematech_FI_IniciaFechamentoRecebimentoNaoFiscalMFD(\"X\", \"%\", \"0000\", \"0000\")");

			if (!fake())
				check(Bematech_FI_IniciaFechamentoRecebimentoNaoFiscalMFD(
					"X", "%", "0000", "0000"
					));
			else
				debug_log("FAKE >> Bematech_FI_IniciaFechamentoRecebimentoNaoFiscalMFD");

			set_pgto_seq(IDX_NOT_FOUND);
			
/*			check(Bematech_FI_IniciaFechamentoRecebimentoNaoFiscalMFD(
			    static_cast<char*>(&ad), static_cast<char*>(&tp),
			    acr.c_str(), dsc.c_str()));*/
		}
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Fecha Comprovante Não Fiscal Não Vinculado.
 \param msg Mensagem promocional.
 */
void ecf_bematech::fecha_recibo(const std::string& msg)
{
	debug_log(std::string("ecf_bematech::fecha_recibo(") +
	    dquote(msg.c_str()) + std::string(")"));
	
	try {
		xdebug_log(std::string("Bematech_FI_FechaRecebimentoNaoFiscalMFD(") +
		    dquote(msg.c_str()) + std::string(")"));
		
		reset_dont_pay();
		
		if (!fake())
			check(Bematech_FI_FechaRecebimentoNaoFiscalMFD(msg.c_str()));
		else
			debug_log("FAKE >> Bematech_FI_FechaRecebimentoNaoFiscalMFD");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Fecha Comprovante de Crédito e Débito.
 */
void ecf_bematech::fecha_ccd(void)
{
	debug_log("ecf_bematech::fecha_ccd()");
	
	try {
		xdebug_log("Bematech_FI_FechaComprovanteNaoFiscalVinculado()");
		
		reset_dont_pay();
		
		if (!fake())
			check(Bematech_FI_FechaComprovanteNaoFiscalVinculado());
		else
			debug_log("FAKE >> Bematech_FI_FechaComprovanteNaoFiscalVinculado");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Cancela Comprovante Não Fiscal Vinculado diferente do último.
 \param moeda Nome da forma de pagamento do CCD a ser cancelado.
 \param valor Valor do CCD a ser cancelado.
 \param coo COO ao qual o CCD a ser cancelado, está vinculado.
 \param ccd Número do CCD a ser cancelado.
 \param doc CPF do cliente inscrito no CCD a ser cancelado.
 \param nome Nome do cliente...
 \param endr Endereço do cliente...
 */
void ecf_bematech::cancela_nfiscal_pos(const std::string& moeda, float valor, 
    unsigned int coo, unsigned int ccd, const std::string& doc, 
    const std::string& nome, const std::string& endr)
{
	debug_log(std::string("ecf_bematech::cancela_nfiscal_pos(") +
	    dquote(moeda.c_str()) + std::string(", ") + into_string(valor) +
	    std::string(", ") + into_string(coo) + std::string(", ") +
	    dquote(doc.c_str()) + std::string(", ") + dquote(nome.c_str()) +
	    std::string(", ") + dquote(endr.c_str()) + std::string(")"));
	
	try {
		std::string ccoo = format_valor(static_cast<float>(coo), "%06.0f");
		std::string cccd = format_valor(static_cast<float>(ccd), "%06.0f");
		std::string cvlr = format_valor(valor, "%11.2f");
		std::string tmp  = moeda.substr(0, m_size_fpgto);
		trim(tmp);

		xdebug_log(
		    std::string("Bematech_FI_EstornoNaoFiscalVinculadoPosteriorMFD(") +
		    dquote(tmp.c_str()) + std::string(", ") +
			dquote(cvlr.c_str()) + std::string(", ") +
			dquote(ccoo.c_str()) + std::string(", ") +
		    dquote(cccd.c_str()) + std::string(", ") +
			dquote(doc.c_str()) + std::string(", ") + 
		    dquote(nome.substr(0, BEMATECH_SIZE_NOME).c_str()) +
		    std::string(", ") +
		    dquote(endr.substr(0, BEMATECH_SIZE_ENDR).c_str()));

		if (!fake())
			check(Bematech_FI_EstornoNaoFiscalVinculadoPosteriorMFD(
				tmp.c_str(), cvlr.c_str(),
				ccoo.c_str(), cccd.c_str(), doc.c_str(),
				nome.substr(0, BEMATECH_SIZE_NOME).c_str(),
				endr.substr(0, BEMATECH_SIZE_ENDR).c_str()));
		else
			debug_log("FAKE >> Bematech_FI_EstornoNaoFiscalVinculadoPosteriorMFD");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Verifica se há um comprovante não fiscal aberto.
 \returns Valor lógico.
 */
bool ecf_bematech::nao_fiscal_aberto(void)
{
	debug_log("ecf_bematech::nao_fiscal_aberto()");
	
	try {
		int							ists;
		bematech_status_estendido   sts;

		xdebug_log("Bematech_FI_StatusEstendidoMFD(...)");
		check(Bematech_FI_StatusEstendidoMFD(&ists));

		sts.byte = (ists & 0x00FF);

		xdebug_log(std::string("ecf_bematech::nao_fiscal_aberto() = ") +
			into_string((sts.bits.compr_nao_fiscal_aberto != 0)));

		return (sts.bits.compr_nao_fiscal_aberto != 0);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Gera arquivo de texto contendo a tabela de alíquotas tributárias
 do equipamento.
 \returns Path do arquivo gerado.
 */
std::string ecf_bematech::get_tab_aliq(void)
{
	std::string			arq;
	std::ofstream		out;
	std::map<short, aliq_trib::aliquota>::iterator 
						i;
	short				idx = 0;
	aliq_trib::aliquota	aliq("T:00:8888");
	
	try {
		// Carga da tabela de alíquotas...
		get_idx_aliquota(aliq);

		// Definição do nome do arquivo a ser gerado
		arq = std::string(BEMATECH_OUTPUT_PATH) + 
				fs::random_name("bema_aliq_");
		
		out.open(arq.c_str());

		for (i = m_aliqs.begin(); i != m_aliqs.end(); i++)  {
			std::string ln_tmp = "";
			std::string fmt_idx = format_monetary(
							static_cast<float>(++idx), "%!=0#2.0i");
			std::string fmt_pct = format_monetary(
							i->second.get_percentual(), "%!=0#2.2i");
			ln_tmp = fmt_idx + std::string(":") +
						into_string(i->second.get_tipo()) +
						std::string(":") +
						fmt_pct;
			
			std::cerr << ln_tmp << std::endl;
			
			out << ln_tmp << std::endl;
		}
		
		out.close();
	}
	catch (...) {
		throw;
	}
	
	return arq.c_str();
}


/**
 \brief Gera arquivo de texto contendo a tabela de relatórios gerenciais
 do equipamento.
 \returns Path do arquivo gerado. */
std::string ecf_bematech::get_tab_relger(void)
{
	std::string	arq = "";
	
	return arq.c_str();
}


/**
 \brief Gera arquivo de texto contendo a tabela de totalizadores não
 sujeitos ao ICMS.
 \returns Path do arquivo gerado. */
std::string ecf_bematech::get_tab_totcnf(void)
{
	std::string arq = "";
	
	return arq.c_str();
}


/**
 \brief Gera arquivo de texto contendo a tabela de formas de pagamento
 do equipamento.
 \returns Path do arquivo gerado. */
std::string ecf_bematech::get_tab_fpgto(void)
{
	std::string arq = "";
	
	return arq.c_str();
}


/**
 \brief Obtém flag de simulação de execução.
 \returns Valor lógico.
 */
bool ecf_bematech::fake(void)
{
	return m_fake;
}


/**
 \brief Define o número de sequência do meio de pagamento
 */
void ecf_bematech::set_pgto_seq(short val)
{
	debug_log(std::string("ecf_bematech::set_pgto_seq(") +
	          into_string(val) + std::string(")"));
	m_pgto_seq = val;
}


/**
 \brief Obtém o número de sequencia de pagamento
 \returns Valor inteiro.
 */
short ecf_bematech::get_pgto_seq(void)
{
	debug_log(std::string("ecf_bematech::get_pgto_seq() = ") +
	          into_string(m_pgto_seq));
	return m_pgto_seq;
}


/**
 \brief Carrega arquivo de configurações específicas para o driver.
 */
void ecf_bematech::load_specific(void)
{
	std::ifstream   hnd;
	std::string		line;
	char			buff[2048];

	debug_log(std::string("ecf_bematech::load_specific(" + 
		dquote(m_cfg_file.c_str()) + std::string(")")));
	
	if (!m_cfg_file.empty()) {
		if (!fs::found(m_cfg_file)) {
			throw spooler_exp(std::string("Configuracoes do driver (") + 
							  dquote(m_cfg_file.c_str()) + 
							  std::string(") nao encontradas"));
		}

		try {
			hnd.open(m_cfg_file.c_str(), std::ifstream::in);
			hnd.exceptions(std::ios::badbit);

			m_specific.clear();

			do  {
				memset(buff, 0, sizeof(buff));
				hnd.getline(buff, sizeof(buff));
				line = buff;
				if (!hnd.eof())
					m_specific.from_raw_data(line);
			} while (!hnd.eof());
			hnd.close();

			xdebug_log(std::string(BEMATECH_IGNORAR_POUCO_PAPEL) +
					   std::string(" [") +
					   m_specific.get_value_by_key(
					   BEMATECH_IGNORAR_POUCO_PAPEL).c_str() +
					   std::string("]"));
			m_ignorar_pouco_papel = (atoi(m_specific.get_value_by_key(
								BEMATECH_IGNORAR_POUCO_PAPEL).c_str()) > 0);
			
			xdebug_log(std::string(BEMATECH_REC_FOLDER) + std::string(" [") +
						m_specific.get_value_by_key(
						BEMATECH_REC_FOLDER).c_str() + std::string("]"));
			m_rec_folder = m_specific.get_value_by_key(
						BEMATECH_REC_FOLDER).c_str();
		}
		catch (...) {
			error_log(std::string("ecf_bematech::load_specific(): ") +
				std::string("Erro ao carregar parametros."));
			
			bool fim = hnd.eof();

			if (hnd.is_open())
				hnd.close();

			if (!fim)
				throw;
		}
	}
	else {
		debug_log(std::string("ecf_bematech::load_specific()") + 
			std::string("Nenhum arquivo de configuracoes especificas informado"));
		
		xdebug_log("DEFAULT : " + std::string(BEMATECH_IGNORAR_POUCO_PAPEL) +
			std::string(" = 0"));
		m_ignorar_pouco_papel = 0;
		
		xdebug_log("DEFAULT : " + std::string(BEMATECH_REC_FOLDER) +
			std::string(" = '") + std::string(BEMATECH_OUTPUT_PATH) + 
			std::string("'"));
		m_rec_folder = BEMATECH_OUTPUT_PATH;
	}
}


/**
 \brief Retorna flag para ignorar ou nao o sensor de pouco papel.
 */
bool ecf_bematech::ignorar_pouco_papel(void)
{
	return m_ignorar_pouco_papel;
}


/**
 \brief Define flag para ignorar meios de pagamento, em comprovantes nao
 fiscais (de saida).
 */
void ecf_bematech::set_dont_pay(void)
{
	m_dont_pay = true;
}


/**
 \brief Redefine (para o valor original) a flag que sinaliza quando meios de
 pagamento devem ser suprimidos. Por padrao = FALSE.
 */
void ecf_bematech::reset_dont_pay(void)
{
	m_dont_pay = false;
}


/**
 \brief Obtém o valor da flag de supressão de meios de pagamento (em compro-
 vantes não fiscais).
 */
bool ecf_bematech::get_dont_pay(void)
{
	return m_dont_pay;
}
