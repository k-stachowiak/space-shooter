
void test() {

        string key1, val1;
        expect_list exp_simple_kvp(
                        expect_atom_cap(key1),
                        expect_atom_cap(val1));

        string op, key2, val2;
        expect_list exp_expression_kvp(
                        expect_atom_cap(op),
                        expect_atom_cap(key2),
                        exoect_atom_cap(val2));

        bool was_simple = false;
        bool was_expr = false;
        expect_or exp_kvp( { exp_simple_kvp, was_simple },
                           { exp_expression_kvp, was_expr });

        check(exp_kvp, token);
}

