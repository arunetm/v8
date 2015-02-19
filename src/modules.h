// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_MODULES_H_
#define V8_MODULES_H_

#include "src/zone.h"

namespace v8 {
namespace internal {


class AstRawString;


class ModuleDescriptor : public ZoneObject {
 public:
  // ---------------------------------------------------------------------------
  // Factory methods.

  static ModuleDescriptor* New(Zone* zone) {
    return new (zone) ModuleDescriptor();
  }

  // ---------------------------------------------------------------------------
  // Mutators.

  // Add a name to the list of exports. If it already exists, or this descriptor
  // is frozen, that's an error.
  void AddLocalExport(const AstRawString* export_name,
                      const AstRawString* local_name, Zone* zone, bool* ok);

  // Do not allow any further refinements, directly or through unification.
  void Freeze() { frozen_ = true; }

  // Assign an index.
  void Allocate(int index) {
    DCHECK(IsFrozen() && index_ == -1);
    index_ = index;
  }

  // ---------------------------------------------------------------------------
  // Accessors.

  // Check whether this is closed (i.e. fully determined).
  bool IsFrozen() { return frozen_; }

  int Length() {
    DCHECK(IsFrozen());
    ZoneHashMap* exports = exports_;
    return exports ? exports->occupancy() : 0;
  }

  // The context slot in the hosting script context pointing to this module.
  int Index() {
    DCHECK(IsFrozen());
    return index_;
  }

  // ---------------------------------------------------------------------------
  // Iterators.

  // Use like:
  //   for (auto it = descriptor->iterator(); !it.done(); it.Advance()) {
  //     ... it.name() ...
  //   }
  class Iterator {
   public:
    bool done() const { return entry_ == NULL; }
    const AstRawString* export_name() const {
      DCHECK(!done());
      return static_cast<const AstRawString*>(entry_->key);
    }
    const AstRawString* local_name() const {
      DCHECK(!done());
      return static_cast<const AstRawString*>(entry_->value);
    }
    void Advance() { entry_ = exports_->Next(entry_); }

   private:
    friend class ModuleDescriptor;
    explicit Iterator(const ZoneHashMap* exports)
        : exports_(exports), entry_(exports ? exports->Start() : NULL) {}

    const ZoneHashMap* exports_;
    ZoneHashMap::Entry* entry_;
  };

  Iterator iterator() const { return Iterator(this->exports_); }

  // ---------------------------------------------------------------------------
  // Implementation.
 private:
  bool frozen_;
  ZoneHashMap* exports_;   // Module exports and their types (allocated lazily)
  int index_;

  ModuleDescriptor() : frozen_(false), exports_(NULL), index_(-1) {}
};

} }  // namespace v8::internal

#endif  // V8_MODULES_H_
