#ifndef ECMASCRIPT_JSON_VISITOR_H
#define ECMASCRIPT_JSON_VISITOR_H

#include "basic_visitor.h"
#include "json.h"
#include "util.h"

#include <numeric>
#include <sstream>

class JSONVisitor : public BasicVisitor {
  std::stringstream buf;

  static char to_hex_digit(int x)
  {
    switch (x) {
    case 0: return '0';
    case 1: return '1';
    case 2: return '2';
    case 3: return '3';
    case 4: return '4';
    case 5: return '5';
    case 6: return '6';
    case 7: return '7';
    case 8: return '8';
    case 9: return '9';
    case 10: return 'a';
    case 11: return 'b';
    case 12: return 'c';
    case 13: return 'd';
    case 14: return 'e';
    case 15: return 'f';
    default: return '?';
    }
  }

  static std::string& encode(std::string& str, int cp)
  {
    switch (cp) {
    case '\"': str += "\\\""; break;
    case '\\': str += "\\\\"; break;
    case '/': str += "\\/"; break;
    case '\b': str += "\\b"; break;
    case '\f': str += "\\f"; break;
    case '\n': str += "\\n"; break;
    case '\r': str += "\\r"; break;
    case '\t': str += "\\t"; break;
    default:
      if (cp >= 0 && cp <= 0x0F) {
        str += "\\u000";
        str += to_hex_digit(cp & 0x0F);
      }
      else if (cp >= 0x10 && cp <= 0x1F) {
        str += "\\u00";
        str += to_hex_digit((cp / 16) & 0x0F);
        str += to_hex_digit(cp & 0x0F);
      }
      else
        str += cp;
      break;
    }
    return str;
  }

  static std::string quote(std::string str)
  {
    return accumulate(str.begin(), str.end(), std::string("\""), encode) + "\"";
  }

  void operator()(const Elision& el) override
  {
    buf << "null";
    for (std::size_t i = 1; i < el.count; ++i) {
      buf << ","
          << "null";
    }
  }

  // void operator()(const Identifier& id) override
  // {
  //   // buf << "{" << quote("type") << ":" << quote("IdentifierExpression");
  //   // buf << "," << quote("identifier") << ":";
  //   buf << "{" << quote("type") << ":" << quote("Identifier");
  //   buf << "," << quote("name") << ":"
  //       << quote(convert_utf16_to_utf8(id.value));
  //   buf << "}";
  //   // buf << "}";
  // }

  void print_identifier(const Token& value)
  {
    buf << "{" << quote("name") << ":" << quote(value.to_string());
    buf << "," << quote("type") << ":" << quote("Identifier");
    buf << "}";
  }

  void operator()(const IdentifierExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("IdentifierExpression");
    buf << "," << quote("identifier") << ":";
    print_identifier(expr.identifier);
    buf << "}";
  }

  // void operator()(const PrimaryExpression& expr) override {}

  // void operator()(const NullLiteral& literal) override
  // {
  //   buf << "{" << quote("type") << ":" << quote("LiteralNullExpression");
  //   buf << "}";
  // }

  // void operator()(const BooleanLiteral& literal) override
  // {
  //   buf << "{" << quote("type") << ":" << quote("LiteralBooleanExpression");
  //   buf << "," << quote("value") << ":" << (literal.value ? "true" :
  //   "false");
  //   buf << "}";
  // }

  // void operator()(const NumericLiteral& literal) override
  // {
  //   if (literal.value.isInfinity()) {
  //     buf << "{" << quote("type") << ":" <<
  //     quote("LiteralInfinityExpression")
  //         << "}";
  //   }
  //   else if (literal.value.isNaN()) {
  //     buf << "{" << quote("type") << ":" << quote("LiteralNaNExpression")
  //         << "}";
  //   }
  //   else {
  //     buf << "{" << quote("type") << ":" <<
  //     quote("LiteralNumericExpression");
  //     buf << "," << quote("value") << ":" << literal.value.ToString();
  //     buf << "}";
  //   }
  // }

