# Carrinho-robô — Project-based Maker Lab (FIAP)

Repositório do projeto do grupo ✏️ **[nome do grupo]** — Engenharia de Software, disciplina Project-based Maker Lab (Profª Dra. Gedeane G. S. Kenshima).

**Integrantes:** ✏️ nome (RM), nome (RM), …

## O que é

Carrinho-robô de duas rodas com tração diferencial, controlado sem fio pelo celular e com parada automática por sensor ultrassônico.

- **Plataforma:** ESP32 DevKit V1
- **Controle:** Wi-Fi — o ESP32 sobe um servidor web e o celular controla pelo navegador
- **Sensor:** HC-SR04 (para automaticamente a menos de 20 cm de um obstáculo)
- **Driver:** ponte H L298N
- **Alimentação:** 2 × 18650 (7,4 V) com regulador step-down 5 V para a lógica

## Estrutura

```
/hardware
  /arquitetura      diagramas de blocos, alimentação e layout físico (SVG)
  /componentes      lista de componentes, pinagem e consumo
/cad
  /stl              chassi exportado para impressão/corte (v0.1, v0.2 …)
  /fonte            modelo paramétrico em OpenSCAD
/src
  codigo.ino        firmware (ESP32 + Wi-Fi + ultrassônico)
/docs
  changelog-chassi.md    o que mudou da V0.1 para a V0.2
  mudancas-codigo.md     o que o grupo acrescentou ao código da aula
  testes.md              roteiro de teste incremental
  /registro-testes       fotos e vídeos dos testes
```

## Arquitetura

| Diagrama | Arquivo |
|---|---|
| Blocos (sinais e pinos) | [`hardware/arquitetura/diagrama-blocos.svg`](hardware/arquitetura/diagrama-blocos.svg) |
| Alimentação | [`hardware/arquitetura/diagrama-alimentacao.svg`](hardware/arquitetura/diagrama-alimentacao.svg) |
| Layout físico e cabos | [`hardware/arquitetura/layout-fisico.svg`](hardware/arquitetura/layout-fisico.svg) |

## Como rodar

1. Instalar a IDE Arduino e adicionar o suporte ao ESP32 (Gerenciador de Placas → `esp32` by Espressif).
2. Selecionar a placa **ESP32 Dev Module** e a porta COM correspondente.
3. Abrir `/src/codigo.ino` e preencher `ssid` e `password` com a rede Wi-Fi **de 2,4 GHz** (o ESP32 não enxerga 5 GHz).
4. Gravar, abrir o monitor serial em **115200** e anotar o IP que aparece.
5. Com o celular na mesma rede, acessar `http://<IP>` no navegador.

Antes do primeiro teste com os motores, seguir o roteiro de [`docs/testes.md`](docs/testes.md) com as rodas fora do chão.

## Escolha do controle sem fio

A aula apresentou três caminhos (Wi-Fi, ESP32-BLE e Arduino+HC-05). O grupo adotou o **controle via Wi-Fi** (opção 1, slide 7): não exige instalar nenhum app no celular — basta abrir o IP do ESP32 no navegador — e permite exibir a leitura do sensor na mesma tela do controle.

O código parte do que foi apresentado nos slides 8 a 13; os acréscimos do grupo estão marcados com `// [GRUPO]` e listados em [`docs/mudancas-codigo.md`](docs/mudancas-codigo.md).

## Estado atual

- [x] Chassi V0.2 definido
- [x] Diagramas de blocos e alimentação
- [x] Firmware com movimento, Wi-Fi e sensor
- [x] Sensor ultrassônico integrado ao código
- [ ] Testes incrementais registrados (`docs/testes.md`)
- [ ] STL da V0.2 exportado para `/cad/stl`
- [ ] ✏️
