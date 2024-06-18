#ifndef KATANA_KLEAVER_STORAGE_H
#define KATANA_KLEAVER_STORAGE_H

#include "raylib.h"

#include <stdlib.h>

#define STORAGE_DATA_FILE   "storage.data"

typedef enum {
    STORAGE_POSITION_HISCORE
} StorageData;

bool SaveStorageValue(unsigned int position, int value);
int LoadStorageValue(unsigned int position);


#endif //KATANA_KLEAVER_STORAGE_H
