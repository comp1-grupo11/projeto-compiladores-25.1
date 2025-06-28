type Point = {
  x: number;
  y: number;
};

function main(): number {
  // Chamada de função básica
  console.log("Iniciando teste da AST\n");

  // Declarações e operadores
  let i: number = 0;
  let f: number = 3.14;
  let c: string = "A";
  i += 2;
  i++;
  i *= 3;
  f /= 2.5;

  // Estrutura condicional complexa
  if (i > 5 && f < 10.0) {
    console.log("Condição complexa satisfeita\n");
  } else {
    console.log("Condição não satisfeita\n");
  }

  // Loop com switch aninhado
  for (let j: number = 0; j < 15; j = j + 1) {
    switch (j % 5) {
      case 0:
        console.log("Múltiplo de 5");
        break;
      case 1:
      case 2:
        console.log("Resíduo baixo");
        break;
      default:
        console.log("Outro resíduo");
    }
  }

  // Uso de struct
  let p1: Point = { x: 0, y: 0 };
  p1.x = 10;
  p1.y = i * 2;

  // Operações com struct
  let p2: Point = { x: p1.y, y: p1.x };

  console.log(`Ponto final: (${p2.x}, ${p2.y})`);

  return 0;
}
