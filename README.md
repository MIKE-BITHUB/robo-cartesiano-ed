# 🤖 Navegação Robótica - TAD Lista em C

Implementação de um sistema completo de simulação, análise e otimização de 
trajetórias de um robô autônomo em um plano cartesiano, desenvolvido como 
Trabalho Prático da disciplina de Estrutura de Dados (UCB - 1º Semestre de 2026).

## 📋 Sobre o projeto

O robô se move em um plano cartesiano a partir de uma posição inicial, 
seguindo comandos unitários:
- `N` - Norte (cima)
- `S` - Sul (baixo)
- `L` - Leste (direita)
- `O` - Oeste (esquerda)

O sistema armazena o histórico de posições visitadas e as instruções 
recebidas utilizando **listas encadeadas com ponteiros**, sem uso de vetores.

## ⚙️ Funcionalidades implementadas

- `InicializarCaminho` - cria o caminho a partir de uma sequência de comandos
- `DestruirCaminho` - libera toda a memória alocada
- `DeterminarFim` - calcula a posição final do robô
- `HistoricoPosicoes` - retorna o histórico de posições visitadas
- `CalcularDistanciaTotal` - distância total percorrida
- `CalcularDistanciaGeometrica` - distância euclidiana (linha reta)
- `CalcularDistanciaManhattan` - distância de Manhattan (grade)
- `ContarInstrucoes` - número total de comandos executados

## 🛠️ Como compilar e executar

```bash
gcc 25200292.c main.c -o robo.exe
./robo.exe
```

## 👤 Autor

João Gabriel - Matrícula 25200292
