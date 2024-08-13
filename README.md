
# Gerenciador de Processos

Este projeto consiste na implementação de um escalonador preemptivo que realiza a seleção de processos utilizando quatro diferentes algoritmos: Alternância Circular (Round Robin), Escalonamento por Prioridade, Loteria, e Completely Fair Scheduler (CFS). Cada algoritmo possui características distintas e será implementado com base no comportamento esperado para simular um sistema operacional simples.




## 1. Alternância Circular

Este algoritmo seleciona processos de forma circular e equitativa. Cada processo recebe uma fatia de tempo (quantum) para ser executado. Se o processo não terminar dentro desse quantum, ele é colocado de volta no final da fila de pronto.


## 2. Prioridade

Any additional information goes here



## 3. Loteria

Este algoritmo seleciona os processos de maneira aleatória. Ele é baseado em uma loteria onde cada processo recebe uma quantidade específica de
bilhetes. Ao ter seu bilhete sorteado, o processo recebe o direito de utilizar a CPU conforme o tempo dispanibilizado para ele. Assim que o tempo
disponibilizado acabar, o processo sai da CPU e é realizado um novo sorteio. O algoritmo executa até não existir mais processos para serem sorteados.



## 4. Completely Fair Scheduler (CFS)

Any additional information goes here



## Dependências

- Compilador GCC (ou qualquer outro compilador C compatível)


## Instalação

Para executar o projeto basta rodar os seguintes comandos via terminal:

```bash
  gcc geradorEntrada.c -o geradorEntrada
  ./geradorEntrada
```
```bash
  gcc main.c -o main
  ./main
```

## Autores
- [@Beatriz-SMB](https://github.com/Beatriz-SMB)
- [@RyanBLeon28](https://github.com/RyanBLeon28)
- [@Vinimborges](https://github.com/Vinimborges)

