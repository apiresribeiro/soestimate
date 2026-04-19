Projeto de Sistemas Operativos - FCUL 2025-2026, desenvolvido por:
  Alexandre Ribeiro - 63723
  André Vilarinho - 63727
  Margarida Torres - 63751

Para correr o projeto deve fazer:
  $ make clean
  $ make
  $ ./SOEstimate n_sensors n_servers buff_size

Opcionalmente pode fazer:
  $ make valgrind - Para verificar o output do da ferramenta Valgrind

Deve correr o projeto com número de entidades reduzidas (ex. $ ./SOestimate 1 1 2)
para evitar problemas de sincronização, que são uma limitação desta primeira fase do
projeto.
