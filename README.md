
# Gerenciador de Processos

Este projeto consiste na implementação de um escalonador preemptivo que realiza a seleção de processos utilizando quatro diferentes algoritmos: Alternância Circular (Round Robin), Escalonamento por Prioridade, Loteria, e Completely Fair Scheduler (CFS). Cada algoritmo possui características distintas e será implementado com base no comportamento esperado para simular um sistema operacional simples.




## 1. Algoritmo Ótimo
O algoritmos de substituição de páginas ótimo busca minimizar o número de faltas de página, substituindo a página que será acessada mais tarde no futuro em comparação com as outras páginas que já estão na memória. Ele faz isso com base no conhecimento dos acessos futuros, algo que, na prática, é impossível de se prever com precisão, o que torna esse algoritmo teórico.



## 2. Prioridade

O algoritmo do escalonador por prioridade seleciona o processo que possui a maior prioridade (maior número). Após uma execução (um cloack) do processo selecionado, anteriormente, o algoritmo seleciona mais uma vez o processo de maior prioridade. Quando um processo finaliza sua execução, ele tem sua prioridade é zerada. Esse procedimeto se repete para todos os processos. Depois que todos os processos foram executados, o usuário tem a opção de encerrar o algoritmo (digitando 's') ou passar mais um processo (seguindo o padrão: Nome do processo|id|prioridade|clock). A inserção de novos processos pode ser realizada durante a execução do programa (seguindo o mesmo padrão). 



## 3. Loteria

Este algoritmo seleciona os processos de maneira aleatória. Ele é baseado em uma loteria onde cada processo recebe uma quantidade específica de
bilhetes. Ao ter seu bilhete sorteado, o processo recebe o direito de utilizar a CPU conforme o tempo dispanibilizado para ele. Assim que o tempo
disponibilizado acabar, o processo sai da CPU e é realizado um novo sorteio. O algoritmo executa até não existir mais processos para serem sorteados.



## 4. Completely Fair Scheduler (CFS)

Este escalonador usa uma Red-Black Tree para balancear qual processo usará a CPU de acordo com o seu clock. A cada vez que o processo passa pela CPU é diminuido o seu tempo necessário para conclusão do processo menos o tempo que ficou processando. Caso o valor seja menor que o tempo dado pela CPU, esse processo é removido da árvore(lista de processos) e novamente a árvore é balanceada.
Como o algoritmo é preemptivo, pode ser feita a inserção de um novo processo. Dessa forma, quando é inserido um novo processo, é usado um mutex, para parar a thread que está executando os processos e esse novo processo é inserido e a árvore é novamente balanceada.


## Dependências

- Compilador GCC (ou qualquer outro compilador C compatível)


## Instalação

Para executar o projeto basta rodar os seguintes comandos via terminal:

```bash
  gcc geradorEntrada.c -o geradorEntrada
  ./geradorEntrada
```
```bash
  gcc main.c -o main escalonadorAlternanciaCircular.c escalonadorCFS.c escalonadorLoteria.c escalonadorPrioridade.c `pkg-config --cflags --libs glib-2.0`
  ./main
```

## Autores
- [@Beatriz-SMB](https://github.com/Beatriz-SMB)
- [@Knoshz](https://github.com/Knoshz)
- [@RyanBLeon28](https://github.com/RyanBLeon28)
- [@Vinimborges](https://github.com/Vinimborges)

