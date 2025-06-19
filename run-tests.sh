#!/bin/bash

echo "🔍 Rodando testes em subpastas..."
FAILED=0

for dir in tests/*/; do
  testname=$(basename "$dir")
  input="${dir}input.c"
  output="${dir}output.ts"
  expected="${dir}expected.ts"
  diffout="${dir}diff.txt"
  log="${dir}compiler.log"

  echo "🧪 Testando ${testname}..."

  # Executa o compilador, redirecionando stderr para log
  ./parser "$input" "$output" 2> "$log"

  # Se não houver expected, cria referência inicial e limpa diff/log
  if [ ! -f "$expected" ]; then
    echo "⚠️  expected.ts não encontrado. Criando referência inicial."
    cp "$output" "$expected"
    rm -f "$diffout" "$log"
    continue
  fi

  # Compara, ignorando espaços (-w) e linhas vazias (-B)
  if diff -B -w -q "$output" "$expected" > /dev/null; then
    echo "✅ ${testname} passou"
    rm -f "$diffout"                 # limpa diff antigo, se existir
  else
    echo "❌ ${testname} falhou"
    echo "🔎 Diferenças salvas em ${diffout}"
    diff -B -w "$output" "$expected" > "$diffout"
    FAILED=1
  fi
done

if [ $FAILED -eq 0 ]; then
  echo "🎉 Todos os testes passaram!"
else
  echo "⚠️  Alguns testes falharam. Veja os arquivos diff.txt e compiler.log em cada pasta."
fi