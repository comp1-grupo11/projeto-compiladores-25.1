function somar(a: number, b: number): number {
  return a + b;
}

function main(): number {
  let x: number = 10;
  let y: number = 20;
  let resultado: number = somar(x, y);

  if (resultado > 20) {
    resultado += 5;
  } else {
    resultado -= 2;
  }

  for (let i = 0; i < 3; i++) {
    resultado = resultado + i;
  }

  while (x > 0) {
    x--;
  }

  let opcao: number = 2;
  switch (opcao) {
    case 1:
      resultado = 100;
      break;
    case 2:
      resultado = 200;
      break;
    default:
      resultado = 0;
  }

  let a: number = 5;
  let b: number = 10;
  let maior: number = a > b ? a : b;

  return 0;
}
