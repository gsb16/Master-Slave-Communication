# Trabalho 1 - Comunicação com RawSocket

## Autores:
- Gabriel de Souza Barreto
- Guilherme Bastos de Oliveira

## Objetivo:
Trabalho para a disciplina de Redes 1
- Comunicação simples entre mestre e escravo

## Uso:
### Execução:
##### Mestre:
```
	sudo ./mestre <dispositivo>
```

##### Escravo:
```
	sudo ./escravo <dispositivo>
```
Onde *dispositivo* é o nome do dispositivo de rede no qual o programa deve atuar.

### Comandos:
- *ls*: pode receber até dois argumentos (-l e/ou -a)
- *cd*: recebe um argumento que é o nome do diretório a ser visitado
- *get*: recebe um argumento referente ao arquivo que deverá ser baixado do escravo
- *put*: recebe um argumento referente ao arquivo que deverá ser enviado ao escravo

### Troca de contexto:
Para alterar entre o contexto 'local' e o 'remoto' para os comandos 'ls' e 'cd' basta entrar com 'R' para remoto ou 'L' para local e pressionar 'enter'.
Os comandos 'put' e 'get' funcionam independente de contexto.

## Makefile:
- *make:* compila o programa
- *make clean:* deleta arquivos temporários de compilação
- *make purge:* executa clean e remove executavel
- *make remake:* executa purge e compila

## Decisões de Projeto
- __Preâmbulo__: Se o primeiro byte do buffer não é o preâmbulo esperado, descarta toda a mensagem de dentro do buffer.
- __Padding__: Para enviar mensagens com tamanho de 64 bytes (mínimo para funcionamento seguro de envio/recebimento pelas placas de rede, como visto em sala) foi adicionado no vetor de dados 29 bytes, porém não é utilizado em momento algum.
- __Para e Espera__: O controle de fluxo utilizado é o "para e espera".

## Mensagem
```
[pream|tam|seq|tipo|dados|parid]
|      |   |   |    |     |
|      |   |   |    |     |
|      |   |   |    |     {1 byte - cobre tam+seq+tipo+dados}
|      |   |   |    {0..31 bytes - sempre completa para 31 bytes,
|	   |   |   |	 29 bytes de padding}
|      |   |   {5 bits - Ver tabela}
|      |   {6 bits - timeout}
|      {5 bits - 0..31 bytes}
{1 byte - 0111 1110}
```

## Tabela de tipos
```
0 - ACK     5 - vazio   A - fim         
1 - vazio   6 - cd      B - vazio (quit)
2 - tamanho 7 - ls      C - mostra na tela
3 - OK      8 - get     D - dados
4 - vazio   9 - put     E - erro
                        F - NACK
```

## Bugs conhecidos:
- __Transmissão corrompida:__ em alguns arquivos podem existir uma sequência de bits que é ignorada, possivelmente por uma interpreção da placa de rede como alguma comunicação especial.
- __Retirada do cabo:__ em momentos específicos, pelos testes quando o cabo é removido/inserido durante uma tentativa de conexão pelo SO, a conexão não é recuperada corretamente gerando timeout.
