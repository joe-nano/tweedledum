/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../ir/Module.h"
#include "../../support/SourceManager.h"
#include "PPLexer.h"
#include "Token.h"

#include <memory>

namespace tweedledum::qasm {

/*! \brief
 */
class Parser {
public:
	Parser(SourceManager& source_manager)
	    : source_manager_(source_manager), pp_lexer_(source_manager_)
	{
		module_.reset(new Module());
	}

	Module* parse_module()
	{
		parse_header();
		while (1) {
			switch (current_token_.kind()) {
			case Token::Kinds::eof:
				return module_.release();

			case Token::Kinds::kw_creg:
				parse_creg();
				break;

			case Token::Kinds::kw_qreg:
				parse_qreg();
				break;

			case Token::Kinds::kw_gate:
				parse_gatedecl();
				break;

			case Token::Kinds::identifier:
			case Token::Kinds::kw_cx:
			case Token::Kinds::kw_measure:
			case Token::Kinds::kw_u:
				parse_qop();
				break;

			default:
				emit_error(current_token_.spelling());
				emit_error("expected a top-level entity.");
				return nullptr;
			}
		}
	}

private:
#pragma region Helper functions
	// Consume the current token 'current_token_' and lex the next one.
	// Returns the location of the consumed token.
	uint32_t consume_token()
	{
		prev_token_location_ = current_token_.location();
		current_token_ = pp_lexer_.next_token();
		return prev_token_location_;
	}

	// The parser expects that the current token is of 'expected' kind.  If
	// it is not, it emits a diagnostic, puts the parser in a error state
	// and returns the current_token_. Otherwise consumes the token and
	// returns it.
	Token expect_and_consume_token(Token::Kinds const expected)
	{
		if (!current_token_.is(expected)) {
			// Should emit some error.
			return current_token_;
		}
		Token return_token = current_token_;
		consume_token();
		return return_token;
	}

	// The parser try to see if the current token is of 'expected' kind.  If
	// it is not, returns false.  Otherwise consumes the token and returns
	// true.
	bool try_and_consume_token(Token::Kinds const expected)
	{
		if (!current_token_.is(expected)) {
			return false;
		}
		consume_token();
		return true;
	}

	void emit_error(std::string_view message) const
	{
		fmt::print("[error] {} {}\n",
		    source_manager_.location_str(current_token_.location()),
		    message);
	}
#pragma endregion

#pragma region Parsing Top - level entities
	/*! \brief Parse OpenQASM file header */
	void parse_header()
	{
		consume_token();
		expect_and_consume_token(Token::Kinds::kw_openqasm);
		expect_and_consume_token(Token::Kinds::real);
		expect_and_consume_token(Token::Kinds::semicolon);
	}

	void parse_creg()
	{
		// If we get here, then 'creg' was matched
		consume_token();
		std::string_view name
		    = expect_and_consume_token(Token::Kinds::identifier);
		expect_and_consume_token(Token::Kinds::l_square);
		uint32_t size
		    = expect_and_consume_token(Token::Kinds::nninteger);
		expect_and_consume_token(Token::Kinds::r_square);
		expect_and_consume_token(Token::Kinds::semicolon);
		for (uint32_t i = 0u; i < size; ++i) {
			module_->circuit_.create_cbit(
			    fmt::format("{}_{}", name, i));
		}
		return;
	}

	void parse_qreg()
	{
		// If we get here, then 'qreg' was matched
		consume_token();
		std::string_view name
		    = expect_and_consume_token(Token::Kinds::identifier);
		expect_and_consume_token(Token::Kinds::l_square);
		uint32_t size
		    = expect_and_consume_token(Token::Kinds::nninteger);
		expect_and_consume_token(Token::Kinds::r_square);
		expect_and_consume_token(Token::Kinds::semicolon);
		for (uint32_t i = 0u; i < size; ++i) {
			module_->circuit_.create_qubit(
			    fmt::format("{}_{}", name, i));
		}
		return;
	}

	void parse_gatedecl()
	{
		// If we get here, then either 'gate'
		consume_token();
		expect_and_consume_token(Token::Kinds::identifier);
		if (try_and_consume_token(Token::Kinds::l_paren)) {
			if (!try_and_consume_token(Token::Kinds::r_paren)) {
				parse_idlist();
				expect_and_consume_token(Token::Kinds::r_paren);
			}
		}
		parse_idlist();
		expect_and_consume_token(Token::Kinds::l_brace);
		if (!try_and_consume_token(Token::Kinds::r_brace)) {
			while (!current_token_.is(Token::Kinds::r_brace)) {
				parse_gop();
			}
			expect_and_consume_token(Token::Kinds::r_brace);
		}
	}

	void parse_gate_statement()
	{
		// If we get here, then an identifier was matched
		expect_and_consume_token(Token::Kinds::identifier);
		if (try_and_consume_token(Token::Kinds::l_paren)) {
			if (!try_and_consume_token(Token::Kinds::r_paren)) {
				parse_explist();
				expect_and_consume_token(Token::Kinds::r_paren);
			}
		}
		parse_anylist();
		expect_and_consume_token(Token::Kinds::semicolon);
		return;
	}
#pragma endregion

#pragma region Parsing functions
	/*! \brief Parse a identifier (<idlist>) */
	// <idlist> = <id>
	//          | <idlist> , <id>
	void parse_idlist()
	{
		do {
			expect_and_consume_token(Token::Kinds::identifier);
			if (!try_and_consume_token(Token::Kinds::comma)) {
				break;
			}
		} while (1);
	}

