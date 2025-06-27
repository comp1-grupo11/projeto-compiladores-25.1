type Ponto = {
  x: number;
  y: number;
};

function calcular(a: number, b: number): number {
  let soma: number = a + b;
  let sub: number = a - b;
  let mul: number = a * b;
  let div: number = a / b;
  let mod: number = a % b;

  soma += 1;
  sub -= 1;
  mul *= 2;
  div /= 2;

  let eq: boolean = a == b;
  let ne: boolean = a != b;
  let lt: boolean = a < b;
  let le: boolean = a <= b;
  let gt: boolean = a > b;
  let ge: boolean = a >= b;

  let e: boolean = a && b;
  let ou: boolean = a || b;
  let nao: boolean = !a;

  a++;
  b--;

  let p: Ponto = { x: soma, y: sub };

  let v: number[] = [0, 0, 0, 0];
  v[0] = p.x;
  v[1] = p.y;
  v[2] = v[0] + v[1];
  v[3] = (a != ++b || !(--v[2] < b)) as unknown as number;

  return v[2];
}