  // void operator()(const StringLiteral& literal) override
  // {
  //   buf << "{" << quote("type") << ":" << quote("LiteralStringExpression");
  //   buf << "," << quote("value") << ":"
  //       << convert_utf16_to_utf8(Quote(literal.value));
  //   buf << "}";
  // }

  // void operator()(const RegularExpressionLiteral& literal) override
  // {
  //   buf << "{" << quote("type") << ":" << quote("LiteralRegExpExpression");
  //   buf << "," << quote("value") << ":"
  //       << quote(convert_utf16_to_utf8(literal.value));
  //   buf << "}";
  // }

  // void operator()(const ArrayLiteral& literal) override
  // {

  //   buf << "[";
  //   auto it  = literal.elements->begin();
  //   auto end = literal.elements->end();
  //   if (it != end)
  //     apply(*it++);
  //   while (it != end) {
  //     buf << ",";
  //     apply(*it++);
  //   }
  //   if (auto el = literal.elision) {
  //     for (std::size_t i = 1; i < el->count; ++i) {
  //       buf << ","
  //           << "null";
  //     }
  //   }
  //   buf << "]";
  // }

  // void operator()(const ObjectLiteral& literal) override
  // {
  //   if (literal.declarations)
  //     apply(literal.declarations);
  //   else
  //     buf << "[]";
  // }

  void operator()(const ThisExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("ThisExpression");
    buf << "}";
  }

  void operator()(const LiteralExpression& expr) override
  {
    if (expr.literal.is_null_literal()) {
      buf << "{" << quote("type") << ":" << quote("LiteralNullExpression")
          << "}";
    }
    else if (expr.literal.is_boolean_literal()) {
      buf << "{" << quote("type") << ":" << quote("LiteralBooleanExpression")
          << "," << quote("value") << ":" << expr.literal.to_string() << "}";
    }
    else if (expr.literal.is_numeric_literal()) {
      auto value = Number(expr.literal.numeric_value());
      if (value.isInfinity()) {
        buf << "{" << quote("type") << ":" << quote("LiteralInfinityExpression")
            << "}";
      }
      else if (value.isNaN()) {
        buf << "{" << quote("type") << ":" << quote("LiteralNaNExpression")
            << "}";
      }
      else {
        buf << "{" << quote("type") << ":" << quote("LiteralNumericExpression")
            << "," << quote("value") << ":" << value.ToString() << "}";
      }
    }
    else if (expr.literal.is_string_literal()) {
      buf << "{" << quote("type") << ":" << quote("LiteralStringExpression")
          << "," << quote("value") << ":" << quote(expr.literal.to_string())
          << "}";
    }
    else if (expr.literal.is_regular_expression_literal()) {
      buf << "{" << quote("type") << ":" << quote("LiteralRegExpExpression")
          << "," << quote("value") << ":" << quote(expr.literal.to_string())
          << "}";
    }
    else
      throw std::logic_error(
          "Invalid literal token: " + expr.literal.to_string());
  }

  void operator()(const ArrayExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("ArrayExpression");
    buf << "," << quote("elements") << ":";
    // apply(expr.array);
    buf << "[";
    auto it  = expr.elements->begin();
    auto end = expr.elements->end();
    if (it != end)
      apply(*it++);
    while (it != end) {
      buf << ",";
      apply(*it++);
    }
    if (auto el = expr.elision) {
      for (std::size_t i = 1; i < el->count; ++i) {
        buf << ","
            << "null";
      }
    }
    buf << "]";
    buf << "}";
  }

  void operator()(const ObjectExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("ObjectExpression");
    buf << "," << quote("properties") << ":";
    // if (expr.object) {
    // apply(expr.object);
    if (expr.declarations)
      apply(expr.declarations);
    else
      buf << "[]";
    // }
    // else
    //   buf << "[]";
    buf << "}";
  }

