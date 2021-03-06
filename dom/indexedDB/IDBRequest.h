/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Indexed Database.
 *
 * The Initial Developer of the Original Code is
 * The Mozilla Foundation.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Shawn Wilsher <me@shawnwilsher.com>
 *   Ben Turner <bent.mozilla@gmail.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef mozilla_dom_indexeddb_idbrequest_h__
#define mozilla_dom_indexeddb_idbrequest_h__

#include "mozilla/dom/indexedDB/IndexedDatabase.h"

#include "nsIIDBRequest.h"
#include "nsIIDBOpenDBRequest.h"

#include "mozilla/dom/indexedDB/IDBWrapperCache.h"

class nsIScriptContext;
class nsPIDOMWindow;

BEGIN_INDEXEDDB_NAMESPACE

class HelperBase;
class IDBTransaction;

class IDBRequest : public IDBWrapperCache,
                   public nsIIDBRequest
{
public:
  NS_DECL_ISUPPORTS_INHERITED
  NS_DECL_NSIIDBREQUEST
  NS_DECL_CYCLE_COLLECTION_SCRIPT_HOLDER_CLASS_INHERITED(IDBRequest,
                                                         IDBWrapperCache)

  static
  already_AddRefed<IDBRequest> Create(nsISupports* aSource,
                                      IDBWrapperCache* aOwnerCache,
                                      IDBTransaction* aTransaction);

  // nsIDOMEventTarget
  virtual nsresult PreHandleEvent(nsEventChainPreVisitor& aVisitor);

  nsISupports* Source()
  {
    return mSource;
  }

  void Reset();

  nsresult NotifyHelperCompleted(HelperBase* aHelper);

  void SetError(nsresult rv)
  {
    NS_ASSERTION(NS_FAILED(rv), "Er, what?");
    NS_ASSERTION(mErrorCode == NS_OK, "Already have an error?");

    mErrorCode = rv;
  }

protected:
  IDBRequest();
  ~IDBRequest();

  virtual void RootResultValInternal();
  virtual void UnrootResultValInternal();

  void RootResultVal()
  {
    if (!mRooted) {
      RootResultValInternal();
      mRooted = true;
    }
  }

  void UnrootResultVal()
  {
    if (mRooted) {
      UnrootResultValInternal();
      mRooted = false;
    }
  }

  nsCOMPtr<nsISupports> mSource;
  nsRefPtr<IDBTransaction> mTransaction;

  NS_DECL_EVENT_HANDLER(success)
  NS_DECL_EVENT_HANDLER(error)

  jsval mResultVal;

  PRUint16 mErrorCode;
  bool mHaveResultOrErrorCode;
  bool mRooted;
};

class IDBOpenDBRequest : public IDBRequest,
                         public nsIIDBOpenDBRequest
{
public:
  NS_DECL_ISUPPORTS_INHERITED
  NS_FORWARD_NSIIDBREQUEST(IDBRequest::)
  NS_DECL_NSIIDBOPENDBREQUEST
  NS_DECL_CYCLE_COLLECTION_CLASS_INHERITED(IDBOpenDBRequest, IDBRequest)

  static
  already_AddRefed<IDBOpenDBRequest>
  Create(nsIScriptContext* aScriptContext,
         nsPIDOMWindow* aOwner,
         JSObject* aScriptOwner);

  static
  already_AddRefed<IDBOpenDBRequest>
  Create(IDBWrapperCache* aOwnerCache)
  {
    return Create(aOwnerCache->GetScriptContext(), aOwnerCache->GetOwner(),
                  aOwnerCache->GetScriptOwner());
  }

  void SetTransaction(IDBTransaction* aTransaction);

protected:
  ~IDBOpenDBRequest();

  virtual void RootResultValInternal();
  virtual void UnrootResultValInternal();

  // Only touched on the main thread.
  NS_DECL_EVENT_HANDLER(blocked)
  NS_DECL_EVENT_HANDLER(upgradeneeded)
};

END_INDEXEDDB_NAMESPACE

#endif // mozilla_dom_indexeddb_idbrequest_h__
