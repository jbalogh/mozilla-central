/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "IDBWrapperCache.h"

USING_INDEXEDDB_NAMESPACE

NS_IMPL_CYCLE_COLLECTION_CLASS(IDBWrapperCache)

NS_IMPL_CYCLE_COLLECTION_TRAVERSE_BEGIN_INHERITED(IDBWrapperCache,
                                                  nsDOMEventTargetWrapperCache)
  // Don't need NS_IMPL_CYCLE_COLLECTION_TRAVERSE_SCRIPT_OBJECTS because
  // nsDOMEventTargetHelper does it for us.
NS_IMPL_CYCLE_COLLECTION_TRAVERSE_END

NS_IMPL_CYCLE_COLLECTION_UNLINK_BEGIN_INHERITED(IDBWrapperCache,
                                                nsDOMEventTargetWrapperCache)
  tmp->mScriptOwner = nsnull;
NS_IMPL_CYCLE_COLLECTION_UNLINK_END

NS_IMPL_CYCLE_COLLECTION_TRACE_BEGIN_INHERITED(IDBWrapperCache,
                                               nsDOMEventTargetWrapperCache)
  // Don't need NS_IMPL_CYCLE_COLLECTION_TRACE_PRESERVED_WRAPPER because
  // nsDOMEventTargetHelper does it for us.
  if (tmp->mScriptOwner) {
    NS_IMPL_CYCLE_COLLECTION_TRACE_JS_CALLBACK(tmp->mScriptOwner,
                                               "mScriptOwner")
  }
NS_IMPL_CYCLE_COLLECTION_TRACE_END

NS_INTERFACE_MAP_BEGIN_CYCLE_COLLECTION_INHERITED(IDBWrapperCache)
NS_INTERFACE_MAP_END_INHERITING(nsDOMEventTargetWrapperCache)

NS_IMPL_ADDREF_INHERITED(IDBWrapperCache, nsDOMEventTargetWrapperCache)
NS_IMPL_RELEASE_INHERITED(IDBWrapperCache, nsDOMEventTargetWrapperCache)
