type Pessoa = {
  id: number;
  nome: string;
  altura: number;
};

function calcular_idade(ano_nascimento: number): number {
  const ano_atual = 2024;
  return ano_atual - ano_nascimento;
}

let i: number;

function main(): number {
  const MAX_SIZE = 100;
  let numeros: number[] = [1, 2, 3, 4, 5];
  let valores: number[] = [1.5, 2.7, 3.14];
  let letra: string = "A";
  let mensagem: string = "Teste de compilacao";
  let pi: number = 3.14159265359;

  let usuario: Pessoa = {
    id: 123,
    nome: "Joao",
    altura: 1.75,
  };

  for (let i = 0; i < 5; i++) {
    if (numeros[i] % 2 === 0) {
      console.log(`Par: ${numeros[i]}`);
    } else {
      console.log(`Impar: ${numeros[i]}`);
    }
  }

  let idade: number = calcular_idade(1990);
  switch (idade) {
    case 34:
      console.log("Idade correta!");
      break;
    default:
      console.log(`Idade calculada: ${idade}`);
  }

  let x: number = 10;
  x += 5;
  x--;

  return 0;
}
