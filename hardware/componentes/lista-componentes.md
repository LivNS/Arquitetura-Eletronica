# Lista de componentes (BOM)

## 1. Eletrônica

| # | Componente | Modelo | Qtd | Tensão | Consumo típico | Função no projeto |
|---|---|---|---|---|---|---|
| 1 | Microcontrolador | ESP32 DevKit V1 (30 ou 38 pinos) | 1 | 5 V no VIN / 3,3 V lógica | ~80 mA (picos de 250 mA no Wi-Fi) | servidor web + controle |
| 2 | Driver de motor | Ponte H L298N | 1 | 5–35 V | ~36 mA + motores | aciona os dois motores |
| 3 | Sensor de distância | HC-SR04 | 1 | 5 V | ~15 mA | parada automática frontal |
| 4 | Regulador | step-down MP1584 ou LM2596 | 1 | entrada 7–28 V → 5 V | até 1 A | alimenta o ESP32 |
| 5 | Motor DC | TT com redução 1:48 | 2 | 3–6 V | ~150 mA livre / ~1,2 A travado | tração |
| 6 | Bateria | Li-ion 18650 2200 mAh | 2 | 3,7 V cada (7,4 V em série) | — | fonte única |
| 7 | Suporte de bateria | 2 × 18650 com fios | 1 | — | — | fixação das células |
| 8 | Chave | gangorra KCD1 2 pinos | 1 | — | — | liga/desliga geral |
| 9 | Resistores | 1 kΩ e 2 kΩ | 1 de cada | — | — | divisor no ECHO do HC-SR04 |
| 10 | Fusível + porta-fusível | 2 A vidro 5×20 mm | 1 | — | — | proteção (recomendado) |

## 2. Mecânica e montagem

| # | Item | Especificação | Qtd |
|---|---|---|---|
| 11 | Chassi | 200 × 150 × 4 mm — ver `/cad` | 1 |
| 12 | Roda | 65 mm, eixo TT | 2 |
| 13 | Roda boba | esfera metálica ou rodízio | 1 |
| 14 | Parafusos + porcas | M3 × 10 mm | ~20 |
| 15 | Espaçadores | latão M3 15 mm | 8 |
| 16 | Abraçadeiras | 2,5 × 100 mm | ~30 |
| 17 | Espiral organizador | 6 mm | 1 m |
| 18 | Jumpers | macho-fêmea e macho-macho | ~40 |
| 19 | Protoboard pequena | 170 pontos (para o divisor) | 1 |

## 3. Mapa de pinos — ESP32 (versão Wi-Fi, `/src/codigo.ino`)

Pinagem do L298N conforme a tabela do slide 7 da Aula 16.

| GPIO | Direção | Vai para | Função | Cor sugerida |
|---|---|---|---|---|
| 27 | saída | L298N IN1 | sentido motor esquerdo | branco |
| 26 | saída | L298N IN2 | sentido motor esquerdo | branco |
| 14 | saída PWM | L298N ENA | velocidade motor esquerdo | laranja |
| 33 | saída | L298N IN3 | sentido motor direito | cinza |
| 25 | saída | L298N IN4 | sentido motor direito | cinza |
| 32 | saída PWM | L298N ENB | velocidade motor direito | laranja |
| 5 | saída | HC-SR04 TRIG | dispara o pulso | amarelo |
| 18 | entrada | HC-SR04 ECHO **via divisor 1 kΩ/2 kΩ** | mede o retorno | roxo |
| VIN | alimentação | saída do regulador 5 V | entrada de energia | vermelho |
| GND | referência | barramento comum | terra do sistema | preto |

**Total:** 8 GPIOs. Evitar os GPIOs 6–11 (flash SPI) e os pinos de boot (0, 2, 12, 15).

## 4. Consumo estimado

| Bloco | Corrente |
|---|---|
| 2 motores em regime | ~600 mA |
| 2 motores em pico | ~2,4 A |
| ESP32 + HC-SR04 | ~100 mA (pico 270 mA transmitindo) |
| **Pico total** | **~2,7 A** |

Com 2200 mAh e consumo médio de ~800 mA, a autonomia estimada é de **2 a 2,5 horas**. ✏️ Medir na prática e registrar.
