# BH1750

Biblioteca C++ para o Raspberry Pi Pico W que encapsula a comunicação com o sensor de luminosidade BH1750 através da interface I2C. A classe `BH1750` centraliza a inicialização do barramento, o envio de comandos de configuração e a conversão dos dados crus para lux, tornando simples a integração do sensor em qualquer projeto.

## Recursos principais

- Inicialização segura do sensor com sequência completa de comandos (ligar, reiniciar e modo contínuo de alta resolução).
- Configuração automática dos pinos SDA/SCL com `gpio_set_function` e resistores pull-up internos.
- Operação padrão em `400 kHz` com endereço configurável (`0x23` por padrão).
- Conversão direta dos dados para lux utilizando o fator de 1,2 conforme datasheet.
- Valor sentinela para indicar leituras inválidas quando a comunicação falha ou o sensor não foi inicializado.

## Requisitos

- Raspberry Pi Pico W (RP2040) com SDK 2.2.0.
- Ferramentas de build configuradas com CMake e Ninja (estrutura padrão do Pico SDK).
- Sensor BH1750 conectado ao barramento I2C desejado do seu hardware:
  - Endereço padrão `0x23` (pino ADDR em GND). Ajuste no construtor quando utilizar `0x5C`.
  - Pinos SDA e SCL compatíveis com a instância de I2C escolhida.
  - Resistores pull-up externos recomendados mesmo com os internos habilitados pela biblioteca.

## Como adicionar ao seu projeto

1. Garanta que a pasta `BH1750` esteja dentro do diretório raiz do seu projeto (ao lado do `CMakeLists.txt` principal).
2. No `CMakeLists.txt` principal, inclua a subpasta e vincule a biblioteca ao seu executável:

```cmake
add_subdirectory(BH1750)

add_executable(seu_programa
    main.cpp
    # outros fontes
)

target_link_libraries(seu_programa
    PRIVATE
        BH1750
)
```

3. Certifique-se de inicializar o I2C desejado antes de utilizá-lo em outras partes do código (a biblioteca se encarrega do sensor, mas depende da instância correta).

## Exemplo de uso rápido

```cpp
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "BH1750.h"

constexpr uint I2C_SDA = 14u;
constexpr uint I2C_SCL = 15u;

int main() {
    stdio_init_all();

    BH1750 sensor(i2c1, I2C_SDA, I2C_SCL);

    if (!sensor.inicializarSensor()) {
        while (true) {
            sleep_ms(1000);
        }
    }

    while (true) {
        const float lux = sensor.lerIntensidadeLuminosaLux();

        if (lux >= 0.0f) {
            printf("Luz: %.2f lux\n", lux);
        }

        sleep_ms(500);
    }
}
```

### Pontos de atenção no exemplo

- Os pinos `I2C_SDA` e `I2C_SCL` devem ser compatíveis com a instância `i2c1` (ou ajuste para `i2c0` e respectivos pinos).
- `lerIntensidadeLuminosaLux` retorna `BH1750::INTENSIDADE_LUX_INVALIDA` (`-1.0f`) quando o sensor não está pronto.

## API e exemplos da classe `BH1750`

Constrói o objeto associando a instância I2C, pinos e endereço opcional.

```cpp
BH1750 sensor(i2c0, 12u, 13u);
```

Inicializa o sensor com a sequência de comandos e evita reinicializações redundantes.

```cpp
sensor.inicializarSensor();
```

Lê a intensidade luminosa. Retorna `INTENSIDADE_LUX_INVALIDA` em caso de falha.

```cpp
float lux = sensor.lerIntensidadeLuminosaLux();
if (lux >= 0.0f) {
    // utilizar a leitura
}
```

## Boas práticas

- Prefira manter o sensor em modo contínuo e consumir as leituras no intervalo recomendado (`TEMPO_CONVERSAO_MS`).
- Trate valores negativos como leituras inválidas e considere reinicializar ou verificar a fiação do sensor.
- Utilize resistores pull-up externos de 4,7 kΩ nos pinos SDA/SCL para maior robustez elétrica, especialmente em cabos longos.
- Ajuste a frequência I2C (`FREQUENCIA_I2C_HZ`) se o seu barramento precisar operar em outro valor (alterando antes da chamada `i2c_init`).
