PROJETO TILED - FLORESTA DE GELO 32x32

Abra no Tiled:
  projeto_floresta_gelo.tiled-project

Depois abra:
  mapa1_floresta_gelo_32.tmx
ou:
  mapa1_floresta_gelo_32.tmj

CONFIGURACAO DO MAPA
  Imagem de fundo: 1920x1080
  Tile/grid: 32x32
  Mapa logico: 60 colunas x 34 linhas
  Altura logica: 1088 px, ou seja, 8 px acima de 1080. Isso e normal.

VALORES DOS MARCADORES
  0 = vazio / nao pintado
  1 = chao / colisao solida
  2 = plataforma de gelo
  3 = spawn do jogador
  4 = espinho venenoso
  5 = spawn Andarilho
  6 = spawn Valkiria
  7 = porta / proxima fase

COMO USAR
  1. Selecione a camada que deseja editar.
  2. Selecione o tileset Marcadores_32.
  3. Clique no marcador desejado.
  4. Use o pincel, atalho B, para pintar.
  5. Use a borracha, atalho E, para apagar e voltar para 0.

CAMADAS
  Fundo_Mapa:
    Imagem do cenario. Esta travada para evitar mover sem querer.

  Mapa_Logico:
    Camada unica para usar no switch case, caso queira tudo em uma matriz so.

  Chao_Invisivel:
    Use para colisao do chao. Pode deixar com opacidade baixa ou esconder pelo olhinho.

  Plataforma_Visivel:
    Use para marcar plataformas que voce quer desenhar/controlar separadamente.

  Espinho_Visivel:
    Use para posicionar espinhos venenosos.

  Entidades_Logicas:
    Use para spawn do jogador, inimigos e porta.

DICA
  Se quiser carregar no C++ com switch case simples, use apenas a camada Mapa_Logico.
  Se quiser organizar melhor, use as camadas separadas e leia cada uma no codigo.
