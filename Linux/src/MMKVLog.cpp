/*
 * Tencent is pleased to support the open source community by making
 * MMKV available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "MMKVLog.h"
#include <cstdarg>
#include <string>

#ifdef ENABLE_MMKV_LOG

using namespace std;

bool g_isLogRedirecting = false;

#ifndef NDEBUG
MMKVLogLevel g_currentLogLevel = MMKVLogDebug;
#else
MMKVLogLevel g_currentLogLevel = MMKVLogInfo;
#endif

bool g_isContentChangeNotifying = false;

const char *_getFileName(const char *path) {
    const char *ptr = strrchr(path, '/');
    if (!ptr) {
        ptr = strrchr(path, '\\');
    }
    if (ptr) {
        return ptr + 1;
    } else {
        return path;
    }
}

static const char *MMKVLogLevelDesc(MMKVLogLevel level) {
    switch (level) {
        case MMKVLogDebug:
            return "D";
        case MMKVLogInfo:
            return "I";
        case MMKVLogWarning:
            return "W";
        case MMKVLogError:
            return "E";
        default:
            return "N";
    }
}

void _MMKVLogWithLevel(
    MMKVLogLevel level, const char *file, const char *func, int line, const char *format, ...) {
    if (level >= g_currentLogLevel) {
        string message;
        char buffer[16];

        va_list args;
        va_start(args, format);
        auto length = std::vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        if (length < 0) { // something wrong
            message = {};
        } else if (length < sizeof(buffer)) {
            message = string(buffer, static_cast<unsigned long>(length));
        } else {
            message.resize(static_cast<unsigned long>(length), '\0');
            va_start(args, format);
            std::vsnprintf(const_cast<char *>(message.data()), static_cast<size_t>(length) + 1,
                           format, args);
            va_end(args);
        }

        if (g_logHandler) {
            g_logHandler(level, file, line, func, message);
        } else {
            printf("[%s] <%s:%d::%s> %s\n", MMKVLogLevelDesc(level), file, line, func,
                   message.c_str());
            //fflush(stdout);
        }
    }
}

#endif // ENABLE_MMKV_LOG
