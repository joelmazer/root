//===--- MultiplexExternalSemaSource.h - External Sema Interface-*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines ExternalSemaSource interface, dispatching to all clients
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_SEMA_MULTIPLEX_EXTERNAL_SEMA_SOURCE_H
#define LLVM_CLANG_SEMA_MULTIPLEX_EXTERNAL_SEMA_SOURCE_H

#include "clang/Sema/ExternalSemaSource.h"
#include "clang/Sema/Weak.h"

#include "llvm/ADT/SmallVector.h"

#include <utility>

namespace clang {

  class CXXConstructorDecl;
  class CXXRecordDecl;
  class DeclaratorDecl;
  struct ExternalVTableUse;
  class LookupResult;
  class NamespaceDecl;
  class Scope;
  class Sema;
  class TypedefNameDecl;
  class ValueDecl;
  class VarDecl;


/// \brief An abstract interface that should be implemented by
/// external AST sources that also provide information for semantic
/// analysis.
class MultiplexExternalSemaSource : public ExternalSemaSource {

private:
  llvm::SmallVector<ExternalSemaSource*, 2> Sources; // doesn't own them.

public:
  
  ///\brief Constructs a new multiplexing external sema source and appends the
  /// given element to it.
  ///
  ///\param[in] s1 - A non-null (old) ExternalSemaSource.
  ///\param[in] s2 - A non-null (new) ExternalSemaSource.
  ///
  MultiplexExternalSemaSource(ExternalSemaSource& s1, ExternalSemaSource& s2);

  ~MultiplexExternalSemaSource();

  ///\brief Appends new source to the source list.
  ///
  ///\param[in] source - An ExternalSemaSource.
  ///
  void addSource(ExternalSemaSource &source);

  /// \brief Initialize the semantic source with the Sema instance
  /// being used to perform semantic analysis on the abstract syntax
  /// tree.
  virtual void InitializeSema(Sema &S);

  /// \brief Inform the semantic consumer that Sema is no longer available.
  virtual void ForgetSema();

  /// \brief Load the contents of the global method pool for a given
  /// selector.
  virtual void ReadMethodPool(Selector Sel);

  /// \brief Load the set of namespaces that are known to the external source,
  /// which will be used during typo correction.
  virtual void ReadKnownNamespaces(SmallVectorImpl<NamespaceDecl*> &Namespaces);
  
  /// \brief Do last resort, unqualified lookup on a LookupResult that
  /// Sema cannot find.
  ///
  /// \param R a LookupResult that is being recovered.
  ///
  /// \param S the Scope of the identifier occurrence.
  ///
  /// \return true to tell Sema to recover using the LookupResult.
  virtual bool LookupUnqualified(LookupResult &R, Scope *S);

  /// \brief Read the set of tentative definitions known to the external Sema
  /// source.
  ///
  /// The external source should append its own tentative definitions to the
  /// given vector of tentative definitions. Note that this routine may be
  /// invoked multiple times; the external source should take care not to
  /// introduce the same declarations repeatedly.
  virtual void ReadTentativeDefinitions(SmallVectorImpl<VarDecl*> &TentativeDefs);
  
  /// \brief Read the set of unused file-scope declarations known to the
  /// external Sema source.
  ///
  /// The external source should append its own unused, filed-scope to the
  /// given vector of declarations. Note that this routine may be
  /// invoked multiple times; the external source should take care not to
  /// introduce the same declarations repeatedly.
  virtual void ReadUnusedFileScopedDecls(
                                  SmallVectorImpl<const DeclaratorDecl*> &Decls);
  
  /// \brief Read the set of delegating constructors known to the
  /// external Sema source.
  ///
  /// The external source should append its own delegating constructors to the
  /// given vector of declarations. Note that this routine may be
  /// invoked multiple times; the external source should take care not to
  /// introduce the same declarations repeatedly.
  virtual void ReadDelegatingConstructors(
                                    SmallVectorImpl<CXXConstructorDecl*> &Decls);

  /// \brief Read the set of ext_vector type declarations known to the
  /// external Sema source.
  ///
  /// The external source should append its own ext_vector type declarations to
  /// the given vector of declarations. Note that this routine may be
  /// invoked multiple times; the external source should take care not to
  /// introduce the same declarations repeatedly.
  virtual void ReadExtVectorDecls(SmallVectorImpl<TypedefNameDecl*> &Decls);

  /// \brief Read the set of dynamic classes known to the external Sema source.
  ///
  /// The external source should append its own dynamic classes to
  /// the given vector of declarations. Note that this routine may be
  /// invoked multiple times; the external source should take care not to
  /// introduce the same declarations repeatedly.
  virtual void ReadDynamicClasses(SmallVectorImpl<CXXRecordDecl*> &Decls);

  /// \brief Read the set of locally-scoped external declarations known to the
  /// external Sema source.
  ///
  /// The external source should append its own locally-scoped external
  /// declarations to the given vector of declarations. Note that this routine 
  /// may be invoked multiple times; the external source should take care not 
  /// to introduce the same declarations repeatedly.
  virtual void ReadLocallyScopedExternalDecls(SmallVectorImpl<NamedDecl*> &Decls);

  /// \brief Read the set of referenced selectors known to the
  /// external Sema source.
  ///
  /// The external source should append its own referenced selectors to the 
  /// given vector of selectors. Note that this routine 
  /// may be invoked multiple times; the external source should take care not 
  /// to introduce the same selectors repeatedly.
  virtual void ReadReferencedSelectors(SmallVectorImpl<std::pair<Selector, 
                                                        SourceLocation> > &Sels);

  /// \brief Read the set of weak, undeclared identifiers known to the
  /// external Sema source.
  ///
  /// The external source should append its own weak, undeclared identifiers to
  /// the given vector. Note that this routine may be invoked multiple times; 
  /// the external source should take care not to introduce the same identifiers
  /// repeatedly.
  virtual void ReadWeakUndeclaredIdentifiers(
                     SmallVectorImpl<std::pair<IdentifierInfo*, WeakInfo> > &WI);

  /// \brief Read the set of used vtables known to the external Sema source.
  ///
  /// The external source should append its own used vtables to the given
  /// vector. Note that this routine may be invoked multiple times; the external
  /// source should take care not to introduce the same vtables repeatedly.
  virtual void ReadUsedVTables(SmallVectorImpl<ExternalVTableUse> &VTables);

  /// \brief Read the set of pending instantiations known to the external
  /// Sema source.
  ///
  /// The external source should append its own pending instantiations to the
  /// given vector. Note that this routine may be invoked multiple times; the
  /// external source should take care not to introduce the same instantiations
  /// repeatedly.
  virtual void ReadPendingInstantiations(
               SmallVectorImpl<std::pair<ValueDecl*, SourceLocation> >& Pending);

  // isa/cast/dyn_cast support
  static bool classof(const MultiplexExternalSemaSource*) { return true; }
  //static bool classof(const ExternalSemaSource*) { return true; }
}; 

} // end namespace clang

#endif // LLVM_CLANG_SEMA_MULTIPLEX_EXTERNAL_SEMA_SOURCE_H
