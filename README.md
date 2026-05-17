# Description of datasets for the basic algorithm

| Range         | Registers        | Ideal allocation | Allocation        |
| ------------- | ---------------- | ---------------- | ----------------- |
| `ranges1.txt` | `registers2.txt` | 2 registers      | `allocation1.txt` |
| `ranges2.txt` | `registers2.txt` | 2 registers      | `allocation2.txt` |
| `ranges3.txt` | `registers2.txt` | 2 registers      | `allocation3.txt` |
| `ranges4.txt` | `registers1.txt` | 1 register       | `allocation4.txt` |
| `ranges5.txt` | `registers1.txt` | 1 register       | `allocation5.txt` |
| `ranges6.txt` | `registers3.txt` | 3 registers      | `allocation6.txt` |

## Output

The `output` folder contains the solutions produced by the implemented algorithm for each input instance.

It is important to note that:
- The naming of webs in the output files can differ from the ones in your implementation.
- The specific registers assigned to each web may also vary.

These differences are expected, as multiple valid allocations can exist for the same problem, as long as they respect the register constraints and produce a correct allocation.

# Register Allocation Tool
- Este projeto implementa uma ferramenta de alocação de registos para 
compiladores. Dado um ficheiro com os live ranges das variáveis de um
programa e um ficheiro de configuração com o número de registos disponíveis
e o algoritmo a usar, a ferramenta constrói as webs, o grafo de 
interferência e aplica um algoritmo de graph coloring para determinar a 
alocação ótima de registos. O programa pode ser executado em dois modos: 
→ Modo interativo: menu com opções 
→ Modo batch: ./myProg -b ranges.txt registers.txt allocation.txt

- Para a realização deste projeto implementamos 4 algoritmos, nomeadamente o Basic Register Allocation, Register Allocation com Web Spilling, Register Allocation com Web Splitting e Register Allocation com DSATUR.

### Basic Register Allocation
- O objetivo do Basic Register Allocation é colorir o grafo de interferência com N cores (registos), minimizando o número de cores usadas.
- Na primeira fase, a Simplificação, são removidos nós com grau menor que N para uma stack. Se nenhum nó tiver grau menor que N, o nó com maior grau é adicionado à stack e tenta ser colorido na fase seguinte.
- Na segunda fase, o Coloring, os nós são desempilhados um a um e é atribuído a cada um a cor mais baixa disponível que não esteja nos seus vizinhos. Se no final houver algum nó que não conseguiu ser colorido, o algoritmo falha completamente e todas as webs vão para memória, o número de registos no output é 0 e todas as webs ficam com M. Um warning é também emitido a indicar que a alocação falhou.
- Complexidade: O(W x E)

### Register Allocation com Web Spilling
- O Spilling é utilizado quando o Basic Allocation falha com N registos. Permite mandar até K webs para memória para simplificar o grafo e tentar colorir o resto, onde K é o valor fornecido no ficheiro de configuração (algorithm: spilling, K).
- O algoritmo repete até K tentativas. Em cada tentativa constrói o grafo apenas com as webs não spilled e corre o Basic Allocation.Caso todas as webs tenham sido coloridas, o algoritmo termina com sucesso. Se não, escolhe a web com maior grau e força o seu spill, tentando de novo. Começa sempre com 1 spill, só aumentando se necessário.
- Escolha da web para dar spill: a nossa escolha recaiu sobre a web com maior grau no grafo, ou seja, a que tem mais interferências com outras webs. Ao remover a web mais problemática, libertamos o maior número de interferências de uma só vez, maximizando a simplificação do grafo e aumentando a probabilidade de sucesso na tentativa seguinte. Só são consideradas as webs não coloridas, pois as já coloridas já têm registo atribuído.
- Se após K tentativas o Basic ainda falha, todas as webs vão para memória.
- Complexidade: O(K x (W² x P² + W x E)) , onde K número máximo de spills, W número de webs, E número de interferências,P máximo de program points por web.

### Register Allocation com Web Splitting
- O Splitting é utilizado quando o Basic Allocation falha com N registos. Em vez de mandar webs para memória como o Spilling, parte até K webs em duas metades para reduzir interferências e tentar colorir, onde K é o valor fornecido no ficheiro registers.txt (algorithm: splitting, K).
- O algoritmo repete até K tentativas. Em cada tentativa corre o Basic Allocation com todas as webs, incluindo as já divididas. Caso todas as webs tenham sido coloridas, o algoritmo termina com sucesso.  Se não, para cada web não colorida testa todos os pontos de corte possíveis, escolhe o que minimiza max(degA, degB) e parte essa web em duas metades.
- Escolha do ponto de corte: para cada web não colorida, testamos todos os pontos de corte possíveis. Para cada corte calculamos o grau de cada metade, degA para a metade A e degB para a metade B, e escolhemos o corte que minimiza max(degA, degB). Desta forma garantimos que ambas as metades ficam com o menor número de interferências possível, aumentando a probabilidade de ambas serem coloridas com N registos.
- Se após K tentativas o Basic ainda falha, todas as webs vão para memória. Note-se que o Splitting não garante sucesso, mesmo partindo K webs pode não ser suficiente para colorir com N registos.
- Complexidade: O(K x (W² x P² + W x P x deg)), onde K número máximo de splits, W número de webs, E número de interferências, P máximo de program points por web, deg = grau máximo do graph.

### Register Allocation com DSATUR
- Para o algoritmo livre escolhemos o DSATUR (Degree of SATURation). Ao contrário do Basic que usa uma stack e duas fases com ordenação estática por grau, o DSATUR num só loop escolhe dinamicamente em cada passo o nó com maior saturação, ou seja, o nó cujos vizinhos já têm o maior número de cores distintas atribuídas.
- Implementámos três heurísticas de seleção. O critério principal é a saturação, os nós mais constrangidos são coloridos primeiro. O primeiro tie-breaker é o grau, em empate de saturação, preferimos o nó com maior grau total. O segundo tie-breaker são os nós de articulação, em empate de saturação e grau, preferimos nós cuja remoção desconecta o grafo, por serem estruturalmente mais críticos.
- Em cada passo, o nó selecionado recebe a cor mais baixa disponível que não esteja nos seus vizinhos. Se não houver cor disponível, apenas esse nó falha, ao contrário do Basic que falha tudo de uma vez, o DSATUR só faz spill ao nó que realmente não tem cor disponível, mantendo os restantes com os seus registos.
- Complexidade: O(W² + W x N + E), onde V é o número de webs e E o número de interferências.
