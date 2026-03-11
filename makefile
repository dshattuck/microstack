# Copyright (C) 2026 The Regents of the University of California
# Created by David W. Shattuck, Ph.D.
# SPDX-License-Identifier: LGPL-2.1-only
# https://spdx.org/licenses/LGPL-2.1-only.html

SHELL := /bin/bash
InstallCmd = install -d
ifndef MACHTYPE
MACHTYPE:=$(shell set | grep ^MACHTYPE= | sed s-.*=--)
endif
BinDir ?= ${BaseDir}bin/$(MACHTYPE)
ObjDir ?= $(BaseDir)obj/$(MACHTYPE)/$(Name)/
Repository := $(GitHubRepository)
ifeq ($(GitHubRepository),)
	Repository := dshattuck/microstack
endif

GITHEAD=$(shell git rev-parse --short HEAD)

Name    := $(shell basename `pwd`)
VersionNum = 26a
InstName ?= MouseSuite
LongName = $(Name)$(VersionNum)_$(MACHTYPE)
Target = $(BinDir)/$(LongName)

CC := c++ -DBUILDVERSION='"$(GITHEAD)"' -O3 $(DEBUG) -std=c++23 
Includes:=-I. -I/usr/include/hdf5/serial/
Libraries:= -L/usr/lib/x86_64-linux-gnu/hdf5/serial /usr/lib/x86_64-linux-gnu/hdf5/serial/libhdf5_hl_cpp.a /usr/lib/x86_64-linux-gnu/hdf5/serial/libhdf5_cpp.a /usr/lib/x86_64-linux-gnu/hdf5/serial/libhdf5_hl.a /usr/lib/x86_64-linux-gnu/hdf5/serial/libhdf5.a -lcrypto -lcurl -lpthread -lsz -lz -ldl -lm -ltiff 
InstallDir ?= $(HOME)/$(InstName)$(VersionNum)/bin/$(MACHTYPE)

CCExtension = .cpp
SrcFiles := $(wildcard *$(CCExtension)) $(LocalSrcFiles)
ObjFiles := $(addprefix $(ObjDir),$(SrcFiles:$(CCExtension)=.o))


all: DirCheck $(Target)

DirCheck: $(ObjDir) $(BinDir)

$(ObjDir):
	$(InstallCmd) $(ObjDir)

$(InstallDir):
	$(InstallCmd) $(InstallDir)

$(BinDir):
	$(InstallCmd) $(BinDir)

$(ObjDir)%.o: %$(CCExtension)
	$(CC) $(Includes) -c $< -o $@

install: $(ObjDir) $(BinDir) $(Target) $(InstallDir)
	cp $(Target) $(InstallDir)
	(cd $(InstallDir); ln -f -s $(LongName) $(Name); ln -f -s $(LongName) $(Name)$(VersionNum))

$(Target): $(ObjDir) $(BinDir) $(ObjFiles)
	$(CC) $(ObjFiles) -o $(Target) $(Libraries)

run: $(Target)
	$(Target)

test: $(Target)
	$(Target) -n stack -i /data/RodentToolsData/Ex_488_Em_525_stitched/082870_110900_074*.tif -o test_32nc_buf.h5 -rx 1.866 -ry 1.866 -rz 2.0 -b

full: $(Target)
	$(Target) -i /data/RodentToolsData/Ex_488_Em_525_stitched/082870*.tif -o /data2/Rodent/Ex_488_Em_525_stitched_32nc_buf.h5 -rx 1.866 -ry 1.866 -rz 2.0 -b -c 32

flat: $(Target)
	$(Target) -i /data/RodentToolsData/Ex_488_Em_525_stitched/082870*.tif -o /data2/Rodent/Ex_488_Em_525_stitched_flat2.h5 -rx 1.866 -ry 1.866 -rz 2.0 -b

build: $(Target)

link: deltarget $(Target)

deltarget:
	rm -f $(Target)

clean:
	rm -f $(ObjFiles)

docker: $(Target)
	@printf '\033[0;35m'"Building microstack container"'\033[0m'"\n"
	docker build -t ghcr.io/$(Repository) .

apptainer: docker
	@printf '\033[0;35m'"Building microstack apptainer"'\033[0m'"\n"
	docker save ghcr.io/$(Repository):latest -o microstack.tar
	sudo apptainer build microstack.sif docker-archive:microstack.tar

push: docker
	docker push ghcr.io/$(Repository):latest
