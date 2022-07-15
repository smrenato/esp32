## Repositorio com exemplos de codigos para a diciplina de microcontroladores

### Os exemplos de codigos sao para os micro controladores eps32/esp8266

### Para executar os exemplos primeiro instale o IDF e faça o setup do ambiente

[ESP IDF tutorial de instalção](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)

#### Altere o arquivo makefile, para o nome do codigo na pasta 

----
#### Apos a instalação é ó seguir os passos
##### Compilando e carregando o codigo para esp32 no Linux

1. get_idf 
2. idf.py build
3. idf.py -p /dev/ttyUSB0 flash
4. idf.py -p /dev/ttyUSB0 monitor