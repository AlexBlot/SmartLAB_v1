# Copyright 2015 Adam Green (http://mbed.org/users/AdamGreen/)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
PROJ_ROOT_DIR1  := ${CURDIR}
PROJ_ROOT_DIR   := $(notdir $(PROJ_ROOT_DIR1))
PROJECT_ORG     := $(PROJ_ROOT_DIR)
NOTHING         :=
SPACE           := $(NOTHING) $(NOTHING)
PROJECT         := $(subst $(SPACE),_,$(PROJECT_ORG))
DEVICES         := NCS36510
GCC4MBED_DIR1   := $(IDK_PATH)
GCC4MBED_DIR    := $(subst \,/,$(GCC4MBED_DIR1))
NO_FLOAT_SCANF  := 0
NO_FLOAT_PRINTF := 0
IDK_LIB         := 0
MRI_ENABLE      := 0
INCDIRS         := Include/
LIBS_PREFIX     :=
INCDIRS         += $(GCC4MBED_DIR)/IDK/include/
include $(GCC4MBED_DIR)/build/idk.mk
include $(GCC4MBED_DIR)/build/gcc4mbed.mk