  void operator()(const PropertyName& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("PropertyName");
    if (expr.value.is_identifier_name()) {
      buf << "," << quote("kind") << ":" << quote("identifier");
      buf << "," << quote("value") << ":" << quote(expr.value.to_string());
    }
    else if (expr.value.is_string_literal()) {
      buf << "," << quote("kind") << ":" << quote("string");
      buf << "," << quote("value") << ":" << quote(expr.value.to_string());
    }
    else if (expr.value.is_numeric_literal()) {
      buf << "," << quote("kind") << ":" << quote("number");
      buf << "," << quote("value") << ":" << quote(Number(expr.value.numeric_value()).ToString());
    }
    buf << "}";
  }

  void operator()(const PropertyAssignment& expr) override
  {
    switch (expr.kind) {
    case PropertyAssignment::Kind::INIT:
      buf << "{" << quote("type") << ":" << quote("DataProperty");
      buf << "," << quote("name") << ":";
      apply(expr.name);
      buf << "," << quote("expression") << ":";
      apply(expr.expression);
      buf << "}";
      break;
    case PropertyAssignment::Kind::GET:
      buf << "{" << quote("type") << ":" << quote("Getter");
      buf << "," << quote("name") << ":";
      apply(expr.name);
      buf << "," << quote("body") << ":";
      apply(expr.body);
      buf << "}";
      break;
    case PropertyAssignment::Kind::SET:
      buf << "{" << quote("type") << ":" << quote("Setter");
      buf << "," << quote("name") << ":";
      apply(expr.name);
      buf << "," << quote("parameter") << ":";
      apply(expr.parameter);
      buf << "," << quote("body") << ":";
      apply(expr.body);
      buf << "}";
      break;
    }
  }

  void operator()(const PropertySetParameterList& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("Identifier");
    buf << "," << quote("name") << ":" << quote(expr.value.to_string());
    buf << "}";
  }

  void operator()(const PostfixExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("PostfixExpression");
    buf << "," << quote("operator") << ":" << quote(expr.op);
    buf << "," << quote("operand") << ":";
    apply(expr.lhs);
    buf << "}";
  }

  void operator()(const UnaryExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("PrefixExpression");
    buf << "," << quote("operand") << ":";
    apply(expr.rhs);
    buf << "," << quote("operator") << ":" << quote(expr.op);
    buf << "}";
  }

  void operator()(const BinaryExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("BinaryExpression");
    buf << "," << quote("operator") << ":" << quote(expr.op);
    buf << "," << quote("left") << ":";
    apply(expr.lhs);
    buf << "," << quote("right") << ":";
    apply(expr.rhs);
    buf << "}";
  }

  void operator()(const AssignmentExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("AssignmentExpression");
    buf << "," << quote("operator") << ":" << quote(expr.op);
    buf << "," << quote("binding") << ":";
    apply(expr.lhs);
    buf << "," << quote("expression") << ":";
    apply(expr.rhs);
    buf << "}";
  }

  void operator()(const MemberExpression& expr) override
  {
    buf << "{" << quote("type") << ":";

    if (expr.property)
      buf << quote("StaticMemberExpression");
    else
      buf << quote("ComputedMemberExpression");
    buf << "," << quote("object") << ":";
    apply(expr.object);
    if (expr.property) {
      buf << "," << quote("property") << ":";
      buf << "{" << quote("name") << ":"
          << quote(expr.property.to_string());
      buf << "," << quote("type") << ":" << quote("Identifier");
      buf << "}";
      // apply(expr.property);
    }
    else {
      buf << "," << quote("expression") << ":";
      apply(expr.expression);
    }
    buf << "}";
  }

  void operator()(const CallExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("CallExpression");
    buf << "," << quote("callee") << ":";
    apply(expr.callee);
    buf << "," << quote("arguments") << ":";
    apply(expr.arguments);
    buf << "}";
  }

  void operator()(const NewExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("NewExpression");
    buf << "," << quote("callee") << ":";
    apply(expr.callee);
    buf << "," << quote("arguments") << ":";
    if (expr.arguments)
      apply(expr.arguments);
    else
      buf << "[]";
    buf << "}";
  }

