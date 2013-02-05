/* Copyright 2013 Bas van den Berg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>

#include <clang/Parse/ParseDiagnostic.h>
#include <clang/Sema/SemaDiagnostic.h>

#include "TypeAnalyseVisitor.h"
#include "Decl.h"
#include "Expr.h"
#include "Type.h"
#include "Package.h"
#include "color.h"

using namespace C2;
using namespace clang;

TypeAnalyseVisitor::TypeAnalyseVisitor(const Package& pkg_, clang::DiagnosticsEngine& Diags_)
    : package(pkg_)
    , Diags(Diags_)
    , errors(0)
{}

TypeAnalyseVisitor::~TypeAnalyseVisitor() {}

bool TypeAnalyseVisitor::handle(Decl* decl) {
    switch (decl->dtype()) {
    case DECL_FUNC:
        {
            FunctionDecl* func = DeclCaster<FunctionDecl>::getType(decl);
            assert(func);
            // check return type
            checkType(func->getReturnType());
            // check argument types
            ExprList& args = func->getArgs();
            for (unsigned i=0; i<args.size(); i++) {
                // NOTE arguments are DeclExpressios
                DeclExpr* de = ExprCaster<DeclExpr>::getType(args[i]);
                assert(de);
                checkType(de->getType());
            }
        }
        break;
    case DECL_VAR:
        {
            VarDecl* vd = DeclCaster<VarDecl>::getType(decl);
            assert(vd);
            checkType(vd->getType());
        }
        break;
    case DECL_TYPE:
        {
            TypeDecl* td = DeclCaster<TypeDecl>::getType(decl);
            assert(td);
            checkType(td->getType());
        }
        // TODO analyse members in structs/unions
        break;
    case DECL_ARRAYVALUE:
    case DECL_USE:
        // nothing to do
        break;
    }
    return false;
}

void TypeAnalyseVisitor::checkType(Type* type) {
    if (type->hasBuiltinBase()) return; // always ok

    switch (type->getKind()) {
    case Type::BUILTIN:
        assert(0);
        return;
    case Type::STRUCT:
    case Type::UNION:
        // TODO check members,
        fprintf(stderr, ANSI_BLUE"TODO check struct/union members"ANSI_NORMAL"\n");
        break;
    case Type::ENUM:
        // has no subtypes
        break;
    case Type::USER:
    case Type::FUNC:
    case Type::POINTER:
    case Type::ARRAY:
    case Type::QUALIFIER:
        checkUserType(type->getBaseUserType());
        break;
    }
}

void TypeAnalyseVisitor::checkUserType(IdentifierExpr* id) {
    // NOTE dont support fully qualified name (package::name) yet
    Decl* symbol = package.findSymbol(id->name);
    if (!symbol) {
        Diags.Report(id->getLocation(), diag::err_unknown_typename) << id->name;
        errors++;
        return;
    }
    if (symbol->dtype() != DECL_TYPE) {
        Diags.Report(id->getLocation(), diag::err_not_a_typename) << id->name;
        errors++;
        return;
    }
    // TODO for external packages, check visibility
}

