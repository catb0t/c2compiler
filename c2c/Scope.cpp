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
#include <assert.h>

#include "Scope.h"
#include "Package.h"
#include "Decl.h"

using namespace C2;

Scope::Scope(const std::string& name_) : name(name_) {}

void Scope::addPackage(bool isLocal, const std::string& name_, const Package* pkg) {
    assert(pkg);
    if (isLocal) {
        locals.push_back(pkg);
    }
    packages[name_] = pkg;
}

const Package* Scope::findPackage(const std::string& pkgName) const {
    PackagesConstIter iter = packages.find(pkgName);
    if (iter == packages.end()) return 0;
    return iter->second;
}

bool Scope::isExternal(const Package* pkg) const {
    return (pkg->getName() != name);
}

ScopeResult Scope::findSymbol(const std::string& symbol) const {
    ScopeResult result;
    // return private symbol only if no public symbol is found
    // ambiguous may also be set with visible = false
    for (LocalsConstIter iter = locals.begin(); iter != locals.end(); ++iter) {
        const Package* pkg = *iter;
        Decl* decl = pkg->findSymbol(symbol);
        if (!decl) continue;

        bool external = isExternal(pkg);
        bool visible = !(external && !decl->isPublic());
        if (result.decl) {  // already found
            if (result.visible == visible) {
                result.ambiguous = true;
                if (result.visible) break;
                continue;
            }
            if (!result.visible) { // replace with visible symbol
                result.decl = decl;
                result.pkg = pkg;
                result.external = external;
                result.ambiguous = false;
                result.visible = visible;
            }
        } else {
            result.decl = decl;
            result.pkg = pkg;
            result.external = external;
            result.visible = visible;
        }
    }
    return result;
}

void Scope::dump() {
    printf("Scoped packages:\n");
    for (PackagesIter iter = packages.begin(); iter != packages.end(); ++iter) {
        if (iter->first == name)
            printf("  *%s\n", iter->first.c_str());
        else
            printf("   %s\n", iter->first.c_str());
    }
}

