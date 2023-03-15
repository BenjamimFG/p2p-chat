Português | [English](README.md)
<hr />  

# P2P Chat

Um simples chat peer-to-peer na linha de comando Linux que fiz para praticar programação de sockets após ler o [Guia Beej's Para Programação em Rede](https://beej.us/guide/bgnet/translations/bgnet_ptbr.html).

Esta aplicação permite que 2 usuários conversem entre si diretamente na linha de comando.

# Dependências
* NCursesW
* Pthreads

# Build
Com as [dependências](#dependências) instaladas e no diretório raiz do projeto use o comando:
```bash
make
```

# Rodando a aplicação

Para rodar a aplicação utilize este comando no mesmo diretório que o executável `p2p_chat` está localizado:
```bash
./p2p_chat PORT USERNAME
```
Onde `PORT` é um número entre 0 e 65535 da porta que será associada ao socket do programa. Será usada por um peer quando se conectar a você.  
`USERNAME` é o nome que será exibido ao seu peer na sala de chat.