/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
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
 * The Original Code is Mozilla SVG Project code.
 *
 * The Initial Developer of the Original Code is the Mozilla Foundation.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
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

#ifndef MOZILLA_SVGLENGTHLIST_H__
#define MOZILLA_SVGLENGTHLIST_H__

#include "nsCOMPtr.h"
#include "nsDebug.h"
#include "nsIContent.h"
#include "nsINode.h"
#include "nsIWeakReferenceUtils.h"
#include "nsSVGElement.h"
#include "nsTArray.h"
#include "SVGLength.h"

namespace mozilla {

/**
 * ATTENTION! WARNING! WATCH OUT!!
 *
 * Consumers that modify objects of this type absolutely MUST keep the DOM
 * wrappers for those lists (if any) in sync!! That's why this class is so
 * locked down.
 *
 * The DOM wrapper class for this class is DOMSVGLengthList.
 */
class SVGLengthList
{
  friend class SVGAnimatedLengthList;
  friend class DOMSVGLengthList;
  friend class DOMSVGLength;

public:

  SVGLengthList(){}
  ~SVGLengthList(){}

  // Only methods that don't make/permit modification to this list are public.
  // Only our friend classes can access methods that may change us.

  /// This may return an incomplete string on OOM, but that's acceptable.
  void GetValueAsString(nsAString& aValue) const;

  bool IsEmpty() const {
    return mLengths.IsEmpty();
  }

  PRUint32 Length() const {
    return mLengths.Length();
  }

  const SVGLength& operator[](PRUint32 aIndex) const {
    return mLengths[aIndex];
  }

  bool operator==(const SVGLengthList& rhs) const;

  bool SetCapacity(PRUint32 size) {
    return mLengths.SetCapacity(size);
  }

  void Compact() {
    mLengths.Compact();
  }

  // Access to methods that can modify objects of this type is deliberately
  // limited. This is to reduce the chances of someone modifying objects of
  // this type without taking the necessary steps to keep DOM wrappers in sync.
  // If you need wider access to these methods, consider adding a method to
  // SVGAnimatedLengthList and having that class act as an intermediary so it
  // can take care of keeping DOM wrappers in sync.

protected:

  /**
   * This may fail on OOM if the internal capacity needs to be increased, in
   * which case the list will be left unmodified.
   */
  nsresult CopyFrom(const SVGLengthList& rhs);

  SVGLength& operator[](PRUint32 aIndex) {
    return mLengths[aIndex];
  }

  /**
   * This may fail (return false) on OOM if the internal capacity is being
   * increased, in which case the list will be left unmodified.
   */
  bool SetLength(PRUint32 aNumberOfItems) {
    return mLengths.SetLength(aNumberOfItems);
  }

private:

  // Marking the following private only serves to show which methods are only
  // used by our friend classes (as opposed to our subclasses) - it doesn't
  // really provide additional safety.

  nsresult SetValueFromString(const nsAString& aValue);

  void Clear() {
    mLengths.Clear();
  }

  bool InsertItem(PRUint32 aIndex, const SVGLength &aLength) {
    if (aIndex >= mLengths.Length()) aIndex = mLengths.Length();
    return !!mLengths.InsertElementAt(aIndex, aLength);
  }

  void ReplaceItem(PRUint32 aIndex, const SVGLength &aLength) {
    NS_ABORT_IF_FALSE(aIndex < mLengths.Length(),
                      "DOM wrapper caller should have raised INDEX_SIZE_ERR");
    mLengths[aIndex] = aLength;
  }

  void RemoveItem(PRUint32 aIndex) {
    NS_ABORT_IF_FALSE(aIndex < mLengths.Length(),
                      "DOM wrapper caller should have raised INDEX_SIZE_ERR");
    mLengths.RemoveElementAt(aIndex);
  }

  bool AppendItem(SVGLength aLength) {
    return !!mLengths.AppendElement(aLength);
  }

protected:

