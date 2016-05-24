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
SnapshotStorage::TakeSnapshot(const emu& emu, int slotIndex) {
    YAKC_ASSERT((slotIndex >= 0) && (slotIndex < MaxNumSnapshots));
    kc85_snapshot::take_snapshot(emu.kc85, this->snapshots[slotIndex]);
}

//------------------------------------------------------------------------------
bool
SnapshotStorage::HasSnapshot(int slotIndex) const {
    YAKC_ASSERT((slotIndex >= 0) && (slotIndex < MaxNumSnapshots));
    return kc85_snapshot::is_snapshot(this->snapshots[slotIndex]);
}

//------------------------------------------------------------------------------
bool
SnapshotStorage::HasSnapshots() const {
    for (int i = 0; i < MaxNumSnapshots; i++) {
        if (this->HasSnapshot(i)) {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
void
SnapshotStorage::ApplySnapshot(int slotIndex, emu& emu) {
    YAKC_ASSERT((slotIndex >= 0) && (slotIndex < MaxNumSnapshots));
    kc85_snapshot::apply_snapshot(this->snapshots[slotIndex], emu.kc85);
}