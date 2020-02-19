# IOTMA - IoT Mananger Agent
IOTMA is an agent to mananger and monitoring a machine (real or virtual).

# About

The IoTMA is an IoT agent for infrastructure management, developed in the context of the SWAMP project. It works publishing its metrics in an Orion Context Broker. 

# Requiriments

Linux OS.


# Getting IoTMA from source

Clonning repository:

``` 
#>git clone https://github.com/heideker/iotma
```

## Compilling 

To compile source code you must have the following packages:

````
#> apt-get install make gcc libcurl4-openssl-dev g++ libjsoncpp-dev
````

Enter in IoTMA directory and use the make command to compile source code:

````
#> cd iotma
#iotma> make all
````


## Configuring

There are many configuration keys, but main we need to set the following keys: OrionURL, FiwareService, FiwareServicepath, and NodeUUID. 
To this, there are two forms: by iotma.conf file or by command line - Example:

````
#>./iotma --OrionURL=http://imaiot.org:1026 --FiwareService=openiot --FiwareServicepath=/ --NodeUUID=urn:ngsi-ld:TestNodeIOTMA
````

You can use an iotma.conf file to set these keys, as others available. Even though you use iotma.conf file, the keys set by command line will overwrite the value set in iotma.conf file.

The execution of IoTMA must be in root privilege because its access protected areas os operation system. Use the root user or sudo command. 

The complete list of available keys is shown above:

Key |	Description
---------|-------------
debugMode = 0 | [bolean] Set the debug-mode of IoTMA with a verbose operation
NodeUUID = urn:ngsi-ld:999 | [text] This key informs the unique identifier of the node in Orion. If this entity's Id not found the Orion, the IoTMA will create a new entity.

OrionHost = http://hostname | [texto] URL (hostname ou endereço IP) para o context broker
OrionPort = 1026 | [inteiro] Número da porta para o context broker
OrionPublisherTime = 30 | [inteiro] Intervalo entre as publicações de métricas no context broker, em segundos
CPUPathStat = /proc/stat | [texto] Caminho até o pseudo arquivo de CPU (de acordo com a versão do Linux do hospedeiro)
CPUPathArch = /proc/cpuinfo | [texto] Caminho até o pseudo arquivo arquitetura (de acordo com a versão do Linux do hospedeiro)
NetworkPathStat = /proc/net | [texto] Caminho até o pseudo arquivo rede (de acordo com a versão do Linux do hospedeiro)


# Running the agent


````
#> ./iotma &
````