	/*! \brief Parse a list of arguments (<anylist>) */
	// <anylist> = <idlist> | <mixedlist>
	// <mixedlist> = <id> [ <nninteger> ] | <mixedlist> , <id>
	//             | <mixedlist> , <id> [ <nninteger> ]
	//             | <idlist> , <id>[ <nninteger> ]
	void parse_anylist()
	{
		do {
			parse_argument();
			if (!try_and_consume_token(Token::Kinds::comma)) {
				break;
			}
		} while (1);
	}

	void parse_gop()
	{
		switch (current_token_.kind()) {
		case Token::Kinds::kw_cx:
			parse_cnot();
			break;

		case Token::Kinds::kw_u:
			parse_u();
			break;

		case Token::Kinds::identifier:
			parse_gate_statement();
			break;

		default:
			break;
		}
	}

	void parse_qop()
	{
		switch (current_token_.kind()) {
		case Token::Kinds::kw_measure:
			break;

		case Token::Kinds::identifier:
		case Token::Kinds::kw_cx:
		case Token::Kinds::kw_u:
			parse_gop();

		default:
			break;
		}
	}

#pragma endregion

#pragma region Parsing expressions
	/*! \brief Parse expression list (<explist>) */
	// <explist> = <exp>
	//           | <explist> , <exp>
	void parse_explist()
	{
		do {
			parse_exp();
			if (!try_and_consume_token(Token::Kinds::comma)) {
				break;
			}
		} while (1);
	}

	/*! \brief Parse an expression (<exp>) */
	// <exp> = <real> | <nninteger> | pi | <id>
	//       | <exp> + <exp> | <exp> - <exp>
	//       | <exp> * <exp> | <exp> / <exp>
	//       | - <exp>
	//       | <exp> ^ <exp>
	//       | ( <exp> )
	//       | <unaryop> ( <exp> )
	void parse_exp(uint32_t min_precedence = 1)
	{
		parse_atom();
		while (1) {
			uint32_t next_min_precedence = min_precedence;
			switch (current_token_.kind()) {
			case Token::Kinds::plus:
				if (min_precedence > 1) {
					return;
				}
				next_min_precedence = 2;
				break;

			case Token::Kinds::minus:
				if (min_precedence > 1) {
					return;
				}
				next_min_precedence = 2;
				break;

			case Token::Kinds::star:
				if (min_precedence > 2) {
					return;
				}
				next_min_precedence = 3;
				break;

			case Token::Kinds::slash:
				if (min_precedence > 2) {
					return;
				}
				next_min_precedence = 3;
				break;

			case Token::Kinds::caret:
				if (min_precedence > 3) {
					return;
				}
				next_min_precedence = 4;
				break;

			default:
				return;
			}
			consume_token();
			parse_exp(next_min_precedence);
		}
	}

	void parse_atom()
	{
		if (try_and_consume_token(Token::Kinds::l_paren)) {
			parse_exp();
			expect_and_consume_token(Token::Kinds::r_paren);
		}
		if (try_and_consume_token(Token::Kinds::minus)) {
			parse_exp();
		}
		switch (current_token_.kind()) {
		case Token::Kinds::identifier:
		case Token::Kinds::nninteger:
		case Token::Kinds::kw_pi:
		case Token::Kinds::real:
			consume_token();
			return;

		default:
			break;
		}

		switch (current_token_.kind()) {
		case Token::Kinds::kw_uop_sin:
		case Token::Kinds::kw_uop_cos:
		case Token::Kinds::kw_uop_tan:
		case Token::Kinds::kw_uop_exp:
		case Token::Kinds::kw_uop_ln:
		case Token::Kinds::kw_uop_sqrt:
			break;

		default:
			return;
		}

		consume_token();
		expect_and_consume_token(Token::Kinds::l_paren);
		parse_exp();
		expect_and_consume_token(Token::Kinds::r_paren);
	}
#pragma endregion

#pragma region Parsing functions
	/*! \brief Parse an argument (<argument>) */
	// <argument> = <id>
	//            | <id> [ <nninteger> ]
	void parse_argument()
	{
		expect_and_consume_token(Token::Kinds::identifier);
		if (!try_and_consume_token(Token::Kinds::l_square)) {
			return;
		}
		expect_and_consume_token(Token::Kinds::nninteger);
		expect_and_consume_token(Token::Kinds::r_square);
	}

	void parse_cnot()
	{
		// If we get here 'CX' was matched
		consume_token();
		parse_argument();
		expect_and_consume_token(Token::Kinds::comma);
		parse_argument();
		expect_and_consume_token(Token::Kinds::semicolon);
	}

	void parse_u()
	{
		// If we get here 'U' was matched
		consume_token();
		expect_and_consume_token(Token::Kinds::l_paren);
		parse_exp();
		expect_and_consume_token(Token::Kinds::comma);
		parse_exp();
		expect_and_consume_token(Token::Kinds::comma);
		parse_exp();
		expect_and_consume_token(Token::Kinds::r_paren);
		parse_argument();
		expect_and_consume_token(Token::Kinds::semicolon);
	}
#pragma endregion

	// Delete copy-constructor
	Parser(Parser const&) = delete;
	Parser& operator=(Parser const&) = delete;

	SourceManager& source_manager_;
	PPLexer pp_lexer_;

	// The current token we are peeking.
	Token current_token_;

	// The location of the token we previously consumed.  This is used for
	// diagnostics in which we expected to see a token following another
	// token (e.g., the ';' at the end of a statement).
	uint32_t prev_token_location_;

	// This is the result module we are parsing into.
	std::unique_ptr<Module> module_;
};

Module* parse_file(SourceManager& source_manager)
{
	return Parser(source_manager).parse_module();
}

} // namespace tweedledum::qasm