  void operator()(const ConditionalExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("ConditionalExpression");
    buf << "," << quote("test") << ":";
    apply(expr.test);
    buf << "," << quote("consequent") << ":";
    apply(expr.consequent);
    buf << "," << quote("alternate") << ":";
    apply(expr.alternate);
    buf << "}";
  }

  void operator()(const FunctionExpression& expr) override
  {
    buf << "{" << quote("type") << ":" << quote("FunctionExpression");
    buf << "," << quote("name") << ":";
    if (expr.id)
      print_identifier(expr.id);
    else
      buf << "null";
    buf << "," << quote("parameters") << ":";
    apply(expr.params);
    buf << "," << quote("body") << ":";
    apply(expr.body);
    buf << "}";
  }

  void operator()(const Arguments& args) override
  {
    if (args.list)
      apply(args.list);
    else
      buf << "[]";
  }

  void operator()(const ArgumentList& list) override
  {
    apply(list.data);
  }
  void operator()(const StatementList& list) override
  {
    apply(list.data);
  }

  void operator()(const Block& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("BlockStatement");
    buf << "," << quote("block") << ":";
    buf << "{" << quote("type") << ":" << quote("Block");
    buf << "," << quote("statements") << ":";
    apply(stmt.body);
    buf << "}";
    buf << "}";
  }

  void operator()(const VariableStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("VariableDeclarationStatement");
    buf << "," << quote("declaration") << ":";
    apply(stmt.declarations);
    buf << "}";
  }

  void operator()(const VariableDeclarationList& list) override
  {
    buf << "{" << quote("type") << ":" << quote("VariableDeclaration");
    buf << "," << quote("kind") << ":" << quote("var");
    buf << "," << quote("declarators") << ":";
    apply(list.begin(), list.end());
    buf << "}";
  }

  void operator()(const VariableDeclaration& decl) override
  {
    buf << "{" << quote("type") << ":" << quote("VariableDeclarator");
    buf << "," << quote("binding") << ":";
    buf << "{" << quote("name") << ":"
        << quote(decl.identifier.to_string());
    buf << "," << quote("type") << ":" << quote("Identifier");
    buf << "}";
    // apply(decl.identifier);
    buf << "," << quote("init") << ":";
    if (decl.initializer)
      apply(decl.initializer);
    else
      buf << "null";
    buf << "}";
  }

  void operator()(const EmptyStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("EmptyStatement");
    buf << "}";
  }

  void operator()(const ExpressionStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("ExpressionStatement");
    buf << "," << quote("expression") << ":";
    apply(stmt.expression);
    buf << "}";
  }

  void operator()(const IfStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("IfStatement");
    buf << "," << quote("test") << ":";
    apply(stmt.test);
    buf << "," << quote("consequent") << ":";
    apply(stmt.consequent);
    buf << "," << quote("alternate") << ":";
    if (stmt.alternate)
      apply(stmt.alternate);
    else
      buf << "null";
    buf << "}";
  }

  void operator()(const DoWhileStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("DoWhileStatement");
    buf << "," << quote("body") << ":";
    apply(stmt.body);
    buf << "," << quote("test") << ":";
    apply(stmt.test);
    buf << "}";
  }

  void operator()(const WhileStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("WhileStatement");
    buf << "," << quote("body") << ":";
    apply(stmt.body);
    buf << "," << quote("test") << ":";
    apply(stmt.test);
    buf << "}";
  }

  void operator()(const ForStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("ForStatement");
    buf << "," << quote("body") << ":";
    apply(stmt.body);
    buf << "," << quote("init") << ":";
    if (stmt.init)
      apply(stmt.init);
    else
      buf << "null";
    buf << "," << quote("test") << ":";
    if (stmt.test)
      apply(stmt.test);
    else
      buf << "null";
    buf << "," << quote("update") << ":";
    if (stmt.update)
      apply(stmt.update);
    else
      buf << "null";
    buf << "}";
  }

