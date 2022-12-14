#
# Copyright (C) 2016-2020 Arm Limited. All rights reserved.
#
# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


# Set default Gralloc version for the platform, but allow this to be overriden.
GRALLOC_API_VERSION?=3.x

# Fail build if GRALLOC_API_VERSION is not supported on the platform.
GRALLOC_VALID_VERSIONS := 3.x

# ifeq ($(GRALLOC_EXPERIMENTAL), 1)
    GRALLOC_VALID_VERSIONS += 4.x
# endif

ifeq ($(filter $(GRALLOC_API_VERSION),$(GRALLOC_VALID_VERSIONS)),)
    $(error Gralloc version $(GRALLOC_API_VERSION) is not valid on the current platform. Valid versions are $(GRALLOC_VALID_VERSIONS))
endif

# Derive namespaces and associated (internal and scaled) versions for Gralloc 3.x
# Note: On a given SDK platform, versions of Allocator / Mapper / Common can be
#       changed (somewhat) independently of each other. Scaled internal versions, encapsulating
#       their major and minor versions, provide for building specific combinations
ifeq ($(GRALLOC_API_VERSION), 3.x)
    $(info Building Gralloc 3.x on platform SDK version $(PLATFORM_SDK_VERSION))
    # Allocator = 3.0, Mapper = 3.0 and Common = 1.2
    HIDL_ALLOCATOR_VERSION_SCALED := 300
    HIDL_MAPPER_VERSION_SCALED := 300
    HIDL_COMMON_VERSION_SCALED := 120
endif

GRALLOC_VERSION_MAJOR := $(shell echo $(GRALLOC_API_VERSION) | cut -d. -f1)
