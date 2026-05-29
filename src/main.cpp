#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using json = nlohmann::json;

const int TILE_SIZE = 32;

const float JOGADOR_W = 32.0f;
const float JOGADOR_H = 32.0f;

const float INIMIGO_W = 32.0f;
const float INIMIGO_H = 32.0f;

enum TipoTile
{
    VAZIO = 0,
    CHAO = 1,
    PLATAFORMA = 2,
    SPAWN_JOGADOR = 3,
    ESPINHO = 4,
    ANDARILHO = 5,
    VALKIRIA = 6,
    PORTA = 7
};

struct Entidade
{
    int tipo = 0;
    sf::Sprite sprite;
    sf::RectangleShape fallback;
    sf::FloatRect hitbox;
    bool usaTextura = false;
    bool desenhar = true;
};

bool carregarTextura(sf::Texture& textura, const std::string& caminho)
{
    if (!textura.loadFromFile(caminho))
    {
        std::cout << "Aviso: nao carregou textura: " << caminho << "\n";
        return false;
    }

    std::cout << "Textura carregada: " << caminho << "\n";
    return true;
}

void ajustarSpriteParaTamanho(sf::Sprite& sprite, const sf::Texture& textura, float largura, float altura)
{
    sf::Vector2u tam = textura.getSize();

    if (tam.x == 0 || tam.y == 0)
    {
        return;
    }

    sprite.setScale(largura / tam.x, altura / tam.y);
}

sf::Color corPorTipo(int tipo)
{
    switch (tipo)
    {
    case CHAO:
        return sf::Color(0, 180, 0, 80);          // chão, usado só no debug

    case PLATAFORMA:
        return sf::Color(0, 220, 255);            // plataforma

    case SPAWN_JOGADOR:
        return sf::Color::Yellow;                 // jogador

    case ESPINHO:
        return sf::Color::Red;                    // espinho

    case ANDARILHO:
        return sf::Color(255, 140, 0);            // andarilho

    case VALKIRIA:
        return sf::Color(180, 0, 255);            // valkiria

    case PORTA:
        return sf::Color::Magenta;                // porta

    default:
        return sf::Color::White;
    }
}

