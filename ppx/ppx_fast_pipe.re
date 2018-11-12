open Migrate_parsetree;
open OCaml_406.Ast;

let mapper = {
  ...Ast_mapper.default_mapper,
  expr: (self, e) =>
    switch (e.pexp_desc) {
    | Pexp_apply(
        {
          pexp_desc: Pexp_ident({txt: Lident("|."), loc: _}),
          pexp_attributes: _,
          pexp_loc: _,
        },
        [(Nolabel, arg), (Nolabel, fn)],
      ) =>
      let arg = self.expr(self, arg);

      switch (fn.pexp_desc) {
      /* a->f  === f(a) */
      | Pexp_ident(_) => {
          ...fn,
          pexp_desc: Pexp_apply(fn, [(Nolabel, arg)]),
        }
      /* a->f(b)  === f(a, b) */
      | Pexp_apply(f, args) => {
          ...fn,
          pexp_desc: Pexp_apply(f, [(Nolabel, arg), ...args]),
        }
      /* a->Some === Some(a) */
      | Pexp_construct(ctor, None) => {
          ...fn,
          pexp_desc: Pexp_construct(ctor, Some(arg)),
        }
      /* 1->(x => x + 2) */
      | Pexp_fun(_) => {...fn, pexp_desc: Pexp_apply(fn, [(Nolabel, arg)])}
      /* Action->self.send */
      | Pexp_field(_) => {
          ...fn,
          pexp_desc: Pexp_apply(fn, [(Nolabel, arg)]),
        }
      | Pexp_tuple(es) =>
        let es =
          List.map(
            (e: Parsetree.expression) =>
              switch (e.pexp_desc) {
              | Pexp_ident(_) => {
                  ...e,
                  pexp_desc: Pexp_apply(e, [(Nolabel, arg)]),
                }
              | Pexp_apply(f, args) => {
                  ...e,
                  pexp_desc: Pexp_apply(f, [(Nolabel, arg), ...args]),
                }
              | Pexp_construct(ctor, None) => {
                  ...fn,
                  pexp_desc: Pexp_construct(ctor, Some(arg)),
                }
              | Pexp_fun(_) => {
                  ...e,
                  pexp_desc: Pexp_apply(e, [(Nolabel, arg)]),
                }
              | Pexp_field(_) => {
                  ...fn,
                  pexp_desc: Pexp_apply(fn, [(Nolabel, arg)]),
                }
              | _ => e
              },
            es,
          );
        {...fn, pexp_desc: Pexp_tuple(es)};
      | _ =>
        Location.raise_errorf(~loc=fn.pexp_loc, "Cannot pipe into expression")
      };
    | _ => Ast_mapper.default_mapper.expr(self, e)
    },
};

let () =
  Driver.register(
    ~name="ppx_fast_pipe", ~args=[], Versions.ocaml_406, (_config, _cookies) =>
    mapper
  );
