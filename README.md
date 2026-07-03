# Godot Perlin Noise GDExtension

[![Build Status](https://img.shields.io/github/actions/workflow/status/caiocesar06/godot-perlin-noise/build_addon.yml?branch=master&style=flat-square)](https://github.com/caiocesar06/godot-perlin-noise/actions)

Ferramenta de geração procedural baseada no algoritmo de Perlin Noise, desenvolvida nativamente em C++ via GDExtension para a Godot Engine 4. Este projeto integra paralelismo massivo em CPU utilizando OpenMP, focado em alta performance para geração de terrenos e texturas em tempo real.

Este repositório é fruto do projeto de Iniciação Científica (PIC1422-2025) do CEFET-MG, servindo também como módulo base para a geração procedural de fases do projeto "Formiguinha Matemática".

---

## 📦 Como Usar o Addon (Instalação Rápida)

**Público-alvo:** Equipe de extensão, designers e desenvolvedores de jogos que desejam utilizar o Perlin Noise sem compilar código em C++.

Graças ao nosso pipeline de CI/CD, os binários multiplataforma são gerados automaticamente. Para instalar o Addon no seu jogo:

1. Acesse a aba [Actions](../../actions) deste repositório.
2. Clique no fluxo de execução (Workflow) mais recente que possua um ícone verde de "Success".
3. Role até o final da página e, na seção **Artifacts**, baixe o `.zip` correspondente ao seu sistema operacional (Windows ou Linux).
4. Extraia o conteúdo baixado. Você verá uma pasta `addons/`.
5. Arraste a pasta `addons/` inteira para dentro do diretório raiz (`res://`) do seu projeto Godot.

O Godot detectará automaticamente a GDExtension.

### Exemplo de Uso (GDScript)

Uma vez instalado, a classe `PerlinNoise` estará disponível globalmente:

```gdscript
func _ready():
    # Inicializa o ruído com uma seed aleatória
    var mapa_ruido = PerlinNoise.new()

    # Configura os parâmetros fractais
    mapa_ruido.octaves = 6
    mapa_ruido.persistence = 0.5
    mapa_ruido.lacunarity = 2.0

    # Amostra um valor no espaço 2D
    var elevacao = mapa_ruido.sample(10.5, 20.1)
    print("Elevação: ", elevacao)

```

---

## 🛠️ Como Desenvolver e Compilar a Fonte

**Público-alvo:** Pesquisadores e engenheiros interessados em modificar a matemática do algoritmo ou a infraestrutura do C++.

### Pré-requisitos

* **Godot 4.x** (Executável do editor)
* **CMake** (Mínimo versão 3.16)
* Compilador C++ com suporte a **OpenMP** (GCC/MinGW em Windows, GCC nativo em Linux)
* **Python** (necessário para a compilação do `godot-cpp`)

### 1. Clonagem Estrita

A engine do Godot (o repositório `godot-cpp`) está linkada como um submódulo. É **obrigatório** usar a flag `--recursive` ao clonar o projeto para que a pasta do motor não venha vazia:

```bash
git clone --recursive https://github.com/caiocesar06/godot-perlin-noise.git
cd godot-perlin-noise

```

### 2. Geração e Compilação (CMake)

O script de construção (`CMakeLists.txt`) extrai as dependências do sistema e posiciona os artefatos nativamente na estrutura de Addon correta (`project/addons/perlin_noise/bin/`).

```bash
# Gera os arquivos de configuração (utilizando Ninja como gerador recomendado)
cmake -B build -G "Ninja"

# Inicia a compilação paralela do C++ e do godot-cpp
cmake --build build --config Release

```

Após o build, abra a pasta `project/` contida no repositório com o editor do Godot para acessar a cena de testes e o visualizador C++ isolado.

---

## 🏛️ Créditos e Licença

* **Pesquisador Principal:** Caio César Nascimento Silva
* **Orientação:** Prof. Luis Alberto D'Afonseca
* **Instituição:** CEFET-MG — Departamento de Matemática (NG)

Desenvolvido no escopo do Edital DPPG Nº 94/2025 — PIBIC FAPEMIG.