  void operator()(const ForInStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("ForInStatement");
    buf << "," << quote("body") << ":";
    apply(stmt.body);
    buf << "," << quote("left") << ":";
    if (stmt.decl) {
      VariableDeclarationList list{stmt.decl};
      apply(&list);
    }
    else
      apply(stmt.left);
    // buf << "{" << quote("type") << ":" << quote("VariableDeclaration");
    // buf << "," << quote("kind") << ":" << quote("var");
    // buf << "," << quote("declarators") << ":";
    // buf << "[";
    // apply(stmt.left);
    // buf << "]";
    // buf << "}";
    buf << "," << quote("right") << ":";
    apply(stmt.right);
    buf << "}";
  }

  void operator()(const ContinueStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("ContinueStatement");
    buf << "," << quote("label") << ":";
    if (stmt.label) print_identifier(stmt.label);
    else buf << "null";
    buf << "}";
  }

  void operator()(const BreakStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("BreakStatement");
    buf << "," << quote("label") << ":";
    if (stmt.label) print_identifier(stmt.label);
    else buf << "null";
    buf << "}";
  }

  void operator()(const ReturnStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("ReturnStatement");
    buf << "," << quote("expression") << ":";
    if (stmt.argument) {
      apply(stmt.argument);
    }
    else
      buf << "null";
    buf << "}";
  }

  void operator()(const WithStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("WithStatement");
    buf << "," << quote("object") << ":";
    apply(stmt.object);
    buf << "," << quote("body") << ":";
    apply(stmt.body);
    buf << "}";
  }

  void operator()(const LabelledStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("LabeledStatement");
    buf << "," << quote("label") << ":";
    print_identifier(stmt.label);
    buf << "," << quote("body") << ":";
    apply(stmt.body);
    buf << "}";
  }

  void operator()(const SwitchStatement& stmt) override
  {

    auto i = stmt.cases->begin();
    auto j = stmt.cases->end();
    auto k = i;

    while (k != j && !(*k)->is_default())
      ++k;

    buf << "{" << quote("type") << ":"
        << quote(k == j ? "SwitchStatement" : "SwitchStatementWithDefault");
    buf << "," << quote("discriminant") << ":";
    apply(stmt.discriminant);

    if (k == j) {
      buf << "," << quote("cases") << ":";
      apply(i, j);
    }
    else {
      buf << "," << quote("preDefaultCases") << ":";
      apply(i, k);
      buf << "," << quote("defaultCase") << ":";
      apply(*k++);
      buf << "," << quote("postDefaultCases") << ":";
      apply(k, j);
    }
    buf << "}";
  }

  void operator()(const CaseClause& clause) override
  {
    buf << "{" << quote("type") << ":" << quote("SwitchCase");
    buf << "," << quote("test") << ":";
    apply(clause.test);
    buf << "," << quote("consequent") << ":";
    apply(clause.consequent);
    buf << "}";
  }

  void operator()(const DefaultClause& clause) override
  {
    buf << "{" << quote("type") << ":" << quote("SwitchDefault");
    buf << "," << quote("consequent") << ":";
    apply(clause.consequent);
    buf << "}";
  }

  void operator()(const ThrowStatement& stmt) override
  {
    buf << "{" << quote("type") << ":" << quote("ThrowStatement");
    buf << "," << quote("expression") << ":";
    apply(stmt.argument);
    buf << "}";
  }

  void operator()(const TryStatement& stmt) override
  {
    auto type = stmt.finalizer ? "TryFinallyStatement" : "TryCatchStatement";
    buf << "{" << quote("type") << ":" << quote(type);
    buf << "," << quote("body") << ":";

    buf << "{" << quote("type") << ":" << quote("Block");
    buf << "," << quote("statements") << ":";
    apply(stmt.block->body);
    buf << "}";

    buf << "," << quote("catchClause") << ":";
    if (stmt.handler) {
      buf << "{" << quote("type") << ":" << quote("CatchClause");
      buf << "," << quote("binding") << ":";
      buf << "{" << quote("type") << ":" << quote("Identifier");
      buf << "," << quote("name") << ":" << quote(stmt.binding.to_string());
      buf << "}";
      buf << "," << quote("body") << ":";

      buf << "{" << quote("type") << ":" << quote("Block");
      buf << "," << quote("statements") << ":";
      apply(stmt.handler->body);
      buf << "}";

      buf << "}";
    }
    else
      buf << "null";
    if (stmt.finalizer) {
      buf << "," << quote("finalizer") << ":";
      buf << "{" << quote("type") << ":" << quote("Block");
      buf << "," << quote("statements") << ":";
      apply(stmt.finalizer->body);
      buf << "}";
    }
    buf << "}";
  }

