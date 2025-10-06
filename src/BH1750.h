#ifndef BH1750_H
#define BH1750_H

#include <cstdint>
#include "hardware/i2c.h"

class BH1750 {
public:
	static constexpr uint32_t FREQUENCIA_I2C_HZ = 400000u;
	static constexpr uint8_t ENDERECO_SENSOR_PADRAO = 0x23u;
	static constexpr float FATOR_CONVERSAO_LUX = 1.2f;
	static constexpr uint32_t TEMPO_CONVERSAO_MS = 180u; // Datasheet indica 120 ms típicos
	static constexpr float INTENSIDADE_LUX_INVALIDA = -1.0f;

	BH1750(i2c_inst_t *const interface_i2c,
				 uint8_t pino_sda,
				 uint8_t pino_scl,
				 uint8_t endereco_dispositivo = ENDERECO_SENSOR_PADRAO);

	bool inicializarSensor();

	float lerIntensidadeLuminosaLux();

private:
	enum class ComandoBh1750 : uint8_t {
		DESLIGAR = 0x00u,
		LIGAR = 0x01u,
		REINICIAR = 0x07u,
		MODO_CONTINUO_ALTA_RESOLUCAO = 0x10u
	};

	bool configurarPinosI2c() const;

	bool enviarComando(ComandoBh1750 comando) const;

	bool lerDadosBrutos(uint16_t &dados_brutos) const;

	i2c_inst_t *const interfaceI2c;
	const uint8_t pinoSda;
	const uint8_t pinoScl;
	const uint8_t enderecoSensor;
	bool sensorInicializado;
};

#endif
