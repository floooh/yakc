//------------------------------------------------------------------------------
//  SnapshotStorage.cc
//------------------------------------------------------------------------------
#include "SnapshotStorage.h"

namespace YAKC {

//------------------------------------------------------------------------------
SnapshotStorage::SnapshotStorage() {
//    memset(&this->snapshots, 0, sizeof(this->snapshots));
}

//------------------------------------------------------------------------------
void
SnapshotStorage::TakeSnapshot(const yakc& emu, int slotIndex) {
    YAKC_ASSERT((slotIndex >= 0) && (slotIndex < MaxNumSnapshots));
//    snapshot::take_snapshot(emu, this->snapshots[slotIndex]);
}

//------------------------------------------------------------------------------
bool
SnapshotStorage::HasSnapshot(int slotIndex) const {
    YAKC_ASSERT((slotIndex >= 0) && (slotIndex < MaxNumSnapshots));
//    return snapshot::is_snapshot(this->snapshots[slotIndex]);
return false;
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
SnapshotStorage::ApplySnapshot(int slotIndex, yakc& emu) {
    YAKC_ASSERT((slotIndex >= 0) && (slotIndex < MaxNumSnapshots));
//    snapshot::apply_snapshot(this->snapshots[slotIndex], emu);
}

} // namespace YAKC
