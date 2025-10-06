#include "BH1750.h"

#include "hardware/gpio.h"
#include "pico/stdlib.h"

BH1750::BH1750(i2c_inst_t *const interface_i2c,
						   uint8_t pino_sda,
						   uint8_t pino_scl,
						   uint8_t endereco_dispositivo)
	: interfaceI2c(interface_i2c),
	  pinoSda(pino_sda),
	  pinoScl(pino_scl),
	  enderecoSensor(endereco_dispositivo),
	  sensorInicializado(false)
{
}

bool BH1750::inicializarSensor()
{
	if (sensorInicializado) {
		return true;
	}

	if (interfaceI2c == nullptr) {
		return false;
	}

	i2c_init(interfaceI2c, FREQUENCIA_I2C_HZ);

	if (!configurarPinosI2c()) {
		return false;
	}

	if (!enviarComando(ComandoBh1750::LIGAR)) {
		return false;
	}

	sleep_ms(10u);

	if (!enviarComando(ComandoBh1750::REINICIAR)) {
		return false;
	}

	if (!enviarComando(ComandoBh1750::MODO_CONTINUO_ALTA_RESOLUCAO)) {
		return false;
	}

	sensorInicializado = true;

	return true;
}

float BH1750::lerIntensidadeLuminosaLux()
{
	if (!sensorInicializado) {
		return INTENSIDADE_LUX_INVALIDA;
	}

	uint16_t dados_brutos = 0u;

	if (!lerDadosBrutos(dados_brutos)) {
		return INTENSIDADE_LUX_INVALIDA;
	}

	return static_cast<float>(dados_brutos) / FATOR_CONVERSAO_LUX;
}

bool BH1750::configurarPinosI2c() const
{
	if (interfaceI2c == nullptr) {
		return false;
	}

	gpio_init(pinoSda);
	gpio_init(pinoScl);

	gpio_set_function(pinoSda, GPIO_FUNC_I2C);
	gpio_set_function(pinoScl, GPIO_FUNC_I2C);

	gpio_pull_up(pinoSda);
	gpio_pull_up(pinoScl);

	return true;
}

bool BH1750::enviarComando(ComandoBh1750 comando) const
{
	if (interfaceI2c == nullptr) {
		return false;
	}

	const uint8_t comando_a_enviar = static_cast<uint8_t>(comando);

	const int resultado_escrita = i2c_write_blocking(interfaceI2c,
													 enderecoSensor,
													 &comando_a_enviar,
													 1,
													 false);

	return resultado_escrita == 1;
}

bool BH1750::lerDadosBrutos(uint16_t &dados_brutos) const
{
	if (interfaceI2c == nullptr) {
		return false;
	}

	sleep_ms(TEMPO_CONVERSAO_MS);

	uint8_t buffer_leitura[2] = {0u};

	const int bytes_lidos = i2c_read_blocking(interfaceI2c,
											  enderecoSensor,
											  buffer_leitura,
											  2,
											  false);

	if (bytes_lidos != 2) {
		return false;
	}

	dados_brutos = static_cast<uint16_t>(static_cast<uint16_t>(buffer_leitura[0]) << 8u)
				   | static_cast<uint16_t>(buffer_leitura[1]);

	return true;
}
