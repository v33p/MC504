# Task 2 - Makefile
# 01/06/2015
# Lucas Padilha - 119785 | Pedro Tadahiro - 103797
# Makefile compila todo nosso projeto de forma simples usando o gcc.
# Podemos adicionar testes aqui no makefile tambem.

# TODO: Melhorar esse codigo de make
mkfs: src/mkfs.c src/mkfs.h
	gcc src/mkfs.c -o mkfs
clean: mkfs
	rm mkfs
