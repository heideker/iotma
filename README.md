# IOTMA - IoT Mananger Agent
IOTMA is an agent to mananger and monitoring a machine (real or virtual).

# About

Some thing...

# Requiriments

Linux OS.


# Getting docker


```
#> apt install make gcc libcurl4-openssl-dev g++ git nano zip unzip libjsoncpp-dev
```

Clonning repository:

``` 
#>git clone https://github.com/heideker/IMAIoT
```

## Compilling 

To compile source code you must have the following packages:

````
#> apt-get install make gcc libcurl4-openssl-dev g++ libjsoncpp-dev
````

Enter in IOTMA directory and use the make command to compile source code:

````
#> cd iotma
#iotma> make all
````


## Configuring

Some thing...

Key |	Description
---------|-------------
debugMode = 0 | [boleano] Quando o valor desta variável é igual a 1, o IMAIoT apresenta o valor de todas as suas variáveis em tela além das operações realizadas e resultados obtidos para depuração do agente.
NodeName = testeIMAIT | [texto] Esta variável configura um nome amigável para o agente.
NodeUUID = urn:ngsi-ld:999 | [texto] Esta variável configura o UUID utilizado no context broker para identificar unicamente o agente.
KindOfNode = LoRaGateway | [texto] Identifica o tipo de nó monitorado para simplificar a consulta do mesmo na plataforma de IoT
SampplingTime = 5 | [inteiro] Intervalo de amostragem das métricas em segundos
LogMode = 0 | [boleano] Quando o valor da variável for igual a 1 o IMAIoT registra o log local com as métricas
LogType = txt | [txt | json] Identifica o formato do arquivo de log, se texto separado por ponto e virgula (txt) ou json
LogFileName = imaiot.log | [texto] Nome do arquivo de log. Pode conter o caminho completo até o arquivo destino
LogIntervall = 5 | [inteiro] Intervalo de registro das métricas no arquivo de log, em segundos
ServerMode = 1 | [boleano] Ativa o socket TCP para consulta ao IMAIoT quando o valor for igual a 1
ServerPort = 5999 | [inteiro] Porta utilizada para a abertura do socket TCP
OrionMode = 0 | [boleano] Ativa a publicação das métricas coletadas no context broker quando o valor for igual a 1
OrionHost = http://hostname | [texto] URL (hostname ou endereço IP) para o context broker
OrionPort = 1026 | [inteiro] Número da porta para o context broker
OrionPublisherTime = 30 | [inteiro] Intervalo entre as publicações de métricas no context broker, em segundos
DockerStat = 0 | [boleano] Ativa a coleta de estatísticas sobre containers dockers quando o valor for igual a 1
DockerNames = * | [texto] Lista de nomes de containers, separados por espaço ou o coringa (*) para coletar as estatísticas de todos os containers em execução na máquina
ProcessNames = bash apache | [texto] Lista de nomes de processos do sistema operacional separados por espaço.
CPUStat = 1 | [boleano] Coleta estatísticas de uso de CPU quando o valor for igual a 1
CPUPathStat = /proc/stat | [texto] Caminho até o pseudo arquivo de CPU (de acordo com a versão do Linux do hospedeiro)
CPUPathArch = /proc/cpuinfo | [texto] Caminho até o pseudo arquivo arquitetura (de acordo com a versão do Linux do hospedeiro)
NetworkStat = 0 | [boleano] Coleta estatísticas de rede quando o seu valor for igual a 1
NetworkPathStat = /proc/net | [texto] Caminho até o pseudo arquivo rede (de acordo com a versão do Linux do hospedeiro)

Salvo onde explicitamente apontado nenhuma modificação nestas variáveis é necessária para a execução dos exemplos aqui descritos.

# Running the agent


````
#> ./imaiot &
````
