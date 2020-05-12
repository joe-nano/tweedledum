/*------------------------------------------------------------------------------
| Part of the tweedledum.  This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
*-----------------------------------------------------------------------------*/
#pragma once

#include "../../support/Source.h"
#include "../../support/SourceManager.h"
#include "Lexer.h"
#include "Token.h"

#include <filesystem>
#include <fmt/format.h>
#include <memory>
#include <vector>

namespace tweedledum::qasm {

// clang-format off
static std::string const std_include
    = "gate u3(theta,phi,lambda) q { U(theta,phi,lambda) q; }gate u2(phi,lambda) q { "
      "U(pi/2,phi,lambda) q; }gate u1(lambda) q { U(0,0,lambda) q; }gate cx c,t { CX c,t; }gate id "
      "a { U(0,0,0) a; }gate u0(gamma) q { U(0,0,0) q; }gate x a { u3(pi,0,pi) a; }gate y a { "
      "u3(pi,pi/2,pi/2) a; }gate z a { u1(pi) a; }gate h a { u2(0,pi) a; }gate s a { u1(pi/2) a; "
      "}gate sdg a { u1(-pi/2) a; }gate t a { u1(pi/4) a; }gate tdg a { u1(-pi/4) a; }gate "
      "rx(theta) a { u3(theta, -pi/2,pi/2) a; }gate ry(theta) a { u3(theta,0,0) a; }gate rz(phi) a "
      "{ u1(phi) a; }gate cz a,b { h b; cx a,b; h b; }gate cy a,b { sdg b; cx a,b; s b; }gate swap "
      "a,b { cx a,b; cx b,a; cx a,b; }gate ch a,b {h b; sdg b;cx a,b;h b; t b;cx a,b;t b; h b; s "
      "b; x b; s a;}gate ccx a,b,c{  h c;  cx b,c; tdg c;  cx a,c; t c;  cx b,c; tdg c;  cx a,c; t "
      "b; t c; h c;  cx a,b; t a; tdg b;  cx a,b;}gate crz(lambda) a,b{  u1(lambda/2) b;  cx a,b;  "
      "u1(-lambda/2) b;  cx a,b;}gate cu1(lambda) a,b{  u1(lambda/2) a;  cx a,b;  u1(-lambda/2) b; "
      " cx a,b;  u1(lambda/2) b;}gate cu3(theta,phi,lambda) c, t{  u1((lambda-phi)/2) t;  cx c,t;  "
      "u3(-theta/2,0,-(phi+lambda)/2) t;  cx c,t;  u3(theta/2,phi,0) t;}";
// clang-format on

/*! \brief Pre-processor Lexer class
 *
 * This is the class able to handle includes.  You see, lexers know only about
 * tokens within a single source file.
 *
 */
class PPLexer {
public:
	PPLexer(SourceManager& source_manager)
	    : source_manager_(source_manager), current_lexer_(nullptr)
	{
		Source const* source = source_manager_.main_source();
		if (source != nullptr) {
			current_lexer_ = std::make_unique<Lexer>(source);
		}
	}

	bool add_target_file(std::string_view path)
	{
		Source const* source = source_manager_.add_file(path);
		if (source == nullptr) {
			return false;
		}
		if (current_lexer_ != nullptr) {
			lexer_stack_.push_back(std::move(current_lexer_));
		}
		current_lexer_ = std::make_unique<Lexer>(source);
		return true;
	}

	void add_target_buffer(std::string_view buffer)
	{
		Source const* source = source_manager_.add_buffer(buffer);
		if (current_lexer_ != nullptr) {
			lexer_stack_.push_back(std::move(current_lexer_));
		}
		current_lexer_ = std::make_unique<Lexer>(source);
	}

	Token next_token()
	{
		if (current_lexer_ == nullptr) {
			std::cerr << "No target to lex.\n";
			return Token(Token::Kinds::error, 0, 0, nullptr);
		}
		Token tok = current_lexer_->next_token();
		if (tok.is(Token::Kinds::pp_include)) {
			handle_include();
			tok = current_lexer_->next_token();
		} else if (tok.is(Token::Kinds::eof)) {
			if (!lexer_stack_.empty()) {
				current_lexer_ = std::move(lexer_stack_.back());
				lexer_stack_.pop_back();
				tok = current_lexer_->next_token();
			} else {
				current_lexer_ = nullptr;
			}
		}
		return tok;
	}

private:
	// The "include" tokens have just been read, read the file to be
	// included from the Lexer, then include it!
	void handle_include()
	{
		Token token = current_lexer_->next_token();
		if (!token.is(Token::Kinds::string)) {
			std::cerr << "Include must be followed by a file "
			             "name\n";
			return;
		}
		std::string_view target_path = token;
		token = current_lexer_->next_token();
		if (!token.is(Token::Kinds::semicolon)) {
			std::cerr << "Missing a ';'\n";
		}
		// This path is relative to the path
		std::filesystem::path path(
		    target_path.substr(1, target_path.length() - 2));
		if (path.filename() == "qelib1.inc") {
			add_target_buffer(std_include);
			return;
		}
		if (path.is_absolute()) {
			add_target_file(path.native());
		} else {
			Source const* current_src = current_lexer_->source();
			std::filesystem::path parent_path
			    = current_src->parent_path();
			path = parent_path / path;
			add_target_file(path.native());
		}
	}

	SourceManager& source_manager_;
	std::vector<std::unique_ptr<Lexer>> lexer_stack_;
	std::unique_ptr<Lexer> current_lexer_;
};

} // namespace tweedledum::qasm
