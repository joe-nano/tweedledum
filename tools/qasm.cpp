/*--------------------------------------------------------------------------------------------------
| Part of the tweedledum project.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-------------------------------------------------------------------------------------------------*/
#include "tweedledum/IO/qasm/Lexer.h"
#include "tweedledum/IO/qasm/PPLexer.h"
#include "tweedledum/IO/qasm/Parser.h"
#include "tweedledum/IO/write_utf8.h"
#include "tweedledum/IR/Module.h"
#include "tweedledum/Support/SourceManager.h"

#include <flags/flags.h>
#include <fmt/format.h>
#include <string>

int main(int argc, char** argv)
{
	flags::args const app(argc, argv);

	auto const path = app.get<std::string>("i");
	auto const test = app.get<std::string>("t", "lexer");

	if (!path) {
		fmt::print("No input file.\n");
		return EXIT_SUCCESS;
	}
	using namespace tweedledum;
	
	SourceManager source_manager;
	Source const* source = source_manager.add_file(path.value());
	if (test == "lexer") {
		qasm::Lexer lexer(source);
		qasm::Token token = lexer.next_token();
		while (!token.is(qasm::Token::Kinds::eof)) {
			fmt::print("{}\n", qasm::token_name(token.kind()));
			token = lexer.next_token();
		}
	} else if (test == "pp_lexer") {
		qasm::PPLexer pp_lexer(source_manager);
		qasm::Token token = pp_lexer.next_token();
		while (!token.is(qasm::Token::Kinds::eof)) {
			fmt::print("{}\n", qasm::token_name(token.kind()));
			token = pp_lexer.next_token();
		}
	} else if (test == "parser") {
		Module* module = qasm::parse_file(source_manager);
		if (module != nullptr) {
			fmt::print("Successfully parsed the input file.\n");
			write_utf8(module);
		}
	}
	return EXIT_SUCCESS;
}
