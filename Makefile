ifeq ($(OS),Windows_NT)
    SHELL := pwsh.exe
else
   SHELL := pwsh
endif
.SHELLFLAGS := -NoProfile -Command 

ifneq (,$(wildcard ./.env))
    include .env
    export
endif

OUT ?= out
PRESET ?= default

PANDOCFLAGS =                        \
  --table-of-contents                \
  --pdf-engine=xelatex               \
  --from=markdown                    \
  --number-sections                  \
  --indented-code-classes=javascript \
  --highlight-style=monochrome       \
  -V mainfont="Palatino"             \
  -V documentclass=report            \
  -V papersize=A5                    \
  -V geometry:margin=1in

all: phony output/book.pdf

$(OUT)/%.pdf: docs/%.md Makefile | $(OUT)
	pandoc $< -o $@ $(PANDOCFLAGS)

$(OUT):
	mkdir $(OUT)

.PHONY: list-presets configure build test clean

list-presets:
	cmake --list-presets=all .

$(OUT)/$(PRESET)/build: CMakeLists.txt CMakePresets.json vcpkg.json
	cmake --preset $(PRESET)

configure: $(OUT)/$(PRESET)/build

build: | $(OUT)/$(PRESET)/build
	cmake --build --preset $(PRESET)

test: build
	ctest --preset $(PRESET)

clean:
	@Get-ChildItem . -r $(OUT) | Remove-Item -r -force 

