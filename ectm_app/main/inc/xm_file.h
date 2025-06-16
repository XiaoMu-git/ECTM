#ifndef _XM_FILE_H_
#define _XM_FILE_H_

#include "xm_config.h"
#include "cJSON.h"

void initSpiffs(void);
size_t readFile(const char *path, char *buff, uint32_t length);
size_t writeFile(const char *path, char *buff, uint32_t length);
cJSON* getJsonNode(cJSON *root, char *keys);

#endif // !_XM_FILE_H_
