//------------------------------------------------------------------------------
//  SnapshotStorage.cc
//------------------------------------------------------------------------------
#include "SnapshotStorage.h"

using namespace yakc;

//------------------------------------------------------------------------------
SnapshotStorage::SnapshotStorage() {
    YAKC_MEMSET(&this->snapshots, 0, sizeof(this->snapshots));
}

//------------------------------------------------------------------------------
void
SnapshotStorage::TakeSnapshot(const kc85& kc, int slotIndex) {
    YAKC_ASSERT((slotIndex >= 0) && (slotIndex < MaxNumSnapshots));
    kc85_snapshot::take_snapshot(kc, this->snapshots[slotIndex]);
}

//------------------------------------------------------------------------------
bool
SnapshotStorage::HasSnapshot(int slotIndex) const {
    YAKC_ASSERT((slotIndex >= 0) && (slotIndex < MaxNumSnapshots));
    return kc85_snapshot::is_snapshot(this->snapshots[slotIndex]);
}

//------------------------------------------------------------------------------
void
SnapshotStorage::ApplySnapshot(int slotIndex, kc85& kc) {
    YAKC_ASSERT((slotIndex >= 0) && (slotIndex < MaxNumSnapshots));
    kc85_snapshot::apply_snapshot(this->snapshots[slotIndex], kc);
}