  /* Rationale for using nsTArray<SVGLength> and not nsTArray<SVGLength, 1>:
   *
   * It might seem like we should use nsAutoTArray<SVGLength, 1> instead of
   * nsTArray<SVGLength>. That would preallocate space for one SVGLength and
   * avoid an extra memory allocation call in the common case of the 'x'
   * and 'y' attributes each containing a single length (and the 'dx' and 'dy'
   * attributes being empty). However, consider this:
   *
   * An empty nsTArray uses sizeof(Header*). An nsAutoTArray<class E,
   * PRUint32 N> on the other hand uses sizeof(Header*) +
   * (2 * sizeof(PRUint32)) + (N * sizeof(E)), which for one SVGLength is
   * sizeof(Header*) + 16 bytes.
   *
   * Now consider that for text elements we have four length list attributes
   * (x, y, dx, dy), each of which can have a baseVal and an animVal list. If
   * we were to go the nsAutoTArray<SVGLength, 1> route for each of these, we'd
   * end up using at least 160 bytes for these four attributes alone, even
   * though we only need storage for two SVGLengths (16 bytes) in the common
   * case!!
   *
   * A compromise might be to template SVGLengthList to allow
   * SVGAnimatedLengthList to preallocate space for an SVGLength for the
   * baseVal lists only, and that would cut the space used by the four
   * attributes to 96 bytes. Taking that even further and templating
   * SVGAnimatedLengthList too in order to only use nsTArray for 'dx' and 'dy'
   * would reduce the storage further to 64 bytes. Having different types makes
   * things more complicated for code that needs to look at the lists though.
   * In fact it also makes things more complicated when it comes to storing the
   * lists.
   *
   * It may be worth considering using nsAttrValue for length lists instead of
   * storing them directly on the element.
   */
  nsTArray<SVGLength> mLengths;
};


/**
 * This SVGLengthList subclass is for SVGLengthListSMILType which needs to know
 * which element and attribute a length list belongs to so that it can convert
 * between unit types if necessary.
 */
class SVGLengthListAndInfo : public SVGLengthList
{
public:

  SVGLengthListAndInfo()
    : mElement(nsnull)
    , mAxis(0)
    , mCanZeroPadList(false)
  {}

  SVGLengthListAndInfo(nsSVGElement *aElement, PRUint8 aAxis, bool aCanZeroPadList)
    : mElement(do_GetWeakReference(static_cast<nsINode*>(aElement)))
    , mAxis(aAxis)
    , mCanZeroPadList(aCanZeroPadList)
  {}

  void SetInfo(nsSVGElement *aElement, PRUint8 aAxis, bool aCanZeroPadList) {
    mElement = do_GetWeakReference(static_cast<nsINode*>(aElement));
    mAxis = aAxis;
    mCanZeroPadList = aCanZeroPadList;
  }

  nsSVGElement* Element() const {
    nsCOMPtr<nsIContent> e = do_QueryReferent(mElement);
    return static_cast<nsSVGElement*>(e.get());
  }

  PRUint8 Axis() const {
    NS_ABORT_IF_FALSE(mElement, "Axis() isn't valid");
    return mAxis;
  }

  /**
   * The value returned by this function depends on which attribute this object
   * is for. If appending a list of zeros to the attribute's list would have no
   * affect on rendering (e.g. the attributes 'dx' and 'dy' on <text>), then
   * this method will return true. If appending a list of zeros to the
   * attribute's list could *change* rendering (e.g. the attributes 'x' and 'y'
   * on <text>), then this method will return false.
   *
   * The reason that this method exists is because the SMIL code needs to know
   * what to do when it's asked to animate between lists of different length.
   * If this method returns true, then it can zero pad the short list before
   * carrying out its operations. However, in the case of the 'x' and 'y'
   * attributes on <text>, zero would mean "zero in the current coordinate
   * system", whereas we would want to pad shorter lists with the coordinates
   * at which glyphs would otherwise lie, which is almost certainly not zero!
   * Animating from/to zeros in this case would produce terrible results.
   *
   * Currently SVGLengthListSMILType simply disallows (drops) animation between
   * lists of different length if it can't zero pad a list. This is to avoid
   * having some authors create content that depends on undesirable behaviour
   * (which would make it difficult for us to fix the behavior in future). At
   * some point it would be nice to implement a callback to allow this code to
   * determine padding values for lists that can't be zero padded. See
   * https://bugzilla.mozilla.org/show_bug.cgi?id=573431
   */
  bool CanZeroPadList() const {
    //NS_ASSERTION(mElement, "CanZeroPadList() isn't valid");
    return mCanZeroPadList;
  }