int main()
{
    std::ifstream arquivo("mapa1_floresta_gelo_32.json");

    if (!arquivo.is_open())
    {
        std::cout << "Erro: nao foi possivel abrir mapa1_floresta_gelo_32.json\n";
        return 1;
    }

    json mapa;
    arquivo >> mapa;
    arquivo.close();

    int larguraMapaTiles = mapa["width"];
    int alturaMapaTiles = mapa["height"];
    int tileW = mapa["tilewidth"];
    int tileH = mapa["tileheight"];

    int larguraMapaPixels = larguraMapaTiles * tileW;
    int alturaMapaPixels = alturaMapaTiles * tileH;

    std::cout << "Mapa carregado!\n";
    std::cout << "Tiles: " << larguraMapaTiles << " x " << alturaMapaTiles << "\n";
    std::cout << "Pixels: " << larguraMapaPixels << " x " << alturaMapaPixels << "\n";

    int firstgidMarcadores = 1;

    if (mapa.contains("tilesets"))
    {
        for (const auto& tileset : mapa["tilesets"])
        {
            if (tileset.contains("source"))
            {
                std::string source = tileset["source"];

                std::cout << "Tileset encontrado: " << source
                    << " firstgid=" << tileset["firstgid"] << "\n";

                if (source.find("Marcadores_32") != std::string::npos)
                {
                    firstgidMarcadores = tileset["firstgid"];
                    break;
                }
            }
        }
    }

    std::cout << "firstgid Marcadores_32 usado = " << firstgidMarcadores << "\n";

    sf::RenderWindow janela(sf::VideoMode(1280, 720), "Teste fase - Floresta de Gelo");
    janela.setFramerateLimit(60);

    sf::Texture texFundo;
    sf::Texture texPlataforma;
    sf::Texture texEspinho;
    sf::Texture texJogador;
    sf::Texture texAndarilho;
    sf::Texture texValkiria;
    sf::Texture texPorta;

    bool temFundo = carregarTextura(texFundo, "assets/chao.png");;

    bool temPlataforma = carregarTextura(texPlataforma, "assets/plataforma.png");
    bool temEspinho = carregarTextura(texEspinho, "assets/espinho.png");
    bool temJogador = carregarTextura(texJogador, "assets/jogador.png");
    bool temAndarilho = carregarTextura(texAndarilho, "assets/andarilho.png");
    bool temValkiria = carregarTextura(texValkiria, "assets/valkiria.png");
    bool temPorta = carregarTextura(texPorta, "assets/porta.png");

    sf::Sprite fundo;

    std::vector<Entidade> entidades;
    std::vector<sf::FloatRect> colisoes;
    std::vector<sf::FloatRect> espinhos;
    std::vector<sf::FloatRect> portas;

    sf::Vector2f spawnJogador(64.0f, 64.0f);

    int qtdChao = 0;
    int qtdPlataforma = 0;
    int qtdSpawn = 0;
    int qtdEspinho = 0;
    int qtdAndarilho = 0;
    int qtdValkiria = 0;
    int qtdPorta = 0;

    for (const auto& layer : mapa["layers"])
    {
        std::string tipoLayer = layer["type"];
        std::string nomeLayer = layer["name"];

        std::cout << "\nLendo camada: " << nomeLayer
            << " tipo=" << tipoLayer << "\n";

        if (tipoLayer == "imagelayer")
        {
            std::string caminho = layer["image"];

            if (carregarTextura(texFundo, caminho))
            {
                temFundo = true;
                fundo.setTexture(texFundo);
                fundo.setPosition(layer.value("x", 0.0f), layer.value("y", 0.0f));
            }
        }

        if (tipoLayer == "tilelayer")
        {
            const auto& data = layer["data"];

            for (int i = 0; i < data.size(); i++)
            {
                int gid = data[i];

                if (gid == 0)
                {
                    continue;
                }

                int valor = gid - firstgidMarcadores + 1;

                if (valor < 1 || valor > 7)
                {
                    std::cout << "Ignorando GID fora dos marcadores: gid="
                        << gid << " valor_logico=" << valor
                        << " camada=" << nomeLayer << "\n";
                    continue;
                }

                int coluna = i % larguraMapaTiles;
                int linha = i / larguraMapaTiles;

                float x = coluna * TILE_SIZE;
                float y = linha * TILE_SIZE;

                Entidade ent;
                ent.tipo = valor;

                float largura = TILE_SIZE;
                float altura = TILE_SIZE;

                if (valor == SPAWN_JOGADOR)
                {
                    largura = JOGADOR_W;
                    altura = JOGADOR_H;
                }
                else if (valor == ANDARILHO || valor == VALKIRIA)
                {
                    largura = INIMIGO_W;
                    altura = INIMIGO_H;
                }

                ent.hitbox = sf::FloatRect(x, y, largura, altura);

                ent.fallback.setPosition(x, y);
                ent.fallback.setSize(sf::Vector2f(largura, altura));
                ent.fallback.setFillColor(corPorTipo(valor));

                switch (valor)
                {
                case CHAO:
                {
                    qtdChao++;

                    // Chão é invisível visualmente.
                    // Ele só entra na colisão.
                    ent.desenhar = true;

                    colisoes.push_back(ent.hitbox);
                    entidades.push_back(ent);

                    break;
                }

                case PLATAFORMA:
                {
                    qtdPlataforma++;

                    if (temPlataforma)
                    {
                        ent.sprite.setTexture(texPlataforma);
                        ent.sprite.setPosition(x, y);
                        ajustarSpriteParaTamanho(ent.sprite, texPlataforma, 32.0f, 32.0f);
                        ent.usaTextura = true;
                    }

                    colisoes.push_back(ent.hitbox);
                    entidades.push_back(ent);

                    break;
                }

                case SPAWN_JOGADOR:
                {
                    qtdSpawn++;

                    // Se tiver mais de um spawn, ele usa o último encontrado.
                    spawnJogador = sf::Vector2f(x, y);

                    // O spawn não precisa ser desenhado.
                    ent.desenhar = false;

                    entidades.push_back(ent);

                    break;
                }

                case ESPINHO:
                {
                    qtdEspinho++;

                    if (temEspinho)
                    {
                        ent.sprite.setTexture(texEspinho);
                        ent.sprite.setPosition(x, y);
                        ajustarSpriteParaTamanho(ent.sprite, texEspinho, 32.0f, 32.0f);
                        ent.usaTextura = true;
                    }

                    espinhos.push_back(ent.hitbox);
                    entidades.push_back(ent);

                    break;
                }

                case ANDARILHO:
                {
                    qtdAndarilho++;

                    if (temAndarilho)
                    {
                        ent.sprite.setTexture(texAndarilho);
                        ent.sprite.setPosition(x, y);
                        ajustarSpriteParaTamanho(ent.sprite, texAndarilho, 64.0f, 64.0f);
                        ent.usaTextura = true;
                    }

                    entidades.push_back(ent);

                    break;
                }

                case VALKIRIA:
                {
                    qtdValkiria++;

                    if (temValkiria)
                    {
                        ent.sprite.setTexture(texValkiria);
                        ent.sprite.setPosition(x, y);
                        ajustarSpriteParaTamanho(ent.sprite, texValkiria, 64.0f, 64.0f);
                        ent.usaTextura = true;
                    }

                    entidades.push_back(ent);

                    break;
                }

                case PORTA:
                {
                    qtdPorta++;

                    if (temPorta)
                    {
                        ent.sprite.setTexture(texPorta);
                        ent.sprite.setPosition(x, y);
                        ajustarSpriteParaTamanho(ent.sprite, texPorta, 32.0f, 32.0f);
                        ent.usaTextura = true;
                    }

                    portas.push_back(ent.hitbox);
                    entidades.push_back(ent);

                    break;
                }

                default:
                {
                    entidades.push_back(ent);
                    break;
                }
                }
            }
        }
    }

    std::cout << "\n===== RESUMO DO MAPA =====\n";
    std::cout << "Chao: " << qtdChao << "\n";
    std::cout << "Plataformas: " << qtdPlataforma << "\n";
    std::cout << "Spawn jogador: " << qtdSpawn << "\n";
    std::cout << "Espinhos: " << qtdEspinho << "\n";
    std::cout << "Andarilhos: " << qtdAndarilho << "\n";
    std::cout << "Valkirias: " << qtdValkiria << "\n";
    std::cout << "Portas: " << qtdPorta << "\n";
    std::cout << "Entidades desenhaveis/logicas: " << entidades.size() << "\n";
    std::cout << "Colisoes: " << colisoes.size() << "\n";
    std::cout << "Spawn final jogador: " << spawnJogador.x << ", " << spawnJogador.y << "\n";
    std::cout << "==========================\n";

    sf::RectangleShape jogadorFallback;
    jogadorFallback.setSize(sf::Vector2f(JOGADOR_W, JOGADOR_H));
    jogadorFallback.setFillColor(sf::Color::Yellow);

    sf::Sprite jogadorSprite;
    bool jogadorUsaTextura = false;

    if (temJogador)
    {
        jogadorSprite.setTexture(texJogador);
        ajustarSpriteParaTamanho(jogadorSprite, texJogador, JOGADOR_W, JOGADOR_H);
        jogadorUsaTextura = true;
    }

    sf::Vector2f posJogador(spawnJogador.x, spawnJogador.y);
    sf::Vector2f velJogador(0.0f, 0.0f);

    const float velocidade = 260.0f;
    const float gravidade = 1600.0f;
    const float forcaPulo = -600.0f;

    bool noChao = false;
    bool mostrarDebug = true;

    sf::Clock clock;

    sf::View camera;
    camera.setSize(1280.0f, 720.0f);
    camera.setCenter(posJogador);

    while (janela.isOpen())
    {
        float dt = clock.restart().asSeconds();

        sf::Event evento;

        while (janela.pollEvent(evento))
        {
            if (evento.type == sf::Event::Closed)
            {
                janela.close();
            }

            if (evento.type == sf::Event::KeyPressed)
            {
                if (evento.key.code == sf::Keyboard::F1)
                {
                    mostrarDebug = !mostrarDebug;
                }

                if (evento.key.code == sf::Keyboard::Space && noChao)
                {
                    velJogador.y = forcaPulo;
                    noChao = false;
                }
            }
        }

        velJogador.x = 0.0f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            velJogador.x = -velocidade;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            velJogador.x = velocidade;
        }

        velJogador.y += gravidade * dt;

        sf::FloatRect boundsJogador(posJogador.x, posJogador.y, JOGADOR_W, JOGADOR_H);

        // Movimento horizontal
        posJogador.x += velJogador.x * dt;
        boundsJogador = sf::FloatRect(posJogador.x, posJogador.y, JOGADOR_W, JOGADOR_H);

        for (const auto& col : colisoes)
        {
            if (boundsJogador.intersects(col))
            {
                if (velJogador.x > 0.0f)
                {
                    posJogador.x = col.left - JOGADOR_W;
                }
                else if (velJogador.x < 0.0f)
                {
                    posJogador.x = col.left + col.width;
                }

                boundsJogador = sf::FloatRect(posJogador.x, posJogador.y, JOGADOR_W, JOGADOR_H);
            }
        }

        // Movimento vertical
        posJogador.y += velJogador.y * dt;
        boundsJogador = sf::FloatRect(posJogador.x, posJogador.y, JOGADOR_W, JOGADOR_H);

        noChao = false;

        for (const auto& col : colisoes)
        {
            if (boundsJogador.intersects(col))
            {
                if (velJogador.y > 0.0f)
                {
                    posJogador.y = col.top - JOGADOR_H;
                    velJogador.y = 0.0f;
                    noChao = true;
                }
                else if (velJogador.y < 0.0f)
                {
                    posJogador.y = col.top + col.height;
                    velJogador.y = 0.0f;
                }

                boundsJogador = sf::FloatRect(posJogador.x, posJogador.y, JOGADOR_W, JOGADOR_H);
            }
        }

        for (const auto& esp : espinhos)
        {
            if (boundsJogador.intersects(esp))
            {
                std::cout << "Tomou dano no espinho!\n";
            }
        }

        for (const auto& porta : portas)
        {
            if (boundsJogador.intersects(porta))
            {
                std::cout << "Entrou na porta da proxima fase!\n";
            }
        }

        if (posJogador.y > alturaMapaPixels + 300)
        {
            posJogador = spawnJogador;
            velJogador = sf::Vector2f(0.0f, 0.0f);
        }

        jogadorFallback.setPosition(posJogador);

        if (jogadorUsaTextura)
        {
            jogadorSprite.setPosition(posJogador);
        }

        sf::Vector2f centro(
            posJogador.x + JOGADOR_W / 2.0f,
            posJogador.y + JOGADOR_H / 2.0f
        );

        float metadeW = camera.getSize().x / 2.0f;
        float metadeH = camera.getSize().y / 2.0f;

        if (centro.x < metadeW) centro.x = metadeW;
        if (centro.y < metadeH) centro.y = metadeH;
        if (centro.x > larguraMapaPixels - metadeW) centro.x = larguraMapaPixels - metadeW;
        if (centro.y > alturaMapaPixels - metadeH) centro.y = alturaMapaPixels - metadeH;

        camera.setCenter(centro);

        janela.clear();
        janela.setView(camera);

        if (temFundo)
        {
            janela.draw(fundo);
        }

        for (const auto& ent : entidades)
        {
            if (ent.desenhar)
            {
                if (ent.usaTextura)
                {
                    janela.draw(ent.sprite);
                }
                else
                {
                    janela.draw(ent.fallback);
                }
            }

            if (mostrarDebug)
            {
                sf::RectangleShape debug;
                debug.setPosition(ent.hitbox.left, ent.hitbox.top);
                debug.setSize(sf::Vector2f(ent.hitbox.width, ent.hitbox.height));
                debug.setFillColor(sf::Color::Transparent);

                if (ent.tipo == CHAO)
                {
                    debug.setOutlineColor(sf::Color(0, 255, 0, 180));
                }
                else
                {
                    debug.setOutlineColor(sf::Color::White);
                }

                debug.setOutlineThickness(1.0f);
                janela.draw(debug);
            }
        }

        if (jogadorUsaTextura)
        {
            janela.draw(jogadorSprite);
        }
        else
        {
            janela.draw(jogadorFallback);
        }

        if (mostrarDebug)
        {
            sf::RectangleShape debugJogador;
            debugJogador.setPosition(posJogador);
            debugJogador.setSize(sf::Vector2f(JOGADOR_W, JOGADOR_H));
            debugJogador.setFillColor(sf::Color::Transparent);
            debugJogador.setOutlineColor(sf::Color::Yellow);
            debugJogador.setOutlineThickness(2.0f);
            janela.draw(debugJogador);
        }

        janela.display();
    }

    return 0;
}