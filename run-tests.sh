#!/bin/bash

echo "🔍 Rodando testes em subpastas..."
FAILED=0


# Limpa arquivos de saída antigos antes de rodar os testes
find tests/ -type f -name 'output.ts' -delete

for dir in tests/*/; do
  testname=$(basename "$dir")
  input="${dir}input.c"
  output="${dir}output.ts"
  expected="${dir}expected.ts"
  diffout="${dir}diff.txt"
  log="${dir}compiler.log"

  echo "🧪 Testando ${testname}..."

  # Executa o compilador, redirecionando stderr para log
  ./typec "$input" "$output" 2> "$log"

  # Se não gerou output.ts ou está vazio, falha (erro léxico ou sintático)
  if [ ! -f "$output" ] || [ ! -s "$output" ]; then
    echo "❌ ${testname} falhou (erro léxico/sintático)"
    FAILED=1
    continue
  fi

  # Se não houver expected.ts, cria referência inicial e considera sucesso
  if [ ! -f "$expected" ]; then
    echo "⚠️  expected.ts não encontrado. Criando referência inicial."
    cp "$output" "$expected"
    rm -f "$diffout" "$log"
    echo "✅ ${testname} passou (primeira execução, referência criada)"
    continue
  fi

  # Compara output.ts e expected.ts
  if diff -B -w -q "$output" "$expected" > /dev/null; then
    echo "✅ ${testname} passou"
    rm -f "$diffout"
  else
    echo "❌ ${testname} falhou (diferença encontrada)"
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