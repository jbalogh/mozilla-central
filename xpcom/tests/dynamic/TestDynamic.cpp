/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * The contents of this file are subject to the Netscape Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation. All
 * Rights Reserved.
 *
 * Contributor(s): 
 *   Suresh Duddi <dp@netscape.com>
 *   Christopher Blizzard <blizzard@mozilla.org>
 */

#include <stdio.h>
#include "TestFactory.h"
#include "nsIGenericFactory.h"

/**
 * ITestClass implementation
 */

class TestDynamicClassImpl: public ITestClass {
  NS_DECL_ISUPPORTS

  TestDynamicClassImpl() {
    NS_INIT_REFCNT();
  }
  void Test();
};

NS_IMPL_ISUPPORTS1(TestDynamicClassImpl, ITestClass);

void TestDynamicClassImpl::Test() {
  printf("hello, dynamic world!\n");
}

/**
 * Generic Module
 */

NS_GENERIC_FACTORY_CONSTRUCTOR(TestDynamicClassImpl);

static nsModuleComponentInfo components[] =
{
  { "Test Dynamic", NS_TESTLOADEDFACTORY_CID, NS_TESTLOADEDFACTORY_CONTRACTID,
    TestDynamicClassImplConstructor
  }
};

NS_IMPL_NSGETMODULE("nsTestDynamicModule", components)