  // For the SMIL code. See comment in SVGLengthListSMILType::Add().
  void SetCanZeroPadList(bool aCanZeroPadList) {
    mCanZeroPadList = aCanZeroPadList;
  }

  nsresult CopyFrom(const SVGLengthListAndInfo& rhs) {
    mElement = rhs.mElement;
    mAxis = rhs.mAxis;
    mCanZeroPadList = rhs.mCanZeroPadList;
    return SVGLengthList::CopyFrom(rhs);
  }

  // Instances of this special subclass do not have DOM wrappers that we need
  // to worry about keeping in sync, so it's safe to expose any hidden base
  // class methods required by the SMIL code, as we do below.

  /**
   * Exposed so that SVGLengthList baseVals can be copied to
   * SVGLengthListAndInfo objects. Note that callers should also call
   * SetInfo() when using this method!
   */
  nsresult CopyFrom(const SVGLengthList& rhs) {
    return SVGLengthList::CopyFrom(rhs);
  }
  const SVGLength& operator[](PRUint32 aIndex) const {
    return SVGLengthList::operator[](aIndex);
  }
  SVGLength& operator[](PRUint32 aIndex) {
    return SVGLengthList::operator[](aIndex);
  }
  bool SetLength(PRUint32 aNumberOfItems) {
    return SVGLengthList::SetLength(aNumberOfItems);
  }

private:
  // We must keep a weak reference to our element because we may belong to a
  // cached baseVal nsSMILValue. See the comments starting at:
  // https://bugzilla.mozilla.org/show_bug.cgi?id=515116#c15
  // See also https://bugzilla.mozilla.org/show_bug.cgi?id=653497
  nsWeakPtr mElement;
  PRUint8 mAxis;
  bool mCanZeroPadList;
};


/**
 * This class wraps SVGLengthList objects to allow frame consumers to process
 * SVGLengthList objects as if they were simply a list of float values in user
 * units. When consumers request the value at a given index, this class
 * dynamically converts the corresponding SVGLength from its actual unit and
 * returns its value in user units.
 *
 * Consumers should check that the user unit values returned are finite. Even
 * if the consumer can guarantee the list's element has a valid viewport
 * ancestor to resolve percentage units against, and a valid presContext and
 * styleContext to resolve absolute and em/ex units against, unit conversions
 * could still overflow. In that case the value returned will be
 * numeric_limits<float>::quiet_NaN().
 */
class NS_STACK_CLASS SVGUserUnitList
{
public:

  SVGUserUnitList()
    : mList(nsnull)
  {}

  void Init(const SVGLengthList *aList, nsSVGElement *aElement, PRUint8 aAxis) {
    mList = aList;
    mElement = aElement;
    mAxis = aAxis;
  }

  void Clear() {
    mList = nsnull;
  }

  PRUint32 Length() {
    return mList ? mList->Length() : 0;
  }

  /// This may return a non-finite value
  float operator[](PRUint32 aIndex) {
    return (*mList)[aIndex].GetValueInUserUnits(mElement, mAxis);
  }

private:
  const SVGLengthList *mList;
  nsSVGElement *mElement;
  PRUint8 mAxis;
};

} // namespace mozilla

#endif // MOZILLA_SVGLENGTHLIST_H__
