function soma(a: number, b: number): number {
  return a + b;
}

function main(): number {
  let resultado: number = soma(10, 20);

  if (resultado > 15) {
    resultado = resultado * 2;
  } else {
    resultado = resultado - 5;
  }

  for (let i: number = 0; i < 3; i = i + 1) {
    resultado = resultado + i;
  }

  return resultado;
}
