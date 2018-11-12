let f1: int => int = x => x + 1;
assert(1->f1 === 2);
assert(1->f1->f1 === 3);

let f2: (int, int) => int = (a, b) => a + b;
assert(1->f2(1) === 2);
assert(1->f2(1)->f2(1) === 3);

let f3: (int, ~b: int, ~c: int) => int = (a, ~b, ~c) => a + b + c;
assert(1->f3(~b=2, ~c=3) === 6);
assert(1->f3(~b=2, ~c=3)->f3(~b=2, ~c=3) === 11);

let f4: (int, int) => int = (a, b) => a + b;
assert(1->f4(1->f4(2)) === 4);
assert(1->f4(1->f4(2)->f4(2)) === 6);

let f5: (int, int, int) => int = (a, b, c) => a + b + c;
assert(1->f5(2, 1->f5(2, 3)) === 9);
assert(1->f5(2, 1->f5(2, 3))->f5(2, 3) === 14);

let () = {
  let x: option(int) = 1->Some;
  switch (x) {
  | Some(1) => assert(true)
  | _ => assert(false)
  };
};

let () = {
  let x: option(option(int)) = 1->Some->Some;
  switch (x) {
  | Some(Some(1)) => assert(true)
  | _ => assert(false)
  };
};

let x: int = (1, 2)->(((a, b)) => a + b);
assert(x === 3);

/* Tuple dispatch */
let () = {
  let f: int => int = x => x + 1;

  let x: (int, int, int) = 1->(f, f, f);
  switch (x) {
  | (2, 2, 2) => assert(true)
  | _ => assert(false)
  };
};

let () = {
  let f: (int, ~a: int, ~b: int) => int = (x, ~a, ~b) => x + a + b;

  let x: (int, int, int) =
    1->(f(~a=2, ~b=3), f(~a=2, ~b=3), f(~a=2, ~b=3));

  switch (x) {
  | (6, 6, 6) => assert(true)
  | _ => assert(false)
  };
};

let () = {
  let x: (option(int), option(int), option(int)) = 1->(Some, Some, Some);

  switch (x) {
  | (Some(1), Some(1), Some(1)) => assert(true)
  | _ => assert(false)
  };
};

let () = {
  let x =
    (1, 2)->(((a, b)) => a + b, ((a, b)) => a + b, ((a, b)) => a + b);

  switch (x) {
  | (3, 3, 3) => assert(true)
  | _ => assert(false)
  };
};

let fn1 = (~foo=?, ()) =>
  1
  + (
    switch (foo) {
    | Some(x) => x
    | None => 2
    }
  );
let fn2 = (~bar=?, x) => {
  let bar =
    switch (bar) {
    | Some(bar) => bar
    | None => 4
    };
  2 + bar + x;
};

assert(fn1(~foo=1, ())->(fn2(~bar=2)) === 6);

let () = print_endline("Fast pipe tests passed!");
