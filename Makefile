ifeq ($(OS),Windows_NT)
    SHELL := pwsh.exe
else
   SHELL := pwsh
endif
.SHELLFLAGS := -NoProfile -Command 

OUT = out

PRESET = default

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

.PHONY: list-presets
list-presets:
	cmake --list-presets=all .

.PHONY: configure
$(OUT)/$(PRESET)/build: CMakeLists.txt CMakePresets.json vcpkg.json
	cmake --preset $(PRESET)

.PHONY: build
build: | $(OUT)/$(PRESET)/build
	cmake --build $(OUT)/$(PRESET)/build --target install

.PHONY: test
test: build
	ctest $(OUT)/$(PRESET)

.PHONY: clean
clean:
	@Get-ChildItem . -r $(OUT) | Remove-Item -r -force 