  using BasicVisitor::apply;

  template <typename It>
  void apply(It it, It end)
  {
    buf << "[";
    if (it != end)
      apply(*it++);
    while (it != end) {
      buf << ",";
      apply(*it++);
    }
    buf << "]";
  }

  template <typename T>
  void apply(const std::vector<T>& list)
  {
    apply(list.begin(), list.end());
  }

  void operator()(const SourceElements& list) override
  {
    apply(list.data);
  }

  template <typename InputIt>
  auto print_directives(InputIt first, InputIt last)
  {
    auto it = first;
    while (it != last) {
      if (auto stmt = dynamic_cast<ExpressionStatement*>(*it)) {
        if (auto expr = dynamic_cast<LiteralExpression*>(stmt->expression)) {

          if (expr->literal.is_string_literal()) {
            if (it != first)
              buf << ",";
            if (expr->literal == u"use strict") {
              buf << "{" << quote("type") << ":" << quote("UseStrictDirective")
                  << "}";
            }
            else {
              buf << "{" << quote("type") << ":" << quote("UnknownDirective")
                  << "," << quote("value") << ":" << expr->literal.to_string()
                  << "}";
            }
            ++it;
            continue;
          }
        }
      }
      break;
    }
    return it;
  }

  void operator()(const ProgramDeclaration& decl) override
  {
    buf << "{" << quote("type") << ":" << quote("Script");
    buf << "," << quote("body") << ":";
    buf << "{" << quote("type") << ":" << quote("FunctionBody");
    buf << "," << quote("directives") << ":"
        << "[";
    auto it = print_directives(decl.body->begin(), decl.body->end());
    buf << "]";
    buf << "," << quote("statements") << ":";
    apply(it, decl.body->end());
    buf << "}";
    buf << "}";
  }

  void operator()(const FunctionBody& body) override
  {
    buf << "{" << quote("type") << ":" << quote("FunctionBody");
    buf << "," << quote("directives") << ":"
        << "[";
    auto it = print_directives(body.data.begin(), body.data.end());
    buf << "]";
    buf << "," << quote("statements") << ":";
    apply(it, body.data.end());
    buf << "}";
  }

  void operator()(const FunctionDeclaration& decl) override
  {
    buf << "{" << quote("type") << ":" << quote("FunctionDeclaration");
    buf << "," << quote("name") << ":";
    if (decl.id) {
      buf << "{" << quote("name") << ":"
          << quote(decl.id.to_string());
      buf << "," << quote("type") << ":" << quote("Identifier");
      buf << "}";
      // apply(decl.id);
    }
    else
      buf << "null";
    buf << "," << quote("parameters") << ":";
    if (decl.params)
      apply(decl.params);
    else
      buf << "[]";
    buf << "," << quote("body") << ":";
    apply(decl.body);
    buf << "}";
  }

  void operator()(const ElementList& list) override
  {
    apply(list.begin(), list.end());
  }

  void operator()(const PropertyNameAndValueList& list) override
  {
    apply(list.begin(), list.end());
  }

  void operator()(const FormalParameterList& list) override
  {
    buf << "[";
    auto it = list.begin();
    if (it != list.end()) {
      print_identifier(*it);
      ++it;
    }
    while (it != list.end()) {
      buf << ",";
      print_identifier(*it);
      ++it;
    }

    buf << "]";
    // apply(list.begin(), list.end());
  }

public:
  std::string str() const
  {
    return buf.str();
  }
};

#endif