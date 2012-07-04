DIAG(expected_top_level_decl,
     "Expected a top-level declaration, which starts with 'func' or 'var'.")
DIAG(expected_identifier_after_func,
     "Expected an identifier after 'func'.")
DIAG(expected_lparen_to_begin_function_args,
     "Expected '(' after the function name to begin the argument list.")
DIAG(could_not_parse_function_argument_declaration,
     "Could not parse the function argument.")
DIAG(could_not_recover_by_finding_rparen,
     "Attempted to recover by ignoring tokens until the next ')', but none "
     "was found before the end of the file.")
DIAG(expected_rparen_to_end_function_args,
     "Expected ')' to end the function argument list.")
DIAG(expected_arrow_before_return_type,
     "Expected '->' before the function return type.")
DIAG(expected_function_return_type,
     "Expected the function's return type after '->'.")
DIAG(expected_lbrace_for_function_body,
     "Expected '{' to begin the function body.")
DIAG(expected_rbrace_after_function_body,
     "Expected '}' after the function body.")
DIAG(expected_type_ident_in_func_arg_list,
     "Expected a parameter type in the function argument list.")
DIAG(expected_name_ident_in_func_arg_list,
     "Expected a parameter name in the function argument list.")
DIAG(expected_identifier_after_var,
     "Expected an identifier after 'var'.")
DIAG(expected_type_in_var_decl,
     "Expected a type after the name in the variable declaration.")
DIAG(expected_semicolon_after_var_decl,
     "Expected a semicolon at the end of the variable declaration.")
#undef DIAG
