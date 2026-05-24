Projeto de Sistemas Operativos - FCUL 2025-2026, desenvolvido por:
  Alexandre Ribeiro - 63723
  André Vilarinho - 63727
  Margarida Torres - 63751

####################################################
#                 Compilação                       #
####################################################

  make clean
  make SOestimate

####################################################
#                   Execução                       #
####################################################

Nesta fase, o programa recebe dois ficheiros:
  ./bin/SOestimate args.txt settings.txt

####################################################
#                    Formato                       #
####################################################

Formato dos ficheiros de entrada:
  args.txt:

  Cada linha contém:
    1) n_sensors
    2) n_servers
    3) buffers_size
  Exemplo:
    4
    4
    8

  settings.txt:

  Cada linha contém:
    1) log_filename
    2) statistics_filename
    3) period
  Exemplo:
    log.txt
    stats.txt
    5

####################################################
#                   Comandos                       #
####################################################

Comandos suportados:
- measure
- read <cycle_id>
- stat
- help
- end

####################################################
#                      Notas                       #
####################################################

- Se num ciclo não existir resultado válido, pode não ser escrita linha "result" para esse ciclo no log.
- O comando read <j> devolve o resultado encontrado no log para a ronda j; se não existir, indica que não foi encontrado.
